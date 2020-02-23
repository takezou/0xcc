CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

0xcc: $(OBJS)
	$(CC) -o 0xcc $(OBJS) $(CFLAGS)

$(OBJS): 0xcc.h

test: 0xcc
	./test.sh

clean:
	rm -f 0xcc *.o *~ tmp*

.PHONY: test clean
