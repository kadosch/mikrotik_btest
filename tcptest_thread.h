#ifndef TCPTEST_THREAD_H
#define TCPTEST_THREAD_H

#include "direction.h"

#define TRUE 1
#define FALSE 0

#define MAX_RETRY 60

struct thread_args {
	int sockfd;
	pthread_mutex_t *mutex;
	unsigned long bytes;
	double time;
	int bufsize;
	direction_t direction;
	int stop;
	double mbps;
};
typedef struct thread_args thread_args_t;

void init_thread_args(thread_args_t *args, int sockfd, pthread_mutex_t *mutex, int bufsize, direction_t direction);

void *tcptest_thread(void *argument);

#endif
