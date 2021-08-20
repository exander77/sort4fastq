CC = gcc
CFLAGS = -O3

MAIN = sort4fastq
SRCS = sort4fastq.c
OBJS = $(SRCS:.c=.o)

$(MAIN): $(OBJS)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.PHONY: clean

clean:
		rm -f *.o $(MAIN)
