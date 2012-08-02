

all: kmerwrite kmerlookup


kmerwrite: kmer_write.o kmer_record.o
	cc kmer_write.o kmer_record.o -o kmerwrite

kmerlookup: kmer_lookup.o kmer_record.o kmer_db.o
	cc kmer_lookup.o kmer_record.o kmer_db.o -o kmerlookup

clean:
	rm *.o kmerlookup kmerwrite