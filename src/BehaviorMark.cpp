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

		Vector ballPos = mBallState.GetPos();
		AngleDeg b2o = (mBallState.GetPos()- mWorldState.GetOpponent(closest_opp).GetPos()).Dir();
		mark.mBuffer = mSelfState.GetKickableArea();
		mark.mPower = mSelfState.CorrectDashPowerForStamina(ServerParam::instance().maxDashPower());
		mark.mTarget = mWorldState.GetOpponent(closest_opp).GetPos()  + Polar2Vector(mark.mBuffer , b2o);
		mark.mEvaluation = Evaluation::instance().EvaluatePosition(mark.mTarget, false);
		if( mAgent.GetFormation().GetMyRole().mLineType == LT_Defender){
			mark.mEvaluation = Evaluation::instance().EvaluatePosition(mark.mTarget, true);
		}

		behavior_list.push_back(mark);
	}
}

