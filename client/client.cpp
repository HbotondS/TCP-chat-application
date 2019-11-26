#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "winsock2.h"
#include <Ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

void main() {
	// initialize thw Winsock library
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(iResult != NO_ERROR) {
		printf("Error with WSAStartup()\n");
	}

	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(ClientSocket == INVALID_SOCKET) {
		printf("creating the socket failed with the following code: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	// declare the IP & port where we want to connect
	int Port = 13;
	char IP[10] = "127.0.0.1";
	sockaddr_in ServerAddr;
	int AddrLen = sizeof(ServerAddr);
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(Port);
	inet_pton(AF_INET, IP, &ServerAddr.sin_addr.s_addr);

	// connect to server
	if(connect(ClientSocket, (SOCKADDR*) &ServerAddr, AddrLen) == SOCKET_ERROR) {
		int errorCode = WSAGetLastError();
		if(errorCode == 10061) {
			printf("Server is offline, please try again later...\n");
		} else {
			printf("connect() failed with the following code: %ld\n", errorCode);
		}
		WSACleanup();
		return;
	}
	while(1) {
		const int SendBufLen = 1024;
		char SendBuf[SendBufLen];
		int DataLen;
		gets_s(SendBuf, SendBufLen);
		DataLen = strlen(SendBuf) + 1;
		iResult = send(ClientSocket, SendBuf, DataLen, 0);
		if(iResult == SOCKET_ERROR) {
			int errorCode = WSAGetLastError();
			if(errorCode == 10054) {
				printf("Server is offline, please try again later...\n");
			} else {
				printf("send() failed with the following code: %d\n", WSAGetLastError());
			}
			closesocket(ClientSocket);
			WSACleanup();
			return;
		}

		const int RecBufLen = 1024;
		char RecBuf[RecBufLen];
		iResult = recv(ClientSocket, RecBuf, RecBufLen - 1, 0);
		if(iResult == SOCKET_ERROR) {
			printf("recv() failed with the following code: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return;
		}

		// if we closed the socket
		if(iResult == 0) {
			return;
		}
		// print message
		RecBuf[iResult] = '\0';
		printf("%s \n", RecBuf);
	}
	WSACleanup();
	return;
}
