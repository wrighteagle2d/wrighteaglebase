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

#include "Client.h"
#include "DynamicDebug.h"
#include "Formation.h"
#include "Kicker.h"
#include "CommandSender.h"
#include "Parser.h"
#include "Thread.h"
#include "UDPSocket.h"
#include "WorldModel.h"
#include "Agent.h"
#include "Logger.h"
#include "CommunicateSystem.h"
#include "NetworkTest.h"
#include "Dasher.h"
#include "Tackler.h"
#include "TimeTest.h"
#include "VisualSystem.h"
#include "InterceptModel.h"
#include "Plotter.h"

Client::Client() {
	srand(time(0)); //global srand once
	srand48(time(0));

	/** Observer and World Model */
	mpAgent         = 0;
    mpObserver      = new Observer;
	mpWorldModel    = new WorldModel;

	/** Parser thread and CommandSend thread */
	mpParser        = new Parser(mpObserver);
	mpCommandSender = new CommandSender(mpObserver);

    // instance放在这里创建以节省后面的运行效率，部分instance不能放在这里，需要时创建

    /** Assistant instance */
	TimeTest::instance();
    NetworkTest::instance();
    DynamicDebug::instance();
    Logger::instance().Initial(mpObserver, &(mpWorldModel->World(false)));
    Plotter::instance();

    /** Param */
    ServerParam::instance();
    PlayerParam::instance();

	/** Base Model */
    InterceptModel::instance();

    /** Smart Action */
    Dasher::instance();
    Tackler::instance();
    Kicker::instance();

    /** Other Useful instance */
    BehaviorFactory::instance();
    UDPSocket::instance();
    VisualSystem::instance();
    CommunicateSystem::instance();
}

Client::~Client()
{
	delete mpCommandSender;
	delete mpParser;

	delete mpObserver;
	delete mpWorldModel;
	delete mpAgent;
}

void Client::RunDynamicDebug()
{
	static char msg[MAX_MESSAGE];
	DynamicDebug::instance().Initial(mpObserver); // 动态调试的初始化，注意位置不能移动

	DynamicDebug::instance().Run(msg); // 初始化信息
	mpParser->ParseInitializeMsg(msg);

	ConstructAgent();

	MessageType msg_type;

	bool first_parse = true;

	while (true)
	{
		msg_type = DynamicDebug::instance().Run(msg);

		switch (msg_type)
		{
		case MT_Parse:
			if (first_parse) {
				mpObserver->Reset();
				first_parse = false;
			}
			mpParser->Parse(msg);
			break;
		case MT_Run:
			Run();
			Logger::instance().Flush(); //flush log
			mpObserver->SetPlanned();
			break;
		case MT_Send:
			mpCommandSender->Run();
			first_parse = true;
			break;
		default:
			return;
		}
	}
}

void Client::RunNormal()
{
	mpParser->Start(); //分析线程，接受server发来的信息
	mpCommandSender->Start(); //发送命令线程，向server发送信息
	Logger::instance().Start(); //log线程

	int past_cycle = 0;
	do
	{
		WaitFor(100); // wait for the parser thread to connect the server
		if (++past_cycle > 20)
		{
			std::cout << PlayerParam::instance().teamName() << ": Connect Server Error ..." << std::endl;
			return;
		}
	} while (mpParser->IsConnectServerOk() == false);

	ConstructAgent();

	SendOptionToServer();

	MainLoop();

	WaitFor(mpObserver->MyUnum() * 100);
    if (mpObserver->MyUnum() == 0)
    {
        std::cout << PlayerParam::instance().teamName() << " Coach: Bye ..." << std::endl;
    }
    else
    {
        std::cout << PlayerParam::instance().teamName() << " " << mpObserver->MyUnum() << ": Bye ..." << std::endl;
    }
}

void Client::ConstructAgent()
{
	Assert(mpAgent == 0);
	mpAgent = new Agent(mpObserver->MyUnum(), mpWorldModel, false); //要知道号码才能初始化

	Formation::instance.AssignWith(mpAgent);
	mpCommandSender->RegisterAgent(mpAgent);
	CommunicateSystem::instance().Initial(mpObserver , mpAgent); //init communicate system
	VisualSystem::instance().Initial(mpAgent);
}

void Client::MainLoop()
{
	while (mpObserver->WaitForNewInfo()) // 等待新视觉
	{
        NetworkTest::instance().AddDecisionBegin();

        // 比赛结束，bye ...
        if (mpObserver->GetPlayMode() == PM_Time_Over)
        {
            mpAgent->CheckCommands(mpObserver);
            mpAgent->Bye();

            mpObserver->SetPlanned();
            mpObserver->SetCommandSend();
            Logger::instance().SetFlushCond();

            break;
        }

        DynamicDebug::instance().AddMessage("\0", MT_Run); // 动态调试记录Run信息
        Run();

		mpObserver->SetPlanned();
		mpObserver->SetCommandSend(); //唤醒发送命令的线程
		Logger::instance().SetFlushCond(); // set flush cond and let the logger thread flush the logs to file.

        NetworkTest::instance().AddDecisionEnd(mpObserver->CurrentTime());
	}
}
