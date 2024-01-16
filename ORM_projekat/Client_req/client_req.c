#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SIZE 1024


//Receive file from Register-Client
void ReceiveFile(int sockfd, char *fileName)
{
    int n;
    FILE *fp;
    char buffer[SIZE];

    fp = fopen(fileName, "w");
    if(fp==NULL)
    {
        perror("[-]Error in creating file.");
        exit(1);
    }
    while(1)
    {
        n = recv(sockfd, buffer, SIZE, 0);
        if(n<=0)
        {
            break;
            return ;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }
    fclose(fp);
	puts("FILE DOWNLOADED");
    return ;


}

int main()
{
	//Send request to server
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

    // Sending request for downloading a file
    char request[SIZE/2];
    char command[100];
    char fileName[100];

    printf("Provide the following command | REQUEST (file_name.txt)  \n");
    scanf("%[^\n]", request);
    sscanf(request, "%s %s", command, fileName);
    send(clientSocket, request, strlen(request), 0);

    // Reading server respons
    char response[1024];
    int bytesReceived = recv(clientSocket, response, sizeof(response), 0);
    if (bytesReceived <= 0) {
        perror("Error receiving data from server");
        close(clientSocket);
        return -1;
    }

    response[bytesReceived] = '\0';

    if (strcmp(response, "File not found") == 0) {
        printf("File not found on the server.\n");
    } else {
        // Connecting with client that registererd file
        char downloadFromIp[INET_ADDRSTRLEN];
        strcpy(downloadFromIp, response);
    }

	printf("SERVER MESSAGE [%s]\n", response);

    close(clientSocket);

    //Sending request to client who has the file
    int downloadSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (downloadSocket == -1) {
        perror("Error creating download socket");
        return -1;
    }

    struct sockaddr_in download_addr;
    download_addr.sin_family = AF_INET;
    download_addr.sin_addr.s_addr = inet_addr(response);
    download_addr.sin_port = htons(9998);

    if (connect(downloadSocket, (struct sockaddr*)&download_addr, sizeof(download_addr)) == -1) {
        perror("Error connecting to download client");
        close(downloadSocket);
        return -1;
    }

    	puts("Connected to the register client");

    if(send(downloadSocket, fileName, sizeof(fileName), 0) == -1)
    {
    	perror("Error in sending data");
    	return -1;
    }

    ReceiveFile(downloadSocket, fileName);


    close(downloadSocket);

} 
