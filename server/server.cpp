#include <stdio.h>
#include "winsock2.h"
#include <Ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>
#include "MyThread.h"
#include "Client.h"

#pragma comment(lib, "ws2_32.lib")

std::vector<std::string> split(const char* str, char c = ' ') {
	std::vector<std::string> result;

	do {
		const char* begin = str;

		while(*str != c && *str)
			str++;

		result.push_back(std::string(begin, str));
	} while(0 != *str++);

	return result;
}

void main() {
	// initialize thw Winsock library
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(iResult != NO_ERROR) {
		printf("Error with WSAStartup()\n");
	}

	SOCKET ListenSocket;
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(ListenSocket == INVALID_SOCKET) {
		printf("creating the socket failed with the following code: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	// declare the IP & port where we want to connect
	int Port = 3000;
	char IP[10] = "127.0.0.1";
	sockaddr_in service;
	int AddrLen = sizeof(service);
	service.sin_family = AF_INET;
	service.sin_port = htons(Port);

	int s = inet_pton(AF_INET, IP, &service.sin_addr.s_addr);

	if(bind(ListenSocket, (SOCKADDR*) &service, AddrLen) == SOCKET_ERROR) {
		printf("bind() failed with code %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	// Listen for incoming connection requests.
	// on the created socket
	if(listen(ListenSocket, 1) == SOCKET_ERROR) {
		printf("Error listening on socket.\n");
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	SOCKET AcceptSocket;
	std::vector<Client>* clients = new std::vector<Client>();
	printf("Waiting for clients to connect...\n");

	while(1) {
		// Accept the connection.
		AcceptSocket = accept(ListenSocket, NULL, NULL);
		std::string clientName;
		if(AcceptSocket == INVALID_SOCKET) {
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return;
		} else {
			char RecvBuf[1024] = "";
			int BufLen = 1024;
			iResult = recv(AcceptSocket, RecvBuf, BufLen - 1, 0);
			std::cout << RecvBuf << std::endl;
			std::vector<std::string> splittedMsg = split(RecvBuf, '|');
			clientName = splittedMsg.at(1);
			if(splittedMsg.at(0) == "join") {
				std::cout << "Client " << clientName << " is connected" << std::endl;
				// send message to the gourp that a new client is connected
				std::string result;
				result = clientName + " is joind the chat.\n";
				strcpy_s(RecvBuf, result.c_str());
				for(auto client = clients->begin(); client != clients->end(); ++client) {
					send(client->socket, RecvBuf, strlen(RecvBuf), 0);
				}
			}
		}

		clients->push_back(Client(AcceptSocket, clientName));
		MyThread* myThread = new MyThread(AcceptSocket, clients);
		myThread->start();
	}

	printf("Exiting.\n");
	WSACleanup();
	return;
}
