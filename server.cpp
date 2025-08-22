#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib") // Link against WinSock
#else
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

// #include <unistd.h>
// #include <netinet/in.h>

std::vector<int> clients;
std::mutex clients_mutex;

void broadcast(const std::string& msg, int sender_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int client_fd : clients) {
        if (client_fd != sender_fd) {
            send(client_fd, msg.c_str(), msg.length(), 0);
        }
    }
}

void handle_client(int client_fd) {
    char buffer[1024];
    while (true) {
        int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;
        std::string msg(buffer, bytes);
        broadcast(msg, client_fd);
    }
    closesocket(client_fd);
    // Remove from clients list...
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{AF_INET, htons(9001), INADDR_ANY};
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    while (true) {
        sockaddr_in cli_addr;
        socklen_t len = sizeof(cli_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&cli_addr, &len);
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_fd);
        }
        std::thread(handle_client, client_fd).detach();
    }
}
