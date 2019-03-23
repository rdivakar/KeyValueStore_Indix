
#include <winsock2.h>
#include <windows.h>
#include "KeyValueStore.h"
#include "CriticalSection.h"
#include "XMLUtils.h"
#include "NetworkClient.h"
#include "Logger.h"

CriticalSection keyValInstCS;

KeyValueStore *KeyValueStore::_instance = NULL;

KeyValueStore::KeyValueStore()
{
	
}

KeyValueStore * KeyValueStore::getInstance()
{
	if (_instance == NULL)
	{
		keyValInstCS.enter();
		if (_instance == NULL)
		{
			_instance = new KeyValueStore();
		}
		keyValInstCS.leave();
	}
	return _instance;
}

void KeyValueStore::SetKeyValue(string key, string value,time_t time, bool sync)
{
	m_storeCS.enter();
	Logger *log = Logger::GetLoggerInstance();
	auto itr = _keyStore.find(key);
	StoreValue sValue(value,time);

	if(itr != _keyStore.end())
	{
		if (itr->second.isOlderThan(sValue))
		{

		}
		else
		{
			m_storeCS.leave();
			return;
		}
	}

	_keyStore[key] = sValue;

	log->Log("KeyValueStore::SetKeyValue Updated Key %s Value %s Time %s", key.c_str(), sValue.getValue().c_str(), to_string(sValue.getTime()));

	m_storeCS.leave();
	if(sync)
		SyncWithController(key, sValue.getValue(), sValue.getTime());
}

bool KeyValueStore::GetKeyValue(const string key, string & value)
{
	m_storeCS.enter();

	auto itr = _keyStore.find(key);

	if (itr == _keyStore.end())
		return false;

	value = _keyStore[key].getValue();

	m_storeCS.leave();

	return true;
}

bool KeyValueStore::RegisterWithController(int id, int portNumber)
{
	Logger *log = Logger::GetLoggerInstance();
	XMLUtils xmlUtils;
	string dataToSend = xmlUtils.getStoreInfoXML(to_string(id), to_string(portNumber));
	string response = "";

	if (SendToController(dataToSend, response))
	{
		log->Log("KeyValueStore::RegisterWithController Response %s", response.c_str());
		return true;
	}

	return false;
}

bool KeyValueStore::SyncWithController(string key, string value, time_t time)
{
	Logger *log = Logger::GetLoggerInstance();
	XMLUtils xmlUtils;
	string dataToSend = xmlUtils.getKeyValueXML(key, value,to_string(time));
	string response = "";

	log->Log("KeyValueStore::SyncWithController Sending key %s to controller", dataToSend.c_str());
	if (SendToController(dataToSend, response))
	{
		log->Log("KeyValueStore::SyncWithController Response %s", response.c_str());
		return true;
	}
	log->Log("KeyValueStore::SyncWithController Failed to send key %s to controller", dataToSend.c_str());
	return false;
}

bool KeyValueStore::SendToController(string xmlRequest, string& xmlResponse)
{
	Logger *log = Logger::GetLoggerInstance();
	NetworkClient netClient;
	string dataToSend = xmlRequest;

	log->Log("KeyValueStore::SendToController data to send.. %s", dataToSend.c_str());

	if (netClient.Connect(CONTROLLER_PORT) != 0)
	{
		log->Log("KeyValueStore::SendToController Connect to coordinator failed.. %ld", WSAGetLastError());
		return 0;
	}

	if (netClient.Send((void*)dataToSend.c_str(), dataToSend.length()) != 0)
	{
		log->Log("KeyValueStore::SendToController Send failed.. %ld", WSAGetLastError());
		return 0;
	}

	char *buffer = NULL;
	int recvLeng = 0;

	if (netClient.Recv((void**)&buffer, recvLeng) != 0)
	{
		log->Log("KeyValueStore::SendToController Send failed.. %ld", WSAGetLastError());
		return 0;
	}

	if (buffer)
	{
		xmlResponse = buffer;
		log->Log("KeyValueStore::SendToController Got.. %s", buffer);
	}
	else
		log->Log("KeyValueStore::SendToController No data");

	netClient.Disconnect();

	return 1;
}

bool KeyValueStore::SendAllKeysToStore(int portNumber)
{
	if (portNumber == CONTROLLER_PORT)
		return true;

	m_storeCS.enter();
	XMLUtils xmlUtils;
	Logger *log = Logger::GetLoggerInstance();
	for (auto itr = _keyStore.begin(); itr != _keyStore.end(); itr++)
	{
		string keyValueXML = xmlUtils.getKeyValueXML(itr->first, itr->second.getValue(), to_string(itr->second.getTime()));
		string response = "";
		log->Log("KeyValueStore::SendAllKeysToStore Send to %d Request %s", portNumber, keyValueXML.c_str());
		sendKeyOnPort(portNumber, keyValueXML, response);
		log->Log("KeyValueStore::SendAllKeysToStore Received from %d response %s", portNumber, response.c_str());
	}
	m_storeCS.leave();
	return true;
}

void KeyValueStore::SetKeyValue(string keyvalueXML,bool sync)
{
	XMLUtils xmlUtils;
	string key = "", value = "";
	unsigned long long time;
	if (xmlUtils.getKeyValue(keyvalueXML, key, value, time))
	{
		SetKeyValue(key, value, time, sync);
	}
	return;
}

bool KeyValueStore::sendKeyOnPort(int portNumber, string xmlRequest, string& response)
{
	Logger *log = Logger::GetLoggerInstance();
	NetworkClient netClient;
	string dataToSend = xmlRequest;

	log->Log("KeyValueStore::sendKeyOnPort data to send.. %s", dataToSend.c_str());

	if (netClient.Connect(portNumber) != 0)
	{
		log->Log("KeyValueStore::sendKeyOnPort Connect to coordinator failed.. %ld", WSAGetLastError());
		return 0;
	}

	if (netClient.Send((void*)dataToSend.c_str(), dataToSend.length()) != 0)
	{
		log->Log("KeyValueStore::sendKeyOnPort Send failed.. %ld", WSAGetLastError());
		return 0;
	}

	char *buffer = NULL;
	int recvLeng = 0;

	if (netClient.Recv((void**)&buffer, recvLeng) != 0)
	{
		log->Log("KeyValueStore::sendKeyOnPort Send failed.. %ld", WSAGetLastError());
		return 0;
	}

	if (buffer)
	{
		response = buffer;
		log->Log("KeyValueStore::sendKeyOnPort Got.. %s", buffer);
	}
	else
		log->Log("KeyValueStore::sendKeyOnPort No data");

	netClient.Disconnect();

	return true;
}

