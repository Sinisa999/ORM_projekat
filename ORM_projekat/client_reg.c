#include<stdio.h>      //printf
#include<string.h>     //strlen
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close


void send_file(const char* file_path, int socket) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file for reading");
        return;
    }

    char buffer[1024];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(socket, buffer, bytes_read, 0);
    }

    fclose(file);
}

int main()
{
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating client socket");
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9999);
    
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        close(client_socket);
        return;
    }
    
    // Slanje zahteva za prijavu datoteke
    char request[512];
    //snprintf(request, sizeof(request), "REGISTER %s", filename);
    printf("Unesite komadnu u sledecem formatu: REGISTER (ime fajla koja zelite da registrujete) \n");
    scanf("%[^\n]", request);
    //printf("%s", request);
    send(client_socket, request, strlen(request), 0);

    close(client_socket);
    
    int slanje_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (slanje_socket == -1){
		perror("Error creating client socket");
        return;
    }
    
    server_addr.sin_port = htons(9998);
    
    if (bind(slanje_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
		perror("Error  binding server socket");
		close(slanje_socket);
		return -1;
	}
	
	if (listen(slanje_socket, 5) == -1) {
        perror("Error listening on server socket");
        close(slanje_socket);
        return -1;
    }
    
    printf("Client_reg listening on port 9999...\n");
    
    int client_socket_slanje, c;
    struct sockaddr_in client;
    
    
    c = sizeof(struct sockaddr_in);
    
    //accept connection from an incoming client
    client_socket_slanje = accept(slanje_socket, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_socket_slanje < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");
    
    char poruka[100];
    
    int primljena_poruka = recv(client_socket_slanje, poruka, sizeof(poruka), 0);
    
    if(primljena_poruka <= 0)
    {
		perror("Error receiving data from client");
        close(client_socket);
        return;
	}
	
	poruka[100] = '\0';
	
	
    
    
    FILE *fp;
    
    send_file(poruka, slanje_socket);
    
    
    fclose(fp); 
    close(client_socket_slanje);
    close(slanje_socket);
    
    
    
    
    
    
    
		
}
