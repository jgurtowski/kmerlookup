/**Takes a stream of kmers in the form [kmer][space][count]
 **and converts them into a kmer db.
 **Input is from stdin, output goes to stdout
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "kmer_record.h"

char **parseKmerCount(char *line){
  
  char **arr = malloc( 2 * sizeof(char *));

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
  KmerRecord *record;
  char **arr;
  int count;

  while(-1 != getline(&buffer,&bufsize,stdin)){
    arr = parseKmerCount(buffer);
    if(first){
      int ksize = strlen(arr[0]);
      fwrite(&ksize,sizeof(int),1,stdout);
      record = newKmerRecord(ksize);
      first = false;
    }
    setKmer(record,arr[0]);
    count = atoi(arr[1]);
    setCount(record,count);
    writeKmerRecordToFile(record,stdout);
  }
  
  return 0;
}
