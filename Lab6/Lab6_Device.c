#include <stdlib.h>
#include <stdio.h>
//Include dispatch after iofunc to prevent warnings
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include "Lab6.h"


/*int io_read(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
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
*/

int  setupPulseAndTimer (long time)
{
    timer_t             timerid;    // timer ID for timer
    struct sigevent     event;      // event to deliver
    struct itimerspec   timer;      // the timer data structure
    int                 coid;       // connection back to ourselves
    int					chid;

    if ((chid = ChannelCreate (0)) == -1) {
    	printf ("Error:  couldn't create channel!\n");
    	exit (EXIT_FAILURE);
    }

    // create a connection back to ourselves
    coid = ConnectAttach (0, 0, chid, 0, 0);
    if (coid == -1) {
        printf ("Error:  couldn't ConnectAttach to self!\n");
        exit (EXIT_FAILURE);
    }

    // set up the kind of event that we want to deliver -- a pulse



    SIGEV_PULSE_INIT (&event, coid, SIGEV_PULSE_PRIO_INHERIT, PULSE_CODE_TIMER, 0);

    // create the timer, binding it to the event
    if (timer_create (CLOCK_REALTIME, &event, &timerid) == -1) {
        printf ("Error: couldn't create a timer.\n");
        exit (EXIT_FAILURE);
    }

    // setup the timer
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_nsec = time;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_nsec = 0;

    // and start it!
    timer_settime (timerid, 0, &timer, NULL);

    return chid;
}

//Prevent an use of the write function
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
	int     status;
	char    *buf;
	struct _pulse pulse;
	int chid;

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
	case '1':
		//single bell,  then pause of 50 msec
		printf("'\a'");
		chid = setupPulseAndTimer(50000000);
		do{
			MsgReceivePulse(chid,&pulse, sizeof(pulse), NULL);
		} while(pulse.code != PULSE_CODE_TIMER);

		ChannelDestroy(chid);
		break;
	case '2':
		//two bells, then pause of 50 msec
		printf("'\a'");
		nanospin(1000);
		printf("'\a'");
		chid = setupPulseAndTimer(50000000);
		do{
			MsgReceivePulse(chid,&pulse, sizeof(pulse), NULL);
		} while(pulse.code != PULSE_CODE_TIMER);

		ChannelDestroy(chid);
		break;
	case '3':
		//two bells, then pause of 200 msec, then another bell, then a gap of 50 msec.
		printf("'\a'");
		nanospin(1000);
		printf("'\a'");
		chid = setupPulseAndTimer(200000000);
		do{
			MsgReceivePulse(chid,&pulse, sizeof(pulse), NULL);
		} while(pulse.code != PULSE_CODE_TIMER);

		ChannelDestroy(chid);
		printf("'\a'");

		chid = setupPulseAndTimer(50000000);
		do{
			MsgReceivePulse(chid,&pulse, sizeof(pulse), NULL);
		} while(pulse.code != PULSE_CODE_TIMER);

		ChannelDestroy(chid);
		break;
	}

	free(buf);

	if (msg->i.nbytes > 0)
		ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

	return (_RESMGR_NPARTS (0));
}

int main(int argc, char *argv[]) {

	dispatch_t* 			dpp; //Step 1: Create the dispatch structure
	resmgr_io_funcs_t 		io_funcs; //The struct to put the I/O functions in
	resmgr_connect_funcs_t 	connect_funcs; //The struct to put the connection functions in
	iofunc_attr_t 			ioattr;
	thread_pool_attr_t   	pool_attr;
	thread_pool_t        	*tpp;
	char					nameBuffer[64];
	int						id;

	//get the device name
	memset(nameBuffer, '\0', sizeof(nameBuffer));
	strcpy(nameBuffer, RESOURCE_ATTACH);
	strcat(nameBuffer, argv[1]);

	/* initialize dispatch interface */
	if((dpp = dispatch_create()) == NULL) {
		fprintf(stderr,
				"%s: Unable to allocate dispatch handle.\n",
				argv[0]);
		return EXIT_FAILURE;
	}

	//initialize the buffer with open
	//strcpy(internalBuffer, "open");

	//Initialise our function tables with the default implementations
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);

   //Replace the default implementations with pointers to our own
	//io_funcs.read = io_read; //Put the pointers to our functions in the correct struct
	io_funcs.write = io_write; //Put the pointers to our functions in the correct struct

	//Use the default mount point attributes for our device
	iofunc_attr_init(&ioattr, S_IFCHR | 0666, NULL, NULL);

	 id = resmgr_attach(
		dpp,            /* dispatch handle        */
		NULL,   /* resource manager attrs */
		nameBuffer,  /* device name            */
		_FTYPE_ANY,     /* open type              */
		0,              /* flags                  */
		&connect_funcs, /* connect routines       */
		&io_funcs,      /* I/O routines           */
		&ioattr);         /* handle                 */

	/* initialize thread pool attributes */
	memset(&pool_attr, 0, sizeof pool_attr);
	pool_attr.handle = dpp;
	pool_attr.context_alloc = dispatch_context_alloc;
	pool_attr.block_func = dispatch_block;
	pool_attr.unblock_func = dispatch_unblock;
	pool_attr.handler_func = dispatch_handler;
	pool_attr.context_free = dispatch_context_free;
	pool_attr.lo_water = 2;
	pool_attr.hi_water = 4;
	pool_attr.increment = 1;
	pool_attr.maximum = 50;

 /* allocate a thread pool handle */
	if((tpp = thread_pool_create(&pool_attr,
								 POOL_FLAG_EXIT_SELF)) == NULL) {
		fprintf(stderr, "%s: Unable to initialize thread pool.\n",
				argv[0]);
		return EXIT_FAILURE;
	}

	thread_pool_start(tpp);
}
