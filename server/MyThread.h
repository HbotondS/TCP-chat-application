#pragma once
#include <Ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>
#include "Client.h"

class MyThread {
public:
	MyThread(Client, std::vector<Client>*);
	virtual ~MyThread();
	virtual bool start(void);
	virtual bool stop(unsigned int timeout = 0);
	inline volatile bool& isRunning(void) {
		return m_bRunning;
	}
	inline volatile bool& isExited(void) {
		return m_bExited;
	}
protected:
	virtual void run(void);
private:
	friend DWORD WINAPI runStub(LPVOID mthread);
public:
	static const unsigned int INFINIT_WAIT;
private:
	volatile bool m_bRunning;
	volatile bool m_bExited;
	HANDLE m_thread;
	Client currentClient;
	std::vector<Client>* clients;

	std::vector<std::string> splitMsg(std::string);

	void processMsg(std::string);
	void processPublicMsg(std::string);
	void processPrivateMsg(std::string, std::string);
};

