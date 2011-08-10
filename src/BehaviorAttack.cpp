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

#include "BehaviorAttack.h"
#include "BehaviorShoot.h"
#include "BehaviorPass.h"
#include "BehaviorDribble.h"
#include "BehaviorIntercept.h"
#include "BehaviorPosition.h"
#include "WorldState.h"
#include "BehaviorHold.h"


BehaviorAttackPlanner::BehaviorAttackPlanner(Agent & agent): BehaviorPlannerBase<BehaviorAttackData>( agent )
{
}

BehaviorAttackPlanner::~BehaviorAttackPlanner()
{
}

void BehaviorAttackPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	if (mSelfState.IsBallCatchable()  && mStrategy.IsLastOppControl() &&(!(mStrategy.IsLastActiveBehaviorInActOf(BT_Pass)||mStrategy.IsLastActiveBehaviorInActOf(BT_Dribble)))) return;

	BehaviorInterceptPlanner(mAgent).Plan(mActiveBehaviorList);
	BehaviorShootPlanner(mAgent).Plan(mActiveBehaviorList);
	BehaviorPassPlanner(mAgent).Plan(mActiveBehaviorList);
	BehaviorDribblePlanner(mAgent).Plan(mActiveBehaviorList);
	BehaviorPositionPlanner(mAgent).Plan(mActiveBehaviorList);
	BehaviorHoldPlanner(mAgent).Plan(mActiveBehaviorList);

	if (!mActiveBehaviorList.empty()) {
		mActiveBehaviorList.sort(std::greater<ActiveBehavior>());
		behavior_list.push_back(mActiveBehaviorList.front());

		if (mActiveBehaviorList.size() > 1) { //允许非最优行为提交视觉请求
			double plus = 1.0;
			ActiveBehaviorPtr it = mActiveBehaviorList.begin();
			for (++it; it != mActiveBehaviorList.end(); ++it) {
				it->SubmitVisualRequest(plus);
				plus *= 2.0;
			}
		}
	}
}
