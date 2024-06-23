#include <iostream>
#include <fstream>
#include <string>
#include <winsock2.h>
#include <thread>
#include <string>
#include <conio.h>
#include "ws2tcpip.h"
#include <WS2tcpip.h>
#include "Function.h"
// POP3Server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.


#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable : 4996)

#define DEFAULT_PORT "1025"
#define DEFAULT_BUFLEN 128
using namespace std;

int main()
{
	char sendbuf[DEFAULT_BUFLEN] = "+OK\r\n";
	char recvbuf[DEFAULT_BUFLEN] = "";
	int recvbuflen = DEFAULT_BUFLEN;
	int result;
	bool auth = false;

	setlocale(LC_ALL, "Russian");

	WSADATA wsaData;

	result = (WSAStartup(MAKEWORD(1, 0), &wsaData));
	if (result != 0) {
		printf("WSAStartup() failed with error: %d\n", result);
		_getch();
		return 1;
	}

	struct addrinfo* addrResult = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &addrResult);
	if (result != 0) {
		printf("getaddrinfo() failed: %d\n", result);
		WSACleanup();
		return 1;
	}

	SOCKET listenSocket = INVALID_SOCKET;

	listenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (listenSocket == INVALID_SOCKET) {
		printf("socket() failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}

	result = bind(listenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR) {
		printf("bind() failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(addrResult);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen() failed with error: %ld\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET clientSocket;
	while (true) {
		strcpy(sendbuf, "+OK\r\n");
		clientSocket = INVALID_SOCKET;
		clientSocket = accept(listenSocket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			printf("accept() failed with error: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		cout << "client conected";

		result = send(clientSocket, sendbuf, (int)strlen(sendbuf), 0);
		if (result == SOCKET_ERROR) {
			printf("send() failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

		char command[5] = "";
		cout << command << endl;
		char buf[64] = "";
		char end[3] = "";
		char login[32] = "";
		int i = 5;
		int access = 0;

		while (strcmp(command, "QUIT")) {
			//USER 0
			while ((access == 0) && strcmp(command, "QUIT")) {
				strncpy(command, "", 4);

				while (strcmp(command, "USER") && strcmp(command, "QUIT")) {
					strncpy(buf, "", 64);
					strncpy(recvbuf, "", DEFAULT_BUFLEN);
					strncpy(command, "", 4);
					while (strcmp(recvbuf, "\r\n")) {
						result = recv(clientSocket, recvbuf, recvbuflen, 0);
						strcat(buf, recvbuf);
					}
					strcpy(recvbuf, buf);
					if (result > 0)
						strncpy(command, recvbuf, 4);
					else {
						printf("recv failed: %d\n", WSAGetLastError());
						closesocket(clientSocket);
						WSACleanup();
						return 1;
					}
				}

				i = 5;
				end[0] = 0;
				end[1] = 0;
				strncpy(login, "", 32);

				if (strcmp(command, "QUIT")) {
					while (strcmp(end, "\r\n")) {
						login[i - 5] = recvbuf[i];
						login[i + 1] = 0;
						end[0] = recvbuf[i + 1];
						end[1] = recvbuf[i + 2];
						i++;
					}
					cout << login << endl;
					strcpy(sendbuf, "+OK\r\n");
					result = send(clientSocket, sendbuf, (int)strlen(sendbuf), 0);
				}
				else
					break;

				end[0] = 0;
				end[1] = 0;
				i = 5;
				char password[32] = "";
				//PASS 0
				while (strcmp(command, "PASS") && strcmp(command, "QUIT")) {
					strncpy(password, "", 32);
					strncpy(buf, "", 64);
					strncpy(recvbuf, "", DEFAULT_BUFLEN);
					strncpy(command, "", 4);
					while (strcmp(recvbuf, "\r\n") && (result > 0)) {
						result = recv(clientSocket, recvbuf, recvbuflen, 0);
						strcat(buf, recvbuf);
					}
					strcpy(recvbuf, buf);
					if (result > 0)
						strncpy(command, recvbuf, 4);
					else {
						printf("recv failed: %d\n", WSAGetLastError());
						closesocket(clientSocket);
						WSACleanup();
						return 1;
					}
				}
				if (strcmp(command, "QUIT")) {
					while (strcmp(end, "\r\n")) {
						password[i - 5] = recvbuf[i];
						password[i + 1] = 0;
						end[0] = recvbuf[i + 1];
						end[1] = recvbuf[i + 2];
						i++;
					}

					cout << password << endl;
					if (AUTH(login, password)) {
						cout << "AUTH +OK" << endl;
						strcpy(sendbuf, "+OK\r\n");
						access = 1;
						result = send(clientSocket, sendbuf, (int)strlen(sendbuf), 0);
					}
					else {
						access = 0;
						cout << "user not found" << endl;
						strcpy(sendbuf, "-ERR\r\n");
						result = send(clientSocket, sendbuf, (int)strlen(sendbuf), 0);
						break;
					}
				}
				else
					break;
			}

			//PASS 1

			while (strcmp(command, "QUIT") && (access == 1)) {
				strncpy(buf, "", 64);
				strncpy(recvbuf, "", DEFAULT_BUFLEN);
				strncpy(command, "", 4);
				while (strcmp(recvbuf, "\r\n")) {
					result = recv(clientSocket, recvbuf, recvbuflen, 0);
					strcat(buf, recvbuf);
				}
				strcpy(recvbuf, buf);
				if (result > 0)
					strncpy(command, recvbuf, 4);
				else {
					printf("recv failed: %d\n", WSAGetLastError());
					closesocket(clientSocket);
					WSACleanup();
					return 1;
				}

				i = 5;
				strncpy(buf, "", 64);
				end[0] = recvbuf[4];
				end[1] = recvbuf[5];
				while (strcmp(end, "\r\n")) {
					buf[i - 5] = recvbuf[i];
					buf[i + 1] = 0;
					end[0] = recvbuf[i + 1];
					end[1] = recvbuf[i + 2];
					i++;
				}

				//stat
				int stat[2];
				string c_mess = "";

				if (strcmp(command, "STAT") && strcmp(command, "LIST") && strcmp(command, "RETR")) {
					strcpy(sendbuf, "-ERR\r\n");
					result = send(clientSocket, sendbuf, (int)strlen(sendbuf), 0);
					continue;
				}

				if (!strcmp(command, "STAT"))
				{
					c_mess = "+OK";
					STAT(login, stat);
					c_mess = c_mess + to_string(stat[0]) + " " + to_string(stat[1]) + "\r\n";
					c_mess += ".\r\n";
					result = send(clientSocket, c_mess.c_str(), c_mess.size() + 1, 0);
				}

				//list
				if (!strcmp(command, "LIST"))
				{
					int* list = NULL;
					c_mess = "";
					list = LIST(login, list);
					if (!strcmp(buf, "")) {
						c_mess = "+OK" + to_string(sizeof(list)) + " messages (" + to_string(list[0]) + " octets)\r\n";
						for (int j = 1; list[j] != 0; j++)
						{
							c_mess = c_mess + to_string(j) + " " + to_string(list[j]) + "\r\n";
						}
						c_mess += ".\r\n";
						result = send(clientSocket, c_mess.c_str(), c_mess.size() + 1, 0);
					}
					else {
						int h = 0;
						for (int p = 0; list[p] != 0; p++)
							h++;
						if (atoi(buf) < h) {
							c_mess = c_mess + "+OK" + (string)buf + " " + to_string(list[atoi(buf)]) + "\r\n";
							result = send(clientSocket, c_mess.c_str(), c_mess.size() + 1, 0);
						}
						else {
							strcpy(sendbuf, "-ERR\r\n");
							result = send(clientSocket, sendbuf, (int)strlen(sendbuf), 0);
						}
					}
				}
				//retr
				if (!strcmp(command, "RETR"))
				{
					int* list = NULL;
					list = LIST(login, list);
					int k = atoi(buf);
					int h = 0;
					for (int p = 0; list[p] != 0; p++)
						h++;
					if (k < h) {//должно быть +OK специально изменил
						c_mess = "-ERR" + to_string(list[k]) + " octets" + "\r\n" + RETR(login, k) + ".\r\n";
						result = send(clientSocket, c_mess.c_str(), c_mess.size() + 1, 0);
					}
					else {
						strcpy(sendbuf, "-ERR\r\n");
						result = send(clientSocket, sendbuf, (int)strlen(sendbuf), 0);
					}
				}
			}
			if (!strcmp(command, "QUIT")) {
				//strcpy(sendbuf, "+OK dewey POP3 server signing off");
				//result = send(clientSocket, sendbuf, (int)strlen(sendbuf), 0);
				break;
			}
		}
		result = shutdown(clientSocket, SD_SEND);
		cout << "client disconected" << endl;
	}
	if (result == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	return 0;
}
