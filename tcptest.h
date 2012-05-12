#ifndef TCPTEST_H
#define TCPTEST_H

#include "direction.h"

#define MAX_RETRY 60

int tcptest(char *host, char *port, char *user, char *password, direction_t direction, int mtu, int time);

#endif
