#pragma once

#include <memory>
#include <Windows.h>

#ifdef KEY_VALUE_STORE_EXPORTS
#		define CRITICALSECTION_API __declspec(dllexport)
#	else
#		define CRITICALSECTION_API  __declspec(dllimport)
#endif

class CRITICALSECTION_API CriticalSection
{
public:
	
	CriticalSection(void);
	~CriticalSection(void);

	void        enter();
	void        leave(void);
private:
	CRITICAL_SECTION    csec;
	CriticalSection operator=(const CriticalSection &);
	CriticalSection(const CriticalSection &);
};