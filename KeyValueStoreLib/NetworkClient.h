#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include "Common.h"

#ifdef KEY_VALUE_STORE_EXPORTS
#		define NETWORKCLIENT_API __declspec(dllexport)
#	else
#		define NETWORKCLIENT_API __declspec(dllimport)
#endif

class NETWORKCLIENT_API NetworkClient
{

	SOCKET m_socket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;

public:

	NetworkClient(SOCKET ListenSocket = 0);
	int Connect(int portNumber);
	int Send(void* data, int length);
	int Recv(void** data, int& length);
	int Disconnect();
};
