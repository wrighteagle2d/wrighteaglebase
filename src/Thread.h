#ifndef __Thread_H__
#define __Thread_H__


#include "Utilities.h"

#ifdef WIN32
inline void WaitFor(int ms)
{
    Sleep(ms);
}

class ThreadCondition
{
public:
    /**
     * 构造函数和析构函数
     */
    ThreadCondition();
    ~ThreadCondition();

    /**
     * 阻塞和唤醒函数
     */
    bool Wait(int ms);
    void Set();

private:
    HANDLE mEvent;
};

class ThreadMutex
{
public:
    /**
     * 构造函数和析构函数
     */
    ThreadMutex();
    ~ThreadMutex();

    /**
     * 加锁和解锁函数
     */
    void Lock();
    void UnLock();

private:
    HANDLE mEvent;
};

#else

#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

inline void WaitFor(int ms)
{
	usleep(ms * 1000);
}

class ThreadCondition
{
public:
    /**
     * 构造函数和析构函数
     */
    ThreadCondition();
    ~ThreadCondition();

    /**
     * 阻塞和唤醒函数
     */
    bool Wait(int ms);
    void Set();

private:
    pthread_cond_t mCond;
    pthread_mutex_t mMutex;
};

class ThreadMutex
{
public:
    /**
     * 构造函数和析构函数
     */
    ThreadMutex();
    ~ThreadMutex();

    /**
     * 加锁和解锁函数
     */
    void Lock();
    void UnLock();

private:
    pthread_mutex_t mMutex;
};
#endif


class Thread
{
	Thread(const Thread &);
	const Thread &operator=(const Thread &);

public:
	Thread()
	{
	}

	void Start();
	void Join();

private:
	static void *Spawner(void *v);
	virtual void StartRoutine() = 0;

private:
#ifdef WIN32
	HANDLE mThread;
#else
	pthread_t mThread;
#endif
};

#endif

