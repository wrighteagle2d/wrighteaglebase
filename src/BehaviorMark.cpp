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

#include "BehaviorMark.h"
#include "VisualSystem.h"
#include "Formation.h"
#include "Dasher.h"
#include "BasicCommand.h"
#include "BehaviorPosition.h"
#include "Agent.h"
#include "PositionInfo.h"
#include "Logger.h"
#include "Evaluation.h"

const BehaviorType BehaviorMarkExecuter::BEHAVIOR_TYPE = BT_Mark;

namespace
{
bool ret = BehaviorExecutable::AutoRegister<BehaviorMarkExecuter>();
}

BehaviorMarkExecuter::BehaviorMarkExecuter(Agent & agent) :
	BehaviorExecuterBase<BehaviorDefenseData>(agent)
{
	Assert(ret);
}

BehaviorMarkExecuter::~BehaviorMarkExecuter(void)
{
}

bool BehaviorMarkExecuter::Execute(const ActiveBehavior & beh)
{
	Logger::instance().LogGoToPoint(mSelfState.GetPos(), beh.mTarget, "@Mark");

	return Dasher::instance().GoToPoint(mAgent, beh.mTarget, beh.mBuffer, beh.mPower, false, false);
}

BehaviorMarkPlanner::BehaviorMarkPlanner(Agent & agent):
	BehaviorPlannerBase<BehaviorDefenseData>( agent)
{
}

BehaviorMarkPlanner::~BehaviorMarkPlanner()
{
}

void BehaviorMarkPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	Unum closest_opp = mPositionInfo.GetClosestOpponentToTeammate(mSelfState.GetUnum());
	Unum closest_tm = mPositionInfo.GetClosestTeammateToOpponent(closest_opp);

	if (closest_opp && closest_tm && closest_tm == mSelfState.GetUnum()) {
		ActiveBehavior mark(mAgent, BT_Mark);

		mark.mBuffer = mSelfState.GetKickableArea();
		mark.mPower = mSelfState.CorrectDashPowerForStamina(ServerParam::instance().maxDashPower());
		mark.mTarget = mWorldState.GetOpponent(closest_opp).GetPos();
		mark.mEvaluation = Evaluation::instance().EvaluatePosition(mark.mTarget, false);

		behavior_list.push_back(mark);
	}
}

