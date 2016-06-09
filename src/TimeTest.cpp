/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2016                                                    *
 * Copyright (c) 1998-2016 WrightEagle 2D Soccer Simulation Team,                   *
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

#include <fstream>
#include "TimeTest.h"
#include "PlayerParam.h"


/**
 * Constructor.
 */
TimeTest::TimeTest()
{
	mEventQueue.clear();
	mRecordQueue.clear();
	mIsExecute.clear();
	mIsBegin.clear();

	mUpdateTime = Time(-3, 0);
	mUnum       = 0;
}


/**
 * Destructor, which will write all records to files.
 */
TimeTest::~TimeTest()
{
	if (mEventQueue.size() == 0 || mEventQueue.size() != mRecordQueue.size())
	{
		return;
	}

	char *file_name = new char[256];
	std::ofstream out_file;
	TimeCost *pTimeCost;

	for (unsigned i = 0; i < mEventQueue.size(); ++i)
	{
		sprintf(file_name, "Test/TimeTest-%s-%d.txt", mEventQueue[i].data(), mUnum);
		out_file.open(file_name);
		if (out_file.good() == false)
		{
			PRINT_ERROR("open file error  " << file_name);
			continue;
		}

		out_file << mEventQueue[i].data() << std::endl;

		pTimeCost = &(mRecordQueue[i].mEachTime);
        out_file << std::endl << std::endl;
        out_file << "Time cost for each time: " << std::endl;
		out_file << "Num: " << pTimeCost->mNum << std::endl;
		out_file << "Ave: " << pTimeCost->mAveCost / 1000.0 << " ms" << std::endl;
		out_file << "Max: " << pTimeCost->mMaxCost / 1000.0 << " ms  " << pTimeCost->mMaxTime << std::endl;
		out_file << "Min: " << pTimeCost->mMinCost / 1000.0 << " ms  " << pTimeCost->mMinTime << std::endl;

        pTimeCost = &(mRecordQueue[i].mEachCycle);
        out_file << std::endl << std::endl;
        out_file << "Time cost for each cycle: " << std::endl;
		out_file << "Num: " << pTimeCost->mNum << std::endl;
		out_file << "Ave: " << pTimeCost->mAveCost / 1000.0 << " ms" << std::endl;
		out_file << "Max: " << pTimeCost->mMaxCost / 1000.0 << " ms  " << pTimeCost->mMaxTime << std::endl;
		out_file << "Min: " << pTimeCost->mMinCost / 1000.0 << " ms  " << pTimeCost->mMinTime << std::endl;

		out_file.close();
	}

	delete[] file_name;
}


/**
 * 创建实例
 * Instacne.
 */
TimeTest & TimeTest::instance()
{
	static TimeTest time_test;
	return time_test;
}


/**
 * 新的周期到来后进行更新，传入的current_time为更新前的那个周期
 * Update when each cycle begins.
 * \param current_time the time just before update.
 */
void TimeTest::Update(Time current_time)
{
    if (PlayerParam::instance().TimeTest())
    {
	    if (mUpdateTime == current_time)
	    {
		    return;
	    }

	    long last_num           = 0;
	    double last_ave_cost    = 0;
	    long cycle_time_cost    = 0;
	    for (unsigned i = 0; i < mEventQueue.size(); ++i)
	    {
		    if (mIsExecute[i] == false)
		    {
			    continue;
		    }

		    last_num = mRecordQueue[i].mEachCycle.mNum;
		    last_ave_cost = mRecordQueue[i].mEachCycle.mAveCost;
		    cycle_time_cost = mRecordQueue[i].mCycleTimeCost;

		    if (cycle_time_cost > mRecordQueue[i].mEachCycle.mMaxCost)
		    {
			    mRecordQueue[i].mEachCycle.mMaxCost = cycle_time_cost;
			    mRecordQueue[i].mEachCycle.mMaxTime = mUpdateTime;
		    }
		    if (cycle_time_cost < mRecordQueue[i].mEachCycle.mMinCost)
		    {
			    mRecordQueue[i].mEachCycle.mMinCost = cycle_time_cost;
			    mRecordQueue[i].mEachCycle.mMinTime = mUpdateTime;
		    }

		    mRecordQueue[i].mEachCycle.mAveCost = (last_ave_cost * last_num + cycle_time_cost) / (last_num + 1);
		    ++mRecordQueue[i].mEachCycle.mNum;
		    mRecordQueue[i].mCycleTimeCost = 0;
		    mIsExecute[i] = false;
	    }

	    mUpdateTime = current_time;
    }
}


/**
 * 每次测试的开始
 * Begin of each test.
 * \param func_name name of the function which will be tested.
 * \return event ID.
 */
int TimeTest::Begin(std::string func_name)
{
    int i = 0;
    if (PlayerParam::instance().TimeTest())
    {
	    for (; i < (int)mEventQueue.size(); ++i)
	    {
		    if (mEventQueue[i] == func_name)
		    {
			    break;
		    }
	    }

	    if (i == (int)mEventQueue.size())
	    {
		    TimeRecord time_record;
		    mEventQueue.push_back(func_name);
		    mRecordQueue.push_back(time_record);
		    mIsExecute.push_back(false);
		    mIsBegin.push_back(true);
		    mRecordQueue[i].mBeginTime = GetRealTime();
	    }
	    else if (mIsBegin[i] == false)
	    {
		    mIsBegin[i] = true;
		    mRecordQueue[i].mBeginTime = GetRealTime();
	    }
	    else
	    {
		    i = -1;
	    }
    }
	return i;
}


/**
 * 每次测试的结束
 * End of each test.
 * \param event_id.
 */
void TimeTest::End(int event_id)
{
    if (PlayerParam::instance().TimeTest())
    {
	    RealTime end_time       = GetRealTime();
	    long cost_time          = end_time.Sub(mRecordQueue[event_id].mBeginTime);
	    long last_num           = mRecordQueue[event_id].mEachTime.mNum;
	    double last_ave_cost    = mRecordQueue[event_id].mEachTime.mAveCost;
	    if (cost_time > mRecordQueue[event_id].mEachTime.mMaxCost)
	    {
		    mRecordQueue[event_id].mEachTime.mMaxCost = cost_time;
		    mRecordQueue[event_id].mEachTime.mMaxTime = mUpdateTime;
	    }
	    if (cost_time < mRecordQueue[event_id].mEachTime.mMinCost)
	    {
		    mRecordQueue[event_id].mEachTime.mMinCost = cost_time;
		    mRecordQueue[event_id].mEachTime.mMinTime = mUpdateTime;
	    }

	    mRecordQueue[event_id].mEachTime.mAveCost = (last_ave_cost * last_num + cost_time) / (last_num + 1);
	    ++mRecordQueue[event_id].mEachTime.mNum;
	    mRecordQueue[event_id].mCycleTimeCost += cost_time;
	    mIsExecute[event_id] = true;
	    mIsBegin[event_id] = false;
    }
}


/**
 * Constructor.
 * \param func_name name of the function which will be tested.
 */
TimeTestFunc::TimeTestFunc(std::string func_name)
{
	if (PlayerParam::instance().TimeTest())
	{
		mEventID = TimeTest::instance().Begin(func_name);
	}
}


/**
 * Destructor.
 */
TimeTestFunc::~TimeTestFunc()
{
	if (PlayerParam::instance().TimeTest())
	{
		if (mEventID >= 0)
		{
			TimeTest::instance().End(mEventID);
		}
	}
}


// end of TimeTest.cpp

