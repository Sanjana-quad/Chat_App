#include <iostream>
#include <thread>
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


void receive_msgs(int sock_fd) {
    char buffer[1024];
    while (true) {
        int bytes = recv(sock_fd, buffer, sizeof(buffer), 0);
        if (bytes > 0) std::cout << "\n" << std::string(buffer, bytes) << std::endl;
    }
}

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{AF_INET, htons(9001)};
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    std::thread(receive_msgs, sock_fd).detach();

    std::string input;
    while (true) {
        std::getline(std::cin, input);
        send(sock_fd, input.c_str(), input.length(), 0);
    }
    closesocket(sock_fd);
}
