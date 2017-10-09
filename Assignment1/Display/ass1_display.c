#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <process.h>

#include "mystruct.h"

static const char * left_door_open = "Display: Left door has been opened.\n";
static const char * right_door_open = "Display: Right door has been opened.\n";
static const char * left_door_closed = "Display: Left door has been closed.\n";
static const char * right_door_closed = "Display: Right door has been closed.\n";
static const char * scale_weighed = "Display: The scale has been weighed. Weight = ";
static const char * guard_left_locked = "Display: Left door has been locked by the guard.\n";
static const char * guard_left_unlocked = "Display: Left door has been unlocked by the guard.\n";
static const char * guard_right_locked = "Display: Right door has been locked by the guard.\n";
static const char * guard_right_unlocked = "Display: Right door has been unlocked by the guard.\n";
static const char * left_scan = "Display: Left scanner has scanned EmployeeID = ";
static const char * right_scan = "Display: Right scanner has scanned EmployeeID = ";
static const char * error_dispMsg = "Display: ERROR: Unrecognised command.\n";

int main(int argc, char *argv[]) {

	int chid; //the displays server channel ID
	int rcvid; //the id of the process sending the dispMsg.
	DispMsg dispMsg;


	chid = ChannelCreate(0);
	if (chid == -1)
	{
		perror("Inputs: Failed to create the channel.");
		return (EXIT_FAILURE);
	}

	while(1) {
		//Receive the dispMsgs from the controller
		rcvid = MsgReceive(chid, &dispMsg, sizeof(dispMsg), NULL);

		switch(dispMsg.action) {
		case leftOpened:
			printf("%s", left_door_open);
			break;
		case rightOpened:
			printf("%s", right_door_open);
			break;
		case leftClosed:
			printf("%s", left_door_closed);
			break;
		case rightClosed:
			printf("%s", right_door_closed);
			break;
		case scaleWeighed:
			printf("%s%d\n", scale_weighed, dispMsg.weight);
			break;
		case leftLocked:
			printf("%s", guard_left_locked);
			break;
		case leftUnlocked:
			printf("%s", guard_left_unlocked);
			break;
		case rightLocked:
			printf("%s", guard_right_locked);
			break;
		case rightUnlocked:
			printf("%s", guard_right_unlocked);
			break;
		case leftScanned:
			printf("%s%d/n", left_scan, dispMsg.person_id);
			break;
		case rightScanned:
			printf("%s%d/n", right_scan, dispMsg.person_id);
			break;
		default:
			printf("%s", error_dispMsg);
		}

		//Reply with an empty dispMsg to make sure the client isn't send locked
		MsgReply(rcvid, EOK, NULL, 0);
	}

	return(EXIT_SUCCESS);
}
