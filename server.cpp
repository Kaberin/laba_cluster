//server.cpp
#include <iostream>
#include <winsock2.h>
#include <vector>
#include <string>
#include <cstdint>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iomanip>
#include "Types.hpp"
#include "helper.hpp"
#include "libs/json.hpp"
using json = nlohmann::json;
constexpr auto PORT = 5000;

std::vector<SOCKET> aConnections;
int counter = 0;

std::vector<Response> allResponses;
std::mutex responseMutex;
int responsesReceived = 0;
int totalClients = 0;
std::condition_variable allResponsesReceived;

void ClientHandler(int index) {
    while (true) {
        std::string clientResponseString;
        if (!ReceiveString(aConnections[index], clientResponseString)) break;
        json clientResponse = json::parse(clientResponseString);
        Response resp;
        resp.pointsInCricle = clientResponse[POINTS_IN_CIRCLE];
        resp.totalPointsNumber = clientResponse[POINTS_TOTAL];
        resp.id = clientResponse[ID];

        {
            std::lock_guard<std::mutex> lock(responseMutex);
            allResponses.push_back(resp);
            responsesReceived++;
            if (responsesReceived == totalClients) {
                allResponsesReceived.notify_one();
            }
        }

    };
    shutdown(aConnections[index], SD_BOTH);
    closesocket(aConnections[index]);
}


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

    std::cout << "Current IPv4-adress: " << GetLocalIPv4() << '\n';
    SOCKET newConnection;
    std::cout << "Srever is working on port " << PORT << "\n";
    std::cout << "Enter number of slaves-pcs: ";
    int pcNumebr = 0;
    std::cin >> pcNumebr;
    for (int i = 0; i < pcNumebr; ++i) {
        newConnection = accept(s, reinterpret_cast<SOCKADDR*>(&addr), &size);
        if (newConnection == 0) {
            std::cerr << "Error...\n";
            return 1;
        }
        std::cout << "Client connected!\n";
        SendString(newConnection, "Connection with server established.\n");
        aConnections.push_back(newConnection);
        std::cout << "PC " << counter << " is now available.\n";
        counter++;
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)i, NULL, NULL);
    }
    totalClients = aConnections.size();
    std::cout << "All PCs connected! (" << aConnections.size() << "). How many points you want to calculate ? Enter number: ";
    uint64_t totalPoins = 0;
    std::cin >> totalPoins;

    auto pointsForEach = totalPoins / aConnections.size();
    int id = 0;
    for (auto conncetion : aConnections) {
        Request req{
            pointsForEach,
            id++
        };
        json j;
        j[POINTS_TOTAL] = req.totalPointsNumber;
        j[ID] = req.id;
        auto requestString = j.dump();
        SendString(conncetion, requestString);
    }
    std::unique_lock<std::mutex> lock(responseMutex);
    allResponsesReceived.wait(lock, [] { return responsesReceived == totalClients; });
    std::cout << "All responses received!\n";

    uint64_t pointsInCircleTotal = 0, totalPoints = 0;
    for (auto& resp : allResponses) {
        pointsInCircleTotal += resp.pointsInCricle;
        totalPoints += resp.totalPointsNumber;
    }

    double piEstimate = 4.0 * static_cast<double>(pointsInCircleTotal) / static_cast<double>(totalPoints);
    std::cout << "Estimated PI = " << std::setprecision(10) << piEstimate << "\n";

    system("pause");
    closesocket(s);
    WSACleanup();
    return 0;
}