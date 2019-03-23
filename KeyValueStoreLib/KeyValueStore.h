#pragma once

#include <map>
#include <string>
#include "StoreValue.h"
#include "SharedLock.h"
#include "CriticalSection.h"

#ifdef KEY_VALUE_STORE_EXPORTS
#		define KEYVALUESTORE_API __declspec(dllexport)
#	else
#		define KEYVALUESTORE_API  __declspec(dllimport)
#endif

#pragma warning(disable:4251)

using namespace std;

class KEYVALUESTORE_API KeyValueStore
{
	static KeyValueStore *_instance;
	map<string, StoreValue> _keyStore;
	KeyValueStore();
	CriticalSection m_storeCS;
	bool sendKeyOnPort(int portNumber, string xmlRequest, string& response);
public:
	static int shareCount;
	static KeyValueStore * getInstance();
	void SetKeyValue(string key, string value, time_t time = time(NULL), bool sync = true);
	void SetKeyValue(string keyvalueXML, bool sync = true);
	bool GetKeyValue(const string key,string& value);
	bool RegisterWithController(int id, int portNumber);
	bool SyncWithController(string key, string value, time_t time);
	bool SendToController(string xmlRequest, string& xmlResponse);
	bool SendAllKeysToStore(int portNumber);
};