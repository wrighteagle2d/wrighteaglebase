/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2009                                                    *
 * Copyright (C) 1998-2009 WrightEagle 2D Soccer Simulation Team,                   *
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

#include <list>
#include <string>
#include <sstream>
#include "Coach.h"
#include "DynamicDebug.h"
#include "Formation.h"
#include "CommandSender.h"
#include "Parser.h"
#include "Thread.h"
#include "UDPSocket.h"
#include "WorldModel.h"
#include "Agent.h"
#include "Logger.h"
#include "TimeTest.h"
#include "PlayerParam.h"
#include <iostream>
using namespace std;

Coach::Coach()
{
}

Coach::~Coach()
{

}

void Coach::SendOptionToServer()
{
	while (!mpParser->IsEyeOnOk())
	{
		UDPSocket::instance().Send("(eye on)");
		WaitFor(200);
	}

	for (int i = 1; i <= TEAMSIZE; ++i)
    {
        if (i != PlayerParam::instance().ourGoalieUnum() && mpObserver->Teammate_Coach(i).IsAlive())
        {
            mpAgent->CheckCommands(mpObserver);
            mpAgent->ChangePlayerType(i, i);
            mpObserver->SetCommandSend();
            WaitFor(5);
        }
	}

    /** check whether each player's type is changed OK */
	WaitFor(200);
	for (int i = 1; i <= TEAMSIZE; ++i)
    {
        if (i != PlayerParam::instance().ourGoalieUnum() && mpObserver->Teammate_Coach(i).IsAlive())
        {
            while (!mpParser->IsChangePlayerTypeOk(i) && mpObserver->CurrentTime().T() < 1)
            {
			    mpAgent->CheckCommands(mpObserver);
			    mpAgent->ChangePlayerType(i, i);
			    mpObserver->SetCommandSend();
                WaitFor(200);
		    }
        }
	}
}

/*
 * The main loop for coach.
 */
void Coach::Run()
{
	mpObserver->Lock();

	/** 下面几个更新顺序不能变 */
	Formation::instance.SetTeammateFormations();
	mpAgent->CheckCommands(mpObserver); // 检查上周期发送命令情况
	mpWorldModel->Update(mpObserver, 0);

	mpObserver->UnLock();

	//do planning...

	Logger::instance().LogSight();
}
