#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "tcptest.h"
#include "direction.h"
#include "messages.h"
#include "md5.h"

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

void init_thread_args(thread_args_t *args, int sockfd, pthread_mutex_t *mutex, int bufsize, direction_t direction){
	args->sockfd = sockfd;
	args->bufsize = bufsize;
	args->direction = direction;
	args->mutex = mutex;
	args->bytes = 0;
	args->time = 0.0;
	args->stop = 1;
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
	while (args->stop != 0 && failed < MAX_RETRY){
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


int recv_msg(int sockfd, unsigned char *buf, int bufsize, unsigned char *msg, int *recvbytes){
	if ((*recvbytes = recv(sockfd, buf, bufsize, 0)) == -1) {
		perror("recv");
		return -1;
	}
	if ((*recvbytes == sizeof(MSG_OK)) && (memcmp(buf, msg, *recvbytes) == 0)){
		return 0;
	}
	return 1;
}

int send_msg(int sockfd, unsigned char *msg, int len){
	int bytes_sent = 0, failed = 0, bytes;
	
	do{
		if ((bytes = send(sockfd, msg, len, 0)) == -1){
			++failed;
		}
		else
			failed = 0;

		if (failed >= MAX_RETRY){
			perror("send");
			return -1;
		}
		bytes_sent += bytes;
	}while(bytes_sent < len);
	return 0;
}

void craft_response(char *user, char *password, unsigned char *challenge, unsigned char *response){
	md5_state_t lvl1_state, lvl2_state;
	md5_byte_t lvl1_digest[16], lvl2_digest[16];
	int len = strlen(password);

	md5_init(&lvl2_state);
	if (len > 0)
		md5_append(&lvl2_state, (const md5_byte_t *) password, len);
	md5_append(&lvl2_state, (const md5_byte_t *) challenge, CHALLENGE_SIZE);
	md5_finish(&lvl2_state, lvl2_digest);

	md5_init(&lvl1_state);
	if (len > 0)
		md5_append(&lvl1_state, (const md5_byte_t *) password, len);
	md5_append(&lvl1_state, (const md5_byte_t *) lvl2_digest, sizeof(lvl2_digest));
	md5_finish(&lvl1_state, lvl1_digest);

	memcpy(response, lvl1_digest, sizeof(lvl1_digest));
	strncpy((char *) response+sizeof(lvl1_digest), user, RESPONSE_SIZE-sizeof(lvl1_digest));
}

int open_socket(char *host, char *port){
	struct addrinfo hints, *servinfo, *p;
	int rv, sockfd;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
			fprintf(stderr, "Invalid host: %s\n", gai_strerror(rv));
			return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("socket");
			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("connect");
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo);

	if (p == NULL) {
		fprintf(stderr, "failed to connect\n");
		close(sockfd);
		return -1;
	}
	return sockfd;
}

int init_test(int sockfd, char *user, char *password,  direction_t direction, int mtu){
	unsigned char *buffer, challenge[CHALLENGE_SIZE], response[RESPONSE_SIZE];
	int numbytes, rv;

	buffer = (unsigned char *) malloc(mtu);

	if (recv_msg(sockfd, buffer, mtu, MSG_OK, &numbytes) != 0){
		close(sockfd);
		free(buffer);
		return -1;
	}

	switch(direction){
		case RECEIVE:
			rv = send_msg(sockfd, MSG_TCP_DOWN, sizeof(MSG_TCP_DOWN));
			break;
		case SEND:
			rv = send_msg(sockfd, MSG_TCP_UP, sizeof(MSG_TCP_UP));
			break;
		case BOTH:
			rv = send_msg(sockfd, MSG_TCP_BOTH, sizeof(MSG_TCP_BOTH));
			break;
	}
	if (rv != 0){
		close(sockfd);
		free(buffer);
		return -1;
	}

	rv = recv_msg(sockfd, buffer, mtu, MSG_OK, &numbytes);
	if (rv == 0){
		free(buffer);
		return 0;
	}
	else if (rv == 1){
		if (numbytes == CHALLENGE_TOTAL_SIZE && memcmp(buffer, CHALLENGE_HEADER, sizeof(CHALLENGE_HEADER)) == 0){
			memcpy(challenge, buffer+sizeof(CHALLENGE_HEADER), CHALLENGE_SIZE);
			craft_response(user, password, challenge, response);
			if (send_msg(sockfd, response, sizeof(response)) == 0){
				if (recv_msg(sockfd, buffer, mtu, MSG_OK, &numbytes) == 0){
					free(buffer);
					return 0;
				}
			}
		}
	}
	fprintf(stderr, "Auth failed\n");
	close(sockfd);
	free(buffer);
	return -1;
}

int tcptest(char *host, char *port, char *user, char *password, direction_t direction, int mtu, int time){
	int sockfd, elapsed_seconds = 0;
	double mbps;
	pthread_t threads[2];
	thread_args_t threads_arg[2];
	pthread_mutex_t mutexes[2];
	pthread_attr_t attr;

	if ((sockfd = open_socket(host, port)) == -1)
		return -1;

	if (init_test(sockfd, user, password, direction, mtu) == -1){
		close(sockfd);
		return -1;
	}

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&mutexes[RECEIVE], NULL);
	pthread_mutex_init(&mutexes[SEND], NULL);

	if (direction == BOTH){
		init_thread_args(&threads_arg[RECEIVE], sockfd, &mutexes[RECEIVE], mtu, RECEIVE);
		pthread_create(&threads[RECEIVE], &attr, tcptest_thread, (void *) &threads_arg[RECEIVE]);
		init_thread_args(&threads_arg[SEND], sockfd, &mutexes[SEND], mtu, SEND);
		pthread_create(&threads[SEND], &attr, tcptest_thread, (void *) &threads_arg[SEND]);
	}
	else{
		init_thread_args(&threads_arg[direction], sockfd, &mutexes[direction], mtu, direction);
		pthread_create(&threads[direction], NULL, tcptest_thread, (void *) &threads_arg[direction]);
	}

	do{
		sleep(1);
		elapsed_seconds += 1;
		if (direction == RECEIVE || direction == BOTH){
			pthread_mutex_lock(&mutexes[RECEIVE]);
			mbps = threads_arg[RECEIVE].mbps;
			pthread_mutex_unlock(&mutexes[RECEIVE]);
			printf("Rx: %7.2f Mb/s", mbps);
		}
		if (direction == BOTH)
			printf("\t");
		if (direction == SEND || direction == BOTH){
			pthread_mutex_lock(&mutexes[SEND]);
			mbps = threads_arg[SEND].mbps;
			pthread_mutex_unlock(&mutexes[SEND]);
			printf("Tx: %7.2f Mb/s", mbps);
		}
		printf("\r");
		fflush(stdout);
	}while (elapsed_seconds <= time);

	pthread_mutex_lock(&mutexes[RECEIVE]);
	threads_arg[RECEIVE].stop = 0;
	pthread_mutex_unlock(&mutexes[RECEIVE]);

	pthread_mutex_lock(&mutexes[SEND]);
	threads_arg[SEND].stop = 0;
	pthread_mutex_unlock(&mutexes[SEND]);

	if (direction == RECEIVE || direction == BOTH){
		pthread_join(threads[RECEIVE], NULL);
		printf("Rx: %7.2f Mb/s", threads_arg[RECEIVE].mbps);
	}
	if (direction == BOTH)
			printf("\t");
	if (direction == SEND || direction == BOTH){
		pthread_join(threads[SEND], NULL);
		printf("Tx: %7.2f Mb/s", threads_arg[SEND].mbps);
	}
	printf("\n");

	pthread_mutex_destroy(&mutexes[RECEIVE]);
	pthread_mutex_destroy(&mutexes[SEND]);
	close(sockfd);
	return 0;
}
