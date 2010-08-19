#ifndef __BehaviorDribble_H__
#define __BehaviorDribble_H__


#include <vector>
#include "BehaviorBase.h"
#include "VisualSystem.h"


class BehaviorDribbleExecuter : public BehaviorExecuterBase<BehaviorAttackData>
{
public:
    BehaviorDribbleExecuter(Agent & agent);
    virtual ~BehaviorDribbleExecuter(void);

    bool Execute(const ActiveBehavior &);

	void SubmitVisualRequest(const ActiveBehavior & dribble, double plus = 0.0) {
		Assert(dribble.mKeyOppGB.mUnum >= 0);
		Assert(dribble.mKeyOppGT.mUnum >= 0);

		VisualSystem::instance().RaisePlayer(mAgent, -dribble.mKeyOppGB.mUnum, 2.0 + plus);
		VisualSystem::instance().RaisePlayer(mAgent, -dribble.mKeyOppGT.mUnum, 2.0 + plus);
	}

    static const BehaviorType BEHAVIOR_TYPE;
};

class BehaviorDribblePlanner: public BehaviorPlannerBase <BehaviorAttackData>{
public:
    BehaviorDribblePlanner(Agent & agent);
    virtual ~BehaviorDribblePlanner(void);

    void Plan(std::list<ActiveBehavior> & behavior_list);
};


#endif

