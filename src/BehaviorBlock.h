#ifndef BEHAVIORBLOCK_H_
#define BEHAVIORBLOCK_H_

#include "BehaviorBase.h"

class BehaviorBlockExecuter : public BehaviorExecuterBase<BehaviorDefenseData>
{
public:
    BehaviorBlockExecuter(Agent & agent);
    ~BehaviorBlockExecuter(void);

	bool Execute(const ActiveBehavior & act_bhv);

	static const BehaviorType BEHAVIOR_TYPE;
};

class BehaviorBlockPlanner: public BehaviorPlannerBase<BehaviorDefenseData> {
public:
	BehaviorBlockPlanner(Agent & agent);
	virtual ~BehaviorBlockPlanner();

	void Plan(std::list<ActiveBehavior> & behavior_list);
};

#endif /* BEHAVIORFORMATION_H_ */
