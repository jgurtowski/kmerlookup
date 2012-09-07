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


char *ReadCorrectorErrorString[] = {
  "SUCCESS",
  "Repeat",
  "Strange Segments",
  "Too Many Changes"
};

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

void zeroCorrectorStats(ReadCorrectorStats *stats){
  assert( NULL != stats);
  memset(stats, 0, sizeof(ReadCorrectorStats));
}


int readCorrectorCorrectRead(ReadCorrector *corrector, const ReadCorrectorConf* conf, char *read, ReadCorrectorStats *stats){
  assert( NULL != corrector );
  assert( NULL != conf );
  assert( NULL != read );

  int i, num_found_segments, number_strange_segments;
  int *counts;
  zeroSegmentBuffer(corrector->segmenter);
  CountSegment *segments = corrector->segmenter->segments;
  zeroCorrectorStats(stats);

  char old_base;
  do{
    counts = readCorrectorBuildKmerCounts(corrector, read);
    //keep a copy of the previously created segments for comparision to make sure they change in each iteration
    memcpy(corrector->previous_segments, corrector->segmenter->segments, 
	   corrector->segmenter->max_segments * sizeof(CountSegment));
    
    countSegmenterSegment(corrector->segmenter,counts,corrector->num_kmers, conf->segmentation_threshold);
    num_found_segments = corrector->segmenter->num_found_segments;

    printf("%s\t",read);
      for(i =0;i<corrector->num_kmers;++i){
      printf("%d,",counts[i]);
    }
    printf("\t");
    for(i=0;i<num_found_segments;++i){
      printf("{l:%d,r:%d,m:%f},", segments[i].left, segments[i].right, segments[i].mean);
    }
    printf("\n");

    number_strange_segments = 0;
    for(i =0;i<num_found_segments;++i){
      //check the segments to make sure they look reasonable
      if(segmentLength(segments+i)> conf->min_segment_length && segments[i].mean >= conf->repeat_coverage)
        return REPEAT;
      if(segmentLength(segments+i) < conf->min_segment_length || segments[i].mean >= conf->repeat_coverage)
        ++number_strange_segments;
    }

    if(number_strange_segments > 5){
      return STRANGE_SEGMENTS;
    }
    
    for(i=0; i<num_found_segments;++i){
      if(i>0 && segments[i].mean > segments[i-1].mean){ //check segment to left
        //printf("working to left on :%d\n", segments[i-1].right);
        old_base = read[segments[i-1].right];
	read[segments[i-1].right] = findMaxKmerMutateBase(corrector,read+segments[i-1].right,FIRST);
	if(read[segments[i-1].right] != old_base)
	  ++stats->number_of_alterations;
      }
      if(i < num_found_segments-1){//check segment to right
        if(segments[i].mean > segments[i+1].mean){
          //printf("working on right : %d-%d\n", segments[i+1].left, segments[i+1].left + corrector->kmer_size -1);
	  old_base = read[segments[i+1].left+corrector->kmer_size-1];
	  read[segments[i+1].left + corrector->kmer_size -1] = findMaxKmerMutateBase(corrector,read+segments[i+1].left, LAST);
	  if(old_base != read[segments[i+1].left+corrector->kmer_size-1])
	     ++stats->number_of_alterations;
        }
      }
    }
    if((++stats->number_rounds_of_correction) > conf->max_rounds_of_correction)
      return TOO_MANY_CHANGES;
  }while(0 != memcmp(corrector->previous_segments, segments, sizeof(CountSegment) * corrector->segmenter->max_segments));
  
  return 0;
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
