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

#include "BehaviorPenalty.h"
#include "WorldState.h"
#include "Agent.h"
#include "BehaviorGoalie.h"
#include "BehaviorShoot.h"
#include "BehaviorDribble.h"
#include "BehaviorIntercept.h"
#include "Kicker.h"
#include "Dasher.h"
#include "VisualSystem.h"

const BehaviorType BehaviorPenaltyExecuter::BEHAVIOR_TYPE = BT_Penalty;

namespace
{
	bool ret = BehaviorExecutable::AutoRegister<BehaviorPenaltyExecuter>();
}


//==============================================================================
BehaviorPenaltyExecuter::BehaviorPenaltyExecuter(Agent & agent): BehaviorExecuterBase<BehaviorAttackData>(agent)
{
	Assert(ret);
}


//==============================================================================y
bool BehaviorPenaltyExecuter::Execute(const ActiveBehavior &beh)
{
    const PlayerState & opp_goalie_state = mWorldState.GetOpponent(mWorldState.GetOpponentGoalieUnum());

	switch (beh.mDetailType)
    {
    case BDT_Setplay_Scan:
		mAgent.Turn(beh.mAngle);
        VisualSystem::instance().SetCanTurn(mAgent, false);
        break;
    case BDT_Setplay_Move:
        if (mSelfState.IsGoalie())
        {
            Dasher::instance().GoToPoint(mAgent, beh.mTarget, 0.26);
            VisualSystem::instance().RaiseBall(mAgent, 1.0);
        }
        else
        {
        	Dasher::instance().GoToPoint(mAgent, beh.mTarget, 0.26);
        }
        break;
    case BDT_Setplay_GetBall:
		if (mAgent.GetSelfUnum() == mStrategy.GetPenaltyTaker())
        {
			if (mWorldState.GetPlayMode() == PM_Our_Penalty_Ready)
            {
                if (mWorldState.CurrentTime() - mWorldState.GetPlayModeTime() < 
                    ServerParam::instance().penReadyWait() - 6) // wait
                {
                    Dasher::instance().GetTurnBodyToAngleAction(mAgent, 0.0).Execute(mAgent);
                    VisualSystem::instance().RaisePlayer(mAgent, opp_goalie_state.GetUnum(), 1.0);
                    VisualSystem::instance().SetCanTurn(mAgent, false);
                }
				else if (mSelfState.IsKickable() == true)
                {
                    Kicker::instance().KickBall(mAgent, beh.mAngle, beh.mKickSpeed);
                }
                else // 一般不会刚开始就不可踢，以防万一
                {
					Dasher::instance().GoToPoint(mAgent, beh.mTarget, 0.26);
                }
            }
			else if (mWorldState.GetPlayMode() == PM_Our_Penalty_Taken)
            {
				Dasher::instance().GoToPoint(mAgent,beh.mTarget, 0.26);
            }
            else
            {
                std::cerr << "what penalty_taker will do ???" << std::endl;
            }
        }
        else
        {
            Dasher::instance().GetTurnBodyToAngleAction(mAgent, 0.0).Execute(mAgent);
            VisualSystem::instance().RaisePlayer(mAgent, opp_goalie_state.GetUnum(), 1.0);
            VisualSystem::instance().SetCanTurn(mAgent, false);
        }
        break;
    default:
        break;
	}
	return true;
}


BehaviorPenaltyPlanner::BehaviorPenaltyPlanner(Agent & agent): BehaviorPlannerBase<BehaviorAttackData>( agent )
{
}

BehaviorPenaltyPlanner::~BehaviorPenaltyPlanner()
{
}

