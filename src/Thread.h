/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2010                                                    *
 * Copyright (C) 1998-2010 WrightEagle 2D Soccer Simulation Team,                   *
 *                         Multi-Agent Systems Lab.,                                *
 *                         School of Computer Science and Technology,               *
 *                         University of Science and Technology of China, China.    *
 *                                                                                  *
 * This program is free software; you can redistribute it and/or                    *
 * modify it under the terms of the GNU General Public License                      *
 * as published by the Free Software Foundation; either version 2                   *
 * of the License, or (at your option) any later version.                           *
 *                                                                                  *
 * This program is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 * GNU General Public License for more details.                                     *
 *                                                                                  *
 * You should have received a copy of the GNU General Public License                *
 * along with this program; if not, write to the Free Software                      *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.  *
 ************************************************************************************/

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

