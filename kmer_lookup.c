/**
 *Reads a list of kmers one per line from stdin,
 *Looks up the count and writes the kmer and count to stdout
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#include "kmer_record.h"
#include "kmer_db.h"

int main(int argc, char *argv[]){
  if(argc != 2){
    printf("kmerlookup <database>\n");
    return -1;
  }
  
  size_t bufsize = 1024;
  char *buffer = malloc(bufsize * sizeof(char));

  KmerDb *db = newKmerDb(argv[1]);
  if(NULL == db){
    printf("Error opening db %s\n",argv[1]);
    return -1;
  }
  
  KmerRecord *record;

  bool first = true;
  int ksize = 0;

  while(-1 != getline(&buffer, &bufsize, stdin)){
    if(first){
      ksize = strlen(buffer) - 1;
      first = false;
    }
    buffer[ksize] = '\0';
    if(NULL != (record = findRecordFromDb(db,buffer))){
      printf("%s\t%d\n",record->kmer, record->count);
    }else{
      printf("%s\t%d\n",buffer,-1);
    }
    fflush(stdout);
  }
  freeKmerDb(db);
  free(buffer);
  return 0;
}
