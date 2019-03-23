#include "NetworkServer.h"
#include "Thread.h"
#include <sstream>
#include "Logger.h"
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")



unsigned int NetworkServer::listenerThread(void * valArgs)
{

	listenThreadArgs *args = (listenThreadArgs*)((ThreadData*)valArgs)->arg;

	struct addrinfo *result = NULL, *ptr = NULL, hints;
	int iResult;
	string serverPort = args->serverPort;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, serverPort.c_str(), &hints, &result);
	if (iResult != 0) {
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {

		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	Logger *log = Logger::GetLoggerInstance();

	unsigned char recvbuf[DEFAULT_BUFLEN] = "";
	unsigned char recvLenBuff[BUF_LENGTH] = "";
	int recvbuflen = DEFAULT_BUFLEN;
	
	while (1)
	{
		SOCKET ClientSocket = INVALID_SOCKET;
		ClientSocket = accept(ListenSocket, (sockaddr*)NULL, (int*)NULL);
		if (ClientSocket == INVALID_SOCKET) {
			log->Log("accept failed with error: %d", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			exit(1);
		}
		
		iResult = recv(ClientSocket, (char*)recvLenBuff, BUF_LENGTH, 0);
		if (iResult > 0) {
			log->Log("Bytes received: %d", iResult);
		}
		else if (iResult == 0)
		{
			log->Log("recv failed with error: %d", WSAGetLastError());
			log->Log("Connection closing...");
			iResult = shutdown(ClientSocket, SD_SEND);
			closesocket(ClientSocket);
		}
		
		int bytesToRecv = *((int*)(&recvLenBuff));//Dont worry about little or big endian
		bytesToRecv = bytesToRecv + (DEFAULT_BUFLEN - (bytesToRecv%DEFAULT_BUFLEN));
		char *buffer = new char[bytesToRecv+1];
		memset(buffer, 0, bytesToRecv + 1);
		int recvi = 0;
		while (bytesToRecv > 0)
		{
			int recvd = recv(ClientSocket, (char*)recvbuf, recvbuflen, 0);
			bytesToRecv -= recvd;
			memcpy(&buffer[recvi], recvbuf, recvd);
			recvi += recvd;
		}
		
		args->m_callback(ClientSocket,buffer, recvi+1);

		iResult = shutdown(ClientSocket, SD_SEND);

		Sleep(100);
	}
	return 0;
}

NetworkServer::NetworkServer(messageCallBack callBackFunc, int portNumber)
{	
	stringstream ss("");
	ss << portNumber;
	_serverPort = ss.str();

	m_callback = callBackFunc;
}

void NetworkServer::Start()
{
	Logger *log = Logger::GetLoggerInstance();
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		log->Log("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	listenThreadArgs * tArgs = new listenThreadArgs;
	tArgs->m_callback = m_callback;
	tArgs->serverPort = _serverPort;

	Thread thread;
	thread.createThread(listenerThread, tArgs);
}
