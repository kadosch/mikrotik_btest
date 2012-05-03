#ifndef TCPTEST_H
#define TCPTEST_H

enum direction {
	RECEIVE = 0,
	SEND = 1,
	BOTH = 2
};
typedef enum direction direction_t;

int tcptest(char *host, char *port, char *user, char *password, direction_t direction, int mtu, int time);

#endif
