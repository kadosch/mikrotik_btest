CC = gcc
CFLAGS = -W -Wall -s -Os -ffunction-sections -fdata-sections
LDFLAGS = -s -Wl,--gc-sections
LDLIBS = -lpthread

all: mikrotik_btest
	
mikrotik_btest: main.o parse_opt.o tcptest_thread.o tcptest.o messages.o md5.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

main.o: main.c parse_opt.h tcptest.h return_codes.h
	$(CC) $(CFLAGS) -c -o $@ $<

parse_opt.o: parse_opt.c parse_opt.h direction.h return_codes.h
	$(CC) $(CFLAGS) -c -o $@ $<

tcptest.o: tcptest.c tcptest.h tcptest_thread.h direction.h messages.h md5.h return_codes.h
	$(CC) $(CFLAGS) -c -o $@ $<

tcptest_thread.o: tcptest_thread.c tcptest_thread.h direction.h
	$(CC) $(CFLAGS) -c -o $@ $<

messages.o: messages.c messages.h
	$(CC) $(CFLAGS) -c -o $@ $<

md5.o: md5.c md5.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f mikrotik_btest *.o
