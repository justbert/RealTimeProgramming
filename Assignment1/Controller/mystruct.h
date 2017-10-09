/*
 * mystruct.h
 *
 *  Created on: 2016-10-14
 *      Author: Justin
 */

#ifndef MYSTRUCT_H_
#define MYSTRUCT_H_

//INPUT CONSTANTS
#define DOOR_OPEN_LEFT 0x0001
#define DOOR_OPEN_RIGHT 0x0002
#define DOOR_CLOSE_LEFT 0x0004
#define DOOR_CLOSE_RIGHT 0x0008
#define WEIGH_SCALE 0x0010
#define GUARD_LEFT_LOCK 0x0020
#define GUARD_LEFT_UNLOCK 0x0040
#define GUARD_RIGHT_LOCK 0x0080
#define GUARD_RIGHT_UNLOCK 0x0100
#define LEFT_SCAN 0x0200
#define RIGHT_SCAN 0x0400



//Message from the input server to the controller server
typedef struct inputMessage {
	int input;
	union {
		int person_id;
		int weight;
	};
} InputMsg;

typedef enum DisplayEnums {
	leftOpened,
	rightOpened,
	leftClosed,
	rightClosed,
	scaleWeighed,
	leftLocked,
	leftUnlocked,
	rightLocked,
	rightUnlocked,
	leftScanned,
	rightScanned,
	error
} DispEnum;

typedef struct DisplayMessage {
	DispEnum action;
	union {
		int person_id;
		int weight;
	};
} DispMsg;

#endif /* MYSTRUCT_H_ */
