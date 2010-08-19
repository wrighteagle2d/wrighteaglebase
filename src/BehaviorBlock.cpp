#include "BehaviorBlock.h"
#include "VisualSystem.h"
#include "Formation.h"
#include "Dasher.h"
#include "BasicCommand.h"
#include "BehaviorPosition.h"
#include "Agent.h"
#include "PositionInfo.h"
#include "Logger.h"
#include "Evaluation.h"

const BehaviorType BehaviorBlockExecuter::BEHAVIOR_TYPE = BT_Block;

namespace
{
bool ret = BehaviorExecutable::AutoRegister<BehaviorBlockExecuter>();
}

BehaviorBlockExecuter::BehaviorBlockExecuter(Agent & agent) :
	BehaviorExecuterBase<BehaviorDefenseData>(agent)
{
	Assert(ret);
}

BehaviorBlockExecuter::~BehaviorBlockExecuter(void)
{
}

bool BehaviorBlockExecuter::Execute(const ActiveBehavior & beh)
{
	Logger::instance().LogGoToPoint(mSelfState.GetPos(), beh.mTarget, "@Mark");

	return Dasher::instance().GoToPoint(mAgent, beh.mTarget, beh.mBuffer, beh.mPower, true, false);
}

BehaviorBlockPlanner::BehaviorBlockPlanner(Agent & agent):
	BehaviorPlannerBase<BehaviorDefenseData>( agent)
{
}

BehaviorBlockPlanner::~BehaviorBlockPlanner()
{
}

void BehaviorBlockPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	ActiveBehavior block(mAgent, BT_Block);

	block.mBuffer = 0.5;
	block.mPower = mSelfState.CorrectDashPowerForStamina(ServerParam::instance().maxDashPower());
	block.mTarget = mAnalyser.mLightHouse;
	block.mEvaluation = Evaluation::instance().EvaluatePosition(block.mTarget, false);

	behavior_list.push_back(block);
}

