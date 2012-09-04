#ifndef KMER_COUNT_CONTAINER_H
#define KMER_COUNT_CONTAINER_H

#include <sys/stat.h>

#include "read_queue.h"


typedef struct KmerCountContainer_struct{
  ReadQueue *queue;
  int kmersize;
  pthread_mutex_t io_mutex;
  
  //for creating a new kmer db
  struct stat statbuf;
  int mapped_file_handle;
  char *mapped_memory;
}KmerCountContainer;

/**
 *returns new KmerCountContainer
 */
KmerCountContainer *kmerCountContainerNew(int readsize, int kmersize, const char *db_filename);
void kmerCountContainerFree(KmerCountContainer *container);


#endif
