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

#include "BehaviorSetplay.h"
#include "Formation.h"
#include "Kicker.h"
#include "Dasher.h"
#include "VisualSystem.h"
#include "BehaviorPass.h"
#include "Geometry.h"
#include "ServerParam.h"
#include "BehaviorShoot.h"
#include "BehaviorIntercept.h"
#include "Evaluation.h"
#include <stdio.h>
#include <algorithm>
using namespace std;

const BehaviorType BehaviorSetplayExecuter::BEHAVIOR_TYPE = BT_Setplay;

namespace
{
bool ret = BehaviorExecutable::AutoRegister<BehaviorSetplayExecuter>();
}

BehaviorSetplayExecuter::BehaviorSetplayExecuter(Agent & agent): BehaviorExecuterBase<BehaviorAttackData>( agent )
{
	Assert(ret);
}

BehaviorSetplayExecuter::~BehaviorSetplayExecuter(void)
{
}

bool BehaviorSetplayExecuter::Execute(const ActiveBehavior &setplay)
{
	if (mWorldState.GetPlayMode() == PM_Before_Kick_Off) {
		if (setplay.mDetailType == BDT_Setplay_Move) {
			return mAgent.Move(setplay.mTarget);
		}
		else if (setplay.mDetailType == BDT_Setplay_Scan) {
			if (mWorldState.GetPlayMode() == PM_Before_Kick_Off) {
				VisualSystem::instance().ForbidDecision(mAgent);
				return mAgent.Turn(50.0);
			}
			else {
				return true; //交给视觉决策
			}
		}
		else {
			PRINT_ERROR("Setplay Detail Type Error");
			return false;
		}
	}
	else {
		if (setplay.mDetailType == BDT_Setplay_GetBall) {
			return Dasher::instance().GetBall(mAgent);
		}
		else {
			PRINT_ERROR("Setplay Detail Type Error");
			return false;
		}
	}
}


BehaviorSetplayPlanner::BehaviorSetplayPlanner(Agent & agent): BehaviorPlannerBase<BehaviorAttackData>( agent )
{
}

BehaviorSetplayPlanner::~BehaviorSetplayPlanner(void)
{
}

void BehaviorSetplayPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	ActiveBehavior setplay(mAgent, BT_Setplay);

	setplay.mBuffer = 0.5;

	if (mWorldState.GetPlayMode() != PM_Play_On) {
		if (mWorldState.GetPlayMode() == PM_Before_Kick_Off) {
			setplay.mDetailType = BDT_Setplay_Move;
			setplay.mTarget = TeammateFormationTactic(KickOffPosition)(mSelfState.GetUnum(), mWorldState.GetKickOffMode() == KO_Ours);

			if (setplay.mTarget.Dist(mSelfState.GetPos()) < setplay.mBuffer) {
				setplay.mDetailType = BDT_Setplay_Scan;
			}

			setplay.mEvaluation = Evaluation::instance().EvaluatePosition(setplay.mTarget, true);

			behavior_list.push_back(setplay);
		}
		else if (mWorldState.GetPlayMode() < PM_Our_Mode) {
			if (mPositionInfo.GetClosestTeammateToBall() == mSelfState.GetUnum()) {
				if (!mSelfState.IsKickable()) {
					setplay.mDetailType = BDT_Setplay_GetBall;
					setplay.mTarget = mBallState.GetPos();
					setplay.mEvaluation = Evaluation::instance().EvaluatePosition(setplay.mTarget, true);

					behavior_list.push_back(setplay);
				}
				else {
					if (mWorldState.CurrentTime().T() - mWorldState.GetPlayModeTime().T() < 20) {
						setplay.mDetailType = BDT_Setplay_Scan;
						setplay.mEvaluation = Evaluation::instance().EvaluatePosition(setplay.mTarget, true);

						behavior_list.push_back(setplay);
					}
				}
			}
		}
	}
}
