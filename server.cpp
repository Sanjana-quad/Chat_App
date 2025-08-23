// #include <iostream>
// #include <thread>
// #include <vector>
// #include <mutex>
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

// // #include <unistd.h>
// // #include <netinet/in.h>

// std::vector<int> clients;
// std::mutex clients_mutex;

// void broadcast(const std::string& msg, int sender_fd) {
//     std::lock_guard<std::mutex> lock(clients_mutex);
//     for (int client_fd : clients) {
//         if (client_fd != sender_fd) {
//             send(client_fd, msg.c_str(), msg.length(), 0);
//         }
//     }
// }

// void handle_client(int client_fd) {
//     char buffer[1024];
//     while (true) {
//         int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
//         if (bytes <= 0) break;
//         std::string msg(buffer, bytes);
//         broadcast(msg, client_fd);
//     }
//     #ifdef _WIN32
//     closesocket(client_fd);
//     #else
//     close(client_fd);
//     #endif

// }

// int main() {
//     #ifdef _WIN32
// WSADATA wsaData;
// if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//     std::cerr << "WSAStartup failed\n";
//     return 1;
// }
// #endif


//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     sockaddr_in address{AF_INET, htons(9001), INADDR_ANY};
//     bind(server_fd, (struct sockaddr*)&address, sizeof(address));
//     listen(server_fd, 5);

//     while (true) {
//         sockaddr_in cli_addr;
//         socklen_t len = sizeof(cli_addr);
//         int client_fd = accept(server_fd, (struct sockaddr*)&cli_addr, &len);
//         {
//             std::lock_guard<std::mutex> lock(clients_mutex);
//             clients.push_back(client_fd);
//         }
//         std::thread(handle_client, client_fd).detach();
//     }
//     #ifdef _WIN32
//     WSACleanup();
//     #endif

// }

// server.cpp







// #include <iostream>
// #include <thread>
// #include <vector>
// #include <mutex>
// #include <string>
// #include <algorithm>

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

// std::vector<int> clients;
// std::mutex clients_mutex;

// void broadcast(const std::string& msg, int sender_fd) {
//     std::lock_guard<std::mutex> lock(clients_mutex);
//     for (int client_fd : clients) {
//         if (client_fd != sender_fd) {
//             int sent = send(client_fd, msg.c_str(), static_cast<int>(msg.length()), 0);
//             if (sent == -1
// #ifdef _WIN32
//                 || sent == SOCKET_ERROR
// #endif
//             ) {
// #ifdef _WIN32
//                 std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
// #else
//                 perror("send failed");
// #endif
//             }
//         }
//     }
// }

// void handle_client(int client_fd) {
//     char buffer[1024];
//     while (true) {
//         int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
//         if (bytes == 0) {
//             std::cout << "Client disconnected" << std::endl;
//             break;
//         }
// #ifdef _WIN32
//         if (bytes == SOCKET_ERROR) {
//             std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
//             break;
//         }
// #else
//         if (bytes < 0) {
//             perror("recv failed");
//             break;
//         }
// #endif
//         std::string msg(buffer, bytes);
//         std::cout << "Received from client: " << msg << std::endl;
//         broadcast(msg, client_fd);
//     }

// #ifdef _WIN32
//     closesocket(client_fd);
// #else
//     close(client_fd);
// #endif

//     {
//         std::lock_guard<std::mutex> lock(clients_mutex);
//         clients.erase(std::remove(clients.begin(), clients.end(), client_fd), clients.end());
//     }
// }

// int main() {
// #ifdef _WIN32
//     WSADATA wsaData;
//     if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//         std::cerr << "WSAStartup failed\n";
//         return 1;
//     }
// #endif

//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
// #ifdef _WIN32
//     if (server_fd == INVALID_SOCKET) {
//         std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
//         WSACleanup();
//         return 1;
//     }
// #else
//     if (server_fd < 0) {
//         perror("Socket creation failed");
//         return 1;
//     }
// #endif

//     sockaddr_in address{};
//     address.sin_family = AF_INET;
//     address.sin_port = htons(9001);
//     address.sin_addr.s_addr = INADDR_ANY;

//     if (
// #ifdef _WIN32
//         bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR
// #else
//         bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0
// #endif
//     ) {
// #ifdef _WIN32
//         std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
//         closesocket(server_fd);
//         WSACleanup();
// #else
//         perror("Bind failed");
//         close(server_fd);
// #endif
//         return 1;
//     }


