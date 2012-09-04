/**
 *Takes reads from stdin, reads must be the same length one per line, trys to correct errors 
 *and smooth heterozygosity
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#include "read_queue.h"
#include "kmer_db.h"
#include "reverse_complement.h"
#include "kmer_count_container.h"
#include "count_segmenter.h"

static int LINE_SIZE = 1024;

//will actually process the data
void *consumer_func(void *data);

int main(int argc, char *argv[]){

  if(argc != 4){
    printf("Usage: kmercorrect_t kmer_db kmer_size num_threads\n");
    return 1;
  }
  
  char *line = malloc(sizeof(char) * LINE_SIZE);

  fgets(line, LINE_SIZE, stdin);
  //remove newline 
  line[strlen(line)-1] = '\0';
  
  int read_len = strlen(line);    

  char *kdb_filename = argv[1];
  
  int kmer_size = atoi(argv[2]);

  KmerCountContainer *container = kmerCountContainerNew(read_len, kmer_size, kdb_filename);

  int num_consumers = atoi(argv[3]);

  pthread_t *consumers = malloc(num_consumers * sizeof(pthread_t));
  
  //init consumer threads
  int i;
  for(i=0;i<num_consumers;++i){
    if(pthread_create(&consumers[i],NULL,consumer_func,container)){
      printf("Couldn't create consumer");
      return -1;
    }
  }
  
  //just take reads from stdin (one per line) and feed them into the queue
  int num_reads = 1;
  readQueuePut(container->queue,line);
  while(NULL != fgets(line, LINE_SIZE, stdin)){
    line[read_len] = '\0'; // remove newline
    if(0 == (++num_reads) % 1000000)
      fprintf(stderr,"Reads processed: %d\n",num_reads);
    readQueuePut(container->queue,line);
  }
  
  readQueueSetStatus(container->queue,1);//queue is now empty
  
  for(i=0;i<num_consumers;++i){
    pthread_join(consumers[i],NULL);
  }
  
  free(consumers);
  kmerCountContainerFree(container);
  free(line);
  return 0;
}

//process the reads
void *consumer_func(void *data){

  KmerCountContainer *container = (KmerCountContainer *)data;
  ReadQueue *queue = container->queue;
  int ksize = container->kmersize;
  
  //create a new kmer_db in each thread b/c it is not thread safe
  KmerDb *db = newKmerDbWithMmap(container->mapped_file_handle, container->mapped_memory, container->statbuf);

  //allocate space for read
  char *read = malloc(sizeof(char) * (queue->readsize+1));
  read[queue->readsize] = '\0';
  
  //space for each kmer and reverse complement
  char *kmer = malloc(sizeof(char) * (ksize+1));
  char *kmer_rev = malloc(sizeof(char) * (ksize+1));
  kmer[ksize] = '\0';
  
  //space for each count output
  int num_kmers = queue->readsize - ksize + 1;
  int *counts = malloc(sizeof(int) * num_kmers);

  //create a new segmenter
  CountSegmenter *segmenter = newCountSegmenter(num_kmers);
  CountSegment *segment;  
  
  KmerRecord *record;
  
  //work loop, as long as there is data in the queue, 
  //pull the reads out and look them up in the db
  int i;
  while (0 == readQueuePop(queue,read)){
    for(i=0;i<num_kmers;++i){
      memcpy(kmer,read+i,ksize);
      reverseComplement(kmer, kmer+ksize, kmer_rev);
      //take the lower order kmer from either forward or reverse
      if( strcmp( kmer, kmer_rev ) < 0)
        record = findRecordFromDb(db, kmer);
      else
	record = findRecordFromDb(db, kmer_rev);
        
      if( NULL == record){
        counts[i] = -1;
      }else{
        counts[i] = record->count;
      }
    }
    
    countSegmenterSegment(segmenter, counts, num_kmers, 17.5);
    
    //output data, use the io_mutex to lock output so it doesn't get clobbered
    pthread_mutex_lock(&container->io_mutex);
    printf("%s\t",read);
    for(i=0;i<num_kmers -1;++i){
      printf("%d,",counts[i]);
    }
    printf("%d\t",counts[i]);

    for(i=0;i<segmenter->num_found_segments-1;++i){
      segment = &(segmenter->segments[i]);
      printf("l:%d-r:%d-m:%f,", segment->left,segment->right,segment->mean);
    }
    segment = &(segmenter->segments[i]);
    printf("l:%d-r:%d-m:%f\n", segment->left,segment->right,segment->mean);
    fflush(stdout);
    pthread_mutex_unlock(&container->io_mutex);
  }
  
  freeCountSegmenter(segmenter);
  freeKmerDbWithMmap(db);
  free(counts);
  free(kmer);
  free(kmer_rev);
  free(read);
  return NULL;
}