//==============================================================================
void BehaviorPenaltyPlanner::Plan(std::list<ActiveBehavior> &behaviorlist)
{
	ActiveBehavior penaltyKO(mAgent, BT_Penalty);

    if (mSelfState.IsGoalie())
    {
        switch (mWorldState.GetPlayMode())
        {
            case PM_Penalty_On_Our_Field:
		    case PM_Penalty_On_Opp_Field:
            case PM_Our_Penalty_Score:
            case PM_Our_Penalty_Miss:
            case PM_Opp_Penalty_Score:
            case PM_Opp_Penalty_Miss: // 对手或队友刚罚完点球，原地休息一会
            case PM_Our_Penalty_Setup:
            case PM_Our_Penalty_Ready:
	        case PM_Our_Penalty_Taken: // 队友罚点球，守门员在旁边休息，为了节省体力，不自己dash回去（rcssserver-13.*下不可能是守门员自己罚）
                penaltyKO.mDetailType = BDT_Setplay_Scan;
                break;
            case PM_Opp_Penalty_Setup:
                penaltyKO.mDetailType = BDT_Setplay_Move;
                break;
            case PM_Opp_Penalty_Ready:
            case PM_Opp_Penalty_Taken:
			    penaltyKO.mDetailType = BDT_Setplay_GetBall; // 正在罚点球
			    break;
            default:
                return;
        }
    }
    else
    {
	    switch (mWorldState.GetPlayMode())
        {
		    case PM_Penalty_On_Our_Field:
		    case PM_Penalty_On_Opp_Field:
            case PM_Our_Penalty_Score:
            case PM_Our_Penalty_Miss:
            case PM_Opp_Penalty_Score:
            case PM_Opp_Penalty_Miss:
			    penaltyKO.mDetailType = BDT_Setplay_Scan; // 原地休息
			    break;
            case PM_Our_Penalty_Setup:
            case PM_Opp_Penalty_Setup:
			    penaltyKO.mDetailType = BDT_Setplay_Move; // 走向目标位置
			    break;
            case PM_Our_Penalty_Ready:
		    case PM_Our_Penalty_Taken:
            case PM_Opp_Penalty_Ready:
            case PM_Opp_Penalty_Taken:
			    penaltyKO.mDetailType = BDT_Setplay_GetBall; // 正在罚点球
			    break;
		    default:
			    return;
	    }
    }

    if (penaltyKO.mDetailType == BDT_Setplay_Scan)
    {
		penaltyKO.mAngle = 60.0;
        behaviorlist.push_back(penaltyKO);
    }
	else if (penaltyKO.mDetailType == BDT_Setplay_Move)
    {
        if (mSelfState.IsGoalie())
        {
            penaltyKO.mTarget = Vector(-ServerParam::instance().PITCH_LENGTH / 2.0 + ServerParam::instance().penMaxGoalieDistX() - 1.0, 0.0);
        }
		else if (mAgent.GetSelfUnum() == mStrategy.GetPenaltyTaker())
        {
			if (mWorldState.CurrentTime() - mWorldState.GetPlayModeTime() < 5 && mStrategy.IsPenaltyFirstStep() != false)
            {
				mStrategy.SetPenaltyFirstStep(false);
            }

			if (mSelfState.GetPos().Dist(Vector(6.6, 0.0)) < 1.0 && mStrategy.IsPenaltyFirstStep() != true)
            {
                mStrategy.SetPenaltyFirstStep(true);
            }

            if (mStrategy.IsPenaltyFirstStep() == false)
            {
				penaltyKO.mTarget = Vector(6.6, 0.0);
            }
            else
            {
            	const Vector setup_ball_pos = Vector(ServerParam::instance().PITCH_LENGTH / 2.0 - ServerParam::instance().penDistX(), 0.0);
				penaltyKO.mTarget = (mBallState.GetPos().Dist(setup_ball_pos) < 1.0) ? setup_ball_pos : mBallState.GetPos();
            }
        }
		else
        {
			penaltyKO.mAngle = (mAgent.GetSelfUnum() - 2) * 36.0 + 18.0; // 2 ~ 11
			penaltyKO.mTarget = Polar2Vector(8.26, penaltyKO.mAngle);
		}

		penaltyKO.mEvaluation = 1.0;
		behaviorlist.push_back(penaltyKO);
	}
	else if (penaltyKO.mDetailType == BDT_Setplay_GetBall)
    {
		if (mSelfState.IsGoalie())
        {
			BehaviorInterceptPlanner(mAgent).Plan(behaviorlist);
			if(behaviorlist.empty() || mSelfState.IsBallCatchable())
            BehaviorGoaliePlanner(mAgent).Plan(behaviorlist);
        }
        else if (mStrategy.IsMyPenaltyTaken() == true)
        {
            // 先算dribble，shoot中根据dribble的情况来决策
            BehaviorDribblePlanner(mAgent).Plan(behaviorlist);
            BehaviorShootPlanner(mAgent).Plan(behaviorlist);
            BehaviorInterceptPlanner(mAgent).Plan(behaviorlist);
        }

        if (behaviorlist.empty())
        {
			penaltyKO.mTarget = mBallState.GetPos();
			penaltyKO.mAngle = ((mBallState.GetPos() - mSelfState.GetPos()).Dir() > 0.0) ? 36.0 : -36.0;
			penaltyKO.mKickSpeed = Min(1.0, Kicker::instance().GetMaxSpeed(mAgent,penaltyKO.mAngle, 1));
			penaltyKO.mEvaluation = 1.0;
			behaviorlist.push_back(penaltyKO);
        }
    }
}
