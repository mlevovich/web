all: TCP_Sender TCP_Receiver RUDP_Sender RUDP_Receiver

TCP_Sender: TCP_Sender.o
	gcc -Wall -g TCP_Sender.o -o TCP_Sender

TCP_Receiver: TCP_Receiver.o
	gcc -Wall -g TCP_Receiver.o -o TCP_Receiver

RUDP_Sender: RUDP_Sender.o RUDP_API.o
	gcc -Wall -g RUDP_Sender.o RUDP_API.o -o RUDP_Sender

RUDP_Receiver: RUDP_Receiver.o RUDP_API.o
	gcc -Wall -g RUDP_Receiver.o RUDP_API.o -o RUDP_Receiver

TCP_Receiver.o: TCP_Receiver.c
	gcc -Wall -g -c TCP_Receiver.c

TCP_Sender.o: TCP_Sender.c 
	gcc -Wall -g -c TCP_Sender.c

RUDP_API.o: RUDP_API.c 
	gcc -Wall -g -c RUDP_API.c

RUDP_Receiver.o: RUDP_Receiver.c
	gcc -Wall -g -c RUDP_Receiver.c

RUDP_Sender.o: RUDP_Sender.c 
	gcc -Wall -g -c RUDP_Sender.c

.PHONY: clean all

clean:
	rm -f *.o TCP_Sender TCP_Receiver
