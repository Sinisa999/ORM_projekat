#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void ReceiveFile(int sockfd, char *ime_fajla)
{
    int n;
    FILE *fp;
    char *filename = ime_fajla;
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
    fclose(fp);
    return;


}

int main()
{
	//Slanje zahteva serveru
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

    // Slanje zahteva za preuzimanje datoteke
    char request[512];
    char komanda[100];
    char ime_fajla[100];
    //snprintf(request, sizeof(request), "REQUEST %s", filename);
    printf("Unesite komadnu u sledecem formatu: REQUEST (ime fajla koja zelite da preuzmete) \n");
    scanf("%[^\n]", request);
    sscanf(request, "%s %s", komanda, ime_fajla);
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
    printf("%s", response);

    if (strcmp(response, "File not found") == 0) {
        printf("File not found on the server.\n");
    } else {
        // Povezivanje sa klijentom koji je prijavio datoteku
        char download_from_ip[INET_ADDRSTRLEN];
        strcpy(download_from_ip, response);
        //download_file_from_client(download_from_ip, filename);
    }

    close(client_socket);

    //slanje zahteva klientu kod koga se nalazi fajl
    int download_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (download_socket == -1) {
        perror("Error creating download socket");
        return;
    }

    struct sockaddr_in download_addr;
    download_addr.sin_family = AF_INET;
    download_addr.sin_addr.s_addr = inet_addr(response);
    download_addr.sin_port = htons(9998);

    if (connect(download_socket, (struct sockaddr*)&download_addr, sizeof(download_addr)) == -1) {
        perror("Error connecting to download client");
        close(download_socket);
        return;
    }

    printf("Connected to the other client");

    if(send(download_socket, ime_fajla, sizeof(ime_fajla), 0) == -1)
    {
    	perror("Error in sending data");
    	return;
    }

    //const char kopija = "kopija.txt";

    ReceiveFile(download_socket, ime_fajla);

    close(download_socket);

}
