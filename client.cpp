//client.cpp
#include <iostream>
#include <winsock2.h>
#include <string>
#include <sstream>
#include "helper.hpp"
constexpr auto PORT = 5000;
SOCKET Connectcion;

void ClientHandler() {
    int msg_size;
    while (true) {
        std::string message;
        if (!ReceiveString(Connectcion, message)) break;
        std::cout << message;
    }
    std::cout << "Server disconnected.\n";
}


int main() {
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 1), &data) != 0) {
        LOG_ERROR("Error loading winsock library.");
        return 1;
    }
    std::string adress{ "192.168.0.21" };
    std::cout << "(HARDCODED IP NOW) Enter ipv4 adres of server: " << adress << '\n';
    //std::getline(std::cin, adress);
    //std::cout << "Entered adress: " << adress << '\n';
    if (!ValidateIP(adress)) {
        LOG_ERROR("This IP is invalid.");
        system("pause");
        return 1;
    }
    SOCKADDR_IN addr{};
    addr.sin_addr.S_un.S_addr = inet_addr(adress.c_str());
    addr.sin_port = htons(PORT);
    addr.sin_family = AF_INET;

    Connectcion = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(Connectcion, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)) != 0) {
        LOG_ERROR("Error connection to the server.");
        system("pause");
        return 1;
    };

    std::string serverMessage;
    if (!ReceiveString(Connectcion, serverMessage)) {
        LOG_ERROR("Server disconnected before sending greeting.");
        return 1;
    }

    std::cout << serverMessage;

    std::string username;
    std::getline(std::cin, username);
    SendString(Connectcion, username);

    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

    std::string msg;
    while (true) {
        std::getline(std::cin, msg);
        SendString(Connectcion, msg);
        Sleep(10);
    }

    system("pause");
    return 0;
}