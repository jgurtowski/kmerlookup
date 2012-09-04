#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "count_segmenter.h"


CountSegmenter *newCountSegmenter(int max_segments){
  CountSegmenter *segmenter = malloc(sizeof(CountSegmenter));
  segmenter -> segments = malloc(max_segments * sizeof(CountSegment));
  segmenter -> max_segments = max_segments;
  segmenter -> num_found_segments = 0;
  
  return segmenter;
};

CountSegmenter *countSegmenterSegment(CountSegmenter *segmenter, int *counts, 
				      int num_counts, float threshold){
  assert( NULL != segmenter);

  int *num_found = &(segmenter -> num_found_segments);
  *num_found = 0;
  
  CountSegment *cur_segment;

  int i,left = 0;
  int64_t sum = counts[0];
  for(i=1;i<num_counts;++i){
    if(counts[i] - counts[i-1] > threshold || 
       counts[i-1] - counts[i] > threshold){
      cur_segment = &(segmenter->segments[*num_found]);
      cur_segment -> left = left;
      cur_segment -> right = i-1;
      cur_segment -> mean = sum / (i - left);
      (*num_found) += 1;
      left = i;
      sum = counts[i];
    }else{  
      sum += counts[i];
    }
  }
  
  cur_segment = &(segmenter->segments[*num_found]);
  cur_segment -> left = left;
  cur_segment -> right = i - 1;
  cur_segment -> mean = sum / (i - left);
  (*num_found) += 1;

  return segmenter;
}

void freeCountSegmenter(CountSegmenter *segmenter){
  assert( NULL != segmenter );
  free(segmenter -> segments);
  free(segmenter);
}


