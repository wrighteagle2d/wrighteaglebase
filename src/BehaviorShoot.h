#ifndef BehaviorShoot_H_
#define BehaviorShoot_H_

#include "BehaviorBase.h"
#include "Types.h"
#include "Agent.h"
#include "PlayerState.h"
#include "Dasher.h"
#include "VisualSystem.h"
#include "Logger.h"

class BehaviorShootExecuter: public BehaviorExecuterBase<BehaviorAttackData> {
public:
	BehaviorShootExecuter(Agent & agent);
	virtual ~BehaviorShootExecuter();

	bool Execute(const ActiveBehavior &);

	void SubmitVisualRequest(const ActiveBehavior & shoot, double plus = 0.0) {
		Assert(shoot.mKeyOppGT.mUnum >= 0);
		Assert(shoot.mKeyOppGB.mUnum >= 0);

		VisualSystem::instance().RaisePlayer(mAgent, -mWorldState.GetOpponentGoalieUnum(), 1.0 + plus);
		VisualSystem::instance().RaisePlayer(mAgent, -shoot.mKeyOppGT.mUnum, 2.0 + plus);
		VisualSystem::instance().RaisePlayer(mAgent, -shoot.mKeyOppGB.mUnum, 2.0 + plus);
	}

	static const BehaviorType BEHAVIOR_TYPE;
};

class BehaviorShootPlanner: public BehaviorPlannerBase<BehaviorAttackData>
{
public:
	BehaviorShootPlanner(Agent & agent);
	virtual ~BehaviorShootPlanner();

	void Plan(std::list<ActiveBehavior> & behavior_list);
};

#endif /* BehaviorShoot_H_ */
