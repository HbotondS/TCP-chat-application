#include "MyThread.h"
#include <limits.h>
#include <windows.h>
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

MyThread::MyThread(Client currentClient, std::vector<Client>* clients): currentClient(currentClient), clients(clients) {
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
		iResult = recv(currentClient.socket, RecvBuf, BufLen - 1, 0);
		if(iResult == SOCKET_ERROR) {
			int errorCode = WSAGetLastError();
			if(errorCode == 10054) {
				std::string result;
				// remove the user from client list who left the chat
				for(auto client = clients->begin(); client != clients->end(); ++client) {
					if(client->socket == currentClient.socket) {
						result = "leave|" + client->nickname + "\n";
						clients->erase(client);
						break;
					}
				}
				printf("Client %d is disconnected\n", currentClient.socket);
				// send message to the gourp that a client is disconnected
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

		std::string buf = RecvBuf;
		processMsg(buf);
	}
}

std::vector<std::string> MyThread::splitMsg(std::string msg) {
	std::vector<std::string> result;
	std::string delimiter = "|";
	size_t pos = 0;
	std::string token;
	while((pos = msg.find(delimiter)) != std::string::npos) {
		token = msg.substr(0, pos);
		result.push_back(token);
		msg.erase(0, pos + delimiter.length());
	}
	result.push_back(msg.substr(0, msg.length() - 2));

	return result;
}

void MyThread::processMsg(std::string msg) {
	std::vector<std::string> result = splitMsg(msg);
	if(result.at(0) == "public") {
		processPublicMsg(result.at(1));
	}
	if(result.at(0) == "private") {
		processPrivateMsg(result.at(1), result.at(2));
	}
}

void MyThread::processPublicMsg(std::string msg) {
	char tempBuf[1024];
	std::string result;
	// the server send the message back to the client
	std::cout << "public message" << std::endl;
	for(auto client = clients->begin(); client != clients->end(); ++client) {
		std::cout << "Sending a datagram to " << client->nickname << std::endl;
		result = "public|" + currentClient.nickname + "|" + msg + "\n";
		strcpy_s(tempBuf, result.c_str());
		send(client->socket, tempBuf, strlen(tempBuf), 0);
	}
}

void MyThread::processPrivateMsg(std::string msg, std::string target) {
	char tempBuf[1024];
	std::string result;
	// the server send the message back to the client
	std::cout << "private message to: " << target << std::endl;
	for(auto client = clients->begin(); client != clients->end(); ++client) {
		std::cout << "Sending a datagram to " << client->nickname << std::endl;
		if(currentClient.socket == client->socket) {
			result = "private|" + currentClient.nickname + "|" + msg + "\n";
			std::cout << "message: " << result << std::endl;
			strcpy_s(tempBuf, result.c_str());
			send(client->socket, tempBuf, strlen(tempBuf), 0);
		} else {
			if(target == client->nickname) {
				result = "private|" + currentClient.nickname + "|" + msg + "\n";
				strcpy_s(tempBuf, result.c_str());
				send(client->socket, tempBuf, strlen(tempBuf), 0);
				break;
			}
		}
	}
}
