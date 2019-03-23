#pragma once

#include "CriticalSection.h"
#include <Windows.h>

#ifdef KEY_VALUE_STORE_EXPORTS
#		define SHAREDLOCK_API __declspec(dllexport)
#	else
#		define SHAREDLOCK_API  __declspec(dllimport)
#endif


class SHAREDLOCK_API SharedLock
{
private:
	int maxCount;
	const char* lockName;
	long availableCount;

	CriticalSection cs;
	HANDLE sem;

public:
	SharedLock(int maxShareCount);
	~SharedLock();
	bool getSharedLock();
	bool getExclusiveLock();
	void release();
};