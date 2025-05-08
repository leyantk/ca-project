CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OBJS = main.c processor.c pipeline.c

all: sim

sim: $(OBJS)
	$(CC) $(CFLAGS) -o sim $(OBJS)

clean:
	rm -f sim *.o
