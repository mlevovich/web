#pragma once 
#include <stdlib.h>

// Node structure definition
typedef struct Node {
    char* fileName; // Dynamic string for the file name
    long fileSize; // Size of the file in bytes
    long timeTaken; // Time taken for the file to be transmitted in milliseconds
    struct Node* next; // Pointer to the next node
} Node;

// Function to add a node to the end of the list
void addNode(Node** head, const char* fileName, long fileSize, long timeTaken);

// Function to free the memory allocated for the linked list
void freeList(Node* head);


