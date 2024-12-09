#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#pragma comment(lib, "ws2_32.lib") 

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
	setlocale(LC_ALL, "Turkish");
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    int bytesSent, bytesReceived;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Winsock baþlatýlamadý. Hata Kodu: %d\n", WSAGetLastError());
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("Soket oluþturulamadý. Hata Kodu: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(SERVER_PORT);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Baðlantý baþarýsýz oldu. Hata Kodu: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("Sunucuya baðlanýldý.\n");

    while (1) {
        printf("Ürün adý ve sipariþ miktarýný girin (örn: Laptop 2) veya çýkmak için 'exit' yazýn: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\\n")] = 0; 

        if (strcmp(buffer, "exit") == 0) {
            send(clientSocket, buffer, strlen(buffer), 0);
            break;
        }

        bytesSent = send(clientSocket, buffer, strlen(buffer), 0);
        if (bytesSent == SOCKET_ERROR) {
            printf("Mesaj gönderilemedi. Hata Kodu: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        printf("Sunucuya gönderildi: %s\n", buffer);

        bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\\0';
            printf("Sunucudan gelen: %s\n", buffer);
        } else if (bytesReceived == 0) {
            printf("Baðlantý kapandý.\n");
            break;
        } else {
            printf("Alým iþlemi sýrasýnda hata oluþtu. Hata Kodu: %d\n", WSAGetLastError());
        }
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}

