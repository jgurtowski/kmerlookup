#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kmer_packer.h"

static unsigned char getBits(char letter){
  if('A' == letter){
    return 0;
  }else if('C' == letter){
    return 1;
  }else if('G' == letter){
    return 2;
  }else if('T' == letter){
    return 4;
  } 
  //N is 3 to keep alphabetical while sorting
  return 3;
}

static char getLetter(unsigned char bits){
  if( 0  == bits){
    return 'A';
  }else if( 1 == bits){
    return 'C';
  }else if( 2 == bits){
    return 'G';
  }else if( 4 == bits){
    return 'T';
  }
  return 'N'; 
}


/**
 *Adds least significant bits from src to the most significant bits in dest
 *Packs items left to right
 * if: 
 *  src = 0000111
 *  dest = 11010110 01010111
 *  num_bits = 3
 *  dest will be:
 *  11111010 11001010
 */
static void addBitsToByteArray(char src, unsigned char *dest, int num_bits, int dest_length){
  assert(num_bits > 0);//only less than a byte for now
  assert(num_bits < 8);

  int i;
  for(i=dest_length-1;i>=1;--i){
    dest[i] >>= num_bits;
    dest[i] |= (dest[i-1] & (255 >> (8-num_bits))) << (8-num_bits);
  }
  dest[i] >>= num_bits;
  dest[i] |= (src << (8-num_bits));
}

/**
 * Removes bits from src array's most signficant bits
 * Destructive, actually changes src
 **/
static char popBitsFromByteArray(char *src, int num_bits, int src_length){
  assert(num_bits > 0);
  assert(num_bits < 8);

  unsigned char mask = 255 << (8-num_bits);
  unsigned char dest;
  int i;
  for(i=0; i<src_length-1; ++i){
    if(i ==0){
      dest = (src[i] & mask) >> (8-num_bits);
    }
    src[i] <<= num_bits;
    src[i] |= (src[i+1] & mask) >> (8-num_bits);
  }
  src[i] <<= num_bits;
  return dest;
}


unsigned char *packKmer(KmerPacker *packer, const char *kmer){
  int i;
  for(i=packer->kmer_size -1; i>=0; --i){
    addBitsToByteArray(getBits(kmer[i]),packer->packed_buffer,3, packer->packed_buffer_size);
  }
  return packer->packed_buffer;
}

char *unpackKmer(KmerPacker *packer, const unsigned char *packed_kmer){
  int i;
  memcpy(packer->scratch_pack_buffer, packer->packed_buffer, packer->packed_buffer_size);
  unsigned char coded_letter;
  for(i=0; i<packer->kmer_size; ++i){
    coded_letter = popBitsFromByteArray(packer->scratch_pack_buffer, 3, packer->packed_buffer_size);
    packer->unpacked_buffer[i] = getLetter(coded_letter);
  }
  return packer->unpacked_buffer;
}

KmerPacker *newKmerPacker(int kmersize){
  KmerPacker *packer = malloc( sizeof(KmerPacker));
  packer -> kmer_size = kmersize;
  int packed_bytes = kmersize * 3 / 8;
  if( (kmersize * 3) % 8 > 0 )
    ++packed_bytes;
  packer -> packed_buffer_size = packed_bytes;
  packer -> packed_buffer = malloc(packed_bytes);
  packer -> scratch_pack_buffer = malloc(packed_bytes);
  packer -> unpacked_buffer = malloc(kmersize + 1);
  packer -> unpacked_buffer[kmersize] = '\0';
  return packer;
}

void freeKmerPacker(KmerPacker *packer){
  free(packer->packed_buffer);
  free(packer->unpacked_buffer);
  free(packer->scratch_pack_buffer);
  free(packer);
}


