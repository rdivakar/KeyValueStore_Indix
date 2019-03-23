#pragma once

#include "Windows.h"

typedef unsigned int (WINAPI *THREADPROC)(void*);

#ifdef KEY_VALUE_STORE_EXPORTS
#		define THREAD_API __declspec(dllexport)
#	else
#		define THREAD_API  __declspec(dllimport)
#endif

#pragma warning(disable:4251)

struct ThreadData
{
	THREADPROC proc;
	void * arg;
	ThreadData(THREADPROC _proc, void * _arg) :
		proc(_proc), arg(_arg) {}
};

class THREAD_API Thread
{
public:
	Thread();
	~Thread();
	HANDLE createThread(THREADPROC cvThreadProc, void *argument = NULL);
	bool GetThreadExitCode(DWORD &exitCode);

private:
	HANDLE threadHandle;
};