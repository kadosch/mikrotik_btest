CC = gcc
CFLAGS = -W -Wall
LDFLAGS = -W -Wall
LDLIBS = -lpthread

all: mikrotik_btest
	
mikrotik_btest: main.o parse_opt.o tcptest.o messages.o md5.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

main.o: main.c parse_opt.h tcptest.h
	$(CC) $(CFLAGS) -c -o $@ $<

parse_opt.o: parse_opt.c parse_opt.h direction.h
	$(CC) $(CFLAGS) -c -o $@ $<

tcptest.o: tcptest.c tcptest.h direction.h messages.h md5.h
	$(CC) $(CFLAGS) -c -o $@ $<

messages.o: messages.c messages.h
	$(CC) $(CFLAGS) -c -o $@ $<

md5.o: md5.c md5.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f mikrotik_btest *.o
