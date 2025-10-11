//helper.hpp
#pragma once
#include <winsock2.h>
#include <sstream>

bool ValidateIP(const std::string& ip) {
    std::stringstream s(ip);
    int part;
    char dot;
    for (auto i = 0; i < 4; i++) {
        if (!(s >> part)) return false;
        if (part > 255 || part < 0) return false;

        if (i < 3) {
            if (!(s >> dot)) return false;
            if (dot != '.') return false;
        }
    }
    if (s >> dot) return false;
    return true;
}

void LOG_ERROR(const std::string& message) {
    std::cerr << "Error: " << message << '\n';
}


bool sendAll(SOCKET s, const char* data, int totalBytes) {
    int sent = 0;
    while (sent < totalBytes) {
        int res = send(s, data + sent, totalBytes - sent, 0);
        if (res <= 0) return false;
        sent += res;
    }
    return true;
}

bool recvAll(SOCKET s, char* data, int totalBytes) {
    int received = 0;
    while (received < totalBytes) {
        int res = recv(s, data + received, totalBytes - received, 0);
        if (res <= 0) return false;
        received += res;
    }
    return true;
}

bool SendString(SOCKET s, const std::string& msg) {
    int msg_size = static_cast<int>(msg.size());
    if (!sendAll(s, reinterpret_cast<char*>(&msg_size), sizeof(int))) return false;
    if (!sendAll(s, msg.c_str(), msg_size)) return false;
    return true;
}

bool ReceiveString(SOCKET s, std::string& out) {
    int msg_size = 0;
    if (!recvAll(s, reinterpret_cast<char*>(&msg_size), sizeof(int))) {
        LOG_ERROR("Client disconnected or error occured");
        return false;
    }

    if (msg_size <= 0 || msg_size > 10'000'000) {
        std::cerr << "Invalid msg_size: " << msg_size << std::endl;
        return false;
    }

    std::string buffer(msg_size, '\0');
    if (!recvAll(s, buffer.data(), msg_size)) return false;

    out = std::move(buffer);
    return true;
}