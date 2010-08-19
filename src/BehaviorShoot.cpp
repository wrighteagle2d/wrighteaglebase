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

#include "BehaviorShoot.h"
#include "ServerParam.h"
#include "PlayerParam.h"
#include "VisualSystem.h"
#include "Utilities.h"
#include "Dasher.h"
#include "Tackler.h"
#include "Kicker.h"
#include "InterceptModel.h"
#include "ActionEffector.h"
#include "TimeTest.h"
#include "Evaluation.h"
#include <list>
#include <cmath>

using namespace std;

namespace {
bool ret = BehaviorExecutable::AutoRegister<BehaviorShootExecuter>();
}

const BehaviorType BehaviorShootExecuter::BEHAVIOR_TYPE = BT_Shoot;

/**
* BehaviorShootExecuter's constructor
*/
BehaviorShootExecuter::BehaviorShootExecuter(Agent & agent): BehaviorExecuterBase<BehaviorAttackData>(agent)
{
	Assert(ret);
}

/**
* BehaviorShootExecuter's constructor
*/
BehaviorShootExecuter::~BehaviorShootExecuter()
{
}

/**
* Execute an active behavior as shoot. May raise ball or player.
* \param act_bhv
* \return true  if success;
*         false otherwise.
*/
bool BehaviorShootExecuter::Execute(const ActiveBehavior & shoot)
{
	Logger::instance().LogShoot(mBallState.GetPos(), shoot.mTarget, "@Shoot");
	return Kicker::instance().KickBall(mAgent, shoot.mTarget, ServerParam::instance().ballSpeedMax(), KM_Hard, 0, true);
}

/**
* BehaviorShootPlanner's constructor
*/
BehaviorShootPlanner::BehaviorShootPlanner(Agent & agent):
	BehaviorPlannerBase<BehaviorAttackData>(agent)
{
}

/**
* BehaviorShootPlanner's destructor
*/
BehaviorShootPlanner::~BehaviorShootPlanner()
{
}

/**
* Plan.
* None or one ActiveBehavior will be push back to behavior_list.
*/
void BehaviorShootPlanner::Plan(list<ActiveBehavior> & behavior_list)
{
	if (mSelfState.IsKickable()) {
		if (mSelfState.GetPos().X() > ServerParam::instance().pitchRectanglar().Right() - PlayerParam::instance().shootMaxDistance()) {
			if (ServerParam::instance().oppPenaltyArea().IsWithin(mBallState.GetPos())) {
				Vector target = random() % 2? ServerParam::instance().oppLeftGoalPost() + Vector(0.0, 1.0): ServerParam::instance().oppRightGoalPost() + Vector(0.0, -1.0);

				ActiveBehavior shoot(mAgent, BT_Shoot);
				shoot.mTarget = target;
				shoot.mEvaluation = 1.0 + FLOAT_EPS;

				behavior_list.push_back(shoot);
			}
		}
	}
}
