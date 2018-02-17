CFLAGS =  -g -Wall

all: project3
	
project3: project3.o node0.o node1.o node2.o  node3.o

project3.o: project3.c project3.h
	gcc $(CFLAGS) -c project3.c
	
node0.o: node0.c
	gcc $(CFLAGS) -c node0.c
	
node1.o: node1.c
	gcc $(CFLAGS) -c node1.c
	
node2.o: node2.c
	gcc $(CFLAGS) -c node2.c
	
node3.o: node3.c
	gcc $(CFLAGS) -c node3.c
	
clean:
	rm -f *.o *.exe project3