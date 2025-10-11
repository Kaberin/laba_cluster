//server.cpp
#include <iostream>
#include <winsock2.h>
#include <vector>
#include <string>

#include "helper.hpp"
constexpr auto PORT = 5000;
constexpr auto MAX_CONNECTIONS = 100;

struct UserInfo {
    std::string name;
    SOCKET connection;
};

std::vector<UserInfo> aConnections(MAX_CONNECTIONS);
int counter = 0;


void ClientHandler(int index) {
    while (true) {
        std::string message;
        if (!ReceiveString(aConnections[index].connection, message)) break;
        for (int i = 0; i < counter; i++) {
            if (i == index) {
                continue;
            }
            SendString(aConnections[i].connection, aConnections[index].name + std::string(": "));
            SendString(aConnections[i].connection, message + std::string("\n"));
        }
    };
    shutdown(aConnections[index].connection, SD_BOTH);
    closesocket(aConnections[index].connection);
}
UserInfo userInfo;

int main() {
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 1), &data) != 0) {
        std::cerr << "Error loading winsock library...\n";
        return 1;
    }

    SOCKADDR_IN addr{};
    addr.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");
    addr.sin_port = htons(PORT);
    addr.sin_family = AF_INET;

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    bind(s, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr));
    listen(s, SOMAXCONN);
    int size = sizeof(addr);

    SOCKET newConnection;
    std::cout << "Srever is working on port " << PORT << "\n";
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        newConnection = accept(s, reinterpret_cast<SOCKADDR*>(&addr), &size);
        if (newConnection == 0) {
            std::cerr << "Error...\n";
            return 1;
        }
        std::cout << "Client connected!\n";
        SendString(newConnection, "Hello, you connected to test server!\n Please, enter your username: ");
        std::string username;
        ReceiveString(newConnection, username);
        aConnections[i].connection = newConnection;
        aConnections[i].name = username;
        std::cout << "User " << aConnections[i].name << " is now available\n";
        counter++;
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)i, NULL, NULL);
    }
    system("pause");
    closesocket(s);
    WSACleanup();
    return 0;
}