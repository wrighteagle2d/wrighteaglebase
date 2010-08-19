#include <sstream>
#include "BehaviorIntercept.h"
#include "Agent.h"
#include "Strategy.h"
#include "InterceptInfo.h"
#include "Formation.h"
#include "Dasher.h"
#include "Logger.h"
#include "VisualSystem.h"
#include "BehaviorDribble.h"
#include "Evaluation.h"

using namespace std;

const BehaviorType BehaviorInterceptExecuter::BEHAVIOR_TYPE = BT_Intercept;

namespace {
bool ret = BehaviorExecutable::AutoRegister<BehaviorInterceptExecuter>();
}

BehaviorInterceptExecuter::BehaviorInterceptExecuter(Agent & agent): BehaviorExecuterBase<BehaviorAttackData>(agent)
{
	Assert(ret);
}

BehaviorInterceptExecuter::~BehaviorInterceptExecuter() {
}

bool BehaviorInterceptExecuter::Execute(const ActiveBehavior & intercept)
{
	Logger::instance().LogIntercept(intercept.mTarget, "@Intercept");

	return Dasher::instance().GetBall(mAgent);
}

BehaviorInterceptPlanner::BehaviorInterceptPlanner(Agent & agent): BehaviorPlannerBase<BehaviorAttackData>(agent)
{
}

BehaviorInterceptPlanner::~BehaviorInterceptPlanner() {
}

void BehaviorInterceptPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	if (mSelfState.IsKickable()) return;

	if (mStrategy.GetMyInterCycle() <= mStrategy.GetMinTmInterCycle() && mStrategy.GetMyInterCycle() <= mStrategy.GetSureOppInterCycle() + 1) {
		ActiveBehavior intercept(mAgent, BT_Intercept, BDT_Intercept_Normal);

		intercept.mTarget = mStrategy.GetMyInterPos();
		intercept.mEvaluation = Evaluation::instance().EvaluatePosition(intercept.mTarget, true);

		behavior_list.push_back(intercept);
	}
}
