#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <ratio>
#include <chrono>
#include <string>

#include "StoreValue.h"
using namespace std;
using namespace std::chrono;

#ifdef KEY_VALUE_STORE_EXPORTS
#		define STOREVALUE_API __declspec(dllexport)
#	else
#		define STOREVALUE_API  __declspec(dllimport)
#endif

#pragma warning(disable:4251)

class STOREVALUE_API StoreValue
{
	time_t _time;
	string _value;

public:
	StoreValue();
	StoreValue(string value, time_t time = time(NULL));
	StoreValue(const StoreValue& in);
	StoreValue& operator=(const StoreValue& in);
	bool isOlderThan(const StoreValue& in);
	string getValue();
	unsigned long long getTime() { return _time; };

};
