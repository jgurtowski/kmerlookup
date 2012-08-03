#ifndef KMER_PACKER_H
#define KMER_PACKER_H

typedef struct KmerPacker_struct{
  unsigned char *packed_buffer;
  char *unpacked_buffer;
  unsigned char *scratch_pack_buffer;
  int kmer_size;
  int packed_buffer_size;
}KmerPacker;


KmerPacker *newKmerPacker(int kmersize);
void freeKmerPacker(KmerPacker *packer);
char *unpackKmer(KmerPacker *packer, const unsigned char *packed_kmer);
unsigned char *packKmer(KmerPacker *packer, const char *kmer);

#endif