//     if (
// #ifdef _WIN32
//         listen(server_fd, 5) == SOCKET_ERROR
// #else
//         listen(server_fd, 5) < 0
// #endif
//     ) {
// #ifdef _WIN32
//         std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
//         closesocket(server_fd);
//         WSACleanup();
// #else
//         perror("Listen failed");
//         close(server_fd);
// #endif
//         return 1;
//     }

//     std::cout << "Server started and listening on port 9001" << std::endl;

//     while (true) {
//         sockaddr_in cli_addr{};
//         socklen_t len = sizeof(cli_addr);
//         int client_fd =
// #ifdef _WIN32
//             accept(server_fd, (struct sockaddr*)&cli_addr, &len);
// #else
//             accept(server_fd, (struct sockaddr*)&cli_addr, &len);
// #endif

// #ifdef _WIN32
//         if (client_fd == INVALID_SOCKET) {
//             std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
//             continue;
//         }
// #else
//         if (client_fd < 0) {
//             perror("Accept failed");
//             continue;
//         }
// #endif

//         {
//             std::lock_guard<std::mutex> lock(clients_mutex);
//             clients.push_back(client_fd);
//         }

//         std::thread(handle_client, client_fd).detach();
//     }

// #ifdef _WIN32
//     closesocket(server_fd);
//     WSACleanup();
// #else
//     close(server_fd);
// #endif

//     return 0;
// }




#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <string>
#include <algorithm>
#include <unordered_map>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

std::vector<int> clients;
std::mutex clients_mutex;
std::unordered_map<int, std::string> client_nicknames;

void broadcast(const std::string& msg, int sender_fd = -1) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int client_fd : clients) {
        if (client_fd != sender_fd) {
            send(client_fd, msg.c_str(), static_cast<int>(msg.length()), 0);
        }
    }
}

void handle_client(int client_fd) {
    char buffer[1024];

    // Step 1: Receive nickname
    int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes <= 0) {
#ifdef _WIN32
        closesocket(client_fd);
#else
        close(client_fd);
#endif
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.erase(std::remove(clients.begin(), clients.end(), client_fd), clients.end());
        }
        return;
    }
    std::string nickname(buffer, bytes);
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        client_nicknames[client_fd] = nickname;
    }

    std::string join_msg = "*** " + nickname + " has joined the chat. ***";
    std::cout << join_msg << std::endl;
    broadcast(join_msg + "\n");

    // Step 2: Receive chat messages
    while (true) {
        bytes = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;
        std::string msg(buffer, bytes);

        std::string full_msg;
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            auto it = client_nicknames.find(client_fd);
            if (it != client_nicknames.end()) {
                full_msg = it->second + ": " + msg;
            } else {
                full_msg = "Unknown: " + msg;
            }
        }

        std::cout << full_msg << std::endl;
        broadcast(full_msg + "\n", client_fd);
    }

    // Client disconnected
    std::string left_nick;
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        left_nick = client_nicknames[client_fd];
        client_nicknames.erase(client_fd);
        clients.erase(std::remove(clients.begin(), clients.end(), client_fd), clients.end());
    }
    std::string leave_msg = "*** " + left_nick + " has left the chat. ***";
    std::cout << leave_msg << std::endl;
    broadcast(leave_msg + "\n");

#ifdef _WIN32
    closesocket(client_fd);
#else
    close(client_fd);
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

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
#else
    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }
#endif

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(9001);
    address.sin_addr.s_addr = INADDR_ANY;

    if (
#ifdef _WIN32
        bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR
#else
        bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0
#endif
    ) {
#ifdef _WIN32
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
#else
        perror("Bind failed");
        close(server_fd);
#endif
        return 1;
    }

    if (
#ifdef _WIN32
        listen(server_fd, 5) == SOCKET_ERROR
#else
        listen(server_fd, 5) < 0
#endif
    ) {
#ifdef _WIN32
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
#else
        perror("Listen failed");
        close(server_fd);
#endif
        return 1;
    }

    std::cout << "Server started and listening on port 9001" << std::endl;

    while (true) {
        sockaddr_in cli_addr{};
        socklen_t len = sizeof(cli_addr);
        int client_fd =
#ifdef _WIN32
            accept(server_fd, (struct sockaddr*)&cli_addr, &len);
#else
            accept(server_fd, (struct sockaddr*)&cli_addr, &len);
#endif

#ifdef _WIN32
        if (client_fd == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }
#else
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }
#endif

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_fd);
        }

        std::thread(handle_client, client_fd).detach();
    }

#ifdef _WIN32
    closesocket(server_fd);
    WSACleanup();
#else
    close(server_fd);
#endif

    return 0;
}
