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

