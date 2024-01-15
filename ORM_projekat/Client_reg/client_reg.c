#include<stdio.h>      //printf
#include<string.h>     //strlen
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close

#define SIZE 1024

//Send file to Request-Client
void sendFile(FILE *fp, int socket) {

    char buffer[SIZE] = {0};


    while (fgets(buffer, SIZE, fp) != NULL)
    {
      if(send (socket, buffer, sizeof(buffer), 0) == -1)
      {
        printf("ERROR sendig data");
	break;
	return;
      }
      bzero(buffer, SIZE);

    }

	puts("FILE SENT");
}

int main()
{
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating client socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9999);

    if (connect(clientSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        close(clientSocket);
        return -1;
    }

    // Send request to register files
    char request[SIZE/2];

    printf("Please provide the following command | REGISTER (filename.txt) \n");

   scanf("%[^\n]", request);
   send(clientSocket, request, strlen(request), 0);


    close(clientSocket);

    int sendingSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (sendingSocket == -1){
		perror("Error creating client socket");
        return -1;
    }

    server_addr.sin_port = htons(9998);

    if (bind(sendingSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
		perror("Error  binding server socket");
		close(sendingSocket);
		return -1;
	}

	if (listen(sendingSocket, 5) == -1) {
        perror("Error listening on server socket");
        close(sendingSocket);
        return -1;
    }

    printf("Client register listening on port 9998...\n");

    int clientSocketSending, c;
    struct sockaddr_in client;


    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    clientSocketSending = accept(sendingSocket, (struct sockaddr *)&client, (socklen_t*)&c);
    if (clientSocketSending < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    char message[SIZE];

    int receivedMessage = recv(clientSocketSending, message, sizeof(message), 0);

    if(receivedMessage <= 0)
    {
		perror("Error receiving data from client");
        close(clientSocket);
        return -1;
	}

	  message[receivedMessage] = '\0';





    //Checking if file allready created
    FILE *fp;
    if(fopen(message, "r") == NULL)
    {
      fp = fopen(message, "w");
      fclose(fp);
    }
    fp = fopen(message, "r");
    if(fp == NULL)
    {
      printf("ERROR reading file");
    }

    sendFile(fp, clientSocketSending);



    fclose(fp);
    close(clientSocketSending);
    close(sendingSocket);


}
