#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "parse_opt.h"
#include "tcptest.h"

int main (int argc, char **argv) {
	program_options_t options;
	int rv;

	rv = parse_opt(&argc, argv, &options);
	if ( rv == -1){
		fprintf(stderr, "Error parsing commands\n");
		exit(-1);
	}
	else if (rv == 1)
		exit(0);

	if (check_opt(&options) != 0)
		exit(-1);

	tcptest(options.host, options.port, options.user, options.password, options.direction, options.mtu, options.time);
	
	exit(0);
}
