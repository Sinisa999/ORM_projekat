#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void request_file(const char* filename) {
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

    // Slanje zahteva za preuzimanje datoteke
    char request[512];
    snprintf(request, sizeof(request), "REQUEST %s", filename);
    send(client_socket, request, strlen(request), 0);

    // Čitanje odgovora servera
    char response[1024];
    ssize_t bytes_received = recv(client_socket, response, sizeof(response), 0);
    if (bytes_received <= 0) {
        perror("Error receiving data from server");
        close(client_socket);
        return;
    }

    response[bytes_received] = '\0';

    if (strcmp(response, "File not found") == 0) {
        printf("File not found on the server.\n");
    } else {
        // Povezivanje sa klijentom koji je prijavio datoteku
        char download_from_ip[INET_ADDRSTRLEN];
        strcpy(download_from_ip, response);
        download_file_from_client(download_from_ip, filename);
    }

    close(client_socket);
}

void download_file_from_client(const char* ip, const char* filename) {
    // Simulacija preuzimanja datoteke (prilagoditi prema potrebama)
    char msg_request[100] = "test.txt";
    int download_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (download_socket == -1) {
        perror("Error creating download socket");
        return;
    }

    struct sockaddr_in download_addr;
    download_addr.sin_family = AF_INET;
    download_addr.sin_addr.s_addr = inet_addr(ip);
    download_addr.sin_port = htons(9998);

    if (connect(download_socket, (struct sockaddr*)&download_addr, sizeof(download_addr)) == -1) {
        perror("Error connecting to download client");
        close(download_socket);
        return;
    }

    /*FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file for download");
        close(download_socket);
        return;
    }*/
    
    printf("Connected to the other client");
    
    if(send(download_socket, msg_request, sizeof(msg_request), 0) == -1)
    {
    	perror("Error in sending data");
    	return;
    }

    /*char buffer[1024];
    ssize_t bytes_received;
    while ((bytes_received = recv(download_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }*/
    
    write_file(download_socket);

    //fclose(file);
    close(download_socket);
}

void write_file(int sockfd)
{
    int n; 
    FILE *fp;
    char *filename = "file2.txt";
    char buffer[1024];

    fp = fopen(filename, "w");
    if(fp==NULL)
    {
        perror("[-]Error in creating file.");
        exit(1);
    }
    while(1)
    {
        n = recv(sockfd, buffer, 1024, 0);
        if(n<=0)
        {
            break;
            return;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, 1024);
    }
    return;
    
}

//void request_file_from_client(


int main() {
    const char* file_to_request = "test.txt";
    request_file(file_to_request);

    return EXIT_SUCCESS;
}

