#include "BehaviorDefense.h"
#include "BehaviorFormation.h"
#include "BehaviorBlock.h"
#include "BehaviorMark.h"
#include "WorldState.h"
#include "Agent.h"
#include "Formation.h"
#include "Dasher.h"
#include "Logger.h"
#include "BehaviorIntercept.h"

BehaviorDefensePlanner::BehaviorDefensePlanner(Agent & agent): BehaviorPlannerBase <BehaviorDefenseData>( agent )
{
}

BehaviorDefensePlanner::~BehaviorDefensePlanner()
{
}

void BehaviorDefensePlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	BehaviorFormationPlanner(mAgent).Plan(behavior_list);
	BehaviorBlockPlanner(mAgent).Plan(behavior_list);
	BehaviorMarkPlanner(mAgent).Plan(behavior_list);

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
