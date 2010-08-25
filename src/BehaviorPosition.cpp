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

#include "BehaviorPosition.h"
#include "Agent.h"
#include "Strategy.h"
#include "InterceptInfo.h"
#include "Dasher.h"
#include "VisualSystem.h"
#include "BehaviorPass.h"
#include "Logger.h"
#include "TimeTest.h"
#include <sstream>
#include "Evaluation.h"


using namespace std;

const BehaviorType BehaviorPositionExecuter::BEHAVIOR_TYPE = BT_Position;

namespace {
bool ret = BehaviorExecutable::AutoRegister<BehaviorPositionExecuter>();
}


BehaviorPositionExecuter::BehaviorPositionExecuter(Agent & agent) :
	BehaviorExecuterBase<BehaviorAttackData> (agent)
{
	Assert(ret);
}

BehaviorPositionExecuter::~BehaviorPositionExecuter(void)
{
}

bool BehaviorPositionExecuter::Execute(const ActiveBehavior & beh)
{
	Logger::instance().LogGoToPoint(mSelfState.GetPos(), beh.mTarget, "@Position");

	return Dasher::instance().GoToPoint(mAgent, beh.mTarget, beh.mBuffer, beh.mPower, false, true);
}

BehaviorPositionPlanner::~BehaviorPositionPlanner()
{
}

BehaviorPositionPlanner::BehaviorPositionPlanner(Agent &agent) :
	BehaviorPlannerBase<BehaviorAttackData> (agent)
{
}


void BehaviorPositionPlanner::Plan(ActiveBehaviorList &behavior_list)
{
	if (!behavior_list.empty()) return;

	if (mSelfState.IsGoalie()) return;
	if (mStrategy.IsOppControl()) return;
	if (mWorldState.GetPlayMode() > PM_Opp_Mode) return;

	ActiveBehavior position(mAgent, BT_Position);

	position.mBuffer = 1.0;
	position.mPower = mSelfState.CorrectDashPowerForStamina(ServerParam::instance().maxDashPower());
	position.mTarget = mStrategy.GetTeammateSBSPPosition(mSelfState.GetUnum(), mBallState.GetPos());
	position.mEvaluation = Evaluation::instance().EvaluatePosition(position.mTarget, false);

	behavior_list.push_back(position);
}

