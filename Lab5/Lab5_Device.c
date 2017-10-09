#include <stdlib.h>
#include <stdio.h>
//Include dispatch after iofunc to prevent warnings
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include "Lab5.h"

/*
#define STATUS 0
#define PULSE 1
#define OPEN 2
#define CLOSED 3
*/
#define TEMP_SIZE 10
#define RESET_TEMP memset(temp, '\0', 10)

static char internalBuffer[10];
//static char* KEYWORDS = {"status", "pulse", "open", "closed"};
static const char* STATUS_KEYWORD = "status";
static const char* PULSE_KEYWORD = "pulse";
static const char* OPEN_KEYWORD = "open";
static const char* CLOSED_KEYWORD = "closed";

int io_read(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{

	int nb;

	nb = strlen(internalBuffer);

	//test to see if we have already sent the whole message.
	if (ocb->offset == nb)
		return 0;

	//We will return which ever is smaller the size of our data or the size of the buffer
	nb = min(nb, msg->i.nbytes);

	//Set the number of bytes we will return
	_IO_SET_READ_NBYTES(ctp, nb);

	//Copy data into reply buffer.
	SETIOV(ctp->iov, internalBuffer, nb);

	//update offset into our data used to determine start position for next read.
	ocb->offset += nb;

	//If we are going to send any bytes update the access time for this resource.
	if (nb > 0)
		ocb->attr->flags |= IOFUNC_ATTR_ATIME;

	return(_RESMGR_NPARTS(1));
}

//Prevent an use of the write function
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
	int     status;
	int 	index;
	int 	offset;
	int 	pulse_number = 0;
	int 	id;
	char    *buf;
	char 	temp[TEMP_SIZE];

	RESET_TEMP;

	if ((status = iofunc_write_verify(ctp, msg, ocb, NULL)) != EOK)
		return (status);

	if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
		return(ENOSYS);

	/* set up the number of bytes (returned by client's write()) */

	_IO_SET_WRITE_NBYTES (ctp, msg->i.nbytes);

	buf = (char *) malloc(msg->i.nbytes + 1);//+1 so that we can add a NULL terminator
	if (buf == NULL)
		return(ENOMEM);

	/*
	 *  Reread the data from the sender's message buffer.
	 *  We're not assuming that all of the data fit into the
	 *  resource manager library's receive buffer.
	 */

	resmgr_msgread(ctp, buf, msg->i.nbytes, sizeof(msg->i));
	buf [msg->i.nbytes] = '\0'; /* just in case the text is not NULL terminated */

	//Do stuff here

	switch(buf[0]) {
	case 'p':
		//Copy the potential keyword into a temporary buffer
		for(index = 0; index < strlen(PULSE_KEYWORD); ++index) {
			temp[index] = buf[index];
		}

		//Pulse keyword received
		if(!strcmp(temp, PULSE_KEYWORD)) {
			//skip over the space
			++index;
			offset = index;

			RESET_TEMP;

			for(index = 0; buf[index+offset]>= '0' && buf[index+offset] <= '9' && index < TEMP_SIZE-1; ++index) {
				temp[index] = buf[index+offset];
			}

			//Add a NULL terminator
			temp[index+1] = '\0';

			pulse_number = atoi(temp);

			if(pulse_number > 0 && pulse_number <= 10) {

				id = name_open(ATTACH_POINT, 0);
				if(id != -1) {
					//connect achieved
					if(MsgSendPulse(id, -1, MY_PULSE, pulse_number) == -1) {
						printf("Error sending message.\n");
					}
				}
			}
		}
		break;
	case 's':

		for(index = 0; index < strlen(STATUS_KEYWORD); ++index) {
			temp[index] = buf[index];
		}

		//Status keyword received
		if(!strcmp(temp, STATUS_KEYWORD)) {
			//check for the space as the next parameter
			if(buf[index] != ' ') {
				break;
			}

			++index; //advance to the location of the next possible parameter
			offset = index;

			switch(buf[offset]) {
			case 'o':
				RESET_TEMP;

				//Copy the potential keyword into a temporary buffer
				for(index = 0; index < strlen(OPEN_KEYWORD); ++index) {
					temp[index] = buf[offset+index];
				}

				//Open keyword received
				if(!strcmp(temp, OPEN_KEYWORD)) {
					strcpy(internalBuffer, OPEN_KEYWORD);
				}
				break;
			case 'c':

				RESET_TEMP;

				//Copy the potential keyword into a temporary buffer
				for(index = 0; index < strlen(CLOSED_KEYWORD); ++index) {
					temp[index] = buf[offset+index];
				}

				//Open keyword received
				if(!strcmp(temp, CLOSED_KEYWORD)) {
					strcpy(internalBuffer, CLOSED_KEYWORD);
				}
				break;
			}

		}
		break;
	}

	free(buf);

	if (msg->i.nbytes > 0)
		ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

	return (_RESMGR_NPARTS (0));
}

int main(int argc, char *argv[]) {

	dispatch_t* dpp; //Step 1: Create the dispatch structure
	resmgr_io_funcs_t io_funcs; //The struct to put the I/O functions in
	resmgr_connect_funcs_t connect_funcs; //The struct to put the connection functions in
	iofunc_attr_t ioattr;
	dispatch_context_t   *ctp;
	int id;

	/* initialize dispatch interface */
	if((dpp = dispatch_create()) == NULL) {
		fprintf(stderr,
				"%s: Unable to allocate dispatch handle.\n",
				argv[0]);
		return EXIT_FAILURE;
	}

	//initialize the buffer with open
	strcpy(internalBuffer, "open");

	//Initialise our function tables with the default implementations
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);

   //Replace the default implementations with pointers to our own
	io_funcs.read = io_read; //Put the pointers to our functions in the correct struct
	io_funcs.write = io_write; //Put the pointers to our functions in the correct struct

	//Use the default mount point attributes for our device
	iofunc_attr_init(&ioattr, S_IFCHR | 0666, NULL, NULL);

	 id = resmgr_attach(
		dpp,            /* dispatch handle        */
		NULL,   /* resource manager attrs */
		RESOURCE_NAME,  /* device name            */
		_FTYPE_ANY,     /* open type              */
		0,              /* flags                  */
		&connect_funcs, /* connect routines       */
		&io_funcs,      /* I/O routines           */
		&ioattr);         /* handle                 */

   //Create the dispatch context. Contains all the info needed for the message receive loop.
	ctp = dispatch_context_alloc(dpp);

   //Message receive loop
	while(1) {
		if((ctp = dispatch_block(ctp)) == NULL) {
			fprintf(stderr, "block error\n");
			return EXIT_FAILURE;
		}
		dispatch_handler(ctp);
	}
	return EXIT_SUCCESS;
}
