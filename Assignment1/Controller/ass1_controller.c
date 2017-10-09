#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>

#include "mystruct.h"

//Global variables
static int rcvid; //The ID of the program that sent the message
static pid_t display_PID; //The process ID of the display program
static int coidDisplay; //ID of the connection to the display program
static int channelID;
static InputMsg inputMsg;
static DispMsg displayMsg;
static persons_in_building;

//States
void Idle();
void DoorScanLeft();
void EnterUnlockLeft();
void EnterDoorOpenLeft();
void WeighIn();
void EnterDoorCloseLeft();
void EnterLockLeft();
void EnterUnlockRight();
void EnterDoorOpenRight();
void EnterDoorCloseRight();
void EnterLockRight();
void DoorScanRight();
void ExitUnlockRight();
void ExitDoorOpenRight();
void WeighOut();
void ExitDoorCloseRight();
void ExitLockRight();
void ExitUnlockLeft();
void ExitDoorOpenLeft();
void ExitDoorCloseLeft();
void ExitLockLeft();

//Helper Functions
void InvalidCommand();
void send_display_message(DispMsg);

//State pointer
void (*statefunc)() = Idle;

int main(int argc, char *argv[]) {

	display_PID = atoi(argv[1]);

	coidDisplay = ConnectAttach (ND_LOCAL_NODE, display_PID, 1, _NTO_SIDE_CHANNEL, 0);
		if (coidDisplay == -1) {
			fprintf (stderr, "Controller: Couldn't connect to the Display Server.\n");
			perror (NULL);
			exit (EXIT_FAILURE);
		}

	//Create the channel for the inputs program
	channelID = ChannelCreate(0);
	if (channelID == -1) {
		perror("Failed to create the channel.");
		exit(EXIT_FAILURE);
	}

	printf("Controller PID is %d\n", getpid());

	//Driving loop from state to state
	while (1) {
		(*statefunc)();
	}

	ChannelDestroy(channelID);
}

//The initial state. Waiting on a message from
void Idle() {

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case LEFT_SCAN:
		statefunc = DoorScanLeft;
		break;
	case RIGHT_SCAN:
		statefunc = DoorScanRight;
		break;
	}
}

void DoorScanLeft() {

	inputMsg.person_id;
	displayMsg.action = leftScanned;
	displayMsg.person_id = inputMsg.person_id;

	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case GUARD_LEFT_UNLOCK:
		statefunc = EnterUnlockLeft;
	}

}

void EnterUnlockLeft() {

	displayMsg.action = leftUnlocked;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case DOOR_OPEN_LEFT:
		statefunc = EnterDoorOpenLeft;
	}
}

void EnterDoorOpenLeft() {

	displayMsg.action = leftOpened;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case WEIGH_SCALE:
		statefunc = WeighIn;
	}
}

void WeighIn() {

	displayMsg.action = scaleWeighed;
	displayMsg.weight = inputMsg.weight;

	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case DOOR_CLOSE_LEFT:
		statefunc = EnterDoorCloseLeft;
	}
}

void EnterDoorCloseLeft() {

	displayMsg.action = leftClosed;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case GUARD_LEFT_LOCK:
		statefunc = EnterLockLeft;
	}
}

void EnterLockLeft() {

	displayMsg.action = leftLocked;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case GUARD_RIGHT_UNLOCK:
		statefunc = EnterUnlockRight;
	}
}

void EnterUnlockRight() {

	displayMsg.action = rightUnlocked;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case DOOR_OPEN_RIGHT:
		statefunc = EnterDoorOpenRight;
	}
}

void EnterDoorOpenRight() {

	displayMsg.action = rightOpened;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case DOOR_CLOSE_RIGHT:
		statefunc = EnterDoorCloseRight;
	}
}

void EnterDoorCloseRight() {

	displayMsg.action = rightClosed;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case GUARD_RIGHT_LOCK:
		statefunc = EnterLockRight;
	}
}

void EnterLockRight() {

	displayMsg.action = rightLocked;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	statefunc = Idle;
}

void DoorScanRight() {

	displayMsg.person_id = inputMsg.person_id;
	displayMsg.action = rightScanned;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case GUARD_RIGHT_UNLOCK:
		statefunc = ExitUnlockRight;
	}
}

void ExitUnlockRight() {

	displayMsg.action = rightUnlocked;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case DOOR_OPEN_RIGHT:
		statefunc = ExitDoorOpenRight;
	}
}

void ExitDoorOpenRight() {

	displayMsg.action = rightOpened;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case WEIGH_SCALE:
		statefunc = WeighOut;
	}
}

void WeighOut() {

	displayMsg.weight = inputMsg.weight;
	displayMsg.action = scaleWeighed;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case DOOR_CLOSE_RIGHT:
		statefunc = ExitDoorCloseRight;
	}
}

void ExitDoorCloseRight() {

	displayMsg.action = rightClosed;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case GUARD_RIGHT_LOCK:
		statefunc = ExitLockRight;
	}
}

void ExitLockRight() {

	displayMsg.action = rightLocked;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case GUARD_LEFT_UNLOCK:
		statefunc = ExitUnlockLeft;
	}
}

void ExitUnlockLeft() {

	displayMsg.action = leftUnlocked;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case DOOR_OPEN_LEFT:
		statefunc = ExitDoorOpenLeft;
	}
}

void ExitDoorOpenLeft() {

	displayMsg.action = leftOpened;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case DOOR_CLOSE_LEFT:
		statefunc = ExitDoorCloseLeft;
	}
}

void ExitDoorCloseLeft() {

	displayMsg.action = leftClosed;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	switch (inputMsg.input) {
	case GUARD_LEFT_LOCK:
		statefunc = ExitLockLeft;
	}
}

void ExitLockLeft() {

	displayMsg.action = leftLocked;
	send_display_message(&displayMsg);

	rcvid = MsgReceive(channelID, &inputMsg, sizeof(inputMsg), NULL);
	//Return an empty message to the input server, to unblock
	MsgReply(rcvid, EOK, NULL, 0);

	statefunc = Idle;
}

void send_display_message(DispMsg* msg) {

	if(MsgSend(coidDisplay, msg, sizeof(DispMsg), NULL, 0) == -1) {
		fprintf (stderr, "Inputs: Error during MsgSend\n");
		perror (NULL);
		exit (EXIT_FAILURE);
	}
}
