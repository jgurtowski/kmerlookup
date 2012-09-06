#ifndef READ_CORRECTOR_H
#define READ_CORRECTOR_H

#include "count_segmenter.h"
#include "kmer_db.h"

extern char *ReadCorrectorErrorString[];

typedef struct ReadCorrectorConf_struct{
  int repeat_coverage; //kmers with this coverage begin to be flagged as repeats
  int min_segment_length; //segments smaller than this are discounted
  int max_rounds_of_correction; //Maximum number of rounds of correction
  float segmentation_threshold; //threshold on which to segment the kmer coverage
}ReadCorrectorConf;

/**
 *Holds stats about the read correction
 */
typedef struct ReadCorrectorStats_struct{
  int number_rounds_of_correction;
  int number_of_alterations;
}ReadCorrectorStats;

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

typedef enum ReadCorrectorError_enum{
  //SUCCESS,
  REPEAT = 1,
  STRANGE_SEGMENTS,
  TOO_MANY_CHANGES
}ReadCorrectorError;


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
 *@param conf (in) Holds important parameters for correcting reads look at definition of ReadCorrectorConf
 *@param read (in/out) read to be corrected (done in place)
 *@param stats (out) contains stats about the correction processes
 */
int readCorrectorCorrectRead(ReadCorrector *corrector, const ReadCorrectorConf *conf, 
			     char *read, ReadCorrectorStats *stats);

/**
 *Zero out stats struct stats
 */
void zerCorrectorStats(ReadCorrectorStats *stats);

#endif
