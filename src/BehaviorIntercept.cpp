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

#include <sstream>
#include "BehaviorIntercept.h"
#include "Agent.h"
#include "Strategy.h"
#include "InterceptInfo.h"
#include "Formation.h"
#include "Dasher.h"
#include "Logger.h"
#include "VisualSystem.h"
#include "BehaviorDribble.h"
#include "Evaluation.h"

using namespace std;

const BehaviorType BehaviorInterceptExecuter::BEHAVIOR_TYPE = BT_Intercept;

namespace {
bool ret = BehaviorExecutable::AutoRegister<BehaviorInterceptExecuter>();
}

BehaviorInterceptExecuter::BehaviorInterceptExecuter(Agent & agent): BehaviorExecuterBase<BehaviorAttackData>(agent)
{
	Assert(ret);
}

BehaviorInterceptExecuter::~BehaviorInterceptExecuter() {
}

bool BehaviorInterceptExecuter::Execute(const ActiveBehavior & intercept)
{
	Logger::instance().LogIntercept(intercept.mTarget, "@Intercept");

	return Dasher::instance().GetBall(mAgent);
}

BehaviorInterceptPlanner::BehaviorInterceptPlanner(Agent & agent): BehaviorPlannerBase<BehaviorAttackData>(agent)
{
}

BehaviorInterceptPlanner::~BehaviorInterceptPlanner() {
}

void BehaviorInterceptPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	if (mSelfState.IsKickable()) return;
	if (mWorldState.GetPlayMode() != PM_Play_On) return;

	if (mStrategy.GetMyInterCycle() <= mStrategy.GetMinTmInterCycle() && mStrategy.GetMyInterCycle() <= mStrategy.GetSureOppInterCycle() + 1) {
		ActiveBehavior intercept(mAgent, BT_Intercept, BDT_Intercept_Normal);

		intercept.mTarget = mStrategy.GetMyInterPos();
		intercept.mEvaluation = Evaluation::instance().EvaluatePosition(intercept.mTarget, true);

		behavior_list.push_back(intercept);
	}
}
