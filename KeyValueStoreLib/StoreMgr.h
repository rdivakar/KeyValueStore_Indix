#pragma once

#include <map>
#include "Thread.h"
#include "CriticalSection.h"
using namespace std;

#ifdef KEY_VALUE_STORE_EXPORTS
#		define STOREMGR_API __declspec(dllexport)
#	else
#		define STOREMGR_API  __declspec(dllimport)
#endif

#pragma warning(disable:4251)

class STOREMGR_API StoreMgr
{
	map<int, int> _storePortMap;
	CriticalSection m_storeCS;
	bool _isController;
	StoreMgr();
	static StoreMgr *_instance;

	bool sendKeyOnPort(int portNumber, string xmlRequest, string& response);

public:
	bool isController();
	static StoreMgr* getInstance();
	bool PropagateKeyValue(string keyValueXML);
	bool RegisterKeyValueStore(string storeXML);
	bool RegisterKeyValueStore(int id, int port);
	void SetController(bool val) { _isController = val; }
	bool SendAllKeysToStore(int id);
	bool SendAllKeysToStore(string xml);
};
