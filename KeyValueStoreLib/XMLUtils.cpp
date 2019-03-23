#include "XMLUtils.h"

#include "rapidxml_utils.hpp"
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"

#include <sstream>

using namespace std;
using namespace rapidxml;

XMLUtils::XMLUtils()
{
}


XMLType XMLUtils::GetXMLType(string xmlstr)
{
	xml_document<char> doc;
	vector<char> xmlVec(xmlstr.begin(), xmlstr.end());
	xmlstr.push_back('\0');
	char *buff = new char[xmlstr.length() + 1];
	memset(buff, 0, xmlstr.length() + 1);
	strcpy_s(buff, xmlstr.length() + 1, xmlstr.c_str());

	doc.parse<0>(buff);

	xml_node<>* cur_node = doc.first_node("RegisterStore");
	if (cur_node != NULL)
	{
		delete[] buff;
		return RegisterStore;
	}

	cur_node = doc.first_node("KeyValue");
	if (cur_node != NULL)
	{
		delete[] buff;
		return KeyValue;
	}

	cur_node = doc.first_node("Response");
	if (cur_node != NULL)
	{
		delete[] buff;
		return Response;
	}
	delete[] buff;
	return Unknown_XML;
}

bool XMLUtils::getKeyValue(string xmlstr, string & key, string & value, unsigned long long& time)
{
	xml_document<char> doc;
	vector<char> xmlVec(xmlstr.begin(), xmlstr.end());
	xmlstr.push_back('\0');
	char *buff = new char[xmlstr.length() + 1];
	memset(buff, 0, xmlstr.length() + 1);
	strcpy_s(buff, xmlstr.length() + 1, xmlstr.c_str());
	
	doc.parse<0>(buff);

	xml_node<>* cur_node = doc.first_node("KeyValue");
	if (cur_node != NULL)
	{
		xml_node<>* keyNode = cur_node->first_node("key");
		xml_node<>* valueNode = cur_node->first_node("value");
		xml_node<>* timeNode = cur_node->first_node("time");
		if (keyNode)
			key = keyNode->value();
		else
		{
			delete[] buff;
			return false;
		}
		if (valueNode)
			value = valueNode->value();
		else
		{
			delete[] buff;
			return false;
		}
		if (timeNode)
		{
			string sTime = timeNode->value();
			time = stoull(sTime);
		}
		else
		{
			delete[] buff;
			return false;
		}

		delete[] buff;
		return true;
	}
	delete[] buff;
	return false;
}

bool XMLUtils::getStoreInfo(string xmlstr,int& id, int & portNumber)
{
	xml_document<char> doc;
	vector<char> xmlVec(xmlstr.begin(), xmlstr.end());
	xmlstr.push_back('\0');
	char *buff = new char[xmlstr.length() + 1];
	memset(buff, 0, xmlstr.length() + 1);
	strcpy_s(buff, xmlstr.length() + 1, xmlstr.c_str());
	doc.parse<0>(buff);

	xml_node<>* cur_node = doc.first_node("RegisterStore");
	if (cur_node != NULL)
	{
		xml_node<>* idNode = cur_node->first_node("id");
		xml_node<>* portNode = cur_node->first_node("portNumber");

		if (idNode)
		{
			string sID = idNode->value();
			id = stoi(sID);
		}
		else
		{
			delete[] buff;
			return false;
		}
		if (idNode)
		{
			string sPort = portNode->value();
			portNumber = stoi(sPort);
		}
		else
		{
			delete[] buff;
			return false;
		}
		delete[] buff;
		return true;
	}
	delete[] buff;
	return false;
}

bool XMLUtils::getErrorCode(string xmlstr, int & errorCode)
{

	xml_document<char> doc;
	char *buff = new char[xmlstr.length() + 1];
	memset(buff, 0, xmlstr.length() + 1);
	strcpy_s(buff, xmlstr.length() + 1, xmlstr.c_str());
	doc.parse<0>(buff);

	xml_node<>* cur_node = doc.first_node("Response");
	if (cur_node != NULL)
	{
		xml_node<>* eNode = cur_node->first_node("errorCode");

		if (eNode)
		{
			string sErrorCode = eNode->value();
			errorCode = stoi(sErrorCode);
		}
		else
		{
			delete[] buff;
			return false;
		}
		delete[] buff;
		return true;
	}
	delete[] buff;
	return false;
}

string XMLUtils::getKeyValueXML(string key, string value, string time)
{
	xml_document<> doc;

	xml_node<>* root = doc.allocate_node(node_element, "KeyValue");
	doc.append_node(root);
	xml_node<>* child = doc.allocate_node(node_element,"key", key.c_str());
	root->append_node(child);
	child = doc.allocate_node(node_element,"value", value.c_str());
	root->append_node(child);
	
	child = doc.allocate_node(node_element,"time", time.c_str());
	root->append_node(child);

	std::string xml_no_indent;
	
	print(std::back_inserter(xml_no_indent), doc, print_no_indenting);

	return xml_no_indent;
}

string XMLUtils::getStoreInfoXML(string id, string  portNumber)
{

	xml_document<> doc;

	xml_node<>* root = doc.allocate_node(node_element, "RegisterStore");
	doc.append_node(root);
	xml_node<>* child = doc.allocate_node(node_element, "id",id.c_str());
	root->append_node(child);
	
	child = doc.allocate_node(node_element, "portNumber", portNumber.c_str());
	root->append_node(child);
	
	std::string xml_no_indent;

	print(std::back_inserter(xml_no_indent), doc, print_no_indenting);

	return xml_no_indent;

}

string XMLUtils::getErrorCodeXML(string errorCode)
{
	xml_document<> doc;
	
	xml_node<>* root = doc.allocate_node(node_element, "Response");
	doc.append_node(root);
	xml_node<>* child = doc.allocate_node(node_element, "errorCode", errorCode.c_str());
	root->append_node(child);

	std::string xml_no_indent;

	print(std::back_inserter(xml_no_indent), doc, print_no_indenting);

	return xml_no_indent;
}
