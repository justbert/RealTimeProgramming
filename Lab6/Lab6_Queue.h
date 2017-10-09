/*
 * Lab6Queue.h
 *
 *  Created on: 2016-12-10
 *      Author: Justin
 */

#ifndef LAB6QUEUE_H_
#define LAB6QUEUE_H_

typedef struct BeatQueueUnit {
	char*		beat;
	BeatUnit*	tailward = NULL;
} BeatUnit;


typedef struct BeatQueue {
	BeatUnit* 	head = NULL;
	BeatUnit* 	tail = NULL;
	int 		numUnits = 0;
} BeatQueue;

BeatQueue* addToQueue(BeatQueue* queue, char* beat) {

	BeatUnit* unit = malloc(sizeof(BeatUnit));
	unit->beat = malloc(sizeof(beat));
	strcpy(unit->beat, beat);

	if(queue->head == NULL && queue->tail == NULL && queue->numUnits == 0) {
		queue->head = unit;
		queue->tail = unit;
		queue->numUnits += 1;
		return queue;
	} else {
		queue->tail->tailward = unit;
		queue->tail = unit;
		queue->numUnits += 1;
		return queue;
	}

	return NULL;

}

char* popFromQueue(BeatQueue* queue) {


	if(queue->head != NULL && queue->numUnits > 1) {
		BeatUnit* head = queue->head;
		queue->head = queue->head->tailward;
		queue->numUnits -= 1;

		char* beat = head->beat;

		free(head);

		return beat;
	} else if (queue->head == queue->tail ){

	}
}

#endif /* LAB6QUEUE_H_ */
