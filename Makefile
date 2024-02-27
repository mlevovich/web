all: TCP_Sender TCP_Receiver

TCP_Sender: TCP_Sender.o
	gcc -Wall TCP_Sender.o -o TCP_Sender

TCP_Receiver: TCP_Receiver.o
	gcc -Wall TCP_Receiver.o -o TCP_Receiver

TCP_Receiver.o: TCP_Receiver.c
	gcc -Wall -c TCP_Receiver.c

TCP_Sender.o: TCP_Sender.c
	gcc -Wall -c TCP_Sender.c

.PHONY: clean all

clean:
	rm -f *.o TCP_Sender TCP_Receiver
