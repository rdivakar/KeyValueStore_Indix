#pragma once

#include <string>

using namespace std;

#ifdef KEY_VALUE_STORE_EXPORTS
#		define XMLUTILS_API __declspec(dllexport)
#	else
#		define XMLUTILS_API  __declspec(dllimport)
#endif

enum XMLType
{
	Unknown_XML = 0,
	RegisterStore = 1,
	KeyValue = 2,
	Response
};


class XMLUTILS_API XMLUtils
{
public:
	XMLUtils();
	XMLType GetXMLType(string xmlstr);

	bool getKeyValue(string xmlstr, string& key, string& value, unsigned long long& time);
	bool getStoreInfo(string xmlstr, int& id, int& portNumber);
	bool getErrorCode(string xmlstr, int& errorCode);

	string getKeyValueXML(string key, string value, string time);
	string getStoreInfoXML(string id, string portNumber);
	string getErrorCodeXML(string errorCode);
};
