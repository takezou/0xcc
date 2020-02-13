CFLAGS=-std=c11 -g -static

0xcc: 0xcc.c

test: 0xcc
	./test.sh

clean:
	rm -f 0xcc *.o *~ tmp*

.PHONY: test clean
