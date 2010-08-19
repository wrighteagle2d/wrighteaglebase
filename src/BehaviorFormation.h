#ifndef BEHAVIORFORMATION_H_
#define BEHAVIORFORMATION_H_

#include "BehaviorBase.h"

class BehaviorFormationExecuter : public BehaviorExecuterBase<BehaviorDefenseData>
{
public:
    BehaviorFormationExecuter(Agent & agent);
    ~BehaviorFormationExecuter(void);

	bool Execute(const ActiveBehavior & act_bhv);

	static const BehaviorType BEHAVIOR_TYPE;
};

class BehaviorFormationPlanner: public BehaviorPlannerBase<BehaviorDefenseData> {
public:
	BehaviorFormationPlanner(Agent & agent);
	virtual ~BehaviorFormationPlanner();

	void Plan(std::list<ActiveBehavior> & behavior_list);
};

#endif /* BEHAVIORFORMATION_H_ */
