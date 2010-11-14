/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2010                                                    *
 * Copyright (c) 1998-2010 WrightEagle 2D Soccer Simulation Team,                   *
 *                         Multi-Agent Systems Lab.,                                *
 *                         School of Computer Science and Technology,               *
 *                         University of Science and Technology of China            *
 * All rights reserved.                                                             *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without               *
 * modification, are permitted provided that the following conditions are met:      *
 *     * Redistributions of source code must retain the above copyright             *
 *       notice, this list of conditions and the following disclaimer.              *
 *     * Redistributions in binary form must reproduce the above copyright          *
 *       notice, this list of conditions and the following disclaimer in the        *
 *       documentation and/or other materials provided with the distribution.       *
 *     * Neither the name of the WrightEagle 2D Soccer Simulation Team nor the      *
 *       names of its contributors may be used to endorse or promote products       *
 *       derived from this software without specific prior written permission.      *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED    *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
 * DISCLAIMED. IN NO EVENT SHALL WrightEagle 2D Soccer Simulation Team BE LIABLE    *
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL       *
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR       *
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER       *
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,    *
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF *
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                *
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

