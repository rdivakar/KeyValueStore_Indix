#pragma once

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <string>
#include "Common.h"

using namespace std;

typedef void(*messageCallBack)(SOCKET socket,void* data, int size);

#ifdef KEY_VALUE_STORE_EXPORTS
#		define NETWORKSERVER_API __declspec(dllexport)
#	else
#		define NETWORKSERVER_API  __declspec(dllimport)
#endif

#pragma warning(disable:4251)

struct listenThreadArgs
{
	string serverPort;
	messageCallBack m_callback;
};

class NETWORKSERVER_API NetworkServer
{
	messageCallBack m_callback;

	SOCKET ListenSocket = INVALID_SOCKET;

	string _serverPort;
	static unsigned int __stdcall listenerThread(void * ptr);

public:
	NetworkServer(messageCallBack callBackFunc,int portNumber);
	void Start();

};