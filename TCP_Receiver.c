#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h> // For TCP_CONGESTION
#include <unistd.h>
#include <time.h>
#include <sys/time.h> // For gettimeofday

#define BUFFER_SIZE 1024

void set_congestion_control(int sockfd, const char *algo) {
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, algo, strlen(algo)) != 0) {
        perror("setsockopt TCP_CONGESTION failed");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno = 0;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    char congestion_control_algo[256] = "reno"; // Default to "reno"
    struct timeval start, end;
    long times[100]; // Assuming a maximum of 100 sends
    int count = 0;
    long total_time = 0, total_bytes_received = 0;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            portno = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-algo") == 0 && i + 1 < argc) {
            strncpy(congestion_control_algo, argv[++i], sizeof(congestion_control_algo) - 1);
        }
    }

    if (portno == 0) {
        fprintf(stderr, "Usage: %s -p PORT -algo ALGO\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    set_congestion_control(sockfd, congestion_control_algo);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(EXIT_FAILURE);
    }
    printf("Starting Receiver...\n");

    listen(sockfd, 5);
    printf("Waiting for TCP connection...\n");

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(EXIT_FAILURE);
    }
    printf("Sender connected, beginning to receive file...\n");

    while(1) {
        gettimeofday(&start, NULL);
        long bytes_received = 0;
        while (1) {
            ssize_t n = read(newsockfd, buffer, BUFFER_SIZE);
            if (n <= 0) break; // Assuming end of file or error
            bytes_received += n;
        }
        gettimeofday(&end, NULL);

        printf("File transfer completed.\n");

        long elapsed = ((end.tv_sec - start.tv_sec) * 1000000L + end.tv_usec) - start.tv_usec; // microseconds
        elapsed /= 1000; // Convert to milliseconds
        times[count++] = elapsed;
        total_time += elapsed;
        total_bytes_received += bytes_received;

        printf("Waiting for Sender response...\n");
        ssize_t n = read(newsockfd, buffer, BUFFER_SIZE); // Expecting "yes" or "no"
        if (n <= 0 || strncmp(buffer, "no", 2) == 0) break;
    } 

    printf("Sender sent exit message.\n");

    // Calculate and print statistics
    printf("----------------------------------\n");
    printf("- * Statistics * -\n");
    for (int i = 0; i < count; i++) { // Corrected condition to avoid accessing uninitialized entry
        double speed = ((double)total_bytes_received / (1024 * 1024)) / (times[i] / 1000.0); // Convert to MB/s
        printf("- Run #%d Data: Time=%.2fms; Speed=%.2fMB/s\n", i + 1, (double)times[i], speed);
    }
    double average_time = (double)total_time / count;
    double average_speed = ((double)total_bytes_received / (1024 * 1024)) / (total_time / 1000.0) / count; // Convert to MB/s
    printf("-\n- Average time: %.2fms\n- Average bandwidth: %.2fMB/s\n", average_time, average_speed);
    printf("----------------------------------\n");
    printf("Receiver end.\n");

    close(newsockfd);
    close(sockfd);

    return 0;
}
