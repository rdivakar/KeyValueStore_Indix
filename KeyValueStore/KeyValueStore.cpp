// KeyValueStore.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include "..\KeyValueStoreLib\NetworkServer.h"
#include "..\KeyValueStoreLib\NetworkClient.h"
#include "..\KeyValueStoreLib\XMLUtils.h"
#include "..\KeyValueStoreLib\StoreMgr.h"
#include "..\KeyValueStoreLib\Logger.h"
#include "..\KeyValueStoreLib\KeyValueStore.h"

NetworkServer *coordinator = NULL;
NetworkServer *store = NULL;


void coordinatorCallBack(SOCKET socket, void *ptr, int length)
{
	char* buff = (char*)ptr;
	
	string xmlstr = buff;
	XMLUtils xmlutils;
	Logger *log = Logger::GetLoggerInstance();
	XMLType type = xmlutils.GetXMLType(xmlstr);
	int success = 0;
	string response = "";

	log->Log("coordinatorCallBack request xml %s", xmlstr.c_str());

	switch (type)
	{
		case RegisterStore:
		{
			StoreMgr* storeMgr = StoreMgr::getInstance();
			success = storeMgr->RegisterKeyValueStore(xmlstr) ? 0 : 1;
			success = storeMgr->SendAllKeysToStore(xmlstr) ? 0 : 1;
		}
		break;
		case KeyValue:
		{
			StoreMgr* storeMgr = StoreMgr::getInstance();
			if(storeMgr->isController())
				success = storeMgr->PropagateKeyValue(xmlstr) ? 0 : 1;
		}
		break;
	}
	response = xmlutils.getErrorCodeXML(to_string(success));

	log->Log("coordinatorCallBack response xml %s", response.c_str());

	NetworkClient client(socket);
	if (client.Send((void*)response.c_str(), response.length()) != 0)
	{
		log->Log("coordinatorCallBack Re-Send failed..");
		return;
	}

	delete[] buff;
	return;
}

void serviceCallBack(SOCKET socket, void *ptr, int length)
{
	char* buff = (char*)ptr;

	string xmlstr = buff;
	XMLUtils xmlutils;
	Logger *log = Logger::GetLoggerInstance();
	XMLType type = xmlutils.GetXMLType(xmlstr);
	int success = 0;
	string response = "";
	log->Log("serviceCallBack request xml %s", xmlstr.c_str());
	switch (type)
	{
		case KeyValue:
		{
			KeyValueStore *keyVal = KeyValueStore::getInstance();
			keyVal->SetKeyValue(xmlstr,false);
		}
		break;
	}
	response = xmlutils.getErrorCodeXML(to_string(success));

	log->Log("serviceCallBack response xml %s", response.c_str());

	NetworkClient client(socket);
	if (client.Send((void*)response.c_str(), response.length()) != 0)
	{
		log->Log("coordinatorCallBack Re-Send failed..");
		return;
	}
	return;
}

int main(int argc, char* argv[])
{
	bool isCoordinator = false;
	int port = 10001;
	int id = 1;
	if (argc == 4)
	{
		id = atoi(argv[1]);
		port = atoi(argv[2]);
		if (strcmp(argv[3], "-controller") == 0)
		{
			isCoordinator = true;
		}
	}
	else
	{
		cout << "invalid args" << endl;
		exit(1);
	}

	if (isCoordinator)
	{
		coordinator = new NetworkServer(coordinatorCallBack, CONTROLLER_PORT);
		coordinator->Start();
	}
	store = new NetworkServer(serviceCallBack, port);
	store->Start();
	

	StoreMgr* storeMgr = StoreMgr::getInstance();
	storeMgr->RegisterKeyValueStore(id, port);
	storeMgr->SetController(isCoordinator);

	KeyValueStore *store = KeyValueStore::getInstance();
	if (store->RegisterWithController(id, port) == false)
		cout << "Failed to register with Controller. Please check if controller service is running." << endl;
	else
		cout << "Registered with controller" << endl;

	while (1)
	{
		cout << "Enter operation SET or GET" << endl;
		string operation;
		cin >> operation;
		if (operation == "SET")
		{
			string key, value;
			cout << "Key : " << endl;
			cin >> key;
			cout << "Value : " << endl;
			cin >> value;

			KeyValueStore *store = KeyValueStore::getInstance();
			store->SetKeyValue(key, value);
			cout << "Done.." << endl;
		}
		else if(operation == "GET")
		{
			string key, value;
			cout << "Key : " << endl;
			cin >> key;
			KeyValueStore *store = KeyValueStore::getInstance();
			store->GetKeyValue(key, value);
			cout << "Value : " << value << endl;
		}
		else
		{

		}
		Sleep(100);
	}
	
	while (1)
		Sleep(1 * 1000);

    return 0;
}

