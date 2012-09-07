#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>

#include "kmer_record.h"
#include "kmer_packer.h"

KmerRecord *newKmerRecord(int ksize){
  KmerRecord *record = malloc( sizeof(KmerRecord) );
  record -> kmer = malloc( (ksize + 1) * sizeof(char) );
  (record -> kmer)[ksize] = '\0';
  record -> count = 0;
  record -> kmer_size = ksize;
  record -> kmer_packer = newKmerPacker(ksize);
  record -> unpacked = false;
  return record;
}

void setKmer(KmerRecord *r, const char *k){
  memcpy(r->kmer, k, r->kmer_size);
  r->unpacked = true;
}

void setCount(KmerRecord *r, int count){
  r->count = count;
}


void freeKmerRecord(KmerRecord *r){
  free(r -> kmer);
  freeKmerPacker(r->kmer_packer);
  free(r);
}

void readKmerRecordFromStream(KmerRecord *record, char *stream){
  readPackedKmerFromStream(record->kmer_packer, stream);
  record -> unpacked = false;
  //memcpy(record->kmer_packer->packed_buffer, stream, record->kmer_packer->packed_buffer_size);
  //char *unpacked_kmer = unpackKmer(record->kmer_packer, record->kmer_packer->packed_buffer);
  //memcpy(record->kmer, unpacked_kmer, record->kmer_size);
  memcpy(&(record->count), stream+record->kmer_packer->packed_buffer_size, sizeof(int));
}

void writeKmerRecordToFile(KmerRecord *record, FILE *fh){
  //unsigned char *packed_kmer = packKmer(record -> kmer_packer, record->kmer);
  ensurePacked(record);
  fwrite(record->kmer_packer->packed_buffer, sizeof(char), record->kmer_packer->packed_buffer_size, fh);
  fwrite(&(record->count), sizeof(int), 1, fh);
}

int compareRecordByKmer(const KmerRecord *record1, const KmerRecord *record2){
  return comparePackedKmers(record1->kmer_packer->packed_buffer, record2->kmer_packer->packed_buffer, 
			    record1->kmer_packer->packed_buffer_size);
}


void ensurePacked(KmerRecord *r){
  packKmer(r -> kmer_packer, r->kmer);
}

char *getKmer(KmerRecord *r){
  if(!r->unpacked){
    char *unpacked_kmer = unpackKmer(r->kmer_packer, r->kmer_packer->packed_buffer);
    memcpy(r->kmer, unpacked_kmer, r->kmer_size);
    r->unpacked = true;
  }
  return r->kmer;
}

void setPackedKmer(KmerRecord *r, const unsigned char *kmer){
  memcpy(r->kmer_packer->packed_buffer, kmer, r->kmer_packer->packed_buffer_size);
  r->unpacked = false;
}
