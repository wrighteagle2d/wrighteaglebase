#ifndef __BehaviorSetplay_H__
#define __BehaviorSetplay_H__

#include "BehaviorBase.h"

class BehaviorSetplayExecuter : public BehaviorExecuterBase<BehaviorAttackData>
{
public:
	BehaviorSetplayExecuter(Agent & agent);
	~BehaviorSetplayExecuter(void);

	bool Execute(const ActiveBehavior &act_bhv);

public:
	static const BehaviorType BEHAVIOR_TYPE;
};

class BehaviorSetplayPlanner: public BehaviorPlannerBase<BehaviorAttackData>
{
public:
	BehaviorSetplayPlanner(Agent & agent);
	~BehaviorSetplayPlanner();

	void Plan(std::list<ActiveBehavior> & behavior_list);
};

#endif
