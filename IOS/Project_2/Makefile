CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
LDLIBS=-pthread

all: proj2

proj2: proj2.o init_args.o processes.o
	$(CC) $(CFLAGS) $(LDLIBS) proj2.o init_args.o processes.o -o proj2

proj2.o: proj2.c
	$(CC) $(CFLAGS) $(LDLIBS) -c proj2.c -o proj2.o

init_args.o: init_args.c init_args.h
	$(CC) $(CFLAGS) $(LDLIBS) -c init_args.c -o init_args.o

processes.o: processes.c processes.h
	$(CC) $(CFLAGS) $(LDLIBS) -c processes.c -o processes.o

clean:
	rm proj2 proj2.out *.o
