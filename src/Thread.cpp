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

#include "Thread.h"
#include <error.h>


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

