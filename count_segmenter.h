#ifndef COUNT_SEGMENTER_H
#define COUNT_SGEMENTER_H

typedef struct CountSegment_struct{
  int left;
  int right;
  float mean;
}CountSegment;

typedef struct CountSegmenter_struct{
  CountSegment *segments;
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

#endif
