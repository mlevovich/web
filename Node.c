#include "head.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h> // For TCP_CONGESTION
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "head.h"

void addNode(Node** head, const char* fileName, long fileSize, long timeTaken) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->fileName = strdup(fileName); // Copy fileName
    newNode->fileSize = fileSize;
    newNode->timeTaken = timeTaken;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
    } else {
        Node* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void freeList(Node* head) {
    Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp->fileName);
        free(temp);
    }
}
