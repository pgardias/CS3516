#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#ifndef PROJECT2_BUFFER_H
#define PROJECT2_BUFFER_H

/* Node object for use with queue */
struct Node {
	struct msg data;
	struct Node *next;
};

struct Node *front = NULL;
struct Node *back = NULL;

/* Inserts item to end of queue */
void addToQueue(struct msg message) {
	struct Node *newNode = (struct Node *) malloc(sizeof(struct Node));
	newNode->data = message;
	newNode->next = NULL;
	if (front == NULL && back == NULL) {
		front = back = newNode;
	} else {
		back->next = newNode;
		back = newNode;
	}
}

/* Removes first element in queue */
void removeFront() {
	if (front == NULL) {
		return;
	}
	struct Node *temp = front;
	if (front == back) {
		front = back = NULL;
	} else {
		front = front->next;
	}
	free(temp);
}

/* Returns first element in queue */
struct msg getFront() {
	if (front == NULL) {
		struct msg temp;
		return temp;
	} else {
		return front->data;
	}
}

/* Returns whether the buffer is empty or not */
int bufferEmpty() {
	if (front == NULL) {
		return 1;
	} else {
		return 0;
	}
}

#endif //PROJECT2_BUFFER_H

