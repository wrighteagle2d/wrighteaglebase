#include "BehaviorPass.h"
#include "Tackler.h"
#include "Dasher.h"
#include "Agent.h"
#include "WorldState.h"
#include "Strategy.h"
#include "Kicker.h"
#include "Dasher.h"
#include "InfoState.h"
#include "PositionInfo.h"
#include "Formation.h"
#include "Types.h"
#include "Logger.h"
#include "VisualSystem.h"
#include "CommunicateSystem.h"
#include "TimeTest.h"
#include "Evaluation.h"

#include <sstream>
using namespace std;


const BehaviorType BehaviorPassExecuter::BEHAVIOR_TYPE = BT_Pass;

namespace {
bool ret = BehaviorExecutable::AutoRegister<BehaviorPassExecuter>();
}

BehaviorPassExecuter::BehaviorPassExecuter(Agent &agent):
	BehaviorExecuterBase<BehaviorAttackData>( agent )
{
	Assert(ret);
}


BehaviorPassExecuter::~BehaviorPassExecuter(void)
{
}


bool BehaviorPassExecuter::Execute(const ActiveBehavior & pass)
{
	Logger::instance().LogPass(false, mBallState.GetPos(), pass.mTarget, "@Pass", true);
	return Kicker::instance().KickBall(mAgent, pass.mTarget, pass.mKickSpeed, KM_Quick);
}

BehaviorPassPlanner::BehaviorPassPlanner(Agent & agent):
	BehaviorPlannerBase<BehaviorAttackData>(agent)
{
}

BehaviorPassPlanner::~BehaviorPassPlanner(void)
{
}

void BehaviorPassPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	if (!mSelfState.IsKickable()) return;

	const std::vector<Unum> & tm2ball = mPositionInfo.GetCloseTeammateToTeammate(mSelfState.GetUnum());

	for (int i = 0; i <= Min(int(tm2ball.size()), 3); ++i) {
		ActiveBehavior pass(mAgent, BT_Pass, BDT_Pass_Direct);

		pass.mTarget = mWorldState.GetTeammate(tm2ball[i]).GetPredictedPos();
		pass.mEvaluation = Evaluation::instance().EvaluatePosition(pass.mTarget, true);

		pass.mKickSpeed = ServerParam::instance().GetBallSpeed(5 + random() % 6, pass.mTarget.Dist(mBallState.GetPos()));
		pass.mKickSpeed = MinMax(1.0, pass.mKickSpeed, ServerParam::instance().ballSpeedMax());

		mActiveBehaviorList.push_back(pass);
	}

	if (!mActiveBehaviorList.empty()) {
		mActiveBehaviorList.sort(std::greater<ActiveBehavior>());
		behavior_list.push_back(mActiveBehaviorList.front());
	}
}

