#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <ctype.h>

#define PORT_NUMBER 12345
#define MAX_CLIENTS 5

void* handle_client(void* arg);

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_address;
    pthread_t thread;

    // Tworzenie gniazda serwera
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Blad funkcji socket");
        exit(1);
    }

    // Konfiguracja adresu serwera
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT_NUMBER);

    // Bindowanie gniazda serwera do adresu
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Blad funkcji bind");
        exit(1);
    }

    // Nas�uchiwanie po��czenia
    listen(server_socket, MAX_CLIENTS);

    printf("[SERWER] Adres: Port: %d", PORT_NUMBER);

    while (1) {
        // Akceptowanie po��cze� od klient�w
        client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1) {
            perror("Blad funkcji accept");
            continue;
        }

        // Tworzenie nowego w�tku dla obs�ugi klienta
        if (pthread_create(&thread, NULL, handle_client, (void*)&client_socket) == -1) {
            perror("Blad funkcji pthread_create");
            close(client_socket);
            continue;
        }

        // Od��czanie w�tku
        pthread_detach(thread);
    }

    // Zamykanie gniazda serwera
    close(server_socket);

    return 0;
}

void* handle_client(void* arg) {
    int client_socket = *(int*)arg;
    char buffer[256];
    int n;

    // Odbieranie danych od klienta
    while ((n = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        // Zamiana wiadomo�ci na wielkie litery
        for (int i = 0; i < n; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        // Wysy�anie danych do klienta
        send(client_socket, buffer, n, 0);
    }

    // Zamykanie po��czenia z klientem
    close(client_socket);

    // Zako�czenie dzia�ania w�tku
    pthread_exit(NULL);
}