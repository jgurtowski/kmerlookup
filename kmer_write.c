/**Takes a stream of kmers in the form [kmer][space][count]
 **and converts them into a kmer db.
 **Input is from stdin, output goes to stdout
 **/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "kmer_record.h"

char **parseKmerCount(char *line, char **arr){
  char delims[] = {'\t','\n'};
  char *result = NULL;
  result = strtok( line, delims);
  int count = 0;
  while( result != NULL && count < 2){
    arr[count] = result;
    result = strtok(NULL, delims);
    ++count;
  }
  return arr;
}

int main(int argc, char *argv[]){
  bool first = true;
  size_t bufsize = 1024;
  char *buffer = malloc(bufsize);
  KmerRecord *record = NULL;
  char **arr = malloc( 2 * sizeof(char *));
  int count;
  int ksize;
  long int lcount;
  while(-1 != getline(&buffer,&bufsize,stdin)){
    parseKmerCount(buffer,arr);
    if(first){
      ksize = strlen(arr[0]);
      fwrite(&ksize,sizeof(int),1,stdout);
      record = newKmerRecord(ksize);
      first = false;
    }
    setKmer(record,arr[0]);
    lcount = atol(arr[1]);
    if(lcount > INT_MAX){
      count = INT_MAX;
    }else{
      count = (int)lcount;
    }
    setCount(record,count);
    writeKmerRecordToFile(record,stdout);
  }
  
  freeKmerRecord(record);
  free(arr);
  free(buffer);
  
  return 0;
}
