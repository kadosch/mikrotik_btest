CC     = gcc
CFLAGS = -g3 -W -Wall

all: btest
	
btest: btest.c parse_opt.o tcptest.o md5.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

parse_opt.o: parse_opt.c
	$(CC) $(CFLAGS) -c -o $@ $<

tcptest.o: tcptest.c
	$(CC) $(CFLAGS) -c -o $@ $^ -lpthread

md5.o: md5.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f btest *.o *~