// TestXMLParser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\KeyValueStoreLib\XMLUtils.h"
#include <iostream>

using namespace std;

int main()
{
	XMLUtils xmlUtils;

	string xml = xmlUtils.getKeyValueXML("Hello", "Hello123", "12345");

	cout << xml << endl;

	string key = "", value = "";
	unsigned long long time;
	xmlUtils.getKeyValue(xml, key, value, time);
	cout << key << " " << value << " " << time << endl;
 
	xml = xmlUtils.getStoreInfoXML("1", "10000");
	cout << xml << endl;
	int id = 0, port = 0;

	xmlUtils.getStoreInfo(xml,id, port);
	cout << id << " " << port << endl;

	xml = xmlUtils.getErrorCodeXML("5000");
	cout << xml << endl;
	int error = 0;
	xmlUtils.getErrorCode(xml, error);
	cout << error << endl;


    return 0;
}

