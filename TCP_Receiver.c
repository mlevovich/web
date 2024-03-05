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
    float times[100]; // Assuming a maximum of 100 sends
    float speeds[100]; // Assuming a maximum of 100 sends
    int count = 0;
    float total_time = 0;
    float total_all_bytes_packeges = 0;
    float total_bytes_received = 0;

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
        float bytes_received = 0; // changed to float
        total_bytes_received = 0;
        gettimeofday(&start, NULL);        

        while (total_bytes_received <= 2097152) {
            bytes_received = recv(newsockfd, buffer, sizeof(buffer), 0);
            total_bytes_received += bytes_received;
            if (bytes_received <= 0) break; // Error or connection closed        
        }
        gettimeofday(&end, NULL);

        printf("File transfer completed.\n");

        float elapsed = ((end.tv_sec - start.tv_sec) * 1000000.0f + end.tv_usec) - start.tv_usec; // microseconds, changed to float
        elapsed /= 1000.0f; // Convert to milliseconds, adjusted for float
        times[count] = elapsed;
        total_time += elapsed;

        // Calculate speed
        float speed = (total_bytes_received / (1024.0f * 1024.0f)) / (elapsed / 1000.0f); // MB/s, changed to float

        speeds[count] = speed;
        total_all_bytes_packeges += total_bytes_received;
        printf("run count #%d\n finished", count);
        count++;
        
        ssize_t n = read(newsockfd, buffer, BUFFER_SIZE); // Expecting "yes" or "no"
        if (n <= 0 || strncmp(buffer, "no", 2) == 0) break;

    } 

    printf("Sender sent exit message.\n");

    // Calculate and print statistics
    printf("----------------------------------\n");
    printf("- * Statistics * -\n");
    for (int i = 0; i < count; i++) { // Corrected condition to avoid accessing uninitialized entry
        printf("- Run #%d Data: Time=%.2fms; Speed=%.9fMB/s\n", i + 1, (double)times[i], (double)speeds[i]);
    }
    double average_time = (double)total_time / count;
    double average_speed = (double)total_all_bytes_packeges / (1024.0 * 1024.0) / ((double)total_time / 1000.0); // MB/s 
    printf("-\n- Average time: %.3fms\n- Average bandwidth: %.3fMB/s\n", average_time, average_speed);
    printf("----------------------------------\n");
    printf("Receiver end.\n");

    close(newsockfd);
    close(sockfd);

    return 0;
}


