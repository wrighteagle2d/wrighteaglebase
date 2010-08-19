#ifndef BEHAVIORMARK_H_
#define BEHAVIORMARK_H_

#include "BehaviorBase.h"

class BehaviorMarkExecuter : public BehaviorExecuterBase<BehaviorDefenseData>
{
public:
    BehaviorMarkExecuter(Agent & agent);
    ~BehaviorMarkExecuter(void);

	bool Execute(const ActiveBehavior & act_bhv);

	static const BehaviorType BEHAVIOR_TYPE;
};

class BehaviorMarkPlanner: public BehaviorPlannerBase<BehaviorDefenseData> {
public:
	BehaviorMarkPlanner(Agent & agent);
	virtual ~BehaviorMarkPlanner();

	void Plan(std::list<ActiveBehavior> & behavior_list);
};

#endif /* BEHAVIORFORMATION_H_ */
