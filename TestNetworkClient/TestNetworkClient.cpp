// TestNetworkClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\KeyValueStoreLib\NetworkClient.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

int main()
{
	NetworkClient netClient;

	if (netClient.Connect(10000) != 0)
	{
		printf("Connect failed.. %ld\n", WSAGetLastError());
		return 0;
	}

	string dataToSend = "The select function determines the status of one or more sockets, waiting if necessary, to perform synchronous I/O.";

	if (netClient.Send((void*)dataToSend.c_str(), dataToSend.length()) != 0)
	{
		printf("Send failed.. %ld\n", WSAGetLastError());
		return 0;
	}

	char *buffer = NULL;
	int recvLeng = 0;

	if (netClient.Recv((void**)&buffer, recvLeng) != 0)
	{
		printf("Send failed.. %ld\n", WSAGetLastError());
		return 0;
	}

	if(buffer)
		printf("Got.. %s\n", buffer);
	else
		printf("No data\n", buffer);

	netClient.Disconnect();

    return 0;
}

