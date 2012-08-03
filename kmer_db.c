#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>

#include "kmer_db.h"
#include "kmer_record.h"

KmerDb *newKmerDb(char *file){
  struct stat statbuf;
  int mapped_file = open(file,O_RDONLY);
  if(0 == mapped_file)
    return NULL;
  fstat(mapped_file, &statbuf);
  char *m = mmap(0,statbuf.st_size,PROT_READ,MAP_SHARED,mapped_file,0);
  if(m == MAP_FAILED)
    return NULL;
  KmerDb *db = malloc( sizeof(KmerDb));
  db -> mmapfile = m;
  db -> mapped_file_handle = mapped_file;
  db -> statbuf = statbuf;
  //get kmersize from file
  int kmersize;
  memcpy(&kmersize, db->mmapfile, sizeof(int));
  //create kmer record
  db -> kmer_record = newKmerRecord(kmersize);
  //calculate number of records
  db -> record_size = kmersize + sizeof(int);
  db -> num_records = (statbuf.st_size - sizeof(int)) 
    / db -> record_size;
  
  return db;
}

void freeKmerDb(KmerDb *kdb){
  assert(kdb != NULL);
  munmap(kdb-> mmapfile, kdb->statbuf.st_size);
  close(kdb->mapped_file_handle);
  freeKmerRecord(kdb->kmer_record);
  free(kdb);
}

KmerRecord *getRecordFromDb(KmerDb *db, uint64_t idx){
  readKmerRecordFromStream(db->kmer_record,
                           db->mmapfile + sizeof(int) + 
                           (idx * db -> record_size));
  return db->kmer_record;
}

//simple binary search to find records
KmerRecord *findRecordFromDb(KmerDb *db, const char *kmer){
  assert( db != NULL);
  
  uint64_t l = 0;
  uint64_t r = db -> num_records;
  uint64_t cur;
  KmerRecord *record;
  while( l <= r){
    cur = (l + r) / 2 ;
    record = getRecordFromDb(db,cur);
    assert( record != NULL );
    int diff = strcmp(kmer,record->kmer);
    if(diff < 0 ){
      r = cur - 1;
    }else if(diff > 0){
      l = cur + 1;
    }else{
      return record;
    }
  }
  return NULL;
}





