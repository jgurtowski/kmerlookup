#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "count_segmenter.h"


CountSegmenter *newCountSegmenter(int max_segments){
  CountSegmenter *segmenter = malloc(sizeof(CountSegmenter));
  segmenter -> segments = malloc(max_segments * sizeof(CountSegment));
  segmenter -> merge_buffer = malloc(max_segments *sizeof(CountSegment));
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

int mergeSmallSegments(CountSegmenter *segmenter, int segment_size){
  assert( NULL != segmenter);

  //first detect if there are bad segments
  CountSegment *segments = segmenter->segments;
  int i;
  segmenter -> num_merged_segments = 0;
  for(i=0;i<segmenter->num_found_segments;++i){
    if(segments[i].right - segments[i].left < segment_size - 1){
      if(segmenter->num_merged_segments > 0 ){
	//there is a previous segment that is larger than the segment_size cutoff, merge with it
	segmenter->merge_buffer[segmenter->num_merged_segments-1].right = segments[i].right;
	if(i+1 < segmenter->num_found_segments && (segments[i+1].right -segments[i+1].left >= segment_size)){
	  //segment to the right of the small one is large, merge it also
	  segmenter->merge_buffer[segmenter->num_merged_segments-1].right = segments[i+1].right;
	  ++i;
	}else{
	  //segment to the right is small too, give up
	  return 1;
	}
      }else if(i+1 < segmenter->num_found_segments && (segments[i+1].right - segments[i+1].left >= segment_size)){
	//there is a segment to the right of this one that is larger than segment_size cutoff, merge with it
	segments[i+1].left = segments[i].left;
      }else{
	//too many small segments, give up
	return 2;
      }
    }else{ //segment passes segment_size cutoff, just add it to the merged list
      segmenter -> merge_buffer[segmenter->num_merged_segments] = segments[i];
      ++segmenter -> num_merged_segments;
    }
  }

  //copy merged segments back to original segments array
  memcpy(segments, segmenter->merge_buffer, sizeof(CountSegment) * segmenter->num_merged_segments);
  segmenter->num_found_segments = segmenter->num_merged_segments;
  return 0;
}



void freeCountSegmenter(CountSegmenter *segmenter){
  assert( NULL != segmenter );
  free(segmenter -> merge_buffer);
  free(segmenter -> segments);
  free(segmenter);
}


const void *segmentMeanAccessor(const void *d, int idx){
  CountSegment *segments = (CountSegment *)d;
  return &segments[idx].mean;
}
