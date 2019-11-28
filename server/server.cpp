#include <stdio.h>
#include <string.h>
#include "winsock2.h"
#include <Ws2tcpip.h>
#include <iostream>
#include "MyThread.h"
#include <vector>

#pragma comment(lib, "ws2_32.lib")

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
	int Port = 13;
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
	std::vector<SOCKET>* clients = new std::vector<SOCKET>();
	printf("Waiting for clients to connect...\n");

	while(1) {
		// Accept the connection.
		AcceptSocket = accept(ListenSocket, NULL, NULL);
		if(AcceptSocket == INVALID_SOCKET) {
			printf("accept failed: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return;
		} else {
			std::cout << "Client " << AcceptSocket << " is connected" << std::endl;
		}

		clients->push_back(AcceptSocket);
		MyThread* myThread = new MyThread(AcceptSocket, clients);
		myThread->start();
	}

	printf("Exiting.\n");
	WSACleanup();
	return;
}
