#include "BehaviorAttack.h"
#include "BehaviorShoot.h"
#include "BehaviorPass.h"
#include "BehaviorDribble.h"
#include "BehaviorIntercept.h"
#include "BehaviorPosition.h"
#include "WorldState.h"


BehaviorAttackPlanner::BehaviorAttackPlanner(Agent & agent): BehaviorPlannerBase<BehaviorAttackData>( agent )
{
}

BehaviorAttackPlanner::~BehaviorAttackPlanner()
{
}

void BehaviorAttackPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	BehaviorInterceptPlanner(mAgent).Plan(mActiveBehaviorList);
	BehaviorShootPlanner(mAgent).Plan(mActiveBehaviorList);
	BehaviorPassPlanner(mAgent).Plan(mActiveBehaviorList);
	BehaviorDribblePlanner(mAgent).Plan(mActiveBehaviorList);
	BehaviorPositionPlanner(mAgent).Plan(mActiveBehaviorList);

	if (!mActiveBehaviorList.empty()) {
		mActiveBehaviorList.sort(std::greater<ActiveBehavior>());
		behavior_list.push_back(mActiveBehaviorList.front());

		if (mActiveBehaviorList.size() > 1) { //允许非最优行为提交视觉请求
			double plus = 1.0;
			ActiveBehaviorPtr it = mActiveBehaviorList.begin();
			for (++it; it != mActiveBehaviorList.end(); ++it) {
				it->SubmitVisualRequest(plus);
				plus *= 2.0;
			}
		}
	}
}
