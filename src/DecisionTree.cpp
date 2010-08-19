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

#include "DecisionTree.h"
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
			MutexPlan<BehaviorSetplayPlanner>(agent, active_behavior_list) ||
			MutexPlan<BehaviorAttackPlanner>(agent, active_behavior_list) ||
			MutexPlan<BehaviorGoaliePlanner>(agent, active_behavior_list);
		}
		else {
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
