#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "read_queue.h"


ReadQueue *readQueueNew(int readsize){
  
  ReadQueue *queue = malloc(sizeof(ReadQueue));
  queue -> readsize = readsize;
  queue -> empty = 0;
  pthread_mutex_init(&(queue->mutex), NULL);
  pthread_cond_init(&(queue->cond), NULL);
  return queue;
}

void readQueuePut(ReadQueue *queue, const char *read){
  assert(NULL != queue);
  assert(NULL != read);
  
  pthread_mutex_lock(&(queue->mutex));
  while(NULL != queue->read){
    pthread_cond_wait(&(queue->cond), &(queue->mutex));
  }
  queue->read = read;
  pthread_cond_broadcast(&(queue->cond));
  pthread_mutex_unlock(&(queue->mutex));
}

int readQueuePop(ReadQueue *queue, char *read){
  assert(NULL != queue);

  pthread_mutex_lock(&(queue->mutex));
  while(NULL == queue->read && 0 == queue->empty){
    pthread_cond_wait(&(queue->cond), &(queue->mutex));
  }
  
  if(queue->empty){
    pthread_mutex_unlock(&(queue->mutex));
    return -1;
  }
  memcpy(read, queue->read, queue->readsize);
  queue->read = NULL;
  pthread_cond_broadcast(&(queue->cond));
  pthread_mutex_unlock(&(queue->mutex));
  return 0;
}

void readQueueFree(ReadQueue *queue){
  assert(NULL != queue);
  pthread_mutex_destroy(&(queue->mutex));
  pthread_cond_destroy(&(queue->cond));
  free(queue);
}

void readQueueIsEmpty(ReadQueue *queue, int *empty){
  assert(NULL != queue);
  pthread_mutex_lock(&queue->mutex);
  *empty = queue->empty;
  pthread_mutex_unlock(&queue->mutex);
}

void readQueueSetStatus(ReadQueue *queue, int empty){
  assert(NULL != queue);
  
  pthread_mutex_lock(&(queue->mutex));
  while(NULL != queue->read){
    pthread_cond_wait(&queue->cond, &queue->mutex);
  }
  queue->empty = empty;
  pthread_cond_broadcast(&(queue->cond));
  pthread_mutex_unlock(&(queue->mutex));
}
