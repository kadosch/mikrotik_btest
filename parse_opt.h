#ifndef PARSE_OPT_H
#define PARSE_OPT_H

#include "direction.h"

struct program_options {
	char host[256];
	char port[16];
	int time;
	int mtu;
	char direction_string[8];
	direction_t direction;
	char user[32];
	char password[32];
};
typedef struct program_options  program_options_t;


int parse_opt(int *argc, char **argv, program_options_t *options);

int check_opt(program_options_t *options);

#define DEFAULT_MTU 1500
#define DEFAULT_TIME 8.0
#define DEFAULT_DIRECTION RECEIVE
#define DEFAULT_PORT "2000"

#endif
