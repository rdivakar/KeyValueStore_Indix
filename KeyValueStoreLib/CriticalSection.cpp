#include "CriticalSection.h"


CriticalSection::CriticalSection(void)
{
	InitializeCriticalSection(&csec);
}
CriticalSection::~CriticalSection(void)
{
	DeleteCriticalSection(&csec);
}
void CriticalSection::enter()
{
	EnterCriticalSection(&csec);	
}
void CriticalSection::leave(void)
{
	LeaveCriticalSection(&csec);
}

CriticalSection CriticalSection::operator=(const CriticalSection & in)
{
	csec = in.csec;
	return *this;
}

CriticalSection::CriticalSection(const CriticalSection & in)
{
	*this = in;
}
