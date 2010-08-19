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

#include "BehaviorBlock.h"
#include "VisualSystem.h"
#include "Formation.h"
#include "Dasher.h"
#include "BasicCommand.h"
#include "BehaviorPosition.h"
#include "Agent.h"
#include "PositionInfo.h"
#include "Logger.h"
#include "Evaluation.h"

const BehaviorType BehaviorBlockExecuter::BEHAVIOR_TYPE = BT_Block;

namespace
{
bool ret = BehaviorExecutable::AutoRegister<BehaviorBlockExecuter>();
}

BehaviorBlockExecuter::BehaviorBlockExecuter(Agent & agent) :
	BehaviorExecuterBase<BehaviorDefenseData>(agent)
{
	Assert(ret);
}

BehaviorBlockExecuter::~BehaviorBlockExecuter(void)
{
}

bool BehaviorBlockExecuter::Execute(const ActiveBehavior & beh)
{
	Logger::instance().LogGoToPoint(mSelfState.GetPos(), beh.mTarget, "@Block");

	return Dasher::instance().GoToPoint(mAgent, beh.mTarget, beh.mBuffer, beh.mPower, true, false);
}

BehaviorBlockPlanner::BehaviorBlockPlanner(Agent & agent):
	BehaviorPlannerBase<BehaviorDefenseData>( agent)
{
}

BehaviorBlockPlanner::~BehaviorBlockPlanner()
{
}

void BehaviorBlockPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	Unum fastest_tm = mStrategy.GetFastestTm();

	if (fastest_tm == mSelfState.GetUnum()) {
		ActiveBehavior block(mAgent, BT_Block);

		block.mBuffer = 0.5;
		block.mPower = mSelfState.CorrectDashPowerForStamina(ServerParam::instance().maxDashPower());
		block.mTarget = mAnalyser.mLightHouse;
		block.mEvaluation = 1.0 + FLOAT_EPS;

		behavior_list.push_back(block);
	}
}

