//client.cpp
#include <iostream>
#include <winsock2.h>
#include <string>
#include <sstream>
#include <thread>
#include "helper.hpp"
#include "libs/json.hpp"
#include "Types.hpp"
#include "Task.hpp"
using json = nlohmann::json;
constexpr auto PORT = 5000;
SOCKET Connectcion;

void ClientHandler() {
    while (true) {
        std::string requestString;
        if (!ReceiveString(Connectcion, requestString)) break;
        Request serverRequset;
        json jreq = json::parse(requestString);
        serverRequset.totalPointsNumber = jreq[POINTS_TOTAL];
        serverRequset.id = jreq[ID];
        Response clientResponse = PiWithMonteCarlo(serverRequset);
        json jresp;
        jresp[POINTS_IN_CIRCLE] = clientResponse.pointsInCricle;
        jresp[POINTS_TOTAL] = clientResponse.totalPointsNumber;
        jresp[ID] = clientResponse.id;
        std::string responseString = jresp.dump();
        std::cout << "Client response:\n" << jresp.dump(4);
        SendString(Connectcion, responseString);
    }
    std::cout << "Server disconnected.\n";
}


int main() {
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 1), &data) != 0) {
        LOG_ERROR("Error loading winsock library.");
        return 1;
    }
    std::string adress{};
    std::cout << "Enter ipv4 adress of server: ";
    std::getline(std::cin, adress);
    std::cout << "Entered adress: " << adress << '\n';
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

    std::thread clientThread(ClientHandler);
    //CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

    // std::string msg;
    // while (true) {
        //     std::getline(std::cin, msg);
        //     SendString(Connectcion, msg);
        //     Sleep(10);
        // }

    clientThread.join();
    system("pause");
    return 0;
}