#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <process.h> 
#include <locale.h>
#pragma comment(lib, "ws2_32.lib") 

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
    char name[50];
    int stock;
} Product;

Product products[5] = {
    {"Laptop", 10},
    {"Phone", 15},
    {"TV", 20},
    {"Keyboard", 5},
    {"Mouse", 25}
};

void handleClient(void* clientSocket) {
    SOCKET socket = *(SOCKET*)clientSocket;
    char buffer[BUFFER_SIZE];
    int bytesReceived;

    free(clientSocket); 

    while ((bytesReceived = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        printf("�stemciden gelen: %s\n", buffer);

        if (strcmp(buffer, "exit") == 0) {
            printf("Bir istemci ba�lant�s� sonland�r�ld�.\n");
            break;
        }

        char productName[50];
        int orderQuantity;
        sscanf(buffer, "%s %d", productName, &orderQuantity);

        char response[BUFFER_SIZE];
        int found = 0;
		int i;
        for ( i = 0; i < 5; i++) {
            if (strcmp(products[i].name, productName) == 0) {
                found = 1;
                if (products[i].stock >= orderQuantity) {
                    products[i].stock -= orderQuantity;
                    snprintf(response, BUFFER_SIZE, "Sipari� ba�ar�l�. %s stokta kalan: %d", products[i].name, products[i].stock);
                } else {
                    snprintf(response, BUFFER_SIZE, "Yetersiz stok. %s stokta kalan: %d", products[i].name, products[i].stock);
                }
                break;
            }
        }

        if (!found) {
            snprintf(response, BUFFER_SIZE, "�r�n bulunamad�: %s", productName);
        }

        int bytesSent = send(socket, response, strlen(response), 0);
        if (bytesSent == SOCKET_ERROR) {
            printf("Mesaj g�nderilemedi. Hata Kodu: %d\n", WSAGetLastError());
            break;
        } else {
            printf("�stemciye mesaj g�nderildi: %s\n", response);
        }
    }

    if (bytesReceived == SOCKET_ERROR) {
        printf("Al�m i�lemi s�ras�nda hata olu�tu. Hata Kodu: %d\n", WSAGetLastError());
    }

    closesocket(socket);
    printf("�stemci i�in ba�lant� kapat�ld�.\n");
    _endthread();
}

int main() {
	setlocale(LC_ALL, "Turkish");
    WSADATA wsaData;
    SOCKET serverSocket, *clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Winsock ba�lat�lamad�. Hata Kodu: %d\n", WSAGetLastError());
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Soket olu�turulamad�. Hata Kodu: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind i�lemi ba�ar�s�z oldu. Hata Kodu: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        printf("Listen i�lemi ba�ar�s�z oldu. Hata Kodu: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Sunucu ba�lat�ld�. Port: %d\n", SERVER_PORT);

    while (1) {
        clientAddrSize = sizeof(clientAddr);
        clientSocket = malloc(sizeof(SOCKET)); 
        if (!clientSocket) {
            printf("Bellek tahsisi ba�ar�s�z oldu.\n");
            break;
        }

        *clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (*clientSocket == INVALID_SOCKET) {
            printf("�stemci ba�lant�s� kabul edilemedi. Hata Kodu: %d\n", WSAGetLastError());
            free(clientSocket);
            continue;
        }

        printf("Bir istemci ba�land�.\n");

        _beginthread(handleClient, 0, (void*)clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    printf("Sunucu kapat�ld�.\n");

    return 0;
}

