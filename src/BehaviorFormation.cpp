#include "BehaviorFormation.h"
#include "VisualSystem.h"
#include "Formation.h"
#include "Dasher.h"
#include "BasicCommand.h"
#include "BehaviorPosition.h"
#include "Agent.h"
#include "PositionInfo.h"
#include "Logger.h"
#include "Evaluation.h"

const BehaviorType BehaviorFormationExecuter::BEHAVIOR_TYPE = BT_Formation;

namespace
{
bool ret = BehaviorExecutable::AutoRegister<BehaviorFormationExecuter>();
}

BehaviorFormationExecuter::BehaviorFormationExecuter(Agent & agent) :
	BehaviorExecuterBase<BehaviorDefenseData>(agent)
{
	Assert(ret);
}

BehaviorFormationExecuter::~BehaviorFormationExecuter(void)
{
}

bool BehaviorFormationExecuter::Execute(const ActiveBehavior & beh)
{
	Logger::instance().LogGoToPoint(mSelfState.GetPos(), beh.mTarget, "@Formation");

	return Dasher::instance().GoToPoint(mAgent, beh.mTarget, beh.mBuffer, beh.mPower, false, true);
}

BehaviorFormationPlanner::BehaviorFormationPlanner(Agent & agent):
	BehaviorPlannerBase<BehaviorDefenseData>( agent)
{
}

BehaviorFormationPlanner::~BehaviorFormationPlanner()
{
}

void BehaviorFormationPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	ActiveBehavior formation(mAgent, BT_Formation);

	formation.mBuffer = 1.0;
	formation.mPower = mSelfState.CorrectDashPowerForStamina(ServerParam::instance().maxDashPower());
	formation.mTarget = mAnalyser.mHome[mSelfState.GetUnum()];
	formation.mEvaluation = Evaluation::instance().EvaluatePosition(formation.mTarget, false);

	behavior_list.push_back(formation);
}

