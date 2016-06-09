/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2016                                                    *
 * Copyright (c) 1998-2016 WrightEagle 2D Soccer Simulation Team,                   *
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

#include "BehaviorFormation.h"
#include "VisualSystem.h"
#include "Formation.h"
#include "Dasher.h"
#include "BasicCommand.h"
#include "BehaviorPosition.h"
#include "Agent.h"
#include "PositionInfo.h"
#include "Logger.h"
#include <cstdlib>
#include "Evaluation.h"

const BehaviorType BehaviorFormationExecuter::BEHAVIOR_TYPE = BT_Formation;

namespace
{
bool ret = BehaviorExecutable::AutoRegister<BehaviorFormationExecuter>();
}

BehaviorFormationExecuter::BehaviorFormationExecuter(Agent & agent) :
	BehaviorExecuterBase<BehaviorDefenseData>(agent)
{
	Assert(ret);
}

BehaviorFormationExecuter::~BehaviorFormationExecuter(void)
{
}

bool BehaviorFormationExecuter::Execute(const ActiveBehavior & beh)
{
	Logger::instance().LogGoToPoint(mSelfState.GetPos(), beh.mTarget, "@Formation");

	return Dasher::instance().GoToPoint(mAgent, beh.mTarget, beh.mBuffer, beh.mPower, false, true);
}

BehaviorFormationPlanner::BehaviorFormationPlanner(Agent & agent):
	BehaviorPlannerBase<BehaviorDefenseData>( agent)
{
}

BehaviorFormationPlanner::~BehaviorFormationPlanner()
{
}

void BehaviorFormationPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	ActiveBehavior formation(mAgent, BT_Formation);

	formation.mBuffer = 1.0;
	formation.mPower = mSelfState.CorrectDashPowerForStamina(ServerParam::instance().maxDashPower());
	formation.mTarget = mAnalyser.mHome[mSelfState.GetUnum()];
	if(formation.mTarget.X() < mSelfState.GetPos().X() && mAgent.GetFormation().GetMyRole().mLineType == LT_Forward){
		formation.mPower = mSelfState.CorrectDashPowerForStamina(ServerParam::instance().maxDashPower())/2;
	}


	if(mAgent.GetFormation().GetMyRole().mLineType == LT_Defender){
		formation.mTarget.SetX(Min(0.0,formation.mTarget.X()));
		if(mBallState.GetPos().X() < -25){
			Unum goalie = mWorldState.GetTeammateGoalieUnum();
			Vector gpos = mWorldState.GetTeammate(goalie).GetPos();
			Vector left = (ServerParam::instance().ourLeftGoalPost() + gpos)/2;
			Vector right = (ServerParam::instance().ourRightGoalPost() + gpos)/2;
			Line l(gpos,mBallState.GetPos());
			if(l.IsPointInSameSide(left,ServerParam::instance().ourGoal())){
			const std::vector<Unum> & t2t = mPositionInfo.GetClosePlayerToPoint(left,goalie);
			if(t2t[0] == mSelfState.GetUnum()){
				formation.mTarget = left;
			}
			else if(mPositionInfo.GetClosePlayerToPoint(right,t2t[0])[0] == mSelfState.GetUnum()){
				formation.mTarget = right;
			} else if(mPositionInfo.GetClosePlayerToPoint(right,t2t[0])[0] == goalie && mPositionInfo.GetClosePlayerToPoint(right,t2t[0])[1] == mSelfState.GetUnum()){
				formation.mTarget = right;
			}
			}
			else {
				const std::vector<Unum> & t2t = mPositionInfo.GetClosePlayerToPoint(right,goalie);
				if(t2t[0] == mSelfState.GetUnum()){
					formation.mTarget = right;
				}
				else if(mPositionInfo.GetClosePlayerToPoint(left,t2t[0])[0] == mSelfState.GetUnum()){
					formation.mTarget = left;
				} else if(mPositionInfo.GetClosePlayerToPoint(left,t2t[0])[0] == goalie && mPositionInfo.GetClosePlayerToPoint(right,t2t[0])[1] == mSelfState.GetUnum()){
					formation.mTarget = left;
				}
			}

		}
		formation.mEvaluation = Evaluation::instance().EvaluatePosition(formation.mTarget, true);
	}

	else formation.mEvaluation = Evaluation::instance().EvaluatePosition(formation.mTarget, false);

	behavior_list.push_back(formation);
}

