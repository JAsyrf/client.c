#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main()

 {
    char server_ip[20];
    int server_port;
    char message[BUFFER_SIZE];

    printf("Enter server IP address: ");
    scanf("%s", server_ip);
    printf("Enter server port number: ");
    scanf("%d", &server_port);

    printf("Enter a message: ");
    getchar();  // Clear newline from previous scanf
    fgets(message, BUFFER_SIZE, stdin);
    message[strcspn(message, "\n")] = '\0';  // Remove trailing newline

    int client_socket;
    struct sockaddr_in server_address;
    struct hostent server;

    // Create client socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error creating client socket");
        exit(1);
    }

    // Get server hostent struct
    server = gethostbyname(server_ip);
    if (server == NULL) {
        fprintf(stderr, "Error getting hostent for server\n");
        exit(1);
    }

    // Prepare server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);
    server_address.sin_port = htons(server_port);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr)&server_address, sizeof(server_address)) < 0) {
        perror("Error connecting to server");
        exit(1);
    }

    // Send message to server
    if (send(client_socket, message, strlen(message), 0) < 0) {
        perror("Error sending data to server");
        exit(1);
    }

    char response[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_socket, response, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("Error receiving data from server");
        exit(1);
    }

    response[bytes_received] = '\0';
    printf("Server response: %s\n", response);

    close(client_socket);
    return 0;
}
