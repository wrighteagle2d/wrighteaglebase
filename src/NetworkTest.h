/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2013                                                    *
 * Copyright (c) 1998-2013 WrightEagle 2D Soccer Simulation Team,                   *
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

/**
 * 1. 这个文件主要的功能是通过在sence信息到来时记录一个时间，在视觉信息到来时
 *    记录一个时间，用两个时间差相减。求出sense信息到来后，sight信息到来的
 *    时间间隔，求出最大时间间隔，最小时间间隔，平均值，方差。统计差数，为我
 *    定等待视觉信息的到来的Buffer做参考。同时可以在比赛的时候测出网络环境的
 *   （及网络延迟）。如果太大我们完全可以提出相应的要求。
 * 2. 通过在BasicAction.cpp 内记录我们每个周期的发送的命令，在sense 时信息的
 *    解析到Server返回的差数，这两值的比较可以测出我们各个命令是否漏发命令。
 *    重另外一个方面反应了网络的性能。
 * 3. 这个类还可以扩展相应的功能，比如可以测出Sense 到Sense 的时间间隔，从另
 *    一方面的反应网络环境。可以测出Sense 信息是否丢失。测出是否丢包。以及丢包率
 *    从各个方面反应出网络环境。（可以进一步实现）。
 */
/**
 * 1. This paper's main function is to sence information coming through a time when the record, when the arrival of visual information in
 * Record a time, with two-phase by the time difference. Sense of information obtained after the arrival of, sight of information coming
 * Time interval, derive the maximum time interval, the minimum time interval, the mean, variance. Statistical difference, for me
 * Will be waiting for the arrival of visual information reference the Buffer. At the same time when the game can detect the network environment
 * (And the network delay). If we can raise too much the corresponding requirements.
 * 2. BasicAction.cpp recorded in each cycle we have sent the order when the information in the sense
 * Analysis of the Server to return the difference, compare these two values can be measured in various orders, we missed an order.
 * The re-addition reaction of one aspect of the performance of the network.
 * 3. This class can also be a corresponding expansion of the functions of, for example, can be measured Sense to Sense of time interval, from another
 * On the one hand, the response of the network environment. Sense can detect whether or not the loss of information. Packet loss is detected. And packet loss rate
 * All aspects of the response from the network environment. (Can be further).
 */

#ifndef __NetworkTest_H__
#define __NetworkTest_H__


#include <map>
#include <vector>
#include <cstring>
#include <fstream>
#include "ActionEffector.h"


typedef struct{
	long val;
	Time ValTime;
}TIMEDATA;

typedef struct{
	TIMEDATA 	MinDelay;		//最大值包括时间
	TIMEDATA 	MaxDelay;		//最小值包括时间
	double 		average;         //平均值
	long 		count;				//统计次数
	double 		variance;		//方差
	double		quadrature;		//二价原点矩
}COUNTUNIT;

typedef struct{
    std::string Name;		//系统时间的记录的名字
	RealTime RT;		//系统时间，没周期的都更新
	bool	IsRecord;	//是否已经记录
}TIMEUNIT;


typedef struct
{
	long threshold;
	int count;
	double probility;
}INTERVAL;

typedef struct
{
	int Kicks;
    int Dashs;
    int Turns;
	int Says;
	int TurnNecks;
	int Catchs;
	int Moves;
	int ChangeViews;
	int Pointtos;
	int Attentiontos;
	int Tackles;
}CMDCOUNT;


struct RealTimeRecord
{
    RealTime    mBeginTime; // 开始的系统时间
    RealTime    mEndTime;   // 结束的系统时间
    int         mCostTime;   // 花费的时间
    Time        mTime;      // 当前周期
};


class StatisticUnit{
public:
	StatisticUnit();
	~StatisticUnit();
	void SetMinVal(TIMEDATA td);
	void SetMaxVal(TIMEDATA td);
	void SetAverage(long val);
	void Setquadrature(long val);
	void SetVariance();
	void SetStatistic(long val, Time current);
	void InterValStacistic(long val);
	void OutPutInterVal(FILE *fp=0);
	void PrintStatic(FILE *fp=0);
	void PrintCommond(FILE *fp);
	void Flush();
	void setCMDData(CMDCOUNT Send, CMDCOUNT Execute)
	{
		CMDSend = Send;
		CMDExecute = Execute;
	}
    void setBegin(std::string BName)
	{
		BeginName = BName;
	}
    void setEndName(std::string EName)
	{
		EndName = EName;
	}
	void setMyID(int ID)
	{
		MyID = ID;
	}
	std::string GetBegin()
	{
		return BeginName;
	}
	std::string GetEndName()
	{
		return EndName;
	}
	void MissBeginIncrement()
	{
		MissBegin++;
	}

private:
	COUNTUNIT CU;
	long InterBase;
	long Increment;
	int MyID;
	int MissBegin;
	CMDCOUNT CMDSend;	//命令发送计数器
	CMDCOUNT CMDExecute; //命令执行计数器
	std::string	 BeginName;
	std::string	 EndName;
	INTERVAL Interval[9];//<=30,<=40,<=50,<=60,<=70,<=80,<=90,<=100,其他，共九情况
};


class NetworkTest{
	NetworkTest();
public:
	~NetworkTest();
    static NetworkTest &instance();

    void SetUnum(Unum unum) { mUnum = unum; }
    void Update(const Time& time);
    void SetCommandExecuteCount(int d,int k,int tu,int s,int tn,int c,int m, int cv,int pt,int tk, int fc);
    void SetCommandSendCount(const CommandInfo& cmd);

    void Begin(const std::string BeginName);
	void End(const std::string BeginName, const std::string EndName);

private:
	std::vector<TIMEUNIT>BeginTimelist;		//开始计时点的名字与系统时间集合体;
	std::vector<TIMEUNIT>EndTimelist;		//结束计时点的名字与系统时间集合体;
	std::vector<StatisticUnit>StatUnit;		//统计单位
	std::map<std::string,std::string>EndtoBegin;
	int getNameID(std::string Name, bool tag);
	long getInterval(const std::string BeginName, const std::string EndName);  //时间间隔
	void Setstatistic(const std::string BeginName, const std::string EndName);

private:
	Time CurrentTime;
	int mUnum;
	double Probility;
	long INTV;
	CMDCOUNT CMDSend;	//命令发送计数器
	CMDCOUNT CMDExecute; //命令执行计数器

public:
    void AddParserBegin();
    void AddParserEnd(Time current_time);
    void AddDecisionBegin();
    void AddDecisionEnd(Time current_time);
    void AddCommandSendBegin();
    void AddCommandSendEnd(Time current_time);

    void WriteRealTimeRecord();

private:
    std::vector<RealTimeRecord> mParserList;
    std::vector<RealTimeRecord> mDecisionList;
    std::vector<RealTimeRecord> mCommandSendList;

    RealTimeRecord  mParserRecord;
    RealTimeRecord  mDecisionRecord;
    RealTimeRecord  mCommandSendRecord;
};


#endif

