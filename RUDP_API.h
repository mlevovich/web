#ifndef RUDP_API_H
#define RUDP_API_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define RUDP_PORT 12345
#define RUDP_BUFFER_SIZE 1500
#define ACK_TIMEOUT 60 // Seconds
#define MAX_RETRANS_ATTEMPTS 5

// Define flags for RUDP header
#define SYN 0x01
#define ACK 0x02
#define FIN 0x04
#define DATA 0x08

typedef struct {
    uint16_t length;
    uint16_t checksum;
    uint8_t flags;
} rudp_header;

// RUDP API Functions
int rudp_socket(int domain, int type, int protocol);
ssize_t rudp_send(int sockfd, const struct sockaddr *dest_addr, socklen_t addrlen, const void *buf, size_t len);
ssize_t rudp_recv(int sockfd, struct sockaddr *src_addr, socklen_t *addrlen, void *buf, size_t len);
int rudp_close(int sockfd);

#endif // RUDP_API_H
