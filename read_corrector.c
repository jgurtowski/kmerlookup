#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "read_corrector.h"
#include "kmer_record.h"
#include "reverse_complement.h"
#include "misc_func.h"
#include "count_segmenter.h"

static const char bases[] = {'A','C','G','T'};
static const char complement_bases[] = {'T','G','C','A'};

ReadCorrector *newReadCorrector(KmerDb *kmer_db, int readsize, int ksize){
  ReadCorrector *corrector = malloc(sizeof(ReadCorrector));
  corrector->kmer_size = ksize;
  corrector->read_size = readsize;
  corrector->kmer = malloc(sizeof(char) * (ksize + 1));
  corrector->kmer_rev = malloc(sizeof(char) * (ksize + 1));
  corrector->kmer[ksize] = '\0';
  int num_kmers = readsize - ksize + 1;
  corrector->num_kmers = num_kmers;
  corrector->read_counts = malloc(sizeof(int) * num_kmers);  
  corrector->segmenter = newCountSegmenter(num_kmers);
  corrector->kmer_db = kmer_db;
  corrector->previous_segments = malloc(sizeof(CountSegment) * num_kmers);

  return corrector;
}


int *readCorrectorBuildKmerCounts(ReadCorrector *corrector, const char *read){
  assert( NULL != corrector );
  assert( NULL != read );

  char *kmer = corrector->kmer;
  char *kmer_rev = corrector->kmer_rev;
  int *counts = corrector->read_counts;

  KmerRecord *record = NULL;

  int i;
  for(i=0;i<corrector->num_kmers;++i){
    memcpy(kmer,read+i,corrector->kmer_size);
    reverseComplement(kmer, kmer+corrector->kmer_size, kmer_rev);
    //take the lower order kmer from either forward or reverse                                                                                                                    
    if( strcmp( kmer, kmer_rev ) < 0)
      record = findRecordFromDb(corrector->kmer_db, kmer);
    else
      record = findRecordFromDb(corrector->kmer_db, kmer_rev);

    if( NULL == record )
      counts[i] = -1;
    else
      counts[i] = record->count;
  }

  return counts;

}

/**
 *@param kmer_start beginning iterator of kmer
 *@param kmer_len length of kmer
 */
static char findMaxKmerMutateBase(ReadCorrector *corrector, const char *kmer_start, BaseToModify mod_base){
  char max_letter = bases[0];
  int max=-1;
  char *kmer = corrector->kmer;
  char *kmer_rev = corrector->kmer_rev;
  int ksize = corrector->kmer_size;

  memcpy(kmer,kmer_start, ksize);
  reverseComplement(kmer,kmer+ksize,kmer_rev);

  KmerRecord *record;
  
  int i;
  for(i=0;i<4;++i){
    if(mod_base == FIRST){
      kmer[0] = bases[i];
      kmer_rev[ksize-1] = complement_bases[i];
    }else{
      kmer[ksize-1] = bases[i];
      kmer_rev[0] = complement_bases[i];
    }
    record = findRecordFromDb(corrector->kmer_db, kmer);
    if( NULL != record && record->count > max){
      max = record->count;
      max_letter = bases[i];
    }
    record = findRecordFromDb(corrector->kmer_db, kmer_rev);
    if( NULL != record && record->count > max){
      max = record->count;
      max_letter = bases[i];
    }
  }
  return max_letter;
}


char *readCorrectorCorrectRead(ReadCorrector *corrector, char *read, int segment_merge_size, float segment_threshold){
  assert( NULL != corrector );
  assert( NULL != read );

  int i;  
  int *counts = readCorrectorBuildKmerCounts(corrector, read);
  printf("%s\t",read);
  for(i =0;i<corrector->num_kmers;++i){
    printf("%d,",counts[i]);
  }
  
  //keep a copy of the previously created segments for comparision to make sure they change in each iteration
  memcpy(corrector->previous_segments, corrector->segmenter->segments, corrector->segmenter->max_segments);
  
  countSegmenterSegment(corrector->segmenter,counts,corrector->num_kmers, segment_threshold);
  if(mergeSmallSegments(corrector->segmenter, segment_merge_size)){
    //we failed to merge segments, just give up
    return read;
  }

  CountSegment *segments = corrector->segmenter->segments;
  int num_found_segments = corrector->segmenter->num_found_segments;
  
  for(i=0;i<num_found_segments;++i){
    printf("{l:%d,r:%d,m:%f},", segments[i].left, segments[i].right, segments[i].mean);
  }
  printf("\n");

  if( 0 == memcmp(corrector->previous_segments, segments, sizeof(CountSegment) * corrector->segmenter->max_segments)){
    //check to see if calculated segments have changed since last iteration, if they have not
    //there is no more work to be done and we just return from recursive correction
    return read;
  }
  int max = array_max(segments, num_found_segments, segmentMeanAccessor, float_comparator);
  
  if(max > 0){
    read[segments[max-1].right] = findMaxKmerMutateBase(corrector,read+segments[max-1].right,FIRST);
  }
  if(max < num_found_segments-1){
    printf("working on base, %d\n", segments[max+1].left + corrector->kmer_size);
    read[segments[max+1].left + corrector->kmer_size -1] = findMaxKmerMutateBase(corrector,read+segments[max+1].left, LAST);
  }
  
  return readCorrectorCorrectRead(corrector,read,segment_merge_size,segment_threshold);
}

void freeReadCorrector(ReadCorrector *corrector){
  assert( NULL != corrector );
  freeCountSegmenter(corrector->segmenter);
  free(corrector->previous_segments);
  free(corrector->read_counts);
  free(corrector->kmer);
  free(corrector->kmer_rev);
  free(corrector);
}
