#include "BehaviorGoalie.h"
#include "Logger.h"
#include "TimeTest.h"
#include "Utilities.h"
#include "Evaluation.h"

const BehaviorType BehaviorGoalieExecuter::BEHAVIOR_TYPE = BT_Goalie;

namespace
{
bool ret = BehaviorExecutable::AutoRegister<BehaviorGoalieExecuter>();
}

bool BehaviorGoalieExecuter::Execute(const ActiveBehavior & act_bhv)
{
	return Dasher::instance().GoToPoint(mAgent, act_bhv.mTarget);
}

BehaviorGoaliePlanner::BehaviorGoaliePlanner(Agent& agent): BehaviorPlannerBase<BehaviorDefenseData>(agent)
{
	Assert(ret);
}

BehaviorGoaliePlanner::~BehaviorGoaliePlanner(void)
{
}

BehaviorGoalieExecuter::BehaviorGoalieExecuter(Agent& agent): BehaviorExecuterBase<BehaviorDefenseData>(agent)
{
	Assert(ret);
}

BehaviorGoalieExecuter::~BehaviorGoalieExecuter(void)
{
}

void BehaviorGoaliePlanner::Plan(std::list<ActiveBehavior>& behavior_list)
{
	Vector target;
	Ray ball_ray(ServerParam::instance().ourGoal(), (mAnalyser.mLightHouse - ServerParam::instance().ourGoal()).Dir());
	ServerParam::instance().ourGoalArea().Intersection(ball_ray, target);

	ActiveBehavior position(mAgent, BT_Goalie);

	position.mTarget = target;
	position.mEvaluation = Evaluation::instance().EvaluatePosition(position.mTarget, false);

	behavior_list.push_back(position);
}
