/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2013                                                    *
 * Copyright (c) 1998-2013 WrightEagle 2D Soccer Simulation Team,                   *
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
	if (act_bhv.mDetailType == BDT_Goalie_Position) {
		return Dasher::instance().GoToPoint(mAgent, act_bhv.mTarget);
	}
	else if (act_bhv.mDetailType == BDT_Goalie_Catch) {
		return mAgent.Catch(GetNormalizeAngleDeg((mBallState.GetPos() - mSelfState.GetPos()).Dir() - mSelfState.GetBodyDir()));
	}
	else {
		return false;
	}
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
	if(mAgent.IsLastActiveBehaviorInActOf(BT_Pass) || mAgent.IsLastActiveBehaviorInActOf(BT_Dribble))
		return;

	if (mSelfState.IsBallCatchable() && mStrategy.IsLastOppControl())
	 {
		ActiveBehavior catchball(mAgent, BT_Goalie, BDT_Goalie_Catch);

		catchball.mEvaluation = 1.0 + FLOAT_EPS;

		behavior_list.push_back(catchball);
	}
	else {
		Vector target;
		Ray ball_ray(ServerParam::instance().ourGoal(), (mAnalyser.mLightHouse - ServerParam::instance().ourGoal()).Dir());
		ServerParam::instance().ourGoalArea().Intersection(ball_ray, target);

		ActiveBehavior position(mAgent, BT_Goalie, BDT_Goalie_Position);

		position.mTarget = target;
		if(ServerParam::instance().ourPenaltyArea().IsWithin(target)){
		position.mEvaluation = Evaluation::instance().EvaluatePosition(position.mTarget, false);
		}
		else
			{
			Ray ball_ray(ServerParam::instance().ourGoal(), (mBallState.GetPos() - ServerParam::instance().ourGoal()).Dir());
			ServerParam::instance().ourGoalArea().Intersection(ball_ray, target);
			position.mEvaluation = Evaluation::instance().EvaluatePosition(position.mTarget, false);
			position.mTarget = target;
		}
		behavior_list.push_back(position);
	}
}
