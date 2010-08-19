#include "BehaviorPosition.h"
#include "Agent.h"
#include "Strategy.h"
#include "InterceptInfo.h"
#include "Dasher.h"
#include "VisualSystem.h"
#include "BehaviorPass.h"
#include "Logger.h"
#include "TimeTest.h"
#include <sstream>
#include "Evaluation.h"


using namespace std;

const BehaviorType BehaviorPositionExecuter::BEHAVIOR_TYPE = BT_Position;

namespace {
bool ret = BehaviorExecutable::AutoRegister<BehaviorPositionExecuter>();
}


BehaviorPositionExecuter::BehaviorPositionExecuter(Agent & agent) :
	BehaviorExecuterBase<BehaviorAttackData> (agent)
{
	Assert(ret);
}

BehaviorPositionExecuter::~BehaviorPositionExecuter(void)
{
}

bool BehaviorPositionExecuter::Execute(const ActiveBehavior & beh)
{
	std::ostringstream dummy;
	Logger::instance().LogPosition(mSelfState.GetPos(), beh.mTarget, BDT_Position_Normal, dummy);

	return Dasher::instance().GoToPoint(mAgent, beh.mTarget, beh.mBuffer, beh.mPower, false, true);
}

BehaviorPositionPlanner::~BehaviorPositionPlanner()
{
}

BehaviorPositionPlanner::BehaviorPositionPlanner(Agent &agent) :
	BehaviorPlannerBase<BehaviorAttackData> (agent)
{
}


void BehaviorPositionPlanner::Plan(ActiveBehaviorList &behavior_list)
{
	if (!behavior_list.empty()) return;
	if (mSelfState.IsGoalie()) return;
	if (!mStrategy.IsTmControl()) return;

	ActiveBehavior position(mAgent, BT_Formation);

	position.mBuffer = 1.0;
	position.mPower = mSelfState.CorrectDashPowerForStamina(ServerParam::instance().maxDashPower());
	position.mTarget = mStrategy.GetTeammateSBSPPosition(mSelfState.GetUnum(), mBallState.GetPos());
	position.mEvaluation = Evaluation::instance().EvaluatePosition(position.mTarget, false);

	behavior_list.push_back(position);
}

