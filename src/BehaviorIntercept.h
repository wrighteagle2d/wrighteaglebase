#ifndef BEHAVIORINTERCEPT_H_
#define BEHAVIORINTERCEPT_H_

#include "BehaviorBase.h"
#include "VisualSystem.h"

class BehaviorInterceptExecuter : public BehaviorExecuterBase<BehaviorAttackData> {
public:
	BehaviorInterceptExecuter(Agent & agent);
	virtual ~BehaviorInterceptExecuter();

	bool Execute(const ActiveBehavior &);

	void SubmitVisualRequest(const ActiveBehavior & intercept, double plus = 0.0) {
		Assert(intercept.mKeyOpp.mUnum >= 0);

		(void) plus;
		VisualSystem::instance().RaisePlayer(mAgent, -intercept.mKeyOpp.mUnum, 2.0);
	}

	static const BehaviorType BEHAVIOR_TYPE;
};

class BehaviorInterceptPlanner: public BehaviorPlannerBase<BehaviorAttackData> {
public:
	BehaviorInterceptPlanner(Agent & agent);
	virtual ~BehaviorInterceptPlanner();

	void Plan(std::list<ActiveBehavior> & behavior_list);
};

#endif /* BEHAVIORINTERCEPT_H_ */
