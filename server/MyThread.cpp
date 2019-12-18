#include "MyThread.h"
#include <limits.h>
#include <windows.h>
#include <string>
#include <algorithm>

#define INVALID_HANDLE_VALUE 0

const unsigned int MyThread::INFINIT_WAIT = UINT_MAX;


DWORD WINAPI runStub(LPVOID mthread) {
	MyThread* pThread = static_cast<MyThread*>(mthread);
	pThread->m_bRunning = true;
	pThread->run();
	pThread->m_bRunning = false;
	pThread->m_bExited = true;
	return 0;
}

MyThread::MyThread(SOCKET ClientSocket, std::vector<Client>* clients): ClientSocket(ClientSocket), clients(clients) {
	m_bRunning = false;
	m_bExited = true;
	m_thread = INVALID_HANDLE_VALUE;
}

MyThread::~MyThread() {}

bool MyThread::start(void) {
	if(m_bExited) {
		m_bExited = false;
		DWORD dw;
		if((m_thread = CreateThread(NULL, 4096, runStub, this, 0,
			&dw)) == INVALID_HANDLE_VALUE) {
			m_bRunning = false;
			m_bExited = true;
			return false;
		}
	}
	return true;
}

bool MyThread::stop(unsigned int timeout) {
	m_bRunning = false;
	if(!m_bExited) {
		for(unsigned int i = 0; (i <= timeout / 100) || (timeout ==
			INFINIT_WAIT); i++) {
			m_bRunning = false;
			if(m_bExited) {
				break;
			}
			Sleep(10);
		}
	}
	if(m_thread != INVALID_HANDLE_VALUE) {
		CloseHandle(m_thread);
		m_thread = INVALID_HANDLE_VALUE;
	}
	return m_bExited;
}
void MyThread::run(void) {
	char SendBuf[1024];
	char RecvBuf[1024] = "";
	int BufLen = 1024;
	int iResult;
	while(1) {
		std::cout << "Receiving datagrams...\n";
		iResult = recv(ClientSocket, RecvBuf, BufLen - 1, 0);
		if(iResult == SOCKET_ERROR) {
			int errorCode = WSAGetLastError();
			if(errorCode == 10054) {
				std::string result;
				// remove the user from client list who left the chat
				for(auto client = clients->begin(); client != clients->end(); ++client) {
					if(client->socket == ClientSocket) {
						result = client->nickname + " left the chat.\n";
						clients->erase(client);
						break;
					}
				}
				printf("Client %d is disconnected\n", ClientSocket);
				// send message to the gourp that a new client is disconnected
				char RecvBuf[1024] = "";
				strcpy_s(RecvBuf, result.c_str());
				for(auto client = clients->begin(); client != clients->end(); ++client) {
					send(client->socket, RecvBuf, strlen(RecvBuf), 0);
				}
			} else {
				printf("recv() failed with the following code: %d\n", errorCode);
			}
			return;
		}

		// the server send the message back to the client
		char tempBuf[1024];
		strcpy_s(tempBuf, RecvBuf);
		std::string buf = tempBuf, result;
		for(auto client = clients->begin(); client != clients->end(); ++client) {
			std::cout << "Sending a datagram to " << client->socket << std::endl;
			if(ClientSocket == client->socket) {
				result = "Me: " + buf;
			} else {
				result = client->nickname + ": " + buf;
			}
			strcpy_s(tempBuf, result.c_str());
			send(client->socket, tempBuf, strlen(tempBuf), 0);
		}
	}
}
