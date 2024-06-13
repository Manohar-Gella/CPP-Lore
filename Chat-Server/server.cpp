#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

std::vector<int> clients;

void BroadcastMessage(const char* message, int length)
{
    for (int client : clients) {
        send(client, message, length, 0);
    }
}

void HandleClient(int clientSocket)
{
    char buffer[256];
    int result;
    while (true) {
        result = recv(clientSocket, buffer, 256, 0);
        if (result > 0) {
            buffer[result] = '\0'; // Ensure null-terminated string
            BroadcastMessage(buffer, result);
        } else if (result == 0) {
            // Connection closed
            close(clientSocket);
            clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
            break;
        } else {
            std::cerr << "Receive failed" << std::endl;
            close(clientSocket);
            clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
            break;
        }
    }
}

int main()
{
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(1234);

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Bind failed" << std::endl;
        close(listenSocket);
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == -1) {
        std::cerr << "Listen failed" << std::endl;
        close(listenSocket);
        return 1;
    }

    std::cout << "Server started. Waiting for connections..." << std::endl;

    while (true) {
        int clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            std::cerr << "Accept failed" << std::endl;
            close(listenSocket);
            return 1;
        }

        clients.push_back(clientSocket);
        std::thread clientThread(HandleClient, clientSocket);
        clientThread.detach();
    }

    close(listenSocket);
    return 0;
}
