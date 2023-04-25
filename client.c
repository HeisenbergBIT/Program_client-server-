#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT_NUMBER 12345

int main()
{
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[256];

    // Tworzenie gniazda klienta
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Blad funkcji socket");
        exit(1);
    }

    // Konfiguracja adresu serwera
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(PORT_NUMBER);

    // Nawi¹zywanie po³¹czenia z serwerem
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Blad funkcji connect");
        exit(1);
    }

    // Wysy³anie danych do serwera
    printf("[KLIENT] Wiadomosc do wyslania: ");
    fgets(buffer, sizeof(buffer), stdin);
    if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
        perror("Blad wysylania wiadomosci");
        exit(1);
    }

    // Odbieranie danych od serwera
    if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
        perror("Blad wysylania wiadomosci");
        exit(1);
    } else
        printf("[KLIENT] odebrano wiadomosc od serwera: %s\n", buffer);

    // Zamykanie gniazda klienta
    close(client_socket);

    return 0;
}
