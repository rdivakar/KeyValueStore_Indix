// TestNetworkServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\KeyValueStoreLib\NetworkServer.h"
#include "..\KeyValueStoreLib\NetworkClient.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

void callBack(SOCKET socket,void *ptr, int length)
{
	char* buff = (char*)ptr;

	NetworkClient client(socket);
	if (client.Send(ptr, length) != 0)
	{
		printf("Re-Send failed.. %ld\n", WSAGetLastError());
		return;
	}

	return;
}

int main()
{
	NetworkServer server(callBack, 10000);

	server.Start();

	while (1)
		Sleep(1 * 1000);

    return 0;
}

