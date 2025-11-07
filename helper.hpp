//helper.hpp
#pragma once
#include <winsock2.h>
#include <sstream>
#include <string>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <vector>


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


std::string GetLocalIPv4() {
    ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
        GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME;

    ULONG bufLen = 15 * 1024;
    std::vector<char> buffer(bufLen);

    IP_ADAPTER_ADDRESSES* adapters = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());

    if (GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, adapters, &bufLen) != NO_ERROR)
        return;                                                                           

    for (auto* adapter = adapters; adapter != nullptr; adapter = adapter->Next) {
        if (adapter->OperStatus != IfOperStatusUp ||
            adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK)
            continue;

        for (auto* addr = adapter->FirstUnicastAddress; addr != nullptr; addr = addr->Next) {
            if (addr->Address.lpSockaddr->sa_family == AF_INET) { // IPv4
                char ipStr[INET_ADDRSTRLEN];
                auto sin = reinterpret_cast<sockaddr_in*>(addr->Address.lpSockaddr);

                if (inet_ntop(AF_INET, &sin->sin_addr, ipStr, sizeof(ipStr)))
                    return ipStr;
            }
        }
    }

    return {}; 
}