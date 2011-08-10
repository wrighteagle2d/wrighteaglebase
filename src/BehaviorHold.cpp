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

#include "BehaviorHold.h"
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

const BehaviorType BehaviorHoldExecuter::BEHAVIOR_TYPE = BT_Hold;

namespace {
bool ret = BehaviorExecutable::AutoRegister<BehaviorHoldExecuter>();
}

BehaviorHoldExecuter::BehaviorHoldExecuter(Agent & agent) :
   BehaviorExecuterBase<BehaviorAttackData>( agent )
{
	Assert(ret);
}

BehaviorHoldExecuter::~BehaviorHoldExecuter(void)
{
}

bool BehaviorHoldExecuter::Execute(const ActiveBehavior & hold)
{
	Logger::instance().LogDribble(mBallState.GetPos(), hold.mTarget, "@Hold", true);
	//	Logger::instance().GetTextLogger("Hold")<< mWorldState.CurrentTime(). + "Hold";
	if(hold.mDetailType == BDT_Hold_Turn){
		return Dasher::instance().GetTurnBodyToAngleAction(mAgent, hold.mAngle).Execute(mAgent);
	}
	else
		return Kicker::instance().KickBallCloseToBody(mAgent ,hold.mAngle, 0.6);
}

BehaviorHoldPlanner::BehaviorHoldPlanner(Agent & agent) :
    BehaviorPlannerBase <BehaviorAttackData>(agent)
{
}


BehaviorHoldPlanner::~BehaviorHoldPlanner(void)
{
}


void BehaviorHoldPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	if (!mSelfState.IsKickable()) return;
	if (mSelfState.IsGoalie()) return;
	if(mStrategy.GetSureOppInterCycle() <= 2 &&
			mStrategy.GetSureOppInterCycle() != 0 ){

		ActiveBehavior hold(mAgent, BT_Hold);
		double        dDist;
		Vector   		posAgent = mSelfState.GetPos();
		PlayerState   objOpp = mWorldState.GetOpponent(mPositionInfo.GetClosestOpponentToBall());
		Vector  	 	posOpp   = objOpp.GetPos();
		dDist = (posOpp - posAgent).Mod();
		AngleDeg      angOpp   = objOpp.GetBodyDir();
		AngleDeg      ang      = 0.0;

		if(dDist < 5 )
		{
			ang = ( posAgent - posOpp ).Dir();
			int iSign = (GetNormalizeAngleDeg( angOpp - ang )) >0 ? -1:1;
			ang +=  iSign*45 - mSelfState.GetBodyDir();
			ang = GetNormalizeAngleDeg( ang );
		}

		if( mBallState.GetPos().Dist(posAgent + Polar2Vector(0.7,ang))< 0.3 )
		{

			Vector   posBallPred = mBallState.GetPredictedPos(1);
			Vector   posPred     = mSelfState.GetPredictedPos(1);
			if( posPred.Dist( posBallPred )< 0.85 * mSelfState.GetKickableArea() )
			{
				hold.mDetailType = BDT_Hold_Turn;
				hold.mEvaluation = 1.0 + FLOAT_EPS;
				hold.mAngle = (Vector(ServerParam::instance().PITCH_LENGTH / 2.0, 0.0) - mSelfState.GetPos()).Dir();
				mActiveBehaviorList.push_back(hold);
			}
		}
		else
		{
			hold.mDetailType = BDT_Hold_Kick;
			hold.mAngle = ang;
			hold.mEvaluation = 1.0 + FLOAT_EPS;
			mActiveBehaviorList.push_back(hold);
		}

		if (!mActiveBehaviorList.empty()) {
			mActiveBehaviorList.sort(std::greater<ActiveBehavior>());
			behavior_list.push_back(mActiveBehaviorList.front());
		}
	}

}
