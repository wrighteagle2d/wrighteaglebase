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

#include "BehaviorPass.h"
#include "Tackler.h"
#include "Dasher.h"
#include "Agent.h"
#include "WorldState.h"
#include "Strategy.h"
#include "Kicker.h"
#include "Dasher.h"
#include "InfoState.h"
#include "PositionInfo.h"
#include "Formation.h"
#include "Types.h"
#include "Logger.h"
#include "VisualSystem.h"
#include "CommunicateSystem.h"
#include "TimeTest.h"
#include "Evaluation.h"

#include <sstream>
using namespace std;


const BehaviorType BehaviorPassExecuter::BEHAVIOR_TYPE = BT_Pass;

namespace {
bool ret = BehaviorExecutable::AutoRegister<BehaviorPassExecuter>();
}

BehaviorPassExecuter::BehaviorPassExecuter(Agent &agent):
	BehaviorExecuterBase<BehaviorAttackData>( agent )
{
	Assert(ret);
}


BehaviorPassExecuter::~BehaviorPassExecuter(void)
{
}


bool BehaviorPassExecuter::Execute(const ActiveBehavior & pass)
{
	Logger::instance().LogPass(false, mBallState.GetPos(), pass.mTarget, "@Pass", true);
	return Kicker::instance().KickBall(mAgent, pass.mTarget, pass.mKickSpeed, KM_Quick);
}

BehaviorPassPlanner::BehaviorPassPlanner(Agent & agent):
	BehaviorPlannerBase<BehaviorAttackData>(agent)
{
}

BehaviorPassPlanner::~BehaviorPassPlanner(void)
{
}

void BehaviorPassPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	if (!mSelfState.IsKickable()) return;

	const std::vector<Unum> & tm2ball = mPositionInfo.GetCloseTeammateToTeammate(mSelfState.GetUnum());

	for (uint i = 0; i < tm2ball.size(); ++i) {
		ActiveBehavior pass(mAgent, BT_Pass, BDT_Pass_Direct);

		pass.mTarget = mWorldState.GetTeammate(tm2ball[i]).GetPredictedPos();

		Vector rel_target = pass.mTarget - mBallState.GetPos();
		const std::vector<Unum> & opp2tm = mPositionInfo.GetCloseOpponentToTeammate(tm2ball[i]);
		AngleDeg min_differ = HUGE_VALUE;

		for (uint j = 0; j < opp2tm.size(); ++j) {
			Vector rel_pos = mWorldState.GetOpponent(opp2tm[j]).GetPos() - mBallState.GetPos();
			if (rel_pos.Mod() > rel_target.Mod() + 3.0) continue;

			AngleDeg differ = GetAngleDegDiffer(rel_target.Dir(), rel_pos.Dir());
			if (differ < min_differ) {
				min_differ = differ;
			}
		}

		if (min_differ < 10.0) continue;

		pass.mEvaluation = Evaluation::instance().EvaluatePosition(pass.mTarget, true);

		pass.mKickSpeed = ServerParam::instance().GetBallSpeed(5 + random() % 6, pass.mTarget.Dist(mBallState.GetPos()));
		pass.mKickSpeed = MinMax(2.0, pass.mKickSpeed, ServerParam::instance().ballSpeedMax());

		mActiveBehaviorList.push_back(pass);
	}

	if (!mActiveBehaviorList.empty()) {
		mActiveBehaviorList.sort(std::greater<ActiveBehavior>());
		behavior_list.push_back(mActiveBehaviorList.front());
	}
}

