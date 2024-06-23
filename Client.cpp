#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <cstdlib> // для использования rand() и srand()
#include <ctime>   // для инициализации генератора случайных чисел
#include <windows.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <map>
#include <vector>
#include <fstream>
#include "Tree.h"

const int port = 1025;
#pragma comment(lib, "ws2_32.lib")


int main() {
    std::string FSMpr = "Properties.txt";
    FSM fsm(FSMpr);

    std::vector<std::vector<unsigned int>> ht;
    Node* root = fsm.createTree(ht);
    fsm.setRoot(root);
    fsm.printTree();

    while (true) {
        setlocale(0, "rus");
        // Initialize socket
        WSAData data;
        int wsOk = WSAStartup(MAKEWORD(2, 2), &data);
        if (wsOk != 0) {
            std::cout << "Can't initialize WinSock" << std::endl;
            return 0;
        }

        // Initialize socket id
        SOCKET sid = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        // Address
        std::string ipAddress = "127.0.0.1";
        inet_pton(AF_INET, ipAddress.c_str(), &addr.sin_addr); // convert string to ip

        // Connect to server
        int conn = connect(sid, (sockaddr*)&addr, sizeof(addr));
        if (conn == SOCKET_ERROR) {
            std::cerr << "Can't connect to server" << std::endl;
            closesocket(sid);
            WSACleanup();
            return 0;
        }

        // Send and receive data
        char buf[4096];
        std::string usertrue = "maksyks"; // правильный username
        std::string passtrue = "123";     // правильный password
        srand(static_cast<unsigned int>(time(0))); // инициализация генератора случайных чисел
        std::string rn = "\r\n";

        // Successful connection
        ZeroMemory(buf, 4096);
        int byteRecv = recv(sid, buf, 4096, 0);
        if (byteRecv == SOCKET_ERROR) {
            std::cout << "Error in recv()" << std::endl;
            closesocket(sid);
            WSACleanup();
            return 0;
        }
        if (byteRecv == 0) {
            std::cout << "Server disconnected" << std::endl;
            closesocket(sid);
            WSACleanup();
            return 0;
        }
        std::cout << "server> " << std::string(buf, 0, byteRecv) << std::endl;
        std::string user;
        std::vector<std::pair<int, int>> io_pairs;
        // Отправка Username и Password
        do {
            // USER
            user = "USER ";
            if (rand() % 100 <= 70) { // 70% шанс отправить правильный user
                user += usertrue;
            }
            else {
                user += "mword"; // неправильный user
            }
            send(sid, user.c_str(), user.size() + 1, 0);            // two pack as telnet
            send(sid, rn.c_str(), rn.size() + 1, 0);				// send \r\n
            std::cout << "client> " << user << std::endl;
            ZeroMemory(buf, 4096);
            byteRecv = recv(sid, buf, 4096, 0);
            std::cout << "server> " << std::string(buf, 0, byteRecv) << std::endl;

            ////////////
            std::string command = user.substr(user.find(' ') + 1);
            int input = input_map[command];
            std::string response = std::string(buf, 0, byteRecv).substr(0, 3);;
            int output = output_map[response];
            if (input != -1 && output != -1) {
                io_pairs.emplace_back(input, output);
            }
            int server_status = fsm.traverseTree(io_pairs);
            ////////////

            Sleep(200);
            // PASS
            std::string pass = "PASS ";
            if (rand() % 100 <= 70) { // 70% шанс отправить правильный пароль
                pass += passtrue;
            }
            else {
                pass += "0000"; // неправильный пароль
            }
            send(sid, pass.c_str(), pass.size() + 1, 0);            // two pack as telnet
            send(sid, rn.c_str(), rn.size() + 1, 0);				// send \r\n
            std::cout << "client> " << pass << std::endl;
            ZeroMemory(buf, 4096);
            byteRecv = recv(sid, buf, 4096, 0);
            std::cout << "server> " << std::string(buf, 0, byteRecv) << std::endl;

            ////////////
            {
                std::string command = pass.substr(pass.find(' ') + 1);
                int input = input_map[command];
                std::string response = std::string(buf, 0, byteRecv).substr(0, 3);;
                int output = output_map[response];
                if (input != -1 && output != -1) {
                    io_pairs.emplace_back(input, output);
                }
                int server_status = fsm.traverseTree(io_pairs);
            }
            ////////////

        } while (std::strncmp(buf, "+OK", 3) != 0); // повторяем пока не получим +OK

        Sleep(200);
        // Остальные команды
        std::string cmd;
        do {
            std::string commands[] = { "RETR", "LIST", "QUIT", "STAT", "wrong_input" };
            std::string cmd = commands[rand() % 5];
            if (rand() % 100 <= 70) { // 70% шанс отправить правильную команду
                if (cmd == "wrong_input") {
                    cmd = commands[rand() % 5]; // отправляем RETR вместо wrong_input
                }
            }
            else {
                cmd = "wrong_input"; // неправильная команда
            }
            send(sid, cmd.c_str(), cmd.size(), 0);   // send commands
            send(sid, rn.c_str(), rn.size() + 1, 0);     // two pack as telnet
            std::cout << "client> " << cmd << std::endl;
            ZeroMemory(buf, 4096);
            byteRecv = recv(sid, buf, 4096, 0);
            std::cout << "server> " << std::string(buf, 0, byteRecv) << std::endl;

            ////////////
            {
                int input = input_map[cmd];
                std::string response = std::string(buf, 0, byteRecv).substr(0, 3);;
                int output = output_map[response];
                if (input != -1 && output != -1) {
                    io_pairs.emplace_back(input, output);
                }
                int server_status = fsm.traverseTree(io_pairs);
            }
            ////////////

            if (cmd == "QUIT")     // заканчиваем если QUIT
                break;

            // Check for the end of multi-line response
            std::string response(buf, byteRecv);
            while (response.find("\r\n.\r\n") == std::string::npos) {
                if (!std::strncmp(buf, "-ERR", 4)) // повторяем если неправильная команда
                    break;
                ZeroMemory(buf, 4096);
                byteRecv = recv(sid, buf, 4096, 0); // повторное получение
                response.append(buf, byteRecv);
                std::cout << std::string(buf, 0, byteRecv) << std::endl;
            }
        } while (cmd != "QUIT");

        closesocket(sid);
        WSACleanup();
        Sleep(200);
    }
    return 0;
}
