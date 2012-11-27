/*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TCPTEST_THREAD_H
#define TCPTEST_THREAD_H

#include <stdint.h>

#include "direction.h"

#define MAX_RETRY 60

struct thread_args {
	int32_t sockfd;
	pthread_mutex_t *mutex;
	uint32_t bytes;
	uint16_t bufsize;
	direction_t direction;
	uint8_t stop;
};
typedef struct thread_args thread_args_t;

void init_thread_args(thread_args_t *args, int32_t sockfd, pthread_mutex_t *mutex, uint16_t bufsize, direction_t direction);

void *tcptest_thread(void *argument);

#endif
