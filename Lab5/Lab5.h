/*
 * Lab5.h
 *
 *  Created on: 2016-11-20
 *      Author: Justin
 */

#ifndef LAB5_H_
#define LAB5_H_

#define ATTACH_POINT "myController"
#define RESOURCE_NAME "/dev/myDevice"
#define MY_PULSE 4

/* We specify the header as being at least a pulse */
typedef struct _pulse msg_header_t;

typedef struct Lab5Struct {
    msg_header_t hdr;
    int data;
} L5S;

#endif /* LAB5_H_ */
