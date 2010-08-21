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

#include "BehaviorDribble.h"
#include "Agent.h"
#include "Kicker.h"
#include "WorldState.h"
#include "Strategy.h"
#include "Formation.h"
#include "Dasher.h"
#include "Types.h"
#include "Logger.h"
#include "VisualSystem.h"
#include "CommunicateSystem.h"
#include "Utilities.h"
#include <sstream>
#include <vector>
#include <utility>
#include "Evaluation.h"


using namespace std;

const BehaviorType BehaviorDribbleExecuter::BEHAVIOR_TYPE = BT_Dribble;

namespace {
bool ret = BehaviorExecutable::AutoRegister<BehaviorDribbleExecuter>();
}

BehaviorDribbleExecuter::BehaviorDribbleExecuter(Agent & agent) :
    BehaviorExecuterBase<BehaviorAttackData>( agent )
{
	Assert(ret);
}

BehaviorDribbleExecuter::~BehaviorDribbleExecuter(void)
{
}

bool BehaviorDribbleExecuter::Execute(const ActiveBehavior & dribble)
{
	Logger::instance().LogDribble(mBallState.GetPos(), dribble.mTarget, "@Dribble", true);
	return Kicker::instance().KickBall(mAgent, dribble.mAngle, dribble.mKickSpeed, KM_Quick);
}

BehaviorDribblePlanner::BehaviorDribblePlanner(Agent & agent) :
    BehaviorPlannerBase <BehaviorAttackData>(agent)
{
}


BehaviorDribblePlanner::~BehaviorDribblePlanner(void)
{
}


void BehaviorDribblePlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	if (!mSelfState.IsKickable()) return;
	if (mWorldState.GetPlayMode() != PM_Play_On) return;
	if (mStrategy.IsForbidenDribble()) return;
	if (mSelfState.IsGoalie()) return;

	double speed = mSelfState.GetEffectiveSpeedMax();

	for (AngleDeg dir = -90.0; dir < 90.0; dir += 2.5) {
		ActiveBehavior dribble(mAgent, BT_Dribble, BDT_Dribble_Normal);

		dribble.mAngle = dir;

		const std::vector<Unum> & opp2ball = mPositionInfo.GetCloseOpponentToBall();
		AngleDeg min_differ = HUGE_VALUE;

		for (uint j = 0; j < opp2ball.size(); ++j) {
			Vector rel_pos = mWorldState.GetOpponent(opp2ball[j]).GetPos() - mBallState.GetPos();
			if (rel_pos.Mod() > 15.0) continue;

			AngleDeg differ = GetAngleDegDiffer(dir, rel_pos.Dir());
			if (differ < min_differ) {
				min_differ = differ;
			}
		}

		if (min_differ < 10.0) continue;

		dribble.mKickSpeed = speed;

		dribble.mEvaluation = 0.0;
		for (int i = 1; i <= 10; ++i) { //这里简单假设该方向可以连续带球10周期
			dribble.mEvaluation += Evaluation::instance().EvaluatePosition(mBallState.GetPos() + Polar2Vector(dribble.mKickSpeed * i, dribble.mAngle), true);
		}
		dribble.mEvaluation /= 10;

		dribble.mTarget = mBallState.GetPos() + Polar2Vector(dribble.mKickSpeed, dribble.mAngle);

		mActiveBehaviorList.push_back(dribble);
	}

	if (!mActiveBehaviorList.empty()) {
		mActiveBehaviorList.sort(std::greater<ActiveBehavior>());
		behavior_list.push_back(mActiveBehaviorList.front());
	}
}
