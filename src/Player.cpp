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

#include "Player.h"
#include "DecisionTree.h"
#include "DynamicDebug.h"
#include "Formation.h"
#include "CommandSender.h"
#include "Parser.h"
#include "Thread.h"
#include "UDPSocket.h"
#include "WorldModel.h"
#include "Agent.h"
#include "VisualSystem.h"
#include "Logger.h"
#include "CommunicateSystem.h"
#include "TimeTest.h"
#include "Dasher.h"

Player::Player():
	mpDecisionTree( new DecisionTree )
{
}

Player::~Player()
{
	delete mpDecisionTree;
}

void Player::SendOptionToServer()
{
	while (!mpParser->IsClangOk())
	{
		mpAgent->CheckCommands(mpObserver);
		mpAgent->Clang(7, 8);
		mpObserver->SetCommandSend();
		WaitFor(200);
	}

	while (!mpParser->IsSyncOk())
	{
		mpAgent->CheckCommands(mpObserver);
		mpAgent->SynchSee();
		mpObserver->SetCommandSend();
		WaitFor(200);
	}

	mpAgent->CheckCommands(mpObserver);
	mpAgent->EarOff(false);
	mpObserver->SetCommandSend();
	WaitFor(200);
}

void Player::Run()
{
    //TIMETEST("Run");

	static Time last_time = Time(-100, 0);

	mpObserver->Lock();

	/** 下面几个更新顺序不能变 */
	Formation::instance.SetTeammateFormations();
	CommunicateSystem::instance().Update(); //在这里解析hear信息，必须首先更新
	mpAgent->CheckCommands(mpObserver);
	mpWorldModel->Update(mpObserver);

	mpObserver->UnLock();

    const Time & time = mpAgent->GetWorldState().CurrentTime();

	if (last_time.T() >= 0) {
		if (time != Time(last_time.T() + 1, 0) && time != Time(last_time.T(), last_time.S() + 1)) {
			if (time == last_time) {
				mpAgent->World().SetCurrentTime(Time(last_time.T(), last_time.S() + 1)); //否则决策数据更新会出问题
			}
		}
	}

	last_time = time;

	Formation::instance.UpdateOpponentRole(); //TODO: 暂时放在这里，教练未发来对手阵型信息时自己先计算

	VisualSystem::instance().ResetVisualRequest();
	mpDecisionTree->Decision(*mpAgent);

	VisualSystem::instance().Decision();
	CommunicateSystem::instance().Decision();


	Logger::instance().LogSight();
}
