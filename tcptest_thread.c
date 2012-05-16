#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "tcptest_thread.h"

void init_thread_args(thread_args_t *args, int sockfd, pthread_mutex_t *mutex, int bufsize, direction_t direction){
	args->sockfd = sockfd;
	args->bufsize = bufsize;
	args->direction = direction;
	args->mutex = mutex;
	args->bytes = 0;
	args->time = 0.0;
	args->stop = FALSE;
	args->mbps = 0.0;
}


void *tcptest_thread(void *argument){
	thread_args_t *args;
	unsigned char *buffer;
	struct timeval t0 , t1;
	double tv_sec , tv_usec ;
	int bytes = 0, failed = 0;

	args = (thread_args_t *) argument;

	buffer = (unsigned char *) malloc(args->bufsize);
	memset(buffer, 0, args->bufsize);

	pthread_mutex_lock (args->mutex);
	while (!args->stop && failed < MAX_RETRY){
		pthread_mutex_unlock (args->mutex);
		if (args->direction == RECEIVE){
			gettimeofday(&t0 , NULL);
			bytes = recv(args->sockfd, buffer, args->bufsize, 0);
			gettimeofday(&t1 , NULL);
		}
		else{
			gettimeofday(&t0 , NULL);
			bytes = send(args->sockfd, buffer, args->bufsize, 0);
			gettimeofday(&t1 , NULL);

		}

		if (bytes == -1){
			bytes = 0;
			++failed;
		}
		else
			failed = 0;

		tv_sec = (double) (t1.tv_sec - t0.tv_sec);
		tv_usec = (double) (t1.tv_usec - t0.tv_usec);
		pthread_mutex_lock (args->mutex);
		args->time += tv_sec + tv_usec * 1.0e-6;
		args->bytes += bytes;
		args->mbps = ((args->bytes * 8) / 1048576) / args->time;
	}
	free(buffer);
	return NULL;
}
