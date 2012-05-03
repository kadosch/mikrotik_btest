#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "tcptest.h"
#include "parse_opt.h"

void trunccopy(char *dst, int dstmaxlen, char *orig, int origlen){
	if (origlen < dstmaxlen)
		strcpy(dst, orig);
	else{
		strncpy(dst, orig, dstmaxlen-1);
		dst[dstmaxlen-1] = '\0';
	}	
}

void set_default_opt(program_options_t *options){
	memset(options, 0, sizeof(*options));
	options->time = 8.0;
	options->mtu = 1500;
	options->direction = RECEIVE;
	strcpy(options->port, "2000");
}

int parse_opt(int *argc, char **argv, program_options_t *options){
	int c, optarglen, option_index;

	static struct option long_options[] = {
			{"port",     required_argument, 0, 'p'},
	        {"time",  required_argument, 0, 't'},
	        {"mtu",  required_argument, 0, 'm'},
	        {"direction",  required_argument, 0, 'd'},
	        {"user",    required_argument, 0, 'u'},
	        {"password",    required_argument, 0, 'a'},
	        {0, 0, 0, 0}};

	set_default_opt(options);

	while ((c = getopt_long(*argc, argv, "p:t:m:d:u:a:", long_options, &option_index)) != -1) {
		if (optarg)
			optarglen = strlen(optarg);
		switch(c){
			case 'p':
				trunccopy(options->port, sizeof(options->port), optarg, optarglen);
				break;
			case 't':
				options->time = atoi(optarg);
				break;
			case 'm':
				options->mtu = atoi(optarg);
				break;
			case 'd':
				trunccopy(options->direction_string, sizeof(options->direction_string), optarg, optarglen);
				break;
			case 'u':
				trunccopy(options->user, sizeof(options->user), optarg, optarglen);
				break;
			case 'a':
				trunccopy(options->password, sizeof(options->password), optarg, optarglen);
				break;
			default:
				return -1;
		}
	}
	if (*argc - optind == 1)
		trunccopy(options->host, sizeof(options->host), argv[optind], strlen(argv[optind]));	
	else
		return -1;
	return 0;
}

int check_opt(program_options_t *options){
	int port;

	port = atoi(options->port);
	if (port < 1 || port >= 65536){
		fprintf(stderr, "Port %s is not valid\n", options->port);
		return -1;
	}

	if (strlen(options->direction_string) > 0){
		if (strcmp(options->direction_string, "receive") == 0){
			options->direction = RECEIVE;
		}
		else if (strcmp(options->direction_string, "send") == 0){
			options->direction = SEND;
		}
		else if (strcmp(options->direction_string, "both") == 0){
			options->direction = BOTH;
		}
		else{
			fprintf(stderr, "Direction %s is not valid\n", options->direction_string);
			return -1;
		}
	}

	if (options->mtu == 0){
		fprintf(stderr, "MTU is not valid\n");
		return -1;
	}

	if (options->time == 0.0){
		fprintf(stderr, "Time is not valid\n");
		return -1;
	}
	
	return 0;
}
