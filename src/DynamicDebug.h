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

#ifndef __DynamicDebug_H__
#define __DynamicDebug_H__


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "Observer.h"
#include "PlayerParam.h"

/**
 * 动态调试中记录消息的类型
 * Message type in dynamic debugging.
 */
enum MessageType
{
    MT_Null,
    MT_Parse,
    MT_Run,
    MT_Send
};

class DynamicDebug
{
	DynamicDebug();

	struct MessageFileHead
	{
		//char    mHeadFlag[2]; // 用来识别是否是动态调试记录的消息文件
		Time    mMaxCycle; // 文件中记录的最大周期
		long long  mIndexTableSize; // 索引表大小
		long long  mIndexTableOffset; // 索引表位置
		long long  mParserTableSize;
		long long  mParserTableOffset;
		long long  mDecisionTableSize;
		long long  mDecisionTableOffset;
		long long  mCommandSendTableSize;
		long long  mCommandSendTableOffset;
		/*int  mIndexTableSize; // 索引表大小
		int  mIndexTableOffset; // 索引表位置
		int  mParserTableSize;
		int  mParserTableOffset;
		int  mDecisionTableSize;
		int  mDecisionTableOffset;
		int  mCommandSendTableSize;
		int  mCommandSendTableOffset;*/
		MessageFileHead(): mMaxCycle( -10 )
		{

		}
	};

	struct MessageIndexTableUnit
	{
		Time    mServerTime; // 数据对应的周期
		long long  mDataSize; // 数据长度
		long long  mDataOffset; // 数据的存储位置
		long long  mTimeOffset; // 时间表的存储位置
		/*int  mDataSize; // 数据长度
		int  mDataOffset; // 数据的存储位置
		int  mTimeOffset; // 时间表的存储位置*/
	};

	struct Message
	{
		MessageType mType;
		std::string mString;

		Message(MessageType type, const char *msg): mType(type), mString(msg)
		{

		}
	};

public:
	/**
     * 构造函数和析构函数
     */
    ~DynamicDebug();

    /**
     * 创建实例，初始化函数
     */
    static DynamicDebug & instance();
    void Initial(Observer *pObserver);

    /**
     * 下面函数是正常比赛要记录server信息时用到的接口
     * AddMessage()这个函数一定要放到各个线程的入口函数的前面调用，否则动态调试得到系统时间时会出错
     * 比如，要在Parser::Parse()，Client::Decision()和CommandSend::Run()之前调用
     */
    void AddMessage(const char *msg, MessageType msg_type);
    void AddTimeParser(timeval &time);
    void AddTimeDecision(timeval &time);
    void AddTimeCommandSend(timeval &time);

    /**
     * 下面函数是动态调试时用到的接口
     */
    MessageType Run(char *msg);
    MessageType GetMessage(char *msg);
    bool FindCycle(int cycle);
    timeval GetTimeParser();
    timeval GetTimeDecision();
    timeval GetTimeCommandSend();

private:
    void Flush();

private:
    Observer    *mpObserver; // WorldModel的指针
    bool        mInitialOK; // 是否已经初始化完毕

    // 文件头部信息，记录4种信息的量
    MessageFileHead mFileHead;

    // 下面4个表用来记录和保存4种信息
    std::vector<MessageIndexTableUnit>  mIndexTable;
    std::vector<timeval>                mParserTimeTable;
    std::vector<timeval>                mDecisionTimeTable;
    std::vector<timeval>                mCommandSendTimeTable;

    std::vector<Message>                mMessageTable;

    // 下面4个指针用来在读写文件时使用
    MessageIndexTableUnit   *mpIndex;
    timeval                 *mpParserTime;
    timeval                 *mpDecisionTime;
    timeval                 *mpCommandSendTime;

    // 当前读取的单元
    MessageIndexTableUnit   *mpCurrentIndex;

    // 用于文件操作
    ThreadMutex     mFileMutex;
    FILE            *mpFile;
    std::ifstream   *mpFileStream;
    std::streambuf  *mpStreamBuffer;

    bool mRunning; // 是否正在运行
    bool mShowMessage; // 是否输出信息
    Time mRuntoCycle; // 要Run到的周期
};

#endif

