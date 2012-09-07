#ifndef KMER_RECORD_H
#define KMER_RECORD_H

#include <stdio.h>
#include <stdbool.h>

#include "kmer_packer.h"

typedef struct KmerRecord_struct{
  char *kmer;
  KmerPacker *kmer_packer;
  int count;
  int kmer_size;
  bool unpacked;
}KmerRecord;

KmerRecord *newKmerRecord(int ksize);
void setKmer(KmerRecord *r, const char *k);
void setCount(KmerRecord *r, int count);
void freeKmerRecord(KmerRecord *r);
void readKmerRecordFromStream(KmerRecord *record, char *stream);
void writeKmerRecordToFile(KmerRecord *record, FILE *fh);


/**
 *Make sure ensurePacked is called on both records before calling this function
 */
int compareRecordByKmer(const KmerRecord *record1, const KmerRecord *record2);

/**
 *Force packed kmer representation to be stored
 */
void ensurePacked(KmerRecord *r);

/**
 *Accessor to kmer
 */
char *getKmer(KmerRecord *r);

/**
 *Utility function, mostly for internal use but could be useful outside
 */
void setPackedKmer(KmerRecord *r, const unsigned char *kmer);

#endif
