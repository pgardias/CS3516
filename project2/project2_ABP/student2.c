#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"
#include "buffer.h"

/* ***************************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

 This code should be used for Project 2, unidirectional or bidirectional
 data transfer protocols from A to B and B to A.
 Network properties:
 - one way network delay averages five time units (longer if there
 are other messages in the channel for GBN), but can be larger
 - packets can be corrupted (either the header or the data portion)
 or lost, according to user-defined probabilities
 - packets may be delivered out of order.

 Compile as gcc -g project2.c student2.c -o p2
 **********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/* 
 * The routines you will write are detailed below. As noted above, 
 * such procedures in real-life would be part of the operating system, 
 * and would be called by other procedures in the operating system.  
 * All these routines are in layer 4.
 */

/* Constants */

#define A 0
#define B 1
#define READY 0
#define WAITING 1
#define MSG_LENGTH MESSAGE_LENGTH // 20

/* Prototypes */

int checksum(struct pkt*);
unsigned short CalculateCRC(char*, int);
int isCorrupt(struct pkt*);
void makeACK(struct pkt*);

/* Global vars */

int A_state;  // READY (0) or WAITING (1)
struct pkt A_pkt;  // active packet for A
int B_prev_pkt;  // previous packet which was acknowledged by B

/* Helper functions */

/* Creates checksum used to check packet */
int checksum(struct pkt *packet) {
	int sum = 0;
	struct pkt obj = *packet;
	char pkt_char[sizeof(obj)];
	obj.checksum = 0;
	memcpy(pkt_char, &obj, sizeof(obj));
	sum = CalculateCRC(pkt_char, sizeof(pkt_char));
	return sum;
}

/* Used to generate checksum */
unsigned short CalculateCRC(char* buffer, int size) {
	unsigned short cword = 0;
	unsigned short ch;
	for (int i = 0; i < size; i++) {
		ch = buffer[i] << 8;
		for (int j = 0; j < 8; j++) {
			if ((ch & 0x8000) ^ (cword & 0x8000)) {
				cword = (cword <<= 1) ^ 4129;
			} else {
				cword <<= 1;
			}
			ch <<= 1;
		}
	}
	return cword;
}

/* Uses checksum to check whether packet is corrupted */
int isCorrupt(struct pkt *packet) {
	if (checksum(packet) == packet->checksum) {
		return 0;
	} else {
		return 1;
	}
}

/* Creates ACK */
void makeACK(struct pkt *packet) {
	packet->acknum = B_prev_pkt;
	packet->seqnum = 1;
	packet->checksum = 0;
	packet->checksum = checksum(packet);
}

/*
 * A_output(message), where message is a structure of type msg, containing
 * data to be sent to the B-side. This routine will be called whenever the
 * upper layer at the sending side (A) has a message to send. It is the job
 * of your protocol to insure that the data in such a message is delivered
 * in-order, and correctly, to the receiving side upper layer.
 */
void A_output(struct msg message) {
	if (A_state == WAITING) {
		addToQueue(message);
	} else {
		A_pkt.seqnum = (A_pkt.seqnum + 1) % 2;
		A_pkt.acknum = 0;
		A_pkt.checksum = 0;
		strncpy(A_pkt.payload, message.data, MSG_LENGTH);
		A_pkt.checksum = checksum(&A_pkt);
		tolayer3(A, A_pkt);
		A_state = WAITING;
		startTimer(A, 1000);
	}
}

/*
 * Just like A_output, but residing on the B side.  USED only when the
 * implementation is bi-directional.
 */
void B_output(struct msg message) {
}

/*
 * A_input(packet), where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the B-side (i.e., as a result
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side.
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {
	if (!isCorrupt(&packet) && packet.acknum == A_pkt.seqnum) {
		stopTimer(AEntity);
		A_state = READY;
		if (!bufferEmpty()) {
			struct msg message = getFront();
			A_output(message);
			removeFront();
		}
	}
}

/*
 * A_timerinterrupt()  This routine will be called when A's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See starttimer()
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
	stopTimer(A);
	tolayer3(A, A_pkt);
	startTimer(A, 1000);
}

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
	A_state = READY;
	A_pkt.seqnum = 1;
}

/*
 * B_input(packet),where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the A-side (i.e., as a result
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side.
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
	if (isCorrupt(&packet) || packet.seqnum != ((B_prev_pkt + 1) % 2)) {
		makeACK(&packet);
		tolayer3(B, packet);
	} else {
		struct msg message;
		strcpy(message.data, packet.payload);
		tolayer5(B, message);
		B_prev_pkt = (B_prev_pkt + 1) % 2;
		makeACK(&packet);
		tolayer3(B, packet);
	}
}

/*
 * B_timerinterrupt()  This routine will be called when B's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See starttimer()
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void B_timerinterrupt() {
}

/*
 * The following routine will be called once (only) before any other
 * entity B routines are called. You can use it to do any initialization
 */
void B_init() {
	B_prev_pkt = 1;
}

