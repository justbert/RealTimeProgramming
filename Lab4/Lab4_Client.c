#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <limits.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include "mystruct.h"

int main(int argc, char *argv[]) {

	int index;
	opMsg message;
	respMsg response;
	pid_t serverPID;
	int coid;
	char breakFlag = 1;

	for(index = 1; index < argc && breakFlag; ++index) {
		if(argv[index] == NULL)
			break;
		switch (index) {
		case 1:
			serverPID = atoi(argv[index]);
			break;
		case 2:
			if(atol(argv[index]) <= INT_MAX) {
				message.x = atoi(argv[index]);
			} else {
				printf("ERROR: Entered value is too large.\n");
				breakFlag = 0;
			}
			break;
		case 3:
			switch(argv[index][0]) {
			case '+':
			case '-':
			case 'x':
			case 'X':
			case '*':
			case '/':
				message.operator = argv[index][0];
				break;
			default:
				printf("ERROR: Unrecognised operator.\n");
				breakFlag = 0;
			}
			break;
		case 4:
			if(atol(argv[index]) <= INT_MAX) {
				message.y = atoi(argv[index]);
			} else {
				printf("ERROR: Entered value is too large.\n");
				breakFlag = 0;
			}
			break;
		}
	}

	//If the parameters were correct, send the message.
	if(breakFlag) {
		//connect to the channel
		coid = ConnectAttach(ND_LOCAL_NODE, serverPID, 1, _NTO_SIDE_CHANNEL, 0);
		if(coid == -1) {
			fprintf (stderr, "Couldn't ConnectAttach\n");
			perror (NULL);
			exit (EXIT_FAILURE);
		}

		//Send the message
		if(MsgSend(coid, &message, sizeof(opMsg), &response, sizeof(respMsg)) == -1){
			fprintf (stderr, "Error during MsgSend\n");
			perror (NULL);
			exit (EXIT_FAILURE);
		}

		switch(response.error) {
		case MSG_ERROR:
			printf(response.msg);
			break;
		case MSG_WARNING:
			printf(response.msg);
		case MSG_OK:
			printf("The server has calculated the answer of %d %c %d as %f.\n", message.x, message.operator, message.y, response.answer);
		}
	} else {
		printf("Parameters weren't correct, no message sent.\n");
	}

	return 0;
}
