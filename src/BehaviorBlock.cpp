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
	Logger::instance().LogGoToPoint(mSelfState.GetPos(), beh.mTarget, "@Block");

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
	Unum fastest_tm = mStrategy.GetFastestTm();

	if (fastest_tm == mSelfState.GetUnum()) {
		ActiveBehavior block(mAgent, BT_Block);

		block.mBuffer = 0.5;
		block.mPower = mSelfState.CorrectDashPowerForStamina(ServerParam::instance().maxDashPower());
		block.mTarget = mAnalyser.mLightHouse;
		block.mEvaluation = 1.0 + FLOAT_EPS;

		behavior_list.push_back(block);
	}
}

