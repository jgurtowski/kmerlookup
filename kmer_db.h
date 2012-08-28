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

/**
 * Use file as the kmer db
 */
KmerDb *newKmerDb(const char *file);

/**
 * Should be paired with newKmerDb calls
 */
void freeKmerDb(KmerDb *kdb);


/**
 * If creating multiple KmerDb's with the same underlying file, pass in the file
 * so it doesn't need to be mapped multiple times.
 */
KmerDb *newKmerDbWithMmap(int mapped_file_handle, char *mmap_arr, struct stat statbuf);


/**
 *Should be paired with newKmerDbWithMmap
 *This destructor will NOT close the underlying database or unmap memory
 */
void freeKmerDbWithMmap(KmerDb *kdb);

KmerRecord *getRecordFromDb(KmerDb *db, uint64_t idx);
KmerRecord *findRecordFromDb(KmerDb *db, const char *kmer);


#endif
