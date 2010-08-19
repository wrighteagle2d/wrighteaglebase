#ifndef DECISIONTREE_H_
#define DECISIONTREE_H_

#include <list>
#include "BehaviorBase.h"

class Agent;

class DecisionTree {
public:
	DecisionTree() {}
	virtual ~DecisionTree() {}

	/**
	 * 做决策
	 * @param agent
	 */
	bool Decision(Agent & agent);

private:
	/**
	* 搜索决策树，完成对（状态、动作）的评估
	* @param agent 当前节点的局策主体
	* @param step 当前节点地深度（这里深度是反过来计数地，step = 1是最后一层，step = max_step 是第一层）
	* @return
	*/
	ActiveBehavior Search(Agent & agent, int step);

	ActiveBehavior GetBestActiveBehavior(Agent & agent, std::list<ActiveBehavior> & behavior_list);

	template <typename BehaviorDerived>
	bool MutexPlan(Agent & agent, std::list<ActiveBehavior> & active_behavior_list){
		BehaviorDerived(agent).Plan(active_behavior_list);
		return !active_behavior_list.empty();
	}
};

#endif /* DECISIONTREE_H_ */
