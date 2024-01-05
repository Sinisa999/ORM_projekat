#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DEFAULT_BUFLEN 512

void register_file(const char* filename) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating client socket");
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(9999);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        close(client_socket);
        return;
    }

    // Slanje zahteva za prijavu datoteke
    char request[512];
    snprintf(request, sizeof(request), "REGISTER %s", filename);
    send(client_socket, request, strlen(request), 0);

    close(client_socket);
}






int main() {
    const char* file_to_register = "test.txt";
    register_file(file_to_register);
    
    struct sockaddr_in server, client;
    int client1_sock, read_size, c;
    char client_message[DEFAULT_BUFLEN];
    
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating client socket");
        return;
    }
    
    puts("Socket created");
    
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(9998);
    
    //Bind
    if( bind(server_socket,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
    
    if (listen(server_socket, 5) == -1) {
        perror("Error listening on server socket");
        close(server_socket);
        return -1;
    }

    printf("Server listening on port 9999...\n");
    
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client1_sock = accept(server_socket, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client1_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");
    
    //Receive a message from client
    while( (read_size = recv(client1_sock , client_message , DEFAULT_BUFLEN , 0)) > 0 )
    {
        printf("Bytes received: %d\n", read_size);
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    return 0;
    
    

    return EXIT_SUCCESS;
}

