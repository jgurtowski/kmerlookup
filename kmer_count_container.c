#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <assert.h>
#include <unistd.h>

#include <pthread.h>

#include "read_queue.h"
#include "kmer_count_container.h"


KmerCountContainer *kmerCountContainerNew(int readsize, int kmersize, const char *db_filename){
  
  KmerCountContainer *container = malloc(sizeof(KmerCountContainer));
  container -> queue = readQueueNew(readsize);
  pthread_mutex_init(&container->io_mutex, NULL);
  container -> kmersize = kmersize;
  
  //open db file 
  container->mapped_file_handle = open(db_filename,O_RDONLY);
  if(0 == container->mapped_file_handle){
    fprintf(stderr,"Error opening file %s\n", db_filename);
    return NULL;
  }
  fstat(container->mapped_file_handle, &container->statbuf);
  container->mapped_memory = mmap(0,container->statbuf.st_size,PROT_READ,MAP_SHARED,container->mapped_file_handle,0);
  if(container->mapped_memory == MAP_FAILED){
    fprintf(stderr, "Error mapping file %s, errno %d \n", db_filename,errno);
    return NULL;
  }
  
  return container;
}

void kmerCountContainerFree(KmerCountContainer *container){
  assert( NULL != container);
  readQueueFree(container -> queue);
  pthread_mutex_destroy(&container->io_mutex);
  
  munmap(container->mapped_memory, container->statbuf.st_size);
  close(container->mapped_file_handle);

  free(container);
}
