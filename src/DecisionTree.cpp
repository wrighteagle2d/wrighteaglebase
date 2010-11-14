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

#include "DecisionTree.h"
#include "BehaviorPenalty.h"
#include "BehaviorSetplay.h"
#include "BehaviorAttack.h"
#include "BehaviorGoalie.h"
#include "BehaviorDefense.h"
#include "BehaviorIntercept.h"
#include "Agent.h"
#include "Strategy.h"
#include "TimeTest.h"

bool DecisionTree::Decision(Agent & agent)
{
	Assert(agent.GetSelf().IsAlive());

	ActiveBehavior beh = Search(agent, 1);

	if (beh.GetType() != BT_None) {
		agent.GetStrategy().SetActiveBehaviorInAct(beh.GetType());
		Assert(&beh.GetAgent() == &agent);
		return beh.Execute();
	}
	return false;
}

ActiveBehavior DecisionTree::Search(Agent & agent, int step)
{
	if (step == 1) {
		if (agent.GetSelf().IsIdling()) {
			return ActiveBehavior(agent, BT_None);
		}

		std::list<ActiveBehavior> active_behavior_list;

		if (agent.GetSelf().IsGoalie()) {
			MutexPlan<BehaviorPenaltyPlanner>(agent, active_behavior_list) ||
			MutexPlan<BehaviorSetplayPlanner>(agent, active_behavior_list) ||
			MutexPlan<BehaviorAttackPlanner>(agent, active_behavior_list) ||
			MutexPlan<BehaviorGoaliePlanner>(agent, active_behavior_list);
		}
		else {
			MutexPlan<BehaviorPenaltyPlanner>(agent, active_behavior_list) ||
			MutexPlan<BehaviorSetplayPlanner>(agent, active_behavior_list) ||
			MutexPlan<BehaviorAttackPlanner>(agent, active_behavior_list) ||
			MutexPlan<BehaviorDefensePlanner>(agent, active_behavior_list);
		}

		if (!active_behavior_list.empty()){
			return GetBestActiveBehavior(agent, active_behavior_list);
		}
		else {
			return ActiveBehavior(agent, BT_None);
		}
	}
	else {
		return ActiveBehavior(agent, BT_None);
	}
}

ActiveBehavior DecisionTree::GetBestActiveBehavior(Agent & agent, std::list<ActiveBehavior> & behavior_list)
{
	agent.GetStrategy().SaveActiveBehaviorList(behavior_list); //behavior_list里面存储了本周期所有behavior决策出的最优activebehavior，这里统一保存一下，供特定behavior下周期plan时用

	behavior_list.sort(std::greater<ActiveBehavior>());

	return behavior_list.front();
}
