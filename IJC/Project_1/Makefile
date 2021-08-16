# Makefile
# Řešení IJC-DU1, 20.3.2021
# Autor: Andrei Shchapaniak, FIT
# Přeloženo: gcc 10.2.0

LC_ALL = cs_CZ.utf8
COMPILER = gcc
CFLAGS = -g -std=c11 -O2 -pedantic -Wall -Wextra

all: primes primes-i steg-decode steg-encode

run: primes primes-i
	ulimit -s 65000 && time ./primes
	ulimit -s 65000 && time ./primes-i

steg-decode: steg-decode.o error.o ppm.o eratosthenes.o bitset.o
	$(COMPILER) $(CFLAGS) $^ -o $@ -lm
primes: primes.o error.o eratosthenes.o bitset.o
	$(COMPILER) $(CFLAGS) $^ -o $@ -lm
primes-i: primes-i.o error.o eratosthenes-i.o bitset-i.o
	$(COMPILER) $(CFLAGS) -DUSE_INLINE $^ -o $@ -lm
steg-encode: steg-encode.o error.o ppm.o eratosthenes.o bitset.o
	$(COMPILER) $(CFLAGS) $^ -o $@ -lm

error.o: error.c error.h
	$(COMPILER) $(CFLAGS) -c $< -o $@
eratosthenes.o: eratosthenes.c eratosthenes.h
	$(COMPILER) $(CFLAGS) -c $< -o $@
bitset.o: bitset.c bitset.h
	$(COMPILER) $(CFLAGS) -c $< -o $@
primes.o: primes.c
	$(COMPILER) $(CFLAGS) -c $< -o $@
primes-i.o: primes.c
	$(COMPILER) $(CFLAGS) -DUSE_INLINE -c $< -o $@
eratosthenes-i.o: eratosthenes.c eratosthenes.h
	$(COMPILER) $(CFLAGS) -DUSE_INLINE -c $< -o $@
bitset-i.o: bitset.c bitset.h
	$(COMPILER) $(CFLAGS) -DUSE_INLINE -c $< -o $@

ppm.o: ppm.c ppm.h
	$(COMPILER) $(CFLAGS) -c $< -o $@
steg-decode.o: steg-decode.c
	$(COMPILER) $(CFLAGS) -c $< -o $@
steg-encode.o: steg-encode.c
	$(COMPILER) $(CFLAGS) -c $< -o $@

clear:
	rm *.o primes primes-i steg-decode steg-encode
