#ifndef READ_QUEUE_H
#define READ_QUEUE_H

#include <pthread.h>

/**
 *Threaded Queue to parallelize kmer operations
 */

typedef struct ReadQueue_struct{
  const char *read;
  int readsize;
  int empty;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
}ReadQueue;

#endif

/**
 *Build a new ReadQueue, must be free'd with readQueueFree
 */
ReadQueue *readQueueNew(int readsize);


void readQueueFree(ReadQueue *queue);
/**
 *@read the read that will be put in the queue
 *The data will not be modified, only a reference
 *to it will be stored
 */
void readQueuePut(ReadQueue *queue, const char *read);

/**
 *@param read the destination for the read to be copied to
 *Ensure that the memory pointed to by read is large enough
 *to hold readsize+1 data
 */
int readQueuePop(ReadQueue *queue, char *read);

/**
 *Sets empty status for the queue
 *@param empty=0 = true empty=1 = false
 */
void readQueueSetStatus(ReadQueue *queue, int empty);


/**
 *Check if the queue is empty.
 *@param empty pointer to integer that will be set with empty status
 */
void readQueueIsEmpty(ReadQueue *queue, int *empty);
