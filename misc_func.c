#include <assert.h>

#include "misc_func.h"

int array_max(const void *arr, int len, const void * (*accessor)(const void *arr, int i), int (*comparator)(const void *a, const void *b)){
  assert( len > 0 );
  
  if(len == 1)
    return 0;
  
  int max = 0;
  int i, comp;
  for(i=1;i<len;++i){
    comp = comparator(accessor(arr,i),accessor(arr,max));
    if(comp > 0){
      max = i;
    }
  }
  return max;
}

const void *int_accessor(const void *arr, int i){
  const int *a = (const int *)arr;
  return a+i;
}

int int_comparator(const void *aa, const void *bb){
  const int *a = (const int *)aa;
  const int *b = (const int *)bb;

  return (*a)-(*b);
}

int float_comparator(const void *aa, const void *bb){
  const float *a = (const float *)aa;
  const float *b = (const float *)bb;

  if((*a) > (*b))
    return 1;
  else if((*a) == (*b))
    return 0;
  return -1;
}
