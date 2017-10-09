#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>

#include "mystruct.h"

void print_error(char* string);
void send_display_message(InputMsg* msg);

static pid_t controllerPID; //the process ID of the controller server
static char buffer[10]; //buffer for the input values;
static InputMsg message;
static int coid; //connection ID to the controller server

int main(int argc, char *argv[]) {

	//First parameter passed to this server should be the PID of the controller.
	controllerPID = atoi(argv[1]);

	coid = ConnectAttach (ND_LOCAL_NODE, controllerPID, 1, _NTO_SIDE_CHANNEL, 0);
	if (coid == -1) {
		fprintf (stderr, "Inputs: Couldn't connect to the Controller Server.\n");
		perror (NULL);
		exit (EXIT_FAILURE);
	}

	 printf("Inputs: PID is %d\n", getpid());

	 while(1) {

		 printf("Inputs: Enter the event type: ");
		 scanf("%s", buffer);

		 //Take the input and differentiate into the correct message
		 switch(buffer[0]) {
		 //Possible paths: gll, glo, grl, gro
		 case 'g':
			 switch(buffer[1]) {
			 case 'l':
				 switch(buffer[2]) {
				 case 'l':
					 message.input = GUARD_LEFT_LOCK;
					 send_display_message(&message);
					 break;
				 case 'o':
					 message.input = GUARD_LEFT_UNLOCK;
					 send_display_message(&message);
					 break;
				 default:
					 print_error(buffer);
				 }
				 break;
			 case 'r':
				 switch(buffer[2]) {
				 case 'l':
					 message.input = GUARD_RIGHT_LOCK;
					 send_display_message(&message);
					 break;
				 case 'o':
					 message.input = GUARD_RIGHT_UNLOCK;
					 send_display_message(&message);
					 break;
				 default:
					 print_error(buffer);
				 }
				 break;
			 default:
				 print_error(buffer);
			 }
			 break;
		 //Possible paths: lc, lo, ls
		 case 'l':
			 switch(buffer[1]) {
			 case 'c':
				 message.input = DOOR_CLOSE_LEFT;
				 send_display_message(&message);
				 break;
			 case 'o':
				 message.input = DOOR_OPEN_LEFT;
				 send_display_message(&message);
				 break;
			 case's':
				 message.input = LEFT_SCAN;

				 printf("Inputs: Enter the Employee ID: ");
				 scanf("%s", buffer);

				 message.weight = atoi(buffer);

				 send_display_message(&message);
				 break;
			 default:
				 print_error(buffer);
			 }
			 break;
		 //Possible paths: rc, ro, rs
		 case 'r':
			 switch(buffer[1]) {
			 case 'c':
				 message.input = DOOR_CLOSE_RIGHT;
				 send_display_message(&message);
				 break;
			 case 'o':
				 message.input = DOOR_OPEN_RIGHT;
				 send_display_message(&message);
				 break;
			 case's':
				 message.input = RIGHT_SCAN;

				 printf("Inputs: Enter the Employee ID: ");
				 scanf("%s", buffer);

				 message.weight = atoi(buffer);

				 send_display_message(&message);
				 break;
			 default:
				 print_error(buffer);
			 }
			 break;
		 //Possible paths: ws
		 case 'w':
			 if(buffer[1] == 's') {
				 message.input = WEIGH_SCALE;

				 printf("Inputs: Enter the weight of the person on the scale: ");
				 scanf("%s", buffer);

				 message.weight = atoi(buffer);

				 send_display_message(&message);

			 } else {
				print_error(buffer);
			 }
			 break;
		 default:
			 print_error(buffer);
		 }

	 }
}

void print_error(char* string) {
	printf("Inputs: %s is an unrecognised command.\n", string);
}

void send_display_message(InputMsg* msg) {

	if(MsgSend(coid, msg, sizeof(InputMsg), NULL, 0) == -1) {
		fprintf (stderr, "Inputs: Error during MsgSend\n");
		perror (NULL);
		exit (EXIT_FAILURE);
	}
}
