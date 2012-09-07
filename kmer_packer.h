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

/**
 *Compare packed kmers
 *@return standard comparator int return
 */
int comparePackedKmers(const unsigned char *packed_kmer1, const unsigned char *packed_kmer2, int size);

/**
 *reads packed kmer from stream, stores into an internal buffer and 
 * returns a pointer to internal buffer
 */
unsigned char *readPackedKmerFromStream(KmerPacker *packer, char *stream);
#endif
