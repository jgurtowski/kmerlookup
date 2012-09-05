#ifndef READ_CORRECTOR_H
#define READ_CORRECTOR_H

#include "count_segmenter.h"
#include "kmer_db.h"

typedef struct ReadCorrector_struct{
  int kmer_size;
  int read_size;
  int *read_counts;
  int num_kmers;
  char *kmer;
  char *kmer_rev;
  CountSegmenter *segmenter;
  CountSegment *previous_segments;
  KmerDb *kmer_db;
}ReadCorrector;

typedef enum BaseToModify_enum{
  FIRST,
  LAST
}BaseToModify;


ReadCorrector *newReadCorrector(KmerDb *kmer_db, int readsize, int ksize);
void freeReadCorrector(ReadCorrector *corrector);
/**
 *Builds count array for kmers in read
 *@return pointer to array containing counts for a read
 * The memory returned will be overwritten by subsequent calls to this function
 */
int *readCorrectorBuildKmerCounts(ReadCorrector *corrector, const char *read);

/**
 *Correct a given read, note this function is destructive to read, it will modify it
 *@return pointer to original (now modified) read
 *@param segment_threshold threshold for segmenting reads by kmer coverage
 *@param segment_merge_size merge segments less than or equal in length to this value
 */
char *readCorrectorCorrectRead(ReadCorrector *corrector, char *read, int segment_merge_size, float segment_threshold);


#endif
