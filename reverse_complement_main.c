/**
 *Reads lines from stdin, reverse complements them and outputs them to stdout
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reverse_complement.h"

#define _GNU_SOURCE

int main(int argc, char *argv[]){

  const int bufsize = 10024;
    
  char *line = malloc(sizeof(char) * bufsize);
  char *complement = malloc(sizeof(char) * bufsize);

  int bytes_read;
  while((bytes_read = getline(&line, &bufsize, stdin)) > 0 ){
    if(reverseComplement(line,line+bytes_read-1,complement)){
      printf("Unknown char in %s, Expecting DNA (A,C,G,T,N)\n", line);
      return -1;
    }
    printf("%s\n", complement);
  }
  
  free(complement);
  free(line);
  return 0;
}
