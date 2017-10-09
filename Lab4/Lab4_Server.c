#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/neutrino.h>
#include "mystruct.h"

int main(int argc, char *argv[]) {

	int rcvid;
	int chid;
	long answerTest;
	double answerFlowTest;
	opMsg message;
	respMsg response;

	static const char* LOP = "WARNING: Loss of precision.\n";
	static const char* OVERFLOW = "WARNING: Possible overflow has occurred.\n";
	static const char* UNDERFLOW = "WARNING: Possible underflow has occurred.\n";
	static const char* UNRECOGNIZED = "Error: Unrecognised operand.\n";
	static const char* DIVBY0 = "Error: Division by zero.\n";


	//create a channel
	chid = ChannelCreate(0);
	if (chid == -1)
	{
		perror("Failed to create the channel.\n");
		exit (EXIT_FAILURE);
	}

	printf("Server PID is %d\n", getpid());

	while(1) {

		//get the message
		rcvid = MsgReceive(chid, &message, sizeof(opMsg), NULL);

		response.error = MSG_OK;

		switch(message.operator) {
		case '+':
			response.answer = (float) message.x + (float) message.y;
			answerTest = message.x + message.y;

			if(response.answer != answerTest) {
				response.error = MSG_WARNING;
				strcpy(response.msg, LOP);
			}
			break;
		case '-':
			response.answer = (float) message.x - (float) message.y;
			answerTest = message.x - message.y;

			if(response.answer != answerTest) {
				response.error = MSG_WARNING;
				strcpy(response.msg, LOP);
			}
			break;
		case 'x':
		case 'X':
		case '*':
			response.answer = (float) message.x * (float) message.y;
			answerFlowTest = message.x * message.y;

			if(response.answer != answerFlowTest || (message.x > 0 && message.y > 0 && response.answer < 0)) {
				response.error = MSG_WARNING;
				strcpy(response.msg, OVERFLOW);
			}
			break;
		case '/':
			//Check for division by 0
			if(message.y == 0) {
				response.error = MSG_ERROR;
				strcpy(response.msg, DIVBY0);
			} else {
				response.answer = (float) message.x / (float) message.y;
				answerFlowTest = (double) message.x / (double) message.y;

				if(response.answer != answerFlowTest) {
					response.error = MSG_WARNING;
					strcpy(response.msg, UNDERFLOW);
				}
			}
			break;
		default:
			response.error = MSG_ERROR;
			strcpy(response.msg, UNRECOGNIZED);
		}

		//send response
		MsgReply(rcvid, EOK, &response, sizeof(respMsg));
	}

	ChannelDestroy(chid);
}
