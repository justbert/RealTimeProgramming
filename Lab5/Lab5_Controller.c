#include <stdlib.h>
#include <stdio.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/dispatch.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "Lab5.h"

int main(int argc, char *argv[]) {

	char 			buffer[10];
	name_attach_t* 	attach;
	int 			rcvid;
	struct _pulse	msg;
	int				fileDesc;

	fileDesc = open(RESOURCE_NAME, O_RDONLY);

	if ((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL) {
	       return EXIT_FAILURE;
	}

	memset(buffer,'\0', 10);
	read(fileDesc, buffer, 10);

	printf("myDevice status is: %s\n", buffer);

	while(1) {
		rcvid = MsgReceivePulse(attach->chid, &msg, sizeof(msg), NULL);

		if(rcvid == -1) {
			printf("Error on receive.\n");
			break;
		}

		if(rcvid == 0) {//pulse received
			switch(msg.code) {
			case(MY_PULSE):

				printf("Pulse received: %d\n", msg.value.sival_int);
				memset(buffer,'\0', 10);
				read(fileDesc, buffer, 10);
				printf("myDevice status is: %s\n", buffer);
				break;
			}
		}
	}
}
