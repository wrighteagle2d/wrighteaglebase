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

