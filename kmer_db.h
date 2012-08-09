#ifndef KMER_DB_H
#define KMER_DB_H

#include <sys/stat.h>
#include <stdint.h>

#include "kmer_record.h"
//typedef struct KmerRecord_struct *KmerRecordPtr;

typedef struct KmerDb_struct{
  int mapped_file_handle;
  char *mmapfile;
  KmerRecord  *kmer_record;
  uint64_t num_records;
  int record_size;
  struct stat statbuf;
}KmerDb;

KmerDb *newKmerDb(const char *file);
void freeKmerDb(KmerDb *kdb);
KmerRecord *getRecordFromDb(KmerDb *db, uint64_t idx);
KmerRecord *findRecordFromDb(KmerDb *db, const char *kmer);

#endif
