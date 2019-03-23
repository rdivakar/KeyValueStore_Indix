#include "NetworkClient.h"
#include "Logger.h"
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")



NetworkClient::NetworkClient(SOCKET ListenSocket)
{
	WSADATA wsaData;
	int iResult;
	Logger *log = Logger::GetLoggerInstance();
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		log->Log("WSAStartup failed with error: %d", iResult);
		return;
	}
	m_socket = ListenSocket;
}

int NetworkClient::Connect(int portNumber) //only localhost
{
	stringstream ss;
	ss << portNumber;
	Logger *log = Logger::GetLoggerInstance();
	memset(&hints, 0, sizeof(hints));

	int iResult = getaddrinfo(NULL, ss.str().c_str(), &hints, &result);
	if (iResult != 0) {
		log->Log("getaddrinfo failed with error: %d", iResult);
		WSACleanup();
		return 1;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		m_socket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (m_socket == INVALID_SOCKET) {
			log->Log("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	if (m_socket == INVALID_SOCKET)
	{
		return 1;
	}

	return 0;
}

int NetworkClient::Send(void * data, int length)
{

	int iResult;
	char sendbuf[DEFAULT_BUFLEN] = "";
	char sendLenbuf[BUF_LENGTH] = "";
	int recvbuflen = DEFAULT_BUFLEN;
	Logger *log = Logger::GetLoggerInstance();
	memcpy(sendLenbuf, &length, sizeof(length));

	iResult = send(m_socket, sendLenbuf, BUF_LENGTH, 0); //Send the length first.

	if (iResult == SOCKET_ERROR) {
		log->Log("send failed with error: %d", WSAGetLastError());
		closesocket(m_socket);
		return 1;
	}

	int sentLength = 0;
	int lengthToBeSent = length;
	char* buffer = (char*)data;

	while (lengthToBeSent > 0)
	{
		memset(sendbuf, 0, DEFAULT_BUFLEN);

		memcpy(sendbuf, &buffer[sentLength], DEFAULT_BUFLEN);
		int sent = send(m_socket, sendbuf, DEFAULT_BUFLEN , 0);

		if (iResult == SOCKET_ERROR) {
			log->Log("send failed with error: %d", WSAGetLastError());
			closesocket(m_socket);
			return 1;
		}

		sentLength += sent;
		lengthToBeSent -= sent;
	}
	

	return 0;
}

int NetworkClient::Recv(void ** data, int & length)
{
	int iResult;
	unsigned char recvbuf[DEFAULT_BUFLEN] = "";
	unsigned char recvLenBuff[BUF_LENGTH] = "";
	int recvbuflen = DEFAULT_BUFLEN;
	Logger *log = Logger::GetLoggerInstance();
	length = 0;

	iResult = recv(m_socket, (char*)recvLenBuff, BUF_LENGTH, 0);
	if (iResult > 0) {
		log->Log("Bytes received: %d", iResult);
	}
	else if (iResult == 0)
	{
		log->Log("recv failed with error: %d", WSAGetLastError());
		log->Log("Connection closing...");
	}
	else
	{
		log->Log("send failed with error: %d", WSAGetLastError());
		closesocket(m_socket);
		return 1;
	}

	int bytesToRecv = *((int*)(&recvLenBuff));  //Dont worry about little or big endian
	bytesToRecv = bytesToRecv + (DEFAULT_BUFLEN - (bytesToRecv%DEFAULT_BUFLEN));
	char *buffer = new char[bytesToRecv + 1];
	memset(buffer, 0, bytesToRecv + 1);
	int recvi = 0;
	while (bytesToRecv > 0)
	{
		int recvd = recv(m_socket, (char*)recvbuf, recvbuflen, 0);
		if (recvd == SOCKET_ERROR) {
			log->Log("send failed with error: %d", WSAGetLastError());
			closesocket(m_socket);
			return 1;
		}
		bytesToRecv -= recvd;
		length = length + recvd;
		memcpy(&buffer[recvi], recvbuf, recvd);
		recvi += recvd;
	}

	*data = buffer;
	return 0;
}

int NetworkClient::Disconnect()
{
	int iResult;
	iResult = shutdown(m_socket, SD_SEND);
	closesocket(m_socket);
	return 0;
}
