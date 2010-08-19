#ifndef __BehaviorPass_H__
#define __BehaviorPass_H__

#include "BehaviorBase.h"
#include "VisualSystem.h"
#include <vector>

class BehaviorPassExecuter: public BehaviorExecuterBase<BehaviorAttackData>
{
public:
    BehaviorPassExecuter(Agent &agent);
    virtual ~BehaviorPassExecuter(void);

    bool Execute(const ActiveBehavior & act_bhv);
    void SubmitVisualRequest(const ActiveBehavior & pass, double plus = 0.0) {
    	if (pass.mKickCycle == 1) {
    		VisualSystem::instance().RaisePlayer(mAgent, pass.mKeyTm.mUnum, 3.0 + plus);
    		VisualSystem::instance().RaisePlayer(mAgent, -pass.mKeyOppGB.mUnum, 3.0 + plus);
    		VisualSystem::instance().RaisePlayer(mAgent, -pass.mKeyOppGT.mUnum, 3.0 + plus);
    	}
    	else {
    		VisualSystem::instance().RaisePlayer(mAgent, pass.mKeyTm.mUnum, 2.0 + plus);
    		VisualSystem::instance().RaisePlayer(mAgent, -pass.mKeyOppGB.mUnum, 3.0 + plus);
    		VisualSystem::instance().RaisePlayer(mAgent, -pass.mKeyOppGT.mUnum, 2.0 + plus);
    	}
    }

    static const BehaviorType BEHAVIOR_TYPE;
};

class BehaviorPassPlanner : public BehaviorPlannerBase<BehaviorAttackData> {
public:
	BehaviorPassPlanner(Agent &agent);
	virtual ~BehaviorPassPlanner(void);

	void Plan(std::list<ActiveBehavior> & behavior_list);
};

#endif

