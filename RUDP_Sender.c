#include "RUDP_API.h"
#include <fcntl.h> // For file operations
#include <errno.h>

// Function prototypes
void usage(char *progname);

int main(int argc, char *argv[]) {
    if (argc != 6 || strcmp(argv[1],"-ip") != 0 || strcmp(argv[3],"-p") != 0 ) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    // fprintf(stderr, "Example: %s -ip 127.0.0.1 -p 12345 myfile.txt\n", progname);
    const char *ip = argv[2];
    int port = atoi(argv[4]);
    const char *filename = argv[5];
    
    // Open the file
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Create the RUDP socket
    int sockfd = rudp_socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to create RUDP socket\n");
        exit(EXIT_FAILURE);
    }

    // Set up the destination address
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &dest_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid IP address format\n");
        exit(EXIT_FAILURE);
    }

    // Buffer for reading from the file
    char buffer[RUDP_BUFFER_SIZE*100];
    ssize_t read_bytes, sent_bytes;

    // Send the file in chunks
    while ((read_bytes = read(file_fd, buffer, sizeof(buffer))) > 0) {        
        sent_bytes = rudp_send(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr), buffer, read_bytes);
        printf("Read %zd bytes, Sent %zd bytes\n", read_bytes, sent_bytes);
        if (sent_bytes < 0) {
            fprintf(stderr, "Failed to send data\n");
            break;
        }
    }

    if (read_bytes < 0) {
        perror("Failed to read from file");
    }

    // Close the file and RUDP connection
    close(file_fd);
    rudp_close(sockfd);

    return 0;
}

void usage(char *progname) {
    fprintf(stderr, "Usage: %s -ip <IP> -p <Port> <Filename>\n", progname);
    fprintf(stderr, "Example: %s -ip 127.0.0.1 -p 12345 myfile.txt\n", progname);
}

