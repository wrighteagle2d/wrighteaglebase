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

#include "BehaviorGoalie.h"
#include "Logger.h"
#include "TimeTest.h"
#include "Utilities.h"
#include "Evaluation.h"

const BehaviorType BehaviorGoalieExecuter::BEHAVIOR_TYPE = BT_Goalie;

namespace
{
bool ret = BehaviorExecutable::AutoRegister<BehaviorGoalieExecuter>();
}

bool BehaviorGoalieExecuter::Execute(const ActiveBehavior & act_bhv)
{
	return Dasher::instance().GoToPoint(mAgent, act_bhv.mTarget);
}

BehaviorGoaliePlanner::BehaviorGoaliePlanner(Agent& agent): BehaviorPlannerBase<BehaviorDefenseData>(agent)
{
	Assert(ret);
}

BehaviorGoaliePlanner::~BehaviorGoaliePlanner(void)
{
}

BehaviorGoalieExecuter::BehaviorGoalieExecuter(Agent& agent): BehaviorExecuterBase<BehaviorDefenseData>(agent)
{
	Assert(ret);
}

BehaviorGoalieExecuter::~BehaviorGoalieExecuter(void)
{
}

void BehaviorGoaliePlanner::Plan(std::list<ActiveBehavior>& behavior_list)
{
	Vector target;
	Ray ball_ray(ServerParam::instance().ourGoal(), (mAnalyser.mLightHouse - ServerParam::instance().ourGoal()).Dir());
	ServerParam::instance().ourGoalArea().Intersection(ball_ray, target);

	ActiveBehavior position(mAgent, BT_Goalie);

	position.mTarget = target;
	position.mEvaluation = Evaluation::instance().EvaluatePosition(position.mTarget, false);

	behavior_list.push_back(position);
}
