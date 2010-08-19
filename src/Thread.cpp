#include "Thread.h"


#ifdef WIN32

#include <windows.h>
ThreadCondition::ThreadCondition()
{
	mEvent = CreateEvent(0, false, false, 0);
}

ThreadCondition::~ThreadCondition()
{
}

bool ThreadCondition::Wait(int ms)
{
	if (ms == 0)
	{
		ms = INFINITE;
	}

	ResetEvent(mEvent);
	DWORD ret = WaitForSingleObject(mEvent,ms);
	return (ret == WAIT_TIMEOUT);
}

void ThreadCondition::Set()
{
	SetEvent(mEvent);
}

ThreadMutex::ThreadMutex()
{
	mEvent = CreateMutex(0, false, 0);
}

ThreadMutex::~ThreadMutex()
{
}

void ThreadMutex::Lock()
{
	WaitForSingleObject(mEvent, 50);
}

void ThreadMutex::UnLock()
{
	ReleaseMutex(mEvent);
}

#else

ThreadCondition::ThreadCondition()
{
	pthread_mutex_init(&mMutex, 0);
	pthread_cond_init(&mCond, 0);
}

ThreadCondition::~ThreadCondition()
{
	pthread_mutex_destroy(&mMutex);
	pthread_cond_destroy(&mCond);
}

bool ThreadCondition::Wait(int ms)
{
	while (pthread_mutex_lock(&mMutex))
	{
	}

	int ret = ETIMEDOUT;
	if (ms > 0)
	{
		timespec timeout;
		RealTime outtime = GetRealTime();
		outtime = outtime + ms;
		timeout.tv_sec = outtime.GetSec();
		timeout.tv_nsec = outtime.GetUsec() * 1000;
		while((ret = pthread_cond_timedwait(&mCond, &mMutex, &timeout)) == EINTR )
		{
		}
	}
	else
	{
		ret = pthread_cond_wait(&mCond, &mMutex);
	}

	while (pthread_mutex_unlock(&mMutex))
	{
	}
	return (ret == ETIMEDOUT);
}

void ThreadCondition::Set()
{
	while(pthread_mutex_lock(&mMutex))
	{
	}
	while(pthread_cond_signal(&mCond))
	{
	}
	while(pthread_mutex_unlock(&mMutex))
	{
	}
}

ThreadMutex::ThreadMutex()
{
	pthread_mutex_init (&mMutex,0);
}

ThreadMutex::~ThreadMutex() {
	pthread_mutex_destroy(&mMutex);
}

void ThreadMutex::Lock()
{
	while (pthread_mutex_lock(&mMutex))
	{
	}
}

void ThreadMutex::UnLock()
{
	while (pthread_mutex_unlock(&mMutex))
	{
	}
}
#endif

void *Thread::Spawner(void *thread)
{
	static_cast<Thread*>(thread)->StartRoutine();
	return (void*) 0;
}

void Thread::Start()
{
#ifdef WIN32
	DWORD dwThreadId;
	mThread = CreateThread(0, 0, &Spawner, this, 0, &dwThreadId);
#else
	pthread_create(&mThread, 0, &Spawner, this);
#endif
}

void Thread::Join()
{
#ifdef WIN32
#else
	pthread_join(mThread, 0);
#endif
}

