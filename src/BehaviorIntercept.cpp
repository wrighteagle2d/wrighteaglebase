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
#include "BehaviorBase.h"

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
	PlayMode play_mode = mWorldState.GetPlayMode();
	if ( play_mode != PM_Play_On &&
	     play_mode != PM_Our_Penalty_Ready &&
	     play_mode != PM_Our_Penalty_Taken &&
	     play_mode != PM_Opp_Penalty_Taken)
	{
		return;
	}
	if (!mInterceptInfo.IsPlayerBallInterceptable(mSelfState.GetUnum()) ){
		if(mSelfState.IsGoalie()){
			ActiveBehavior intercept(mAgent, BT_Intercept, BDT_Intercept_Normal);
			Ray a (mBallState.GetPos(),mBallState.GetVel().Dir());
			Line c(ServerParam::instance().ourLeftGoalPost(),ServerParam::instance().ourRightGoalPost());
			c.Intersection(a,intercept.mTarget);
			if(intercept.mTarget.Y() < ServerParam::instance().ourLeftGoalPost().Y()
				|| intercept.mTarget.Y() > ServerParam::instance().ourRightGoalPost().Y()){
				return;
			}
			else {
				intercept.mEvaluation = Evaluation::instance().EvaluatePosition(intercept.mTarget, true);
				behavior_list.push_back(intercept);
			}
		}
		else {
		return;
		}
	}

	Rectangular GoalieInterRec = ServerParam::instance().ourGoalArea();
	GoalieInterRec.SetRight(ServerParam::instance().ourPenaltyArea().Right());

	if (mStrategy.GetMyInterCycle() <= mStrategy.GetMinTmInterCycle() && mStrategy.GetMyInterCycle() <= mStrategy.GetSureOppInterCycle() + 1 && (!mSelfState.IsGoalie())) {
		ActiveBehavior intercept(mAgent, BT_Intercept, BDT_Intercept_Normal);

		intercept.mTarget = mStrategy.GetMyInterPos();
		intercept.mEvaluation = Evaluation::instance().EvaluatePosition(intercept.mTarget, true);

		behavior_list.push_back(intercept);
	}
	else if (mPositionInfo.GetClosestOpponentToBall() == 0 ||
  		((mWorldState.GetTeammate(mPositionInfo.GetClosestTeammateToBall()).GetPos()-mWorldState.GetOpponent(mPositionInfo.GetClosestOpponentToBall()).GetPos()).Mod() <= 1 &&
			mSelfState.GetUnum() == mPositionInfo.GetCloseTeammateToBall().at(1)))//0.4受PlayerSize影响，这里直接使用0.3
	{
		ActiveBehavior intercept(mAgent, BT_Intercept, BDT_Intercept_Normal);
		intercept.mTarget = mStrategy.GetMyInterPos();
		intercept.mEvaluation = Evaluation::instance().EvaluatePosition(intercept.mTarget, true);
		behavior_list.push_back(intercept);

	}
	else if (mSelfState.IsGoalie() && GoalieInterRec.IsWithin(mStrategy.GetMyInterPos()))
	{
		ActiveBehavior intercept(mAgent, BT_Intercept, BDT_Intercept_Normal);
		intercept.mTarget = mStrategy.GetMyInterPos();
		intercept.mEvaluation = Evaluation::instance().EvaluatePosition(intercept.mTarget, true);
		if(mWorldState.GetPlayMode() == PM_Opp_Penalty_Taken){
			intercept.mEvaluation = Evaluation::instance().EvaluatePosition(intercept.mTarget, false);
		}
		behavior_list.push_back(intercept);
	}
	else if (mSelfState.GetUnum() == mPositionInfo.GetClosestTeammateToBall()
			&& !mSelfState.IsGoalie()){
		ActiveBehavior intercept(mAgent, BT_Intercept, BDT_Intercept_Normal);
		intercept.mTarget = mStrategy.GetMyInterPos();
		intercept.mEvaluation = Evaluation::instance().EvaluatePosition(intercept.mTarget, true);
		behavior_list.push_back(intercept);
	}
	else if (mPositionInfo.GetClosestTeammateToBall() == mWorldState.GetTeammateGoalieUnum()
			&& mSelfState.GetUnum() == mPositionInfo.GetCloseTeammateToBall()[1]){

		ActiveBehavior intercept(mAgent, BT_Intercept, BDT_Intercept_Normal);
		intercept.mTarget = mStrategy.GetMyInterPos();
		intercept.mEvaluation = Evaluation::instance().EvaluatePosition(intercept.mTarget, true);
		behavior_list.push_back(intercept);
	}
}
