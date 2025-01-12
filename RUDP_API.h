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
#define ACK_TIMEOUT 60 // 1 minute
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
// Creates a RUDP socket
// @return the socket number if success, -1 if failed.
int rudp_socket(int domain, int type, int protocol);

/**
 * Sending data to a peer.
 *  The function should wait for an acknowledgment packet, and if it didn’t
 * receive any, retransmits the data up to 5 (configurable) times.
 *
 * @param sockfd - Specifies the socket file descriptor to send from.
 * @param dest_addr - Points to a sockaddr structure containing the destination address.
 * @param addrlen - Specifies the length of the sockaddr structure pointed to by the dest_addr argument.
 * @param buf - a buffer of data to send.
 * @param len - the length of the data buffer 'buf'.
 * @param sendFin - if 1, send FIN.
 * @return the total bytes sent.
 */
ssize_t rudp_send(int sockfd, const struct sockaddr *dest_addr, socklen_t addrlen, const void *buf, size_t len, int sendFin);

/**
 * Receiving data from a peer.
 * will put the received data in the data parameter, and the length of the data
 * in the data_length parameter.
 *
 * @param sockfd - Specifies the socket file descriptor to receive from.
 * @param src_addr - Points to a sockaddr structure containing the source address.
 * @param addrlen - Specifies the length of the sockaddr structure pointed to by the src_addr argument.
 * @param buf - a buffer of data to receive.
 * @param len - the maximum length of the data buffer 'buf'.
 * @param status - operation status: 
 *  1 is data, 0 if nondata, 2 if last data packet,
 * -2 if close connection, -1 is failed.
 * @return the total bytes received.
 */
ssize_t rudp_recv(int sockfd, struct sockaddr *src_addr, socklen_t *addrlen, void *buf, size_t len, int *status);

/* Closes the RUDP socket.
* @param sockfd - Specifies the socket file descriptor to send from.
*/
int rudp_close(int sockfd);

#endif // RUDP_API_H
