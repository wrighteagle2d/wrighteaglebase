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
