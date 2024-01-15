#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_FILENAME 256
#define MAX_PENDING_CONNECTIONS 5

// List of registered files
struct FileEntry {
    char fileName[MAX_FILENAME];
    char ipAddress[INET_ADDRSTRLEN];
};

struct FileEntry registeredFiles[MAX_PENDING_CONNECTIONS];
int numRegisteredFiles = 0;

void handleClient(int clientSocket, struct sockaddr_in client_addr) {
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesReceived <= 0) {
        perror("Error receiving data from client");
        close(clientSocket);
        return;
    }

    buffer[bytesReceived] = '\0';

    char command[MAX_FILENAME];
    char fileName[MAX_FILENAME];

    if (sscanf(buffer, "%s %s", command, fileName) != 2) {
        fprintf(stderr, "Invalid request format\n");
        close(clientSocket);
        return;
    }

    if (strcmp(command, "REGISTER") == 0) {
        // Adding file to list of registered files
        strcpy(registeredFiles[numRegisteredFiles].fileName, fileName);

        inet_ntop(AF_INET, &client_addr.sin_addr, registeredFiles[numRegisteredFiles].ipAddress, sizeof(registeredFiles[numRegisteredFiles].ipAddress));


        printf("File %s registered by %s\n", fileName, registeredFiles[numRegisteredFiles].ipAddress);
        numRegisteredFiles++;
	puts("[+]File");
    } else if (strcmp(command, "REQUEST") == 0) {
        // Check if the file i already registered
        int fileFound = 0;
        int i;
        for (i = 0; i < numRegisteredFiles; i++) {
            if (strcmp(registeredFiles[i].fileName, fileName) == 0) {
                // Sending IP of client who registered the file
                send(clientSocket, registeredFiles[i].ipAddress, strlen(registeredFiles[i].ipAddress), 0);

                // Removing file from flist of registered files
                memmove(&registeredFiles[i], &registeredFiles[i + 1], (numRegisteredFiles - i - 1) * sizeof(struct FileEntry));
                numRegisteredFiles--;
		puts("[-]File");

                // File exchange

                fileFound = 1;
                break;
            }
        }

        if (!fileFound) {
             // Message that the file is not found
            send(clientSocket, "File not found", strlen("File not found"), 0);
        }
    }

    close(clientSocket);
}


int startServer() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating server socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9999);

    if (bind(serverSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding server socket");
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, MAX_PENDING_CONNECTIONS) == -1) {
        perror("Error listening on server socket");
        close(serverSocket);
        return -1;
    }

    printf("Server listening on port 9999...\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&client_addr, &client_addr_len);

        if (clientSocket == -1) {
            perror("Error accepting connection from client");
            continue;
        }

        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, clientIp, sizeof(clientIp));
        printf("Accepted connection from %s:%d\n", clientIp, ntohs(client_addr.sin_port));

        // Creating a thread for every client
        handleClient(clientSocket, client_addr);
    }

    close(serverSocket);
    return 0;
}

int main() {
    if (startServer() == -1) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
