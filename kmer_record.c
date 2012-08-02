#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "kmer_record.h"


KmerRecord *newKmerRecord(int ksize){
  KmerRecord *record = malloc( sizeof(KmerRecord) );
  record -> kmer = malloc( (ksize + 1) * sizeof(char) );
  (record -> kmer)[ksize] = '\0';
  record -> count = 0;
  record -> kmer_size = ksize;
  return record;
}

void setKmer(KmerRecord *r, const char *k){
  memcpy(r->kmer, k, r->kmer_size);
}

void setCount(KmerRecord *r, int count){
  r->count = count;
}

void freeKmerRecord(KmerRecord *r){
  free(r -> kmer);
  free(r);
}

void readKmerRecordFromStream(KmerRecord *record, char *stream){
  int ksize = record->kmer_size;
  memcpy(record->kmer, stream, ksize);
  memcpy(&(record->count), stream+ksize, sizeof(int));
}

void writeKmerRecordToFile(KmerRecord *record, FILE *fh){
  fwrite(record->kmer, sizeof(char), record->kmer_size, fh);
  fwrite(&(record->count), sizeof(int), 1, fh);
}

