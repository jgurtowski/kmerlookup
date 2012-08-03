CC=gcc

all: kmerwrite kmerlookup

%.o: %.c
	$(CC) -ggdb -c -o $@ $<

kmerwrite: kmer_write.o kmer_record.o kmer_packer.o
	$(CC) -ggdb kmer_write.o kmer_record.o kmer_packer.o -o kmerwrite

kmerlookup: kmer_lookup.o kmer_record.o kmer_db.o kmer_packer.o
	$(CC) -ggdb kmer_lookup.o kmer_record.o kmer_db.o kmer_packer.o -o kmerlookup

clean:
	rm *.o kmerlookup kmerwrite