#ifndef __BehaviorPosition_H__
#define __BehaviorPosition_H__


#include "BehaviorBase.h"
#include "Formation.h"
#include "WorldState.h"
#include "BehaviorPass.h"
#include "PositionInfo.h"
#include "VisualSystem.h"


class BehaviorPositionExecuter: public BehaviorExecuterBase<BehaviorAttackData>
{
public:
	BehaviorPositionExecuter(Agent &agent);
	virtual ~BehaviorPositionExecuter(void);

	bool Execute(const ActiveBehavior &);

	void SubmitVisualRequest(const ActiveBehavior & position, double plus = 0.0) {
		(void) position;
		(void) plus;

		if (fabs(mSelfState.GetPos().X() - mPositionInfo.GetTeammateOffsideLine()) < 30.0) {
			VisualSystem::instance().RaisePlayer(mAgent, -mPositionInfo.GetTeammateOffsideLineOpp(), 3.0);
		}
	}

	static const BehaviorType BEHAVIOR_TYPE;
};


class BehaviorPositionPlanner: public BehaviorPlannerBase<BehaviorAttackData>
{
public:
	BehaviorPositionPlanner(Agent &agent);
	virtual ~BehaviorPositionPlanner();
	void Plan(ActiveBehaviorList & behavior_list);
};


#endif

