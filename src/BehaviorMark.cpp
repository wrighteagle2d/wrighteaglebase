#include "BehaviorMark.h"
#include "VisualSystem.h"
#include "Formation.h"
#include "Dasher.h"
#include "BasicCommand.h"
#include "BehaviorPosition.h"
#include "Agent.h"
#include "PositionInfo.h"
#include "Logger.h"
#include "Evaluation.h"

const BehaviorType BehaviorMarkExecuter::BEHAVIOR_TYPE = BT_Mark;

namespace
{
bool ret = BehaviorExecutable::AutoRegister<BehaviorMarkExecuter>();
}

BehaviorMarkExecuter::BehaviorMarkExecuter(Agent & agent) :
	BehaviorExecuterBase<BehaviorDefenseData>(agent)
{
	Assert(ret);
}

BehaviorMarkExecuter::~BehaviorMarkExecuter(void)
{
}

bool BehaviorMarkExecuter::Execute(const ActiveBehavior & beh)
{
	Logger::instance().LogGoToPoint(mSelfState.GetPos(), beh.mTarget, "@Mark");

	return Dasher::instance().GoToPoint(mAgent, beh.mTarget, beh.mBuffer, beh.mPower, false, false);
}

BehaviorMarkPlanner::BehaviorMarkPlanner(Agent & agent):
	BehaviorPlannerBase<BehaviorDefenseData>( agent)
{
}

BehaviorMarkPlanner::~BehaviorMarkPlanner()
{
}

void BehaviorMarkPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	Unum closest_opp = mPositionInfo.GetClosestOpponentToTeammate(mSelfState.GetUnum());
	Unum closest_tm = mPositionInfo.GetClosestTeammateToOpponent(closest_opp);

	if (closest_opp && closest_tm && closest_tm == mSelfState.GetUnum()) {
		ActiveBehavior mark(mAgent, BT_Mark);

		mark.mBuffer = mSelfState.GetKickableArea();
		mark.mPower = mSelfState.CorrectDashPowerForStamina(ServerParam::instance().maxDashPower());
		mark.mTarget = mWorldState.GetOpponent(closest_opp).GetPos();
		mark.mEvaluation = Evaluation::instance().EvaluatePosition(mark.mTarget, false);

		behavior_list.push_back(mark);
	}
}

