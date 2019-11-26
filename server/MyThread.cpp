#include "MyThread.h"
#include <limits.h>
#include <windows.h>

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

MyThread::MyThread(SOCKET AcceptSocket): AcceptSocket(AcceptSocket) {
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
	char RecvBuf[1024];
	int BufLen = 1024;
	int iResult;
	while(1) {
		std::cout << "Receiving datagrams...\n";
		iResult = recv(AcceptSocket, RecvBuf, BufLen - 1, 0);
		if(iResult == SOCKET_ERROR) {
			int errorCode = WSAGetLastError();
			if(errorCode == 10054) {
				printf("Client %d is disconnected\n", AcceptSocket);
			} else {
				printf("recv() failed with the following code: %d\n", errorCode);
			}
			return;
		}

		std::cout << "Sending a datagram...\n";
		// the server send the message back to the client
		send(AcceptSocket, RecvBuf, strlen(RecvBuf), 0);
	}
}
