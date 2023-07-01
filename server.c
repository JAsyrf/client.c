#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define PORT 8484

void handle_client(int client_socket) {
    char buffer[1024];
    char response[1024];

    while (1) {
        int received_bytes = recv(client_socket, buffer, sizeof(buffer), 0);
        if (received_bytes <= 0)
            break;

        buffer[received_bytes] = '\0';

        time_t now;
        time(&now);
        struct tm *local_time = localtime(&now);
        strftime(response, sizeof(response), "%d %B %Y, %H:%M", local_time);

        strcat(response, " - ");
        strcat(response, buffer);

        send(client_socket, response, strlen(response), 0);
    }

    printf("Connection closed\n");
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_length = sizeof(client_address);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started, listening on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
        if (client_socket < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        if (fork() == 0) {
            // Child process
            close(server_socket);
            handle_client(client_socket);
            exit(EXIT_SUCCESS);
        } else {
            // Parent process
            close(client_socket);
        }
    }

    close(server_socket);
    return 0;
}
