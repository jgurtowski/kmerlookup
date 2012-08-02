#ifndef KMER_RECORD_H
#define KMER_RECORD_H

#include <stdio.h>

typedef struct KmerRecord_struct{
  char *kmer;
  int count;
  int kmer_size;
}KmerRecord;

KmerRecord *newKmerRecord(int ksize);
void setKmer(KmerRecord *r, const char *k);
void setCount(KmerRecord *r, int count);
void freeKmerRecord(KmerRecord *r);
void readKmerRecordFromStream(KmerRecord *record, char *stream);
void writeKmerRecordToFile(KmerRecord *record, FILE *fh);

#endif
