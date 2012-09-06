#ifndef COUNT_SEGMENTER_H
#define COUNT_SEGMENTER_H

typedef struct CountSegment_struct{
  int left;
  int right;
  float mean;
}CountSegment;

typedef struct CountSegmenter_struct{
  CountSegment *segments;
  CountSegment *merge_buffer;
  int num_merged_segments;
  int max_segments;
  int num_found_segments;
}CountSegmenter;

/**Create a new countSegmenter
 *@param max_segments maximum number of segments possible
 **/
CountSegmenter *newCountSegmenter(int max_segments);

/**
 *Free the memory allocated by segmenter
 */
void freeCountSegmenter(CountSegmenter *segmenter);

/**
 * Run Segmentation based on given threshold
 * @param counts int array of counts (any timeseries)
 * @param num_counts number of data points in counts array
 * @param threshold, the threshold on which to bin elements
 * @return Pointer to the segmenter that was passed in
 */
CountSegmenter *countSegmenterSegment(CountSegmenter *segmenter, int *counts, int num_counts, float threshold);

/**
 *Accessor function, given an array of segments return the mean for a given index
 */
const void *segmentMeanAccessor(const void *d, int idx);

/**
 *Merge small segments
 *@param segment_size merge any segment <= segment_size in length
 */
int mergeSmallSegments(CountSegmenter *segmenter, int segment_size);

/**
 *Compute the length over a segment
 */
int segmentLength(CountSegment *segment);

/**
 *Zero out segment buffer
 */
void zeroSegmentBuffer(CountSegmenter *segmenter);

#endif
