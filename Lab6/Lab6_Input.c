#include <stdlib.h>
#include <stdio.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/dispatch.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "Lab6.h"

static const char* LOOP_KEYWORD = "-loop";

int main(int argc, char *argv[]) {

	int		beatDesc;
	int 	fileDesc;
	long 	speed;
	char 	temp[50];

	memset(temp, '\0', sizeof(temp));
	strcpy(temp, RESOURCE_ATTACH);

	if(!strcmp(argv[1], LOOP_KEYWORD)) {

		//non-interactive
		//–loop  filename   devicename   speed

		strcat(temp, argv[3]);

		beatDesc = open(temp, O_WRONLY);
		if(beatDesc == -1) {
			printf("Error: Could not open device.\n");
			return EXIT_FAILURE;
		}

		fileDesc = open(argv[2], O_RDONLY);
		if(fileDesc == -1) {
			printf("Error: Could not open file.\n");
			return EXIT_FAILURE;
		}

		//get the amount of milliseconds and convert to nanoseconds
		speed = atol(argv[4]) * 1000000;
		if(speed < 0 || speed > 5000000000) {
			printf("Delay is not within the bounds. Please enter a value between 0 and 5000.\n");
			return EXIT_FAILURE;
		}

	} else {
		//Interactive mode
		//program devicename speed

		strcat(temp, argv[1]);

		//Open the beat device
		beatDesc = open(temp, O_WRONLY);
		if(beatDesc == -1) {
			printf("Error: Could not open device.\n");
			return EXIT_FAILURE;
		}

		//get the amount of milliseconds and convert to nanoseconds
		speed = atol(argv[2]) * 1000000;
		if(speed < 0 || speed > 5000000000) {
			printf("Delay is not within the bounds. Please enter a value between 0 and 5000.\n");
			return EXIT_FAILURE;
		}


	}
}
