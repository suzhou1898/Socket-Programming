CC = g++
CFLAGS = -g -Wall

all: serverA.o serverB.o serverC.o aws.o monitor.o
	$(CC) $(CFLAGS) serverA.cpp -o serverAoutput
	$(CC) $(CFLAGS) serverB.cpp -o serverBoutput
	$(CC) $(CFLAGS) serverC.cpp -o serverCoutput
	$(CC) $(CFLAGS) aws.cpp -o awsoutput
	$(CC) $(CFLAGS) monitor.cpp -o monitoroutput
	$(CC) $(CFLAGS) client.cpp -o client

.PHONY: serverA serverB serverC aws monitor

serverA:
	./serverAoutput

serverB:
	./serverBoutput

serverC:
	./serverCoutput

aws:
	./awsoutput

monitor:
	./monitoroutput


