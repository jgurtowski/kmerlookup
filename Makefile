CC=gcc
CFLAGS=-Wall -O3

all: kmerwrite kmerlookup kmerlookup_t kmercorrect_t

%.o: %.c
	$(CC) $(CFLAGS) -ggdb -c -o $@ $<

kmerwrite: kmer_write.o kmer_record.o kmer_packer.o
	$(CC) $(CFLAGS) -ggdb kmer_write.o kmer_record.o kmer_packer.o -o kmerwrite

kmerlookup: kmer_lookup.o kmer_record.o kmer_db.o kmer_packer.o
	$(CC) $(CFLAGS) -ggdb kmer_lookup.o kmer_record.o kmer_db.o kmer_packer.o -o kmerlookup

kmerlookup_t: kmer_lookup_t.o read_queue.o kmer_db.o kmer_packer.o kmer_record.o reverse_complement.o kmer_count_container.o
	$(CC) $(CFLAGS) -ggdb -lpthread kmer_lookup_t.o read_queue.o kmer_db.o kmer_packer.o kmer_record.o reverse_complement.o kmer_count_container.o -o kmerlookup_t

kmercorrect_t: kmer_correct_t.o read_queue.o kmer_db.o kmer_packer.o kmer_record.o reverse_complement.o kmer_count_container.o count_segmenter.o
	$(CC) $(CFLAGS) -ggdb -lpthread kmer_correct_t.o read_queue.o kmer_db.o kmer_packer.o kmer_record.o reverse_complement.o kmer_count_container.o count_segmenter.o -o kmercorrect_t

clean:
	rm *.o kmerlookup kmerwrite kmerlookup_t