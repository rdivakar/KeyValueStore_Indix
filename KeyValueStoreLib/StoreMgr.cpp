
#include <winsock2.h>
#include <windows.h>
#include "StoreMgr.h"
#include "XMLUtils.h"
#include "NetworkClient.h"
#include "Logger.h"
#include <stdlib.h>
#include <stdio.h>
#include "KeyValueStore.h"

#pragma comment (lib, "Ws2_32.lib")

CriticalSection storeInstCS;

StoreMgr *StoreMgr::_instance = NULL;

StoreMgr::StoreMgr()
{
	_isController = false;
}

bool StoreMgr::sendKeyOnPort(int portNumber, string xmlRequest, string& response)
{
	Logger *log = Logger::GetLoggerInstance();
	NetworkClient netClient;
	string dataToSend = xmlRequest;

	log->Log("StoreMgr::sendKeyOnPort data to send.. %s", dataToSend.c_str());

	if (netClient.Connect(portNumber) != 0)
	{
		log->Log("StoreMgr::sendKeyOnPort Connect to coordinator failed.. %ld", WSAGetLastError());
		return 0;
	}

	if (netClient.Send((void*)dataToSend.c_str(), dataToSend.length()) != 0)
	{
		log->Log("StoreMgr::sendKeyOnPort Send failed.. %ld", WSAGetLastError());
		return 0;
	}

	char *buffer = NULL;
	int recvLeng = 0;

	if (netClient.Recv((void**)&buffer, recvLeng) != 0)
	{
		log->Log("StoreMgr::sendKeyOnPort Send failed.. %ld", WSAGetLastError());
		return 0;
	}

	if (buffer)
	{
		response = buffer;
		log->Log("StoreMgr::sendKeyOnPort Got.. %s", buffer);
	}
	else
		log->Log("StoreMgr::sendKeyOnPort No data");

	netClient.Disconnect();

	return true;
}

bool StoreMgr::isController()
{
	return _isController;
}

StoreMgr * StoreMgr::getInstance()
{
	if (_instance == NULL)
	{
		storeInstCS.enter();
		if (_instance == NULL)
		{
			_instance = new StoreMgr();
		}
		storeInstCS.leave();
	}
	return _instance;
}

bool StoreMgr::PropagateKeyValue(string keyValueXML)
{
	if (!isController())
		return true;
	m_storeCS.enter();
	Logger *log = Logger::GetLoggerInstance();
	
	for (auto itr = _storePortMap.begin(); itr != _storePortMap.end(); itr++)
	{
		int portNumber = itr->second;
		if (portNumber <= 0)
			continue;
		string response = "";
		log->Log("StoreMgr::PropagateKeyValue Send to %d Request %s", portNumber, keyValueXML.c_str());
		sendKeyOnPort(portNumber, keyValueXML, response);

		log->Log("StoreMgr::PropagateKeyValue Received from %d response %s", portNumber, response.c_str());
	}

	m_storeCS.leave();
	return true;
}

bool StoreMgr::RegisterKeyValueStore(string storeXML)
{
	if (!isController())
		return true;
	
	XMLUtils xmlUtils;
	int id = 0, port = 0;
	if (xmlUtils.getStoreInfo(storeXML, id, port))
	{
		RegisterKeyValueStore(id, port);

		return true;
	}
	
	return false;
}

bool StoreMgr::RegisterKeyValueStore(int id, int port)
{
	m_storeCS.enter();
	_storePortMap[id] = port;
	Logger *log = Logger::GetLoggerInstance();
	log->Log("StoreMgr::RegisterKeyValueStore Register id %d on Port %d", id, port);
	m_storeCS.leave();
	return true;
}

bool StoreMgr::SendAllKeysToStore(string xml)
{
	
	XMLUtils xmlUtils;
	int id = 0, port = 0;
	Logger *log = Logger::GetLoggerInstance();
	log->Log("StoreMgr::SendAllKeysToStore Sending All Keys to store %d", id);
	if (xmlUtils.getStoreInfo(xml, id, port))
	{
		SendAllKeysToStore(id);

		return true;
	}
	log->Log("StoreMgr::SendAllKeysToStore Failed to send All Keys to store %d", id);
	return false;
}

bool StoreMgr::SendAllKeysToStore(int id)
{
	m_storeCS.enter();
	KeyValueStore *keyVal = KeyValueStore::getInstance();
	keyVal->SendAllKeysToStore(_storePortMap[id]);
	m_storeCS.leave();
	return true;
}


