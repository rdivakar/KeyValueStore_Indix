
#include "SharedLock.h"

SharedLock::SharedLock(int maxShareCount)
{
	sem = CreateSemaphore(0, maxShareCount, 0xffffff, 0);

	availableCount = maxShareCount;
	maxCount = maxShareCount;

}

SharedLock::~SharedLock()
{
	 CloseHandle(sem);
}

bool SharedLock::getSharedLock()
{
	bool returnValue = false;
	if ((returnValue = (WaitForSingleObject(sem, INFINITE) == WAIT_OBJECT_0)))
	{
		InterlockedDecrement(&availableCount);
	}
	return returnValue;
}

bool SharedLock::getExclusiveLock()
{
	for (int i = 0; i < maxCount; i++)
	{
		WaitForSingleObject(sem, INFINITE);
		InterlockedDecrement(&availableCount);
	}

	InterlockedDecrement(&availableCount);

	return true;
}

void SharedLock::release()
{
	switch (availableCount)
	{
	case -1:

		for (int i = 0; i < maxCount; i++)
		{
			InterlockedIncrement(&availableCount);
		}
		InterlockedIncrement(&availableCount);
		ReleaseSemaphore(sem, maxCount, 0);
		break;

	default:
		InterlockedIncrement(&availableCount);
		ReleaseSemaphore(sem, 1, 0);
		break;
	}
}
