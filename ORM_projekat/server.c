#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_FILENAME 256
#define MAX_PENDING_CONNECTIONS 5

// Spisak prijavljenih datoteka
struct FileEntry {
    char filename[MAX_FILENAME];
    char ip_address[INET_ADDRSTRLEN];
};

struct FileEntry registered_files[MAX_PENDING_CONNECTIONS];
int num_registered_files = 0;

void handle_client(int client_socket) {
    char buffer[1024];
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

    if (bytes_received <= 0) {
        perror("Error receiving data from client");
        close(client_socket);
        return;
    }

    buffer[bytes_received] = '\0';

    char command[MAX_FILENAME];
    char filename[MAX_FILENAME];

    if (sscanf(buffer, "%s %s", command, filename) != 2) {
        fprintf(stderr, "Invalid request format\n");
        close(client_socket);
        return;
    }

    if (strcmp(command, "REGISTER") == 0) {
        // Dodavanje datoteke u spisak prijavljenih datoteka
        strcpy(registered_files[num_registered_files].filename, filename);
        inet_ntop(AF_INET, &client_socket, registered_files[num_registered_files].ip_address, sizeof(registered_files[num_registered_files].ip_address));

        printf("File %s registered by %s\n", filename, registered_files[num_registered_files].ip_address);
        num_registered_files++;
    } else if (strcmp(command, "REQUEST") == 0) {
        // Provera da li je datoteka prijavljena
        int file_found = 0;
        int i;
        for (i = 0; i < num_registered_files; i++) {
            if (strcmp(registered_files[i].filename, filename) == 0) {
                // Slanje IP adrese klijenta koji je prijavio datoteku
                send(client_socket, registered_files[i].ip_address, strlen(registered_files[i].ip_address), 0);

                // Uklanjanje datoteke iz spiska prijavljenih datoteka
                memmove(&registered_files[i], &registered_files[i + 1], (num_registered_files - i - 1) * sizeof(struct FileEntry));
                num_registered_files--;

                // Razmena datoteke
                send_file(client_socket, filename);
                file_found = 1;
                break;
            }
        }

        if (!file_found) {
            // Obaveštenje da datoteka nije pronađena
            send(client_socket, "File not found", strlen("File not found"), 0);
        }
    }

    close(client_socket);
}

void send_file(int client_socket, const char* filename) {
    // Simulacija slanja datoteke (prilagoditi prema potrebama)
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
}

int start_server() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating server socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9999);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding server socket");
        close(server_socket);
        return -1;
    }

    if (listen(server_socket, MAX_PENDING_CONNECTIONS) == -1) {
        perror("Error listening on server socket");
        close(server_socket);
        return -1;
    }

    printf("Server listening on port 9999...\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);

        if (client_socket == -1) {
            perror("Error accepting connection from client");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("Accepted connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // Pokretanje niti za svakog klijenta
        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}

int main() {
    if (start_server() == -1) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

