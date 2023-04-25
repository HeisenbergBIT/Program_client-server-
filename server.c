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

    // Nas³uchiwanie po³¹czenia
    listen(server_socket, MAX_CLIENTS);

    printf("[SERWER] Adres: Port: %d", PORT_NUMBER);

    while (1) {
        // Akceptowanie po³¹czeñ od klientów
        client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1) {
            perror("Blad funkcji accept");
            continue;
        }

        // Tworzenie nowego w¹tku dla obs³ugi klienta
        if (pthread_create(&thread, NULL, handle_client, (void*)&client_socket) == -1) {
            perror("Blad funkcji pthread_create");
            close(client_socket);
            continue;
        }

        // Od³¹czanie w¹tku
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
        // Zamiana wiadomoœci na wielkie litery
        for (int i = 0; i < n; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        // Wysy³anie danych do klienta
        send(client_socket, buffer, n, 0);
    }

    // Zamykanie po³¹czenia z klientem
    close(client_socket);

    // Zakoñczenie dzia³ania w¹tku
    pthread_exit(NULL);
}