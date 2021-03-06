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
#include "kmer_count_container.h"
#include "read_corrector.h"


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
    
  char *read_copy = malloc(sizeof(char) * (queue->readsize+1));
  read[queue->readsize] = '\0';

  ReadCorrector *read_corrector = newReadCorrector(db, queue->readsize, ksize);
  
  //int *counts;
  //int i;
  //work loop, as long as there is data in the queue, 
  //pull the reads out and look them up in the db
  ReadCorrectorStats stats;
  int correct_return_code;
  
  ReadCorrectorConf corrector_conf = {
    .repeat_coverage = 500,
    .min_segment_length = 3,
    .max_rounds_of_correction = 10,
    .segmentation_threshold = 17.5
  };
  
  while (0 == readQueuePop(queue,read)){
    strcpy(read_copy,read);
    correct_return_code = readCorrectorCorrectRead(read_corrector, &corrector_conf,read_copy,&stats);
    
    //print out results of correction
    pthread_mutex_lock(&container->io_mutex);
    if(correct_return_code){
      //some error occurred print out the original read
      printf("%s\t", read);
    }else{
      printf("%s\t",read_copy);
    }
    printf("%s\t%d\t%d\n",ReadCorrectorErrorString[correct_return_code],
	   stats.number_of_alterations,stats.number_rounds_of_correction);
    fflush(stdout);
    pthread_mutex_unlock(&container->io_mutex);
  }
  freeReadCorrector(read_corrector);
  freeKmerDbWithMmap(db);
  free(read);
  free(read_copy);
  return NULL;
}

