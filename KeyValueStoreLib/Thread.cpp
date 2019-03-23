#include "Thread.h"
#include <process.h>



Thread::Thread()
{
	threadHandle = NULL;
}

Thread::~Thread()
{
	if (threadHandle)
	{
		CloseHandle(threadHandle);
		threadHandle = NULL;
	}
}

HANDLE Thread::createThread(THREADPROC cvThreadProc, void * argument)
{
	if (threadHandle)
		return(NULL);
	ThreadData * ud = new ThreadData(cvThreadProc, argument);
	unsigned int cvThreadId = 0;
	threadHandle = (THREADPROC)_beginthreadex(NULL, 0, cvThreadProc, ud, 0, &cvThreadId);
	if (!threadHandle)
		delete ud;

	return threadHandle;
}

bool Thread::GetThreadExitCode(DWORD & exitCode)
{
	return GetExitCodeThread(threadHandle, &exitCode);
}
