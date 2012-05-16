#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "parse_opt.h"
#include "tcptest.h"
#include "return_codes.h"

int main (int argc, char **argv) {
	program_options_t options;
	int rv;

	rv = parse_opt(&argc, argv, &options);
	if ( rv == RETURN_ERROR){
		fprintf(stderr, "Error parsing commands\n");
		exit(EXIT_FAILURE);
	}
	else if (rv == RETURN_PRINT_HELP)
		exit(EXIT_SUCCESS);

	if (check_opt(&options) != RETURN_OK)
		exit(EXIT_FAILURE);

	tcptest(options.host, options.port, options.user, options.password, options.direction, options.mtu, options.time);
	
	exit(EXIT_SUCCESS);
}
