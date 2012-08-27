CC=gcc
CFLAGS=-Wall

all: kmerwrite kmerlookup kmerlookup_t

%.o: %.c
	$(CC) $(CFLAGS) -ggdb -c -o $@ $<

kmerwrite: kmer_write.o kmer_record.o kmer_packer.o
	$(CC) -ggdb kmer_write.o kmer_record.o kmer_packer.o -o kmerwrite

kmerlookup: kmer_lookup.o kmer_record.o kmer_db.o kmer_packer.o
	$(CC) -ggdb kmer_lookup.o kmer_record.o kmer_db.o kmer_packer.o -o kmerlookup

kmerlookup_t: kmer_lookup_t.o read_queue.o kmer_db.o kmer_packer.o kmer_record.o reverse_complement.o
	$(CC) -ggdb -lpthread kmer_lookup_t.o read_queue.o kmer_db.o kmer_packer.o kmer_record.o reverse_complement.o -o kmerlookup_t

clean:
	rm *.o kmerlookup kmerwrite kmerlookup_t