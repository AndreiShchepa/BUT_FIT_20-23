# Makefile
# Řešení IJC-DU2, Makefile, 19.04.2021
# Autor: Andrei Shchapaniak, FIT

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra
LIBFILES=htab_for_each.o htab_find.o htab_erase.o htab_bucket_count.o htab_clear.o htab_free.o htab_hash_function.o htab_init.o htab_item_init.o htab_lookup_add.o htab_move.o htab_size.o

all: tail wordcount wordcount-dynamic

###################### tail ######################
tail: tail.o
	$(CC) $^ -o $@

tail.o: tail.c
	$(CC) $(CFLAGS) -c $< -o $@
##################################################

###################### wordcount ######################
wordcount: io.o wordcount.o libhtab.a
	$(CC) -o wordcount -static io.o wordcount.o -L. -lhtab

wordcount-dynamic: io.o wordcount.o libhtab.so
	$(CC) -o wordcount-dynamic io.o wordcount.o -L. -lhtab
#######################################################

###################### libraries ######################
libhtab.a: $(LIBFILES)
	ar rcs libhtab.a $^

libhtab.so: $(LIBFILES)
	$(CC) $(CFLAGS) -shared -fPIC $^ -o libhtab.so
#######################################################

###################### other ######################
io.o: io.c
	$(CC) $(CFLAGS) -c $< -o $@

htab_bucket_count.o: htab_bucket_count.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

htab_clear.o: htab_clear.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

htab_free.o: htab_free.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

htab_hash_function.o: htab_hash_function.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

htab_init.o: htab_init.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

htab_item_init.o: htab_item_init.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

htab_lookup_add.o: htab_lookup_add.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

htab_move.o: htab_move.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

htab_size.o: htab_size.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

htab_erase.o: htab_erase.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

htab_find.o: htab_find.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

htab_for_each.o: htab_for_each.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

wordcount.o: wordcount.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@
###################################################

###################### run ######################
run:
	./wordcount
	LD_LIBRARY_PATH="." ./wordcount-dynamic
#################################################

###################### clean ######################
clean:
	rm -f *.o tail wordcount wordcount-dynamic libhtab.a libhtab.so
###################################################
