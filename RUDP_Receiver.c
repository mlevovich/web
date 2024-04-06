#include "RUDP_API.h"

// Function prototypes
void usage(char *progname);

int main(int argc, char *argv[]) {    
    if (argc != 3 || strcmp(argv[1], "-p") != 0) {  
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[2]);    
    if (port <= 0) {
        fprintf(stderr, "Invalid port number\n");
        exit(EXIT_FAILURE);
    }

    // Create the RUDP socket
    int sockfd = rudp_socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to create RUDP socket\n");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the provided port
    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d...\n", port);

    // Buffer for receiving data
    char buffer[RUDP_BUFFER_SIZE];
    struct sockaddr_storage sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    ssize_t received_bytes;

    // Receive data chunks
    while ((received_bytes = rudp_recv(sockfd, (struct sockaddr *)&sender_addr, &addr_len, buffer, sizeof(buffer))) > 0) {
        printf("Received %zd bytes\n", received_bytes);
        // Here you can process the received data
    }

    if (received_bytes < 0) {
        fprintf(stderr, "Receive error\n");
    }

    // Close the RUDP connection
    rudp_close(sockfd);

    return 0;
}

void usage(char *progname) {
    fprintf(stderr, "Usage: %s -p <Port>\n", progname);
    fprintf(stderr, "Example: %s -p 12345\n", progname);
}
