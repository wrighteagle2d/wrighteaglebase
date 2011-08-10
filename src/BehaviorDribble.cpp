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
#include <cmath>


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
	if(dribble.mDetailType == BDT_Dribble_Normal){
    Vector ballpos = mBallState.GetPos();
	PlayerState   oppState = mWorldState.GetOpponent(mPositionInfo.GetClosestOpponentToBall());
	  Vector  	 	posOpp   = oppState.GetPos();
    	ballpos = mBallState.GetPredictedPos(1);
    	Vector agentpos = mSelfState.GetPos();
    	Vector agent_v = agentpos - mSelfState.GetPos();
    	AngleDeg agentang = mSelfState.GetBodyDir();
		 AtomicAction act;
		 if ( mSelfState.GetStamina() < 2700){
			 Dasher::instance().GoToPoint(mAgent,act,dribble.mTarget,0.01,30);
		 }
		 else {
			 Dasher::instance().GoToPoint(mAgent,act,dribble.mTarget, 0.01 ,100 );
		 }
        agent_v = mSelfState.GetVel() * mSelfState.GetDecay();
        if(act.mType != CT_Dash){
        	agentpos = mSelfState.GetPredictedPos(1);
        }
        else
        	agentpos = mSelfState.GetPredictedPosWithDash(1,act.mDashPower,act.mDashDir);

    bool collide = mSelfState.GetCollideWithPlayer();
    if ( ballpos.Dist(agentpos) > 0.95 * ( mSelfState.GetKickableArea())
            || collide )//will not be kickable or will make a collision ??
    {
        int p = ( ( mBallState.GetPos() - mSelfState.GetPos() ).Dir() - mSelfState.GetBodyDir()) > 0 ? 1:-1 ;
		double outSpeed = mSelfState.GetVel().Mod();
		if ( act.mType == CT_Dash && fabs( act.mDashDir ) < FLOAT_EPS )
			outSpeed += mSelfState.GetAccelerationFront(act.mDashPower);
		if((agentpos + Polar2Vector(mSelfState.GetKickableArea(),agentang + p * 45) - mBallState.GetPos()).Mod() <
			(	agentpos + Polar2Vector(mSelfState.GetKickableArea(),agentang + p * 45) -mSelfState.GetPos()).Mod()){
	        if ( mSelfState.GetStamina() < 2700)
	        {
	            return Dasher::instance().GoToPoint( mAgent , dribble.mTarget,0.01,30 ); // dash slow
	        }
	        else return Dasher::instance().GoToPoint( mAgent , dribble.mTarget,0.01,100 );

		}
        	return Kicker::instance().KickBall(mAgent ,agentpos + Polar2Vector(mSelfState.GetKickableArea(),agentang + p * 45) , outSpeed,KM_Hard);
    }

    else {
        if ( mSelfState.GetStamina() < 2700)
        {
            return Dasher::instance().GoToPoint( mAgent , dribble.mTarget,0.01,30 ); // dash slow
        }
        else return Dasher::instance().GoToPoint( mAgent , dribble.mTarget,0.01,100 );
    }
	}
	else /*if(dribble.mDetailType == BDT_Dribble_Fast)*/{
	return Kicker::instance().KickBall(mAgent, dribble.mAngle, dribble.mKickSpeed, KM_Quick);
}

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
	if (mStrategy.IsForbidenDribble()) return;
	if (mSelfState.IsGoalie()) return;


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

		dribble.mTarget= mSelfState.GetPos() + Polar2Vector( mSelfState.GetEffectiveSpeedMax(), dir);

		dribble.mEvaluation = Evaluation::instance().EvaluatePosition(dribble.mTarget, true);

		mActiveBehaviorList.push_back(dribble);
	}
	double speed = mSelfState.GetEffectiveSpeedMax();

	for (AngleDeg dir = -90.0; dir < 90.0; dir += 2.5) {
		ActiveBehavior dribble(mAgent, BT_Dribble, BDT_Dribble_Fast);
		dribble.mKickSpeed = speed;
		dribble.mAngle = dir;

		const std::vector<Unum> & opp2ball = mPositionInfo.GetCloseOpponentToBall();
		Vector target = mBallState.GetPos() + Polar2Vector(dribble.mKickSpeed * 10, dribble.mAngle);
		if(!ServerParam::instance().pitchRectanglar().IsWithin(target)){
			continue;
		}
		bool ok = true;
		for (uint j = 0; j < opp2ball.size(); ++j) {
			Vector rel_pos = mWorldState.GetOpponent(opp2ball[j]).GetPos() - target;
			if (rel_pos.Mod() < dribble.mKickSpeed * 12 ||
					mWorldState.GetOpponent(opp2ball[j]).GetPosConf() < PlayerParam::instance().minValidConf()){
			ok = false;
			break;
			}
		}
		if(!ok){
			continue;
		}
		dribble.mEvaluation = 0;
		for (int i = 1; i <= 8; ++i) {
			dribble.mEvaluation += Evaluation::instance().EvaluatePosition(mBallState.GetPos() + Polar2Vector(dribble.mKickSpeed * i, dribble.mAngle), true);
		}
		dribble.mEvaluation /= 8;
		dribble.mTarget = target;

		mActiveBehaviorList.push_back(dribble);
	}

	if (!mActiveBehaviorList.empty()) {
		mActiveBehaviorList.sort(std::greater<ActiveBehavior>());
		behavior_list.push_back(mActiveBehaviorList.front());
	}
}
