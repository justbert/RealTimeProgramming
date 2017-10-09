/*
 * mystruct.h
 *
 *  Created on: 2016-10-22
 *      Author: Justin
 */

#ifndef MYSTRUCT_H_
#define MYSTRUCT_H_

#define MSG_ERROR -1
#define MSG_WARNING 1
#define MSG_OK 0

struct operationMessage {
	int x;
	int y;
	char operator;
} typedef opMsg;

struct operationResponse {
	float answer;
	char error;
	char msg[128];
} typedef respMsg;

#endif /* MYSTRUCT_H_ */
