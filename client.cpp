// #include <iostream>
// #include <thread>
// #include <string>

// #ifdef _WIN32
// #include <winsock2.h>
// #include <ws2tcpip.h>
// #include <windows.h>
// #pragma comment(lib, "ws2_32.lib") // Link against WinSock
// #else
// #include <unistd.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #endif


// void receive_msgs(int sock_fd) {
//     char buffer[1024];
//     while (true) {
//         int bytes = recv(sock_fd, buffer, sizeof(buffer), 0);
//         if (bytes > 0) std::cout << "\n" << std::string(buffer, bytes) << std::endl;
//     }
// }

// int main() {

//     #ifdef _WIN32
//     WSADATA wsaData;
//     if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {

//         std::cerr << "WSAStartup failed\n";

//         return 1;
//     }
//     #endif


//     int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
//     sockaddr_in server_addr{AF_INET, htons(9001)};
//     server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//     connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

//     std::thread(receive_msgs, sock_fd).detach();

//     std::string input;
//     while (true) {
//         std::getline(std::cin, input);
//         send(sock_fd, input.c_str(), input.length(), 0);
//     }
//     #ifdef _WIN32
//     closesocket(sock_fd);
//     #else
//     close(sock_fd);
//     #endif

//     #ifdef _WIN32
//     WSACleanup();
//     #endif

// }

// client.cpp
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
        if (bytes == 0) {
            std::cout << "Disconnected from server" << std::endl;
            break;
        }
#ifdef _WIN32
        if (bytes == SOCKET_ERROR) {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
#else
        if (bytes < 0) {
            perror("recv failed");
            break;
        }
#endif
        std::string msg(buffer, bytes);
        std::cout << "\n" << msg << std::endl;
    }
#ifdef _WIN32
    closesocket(sock_fd);
#else
    close(sock_fd);
#endif
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
#endif

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
    if (sock_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
#else
    if (sock_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }
#endif

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9001);
#ifdef _WIN32
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
#else
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif

    if (
#ifdef _WIN32
        connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR
#else
        connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0
#endif
    ) {
#ifdef _WIN32
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(sock_fd);
        WSACleanup();
#else
        perror("Connection failed");
        close(sock_fd);
#endif
        return 1;
    }

    std::thread(receive_msgs, sock_fd).detach();

    std::string input;
    while (true) {
        std::getline(std::cin, input);
        if (input.empty()) continue; // ignore empty inputs
        int sent = send(sock_fd, input.c_str(), static_cast<int>(input.length()), 0);
#ifdef _WIN32
        if (sent == SOCKET_ERROR) {
            std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
#else
        if (sent < 0) {
            perror("send failed");
            break;
        }
#endif
    }

#ifdef _WIN32
    closesocket(sock_fd);
    WSACleanup();
#else
    close(sock_fd);
#endif

    return 0;
}
