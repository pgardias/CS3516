#include <stdio.h>
#include <stdlib.h>
#include "project3.h"

extern int TraceLevel;

struct distance_table {
	int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt0;
struct NeighborCosts *neighbor0;

/* students to write the following two routines, and maybe some others */

#define THIS_NODE 0
extern int clocktime;
int initCost0[MAX_NODES];
int mins0[MAX_NODES];

/* Prototypes */

void printdt0(int MyNodeNumber, struct NeighborCosts *neighbor,
		struct distance_table *dtptr);

/* Helper Functions */

int min0(int a, int b) {
	return (a <= b ? a : b);
}

void rtinit0() {
	printf("rtinit%d called at %d\n", THIS_NODE, clocktime);
	for (int i = 0; i < MAX_NODES; i++) {
		mins0[i] = INFINITY;
		for (int j = 0; j < MAX_NODES; j++) {
			dt0.costs[i][j] = INFINITY;
		}
	}

	// Get distance table for this node
	neighbor0 = (struct NeighborCosts *) malloc(sizeof(struct NeighborCosts));
	neighbor0 = getNeighborCosts(THIS_NODE);
	printf("***** neighbor0->NodesInNetwork = %d\n", neighbor0->NodesInNetwork);
	for (int i = 0; i < neighbor0->NodesInNetwork; i++) {
		dt0.costs[i][i] = initCost0[i] = mins0[i] = neighbor0->NodeCosts[i];
		printf("***** The cost of the node from %d to %d is %d.\n", THIS_NODE,
				i, neighbor0->NodeCosts[i]);
	}

	for (int i = 0; i < MAX_NODES; i++) {
		// Cases to skip: if self, or if not connected to
		if (i == THIS_NODE || initCost0[i] == INFINITY) {
			continue;
		}
		struct RoutePacket pkt;
		pkt.sourceid = THIS_NODE;
		pkt.destid = i;
		printf("At time t=%d, node %d sends packet to node %d with: ",
		THIS_NODE, clocktime, i);
		for (int i = 0; i < MAX_NODES; i++) {
			pkt.mincost[i] = mins0[i];
		}
		printf("Packet sent to node %d\n", i);
		toLayer2(pkt);
	}
}

void rtupdate0(struct RoutePacket *rcvdpkt) {
	printf("rtpdate%d called at %d\n", THIS_NODE, clocktime);
	int origin_node = rcvdpkt->sourceid;
	_Bool flag = 0;
	printf("Packet received at node %d from node %d\n", THIS_NODE, origin_node);

	// Check if the new table has a shorter distance to self than what is currently in table
	if (dt0.costs[origin_node][origin_node] > rcvdpkt->mincost[THIS_NODE]) { //if a shorter path is reported to node
		dt0.costs[origin_node][origin_node] = rcvdpkt->mincost[THIS_NODE]; //change path to shorter one
		flag = 1; // Flag for updated tables to be sent out

	}

	// Check if new cost to node plus cost to  is less than current cost
	for (int i = 0; i < MAX_NODES; i++) {
		if (dt0.costs[i][origin_node]
				> rcvdpkt->mincost[i] + dt0.costs[origin_node][origin_node]) {
			dt0.costs[i][origin_node] = rcvdpkt->mincost[i]
					+ dt0.costs[origin_node][origin_node];
			flag = 1; // Flag for updated tables to be sent out

		}
	}

	// Update minimum costs table
	for (int i = 0; i < MAX_NODES; i++) {
		for (int j = 0; j < MAX_NODES; j++) {
			if (dt0.costs[i][j] < mins0[i]) {
				flag = 1; // Flag for updated tables to be sent out
				mins0[i] = dt0.costs[i][j];
			}
		}
	}

	// Tables updated, send out updated distance values
	if (flag) {
		printf("Distance table at node %d updated:\n", THIS_NODE);
		printdt0(THIS_NODE, neighbor0, &dt0);
		// Create and format packets to send to each node
		for (int i = 0; i < MAX_NODES; i++) {
			// Cases to skip: if self, or if not connected to
			if (i == THIS_NODE || initCost0[i] == INFINITY) {
				continue;
			}
			struct RoutePacket pkt;
			pkt.sourceid = THIS_NODE;
			pkt.destid = i;
			printf("At time t=%d, node %d sends packet to node %d with: ",
			THIS_NODE, clocktime, i);
			for (int i = 0; i < MAX_NODES; i++) {
				pkt.mincost[i] = mins0[i];
			}
			printf("Packet sent to node %d\n", i);
			toLayer2(pkt);
		}
	} else {
		printf("Distance table at node %d not updated:\n", THIS_NODE);
		printdt0(THIS_NODE, neighbor0, &dt0);
	}
}

/////////////////////////////////////////////////////////////////////
//  printdt
//  This routine is being supplied to you.  It is the same code in
//  each node and is tailored based on the input arguments.
//  Required arguments:
//  MyNodeNumber:  This routine assumes that you know your node
//                 number and supply it when making this call.
//  struct NeighborCosts *neighbor:  A pointer to the structure 
//                 that's supplied via a call to getNeighborCosts().
//                 It tells this print routine the configuration
//                 of nodes surrounding the node we're working on.
//  struct distance_table *dtptr: This is the running record of the
//                 current costs as seen by this node.  It is 
//                 constantly updated as the node gets new
//                 messages from other nodes.
/////////////////////////////////////////////////////////////////////
void printdt0(int MyNodeNumber, struct NeighborCosts *neighbor,
		struct distance_table *dtptr) {
	int i, j;
	int TotalNodes = neighbor->NodesInNetwork; // Total nodes in network
	int NumberOfNeighbors = 0;                     // How many neighbors
	int Neighbors[MAX_NODES];                   // Who are the neighbors

	// Determine our neighbors
	for (i = 0; i < TotalNodes; i++) {
		if ((neighbor->NodeCosts[i] != INFINITY) && i != MyNodeNumber) {
			Neighbors[NumberOfNeighbors] = i;
			NumberOfNeighbors++;
		}
	}
	// Print the header
	printf("                via     \n");
	printf("   D%d |", MyNodeNumber);
	for (i = 0; i < NumberOfNeighbors; i++)
		printf("     %d", Neighbors[i]);
	printf("\n");
	printf("  ----|-------------------------------\n");

	// For each node, print the cost by travelling thru each of our neighbors
	for (i = 0; i < TotalNodes; i++) {
		if (i != MyNodeNumber) {
			printf("dest %d|", i);
			for (j = 0; j < NumberOfNeighbors; j++) {
				printf("  %4d", dtptr->costs[i][Neighbors[j]]);
			}
			printf("\n");
		}
	}
	printf("\n");
}    // End of printdt0

