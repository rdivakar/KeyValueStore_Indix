#include "StoreValue.h"


StoreValue::StoreValue()
{
	_time = time(NULL);
}

StoreValue::StoreValue(string value, time_t time)
{
	_time = time;
	_value = value;
}

StoreValue& StoreValue::operator=(const StoreValue& in)
{
	_value = in._value;
	_time = in._time;
	return *this;
}

bool StoreValue::isOlderThan(const StoreValue & in)
{
	if (_time < in._time)
		return true;

	return false;
}

string StoreValue::getValue()
{
	return _value;
}

StoreValue::StoreValue(const StoreValue& in)
{
	*this = in;
}