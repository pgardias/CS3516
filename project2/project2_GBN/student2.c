#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"

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
#define WINDOW_SIZE 8
#define MAX_PACKETS 50000000 // Max packets in array 'packets'

/* Prototype */

int checksum(struct pkt*);
unsigned short CalculateCRC(char*, int);
int isCorrupt(struct pkt*);

/* Global vars */

struct pkt* packets[MAX_PACKETS];
struct pkt prev_ack_pkt; // Last successful ACK received
int next_seq = 0;
int expected_seq = 0;
int base = 0;

/* Creates checksum used to check packet */
int checksum(struct pkt *packet) {
	int sum = 0;
	for (int i = 0; i < MSG_LENGTH;) {
		sum += ((int) packet->payload[i]) * (++i);
	}
	sum += packet->seqnum * (packet->payload[1]);
	sum += packet->acknum * packet->payload[2];
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
int isCorrupt(struct pkt* packet) {
	if (checksum(packet) == packet->checksum) {
		return 0;
	} else {
		return 1;
	}
}

/* 
 * A_output(message), where message is a structure of type msg, containing 
 * data to be sent to the B-side. This routine will be called whenever the 
 * upper layer at the sending side (A) has a message to send. It is the job 
 * of your protocol to insure that the data in such a message is delivered 
 * in-order, and correctly, to the receiving side upper layer.
 */
void A_output(struct msg message) {
	packets[next_seq] = (struct pkt*) malloc(sizeof(struct pkt));
	memcpy(packets[next_seq]->payload, message.data, MSG_LENGTH);
	packets[next_seq]->seqnum = next_seq;
	packets[next_seq]->acknum = 0;
	packets[next_seq]->checksum = checksum(packets[next_seq]);
	// If within the current orientation of the window, send packet
	if (next_seq < (base + WINDOW_SIZE)) {
		tolayer3(A, *packets[next_seq]);
		if (base == next_seq) {
			startTimer(A, 5000);
		}
	}
	next_seq++;
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
	if (!isCorrupt(&packet)) {
		// Received ACK
		if (packet.acknum == 1) {
			if (base <= packet.seqnum) {
				if ((packet.seqnum + 1) == next_seq) {
					stopTimer(A);
				} else {
					// Set base to last successful packet transmitted
					base = packet.seqnum + 1;
					if (base != next_seq) {
						stopTimer(A);
						startTimer(A, 5000);
					}
				}
			}
		}
	}
}

/*
 * A_timerinterrupt()  This routine will be called when A's timer expires 
 * (thus generating a timer interrupt). You'll probably want to use this 
 * routine to control the retransmission of packets. See starttimer() 
 * and stopTimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
	for (int i = base; i < next_seq, i < (base + WINDOW_SIZE), i < MAX_PACKETS;
			i++) {
		if (packets[i] == NULL) {
			continue;
		}
		tolayer3(A, *packets[i]);
	}
	startTimer(A, 5000);
}

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
}

/*
 * B_input(packet),where packet is a structure of type pkt. This routine 
 * will be called whenever a packet sent from the A-side (i.e., as a result 
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side. 
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
	if (!isCorrupt(&packet)) {
		// Incorrect sequence number
		if (packet.seqnum != expected_seq) {
			tolayer3(B, prev_ack_pkt);
		} else { // Correct sequence number
			struct msg message;
			strcpy(message.data, packet.payload);
			tolayer5(B, message);
			expected_seq++;
			packet.checksum = 0;
			packet.acknum = 1;
			packet.checksum = checksum(&packet);
			tolayer3(B, packet);
			prev_ack_pkt = packet;
		}
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
	prev_ack_pkt.seqnum = 0;
	prev_ack_pkt.acknum = 1;
	prev_ack_pkt.checksum = 0;
	strncpy(prev_ack_pkt.payload, "\0", MSG_LENGTH);
	prev_ack_pkt.checksum = checksum(&prev_ack_pkt);
}

