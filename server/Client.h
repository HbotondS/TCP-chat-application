#pragma once

#include <string>
#include "winsock2.h"

class Client {
public:
	Client(SOCKET socket, std::string nickname)
		: socket(socket), nickname(nickname) {}
	SOCKET socket;
	std::string nickname;
};