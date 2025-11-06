CC     = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11
OBJ    = mymemory.o main.o

all: t2

t2: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

mymemory.o: mymemory.c mymemory.h
	$(CC) $(CFLAGS) -c mymemory.c -o mymemory.o

main.o: main.c mymemory.h
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	-@rm -f $(OBJ) t2
