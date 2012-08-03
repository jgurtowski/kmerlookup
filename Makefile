CC=gcc

all: kmerwrite kmerlookup

kmerwrite: kmer_write.o kmer_record.o
	$(CC) -g kmer_write.o kmer_record.o -o kmerwrite

kmerlookup: kmer_lookup.o kmer_record.o kmer_db.o
	$(CC) -g kmer_lookup.o kmer_record.o kmer_db.o -o kmerlookup

clean:
	rm *.o kmerlookup kmerwrite