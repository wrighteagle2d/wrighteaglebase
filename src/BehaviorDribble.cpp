#include "BehaviorDribble.h"
#include "Agent.h"
#include "Kicker.h"
#include "WorldState.h"
#include "Strategy.h"
#include "Formation.h"
#include "Dasher.h"
#include "Types.h"
#include "Logger.h"
#include "VisualSystem.h"
#include "CommunicateSystem.h"
#include "Utilities.h"
#include <sstream>
#include <vector>
#include <utility>
#include "Evaluation.h"


using namespace std;

const BehaviorType BehaviorDribbleExecuter::BEHAVIOR_TYPE = BT_Dribble;

namespace {
bool ret = BehaviorExecutable::AutoRegister<BehaviorDribbleExecuter>();
}

BehaviorDribbleExecuter::BehaviorDribbleExecuter(Agent & agent) :
    BehaviorExecuterBase<BehaviorAttackData>( agent )
{
	Assert(ret);
}

BehaviorDribbleExecuter::~BehaviorDribbleExecuter(void)
{
}

bool BehaviorDribbleExecuter::Execute(const ActiveBehavior & dribble)
{
	Logger::instance().LogDribble(mBallState.GetPos(), dribble.mTarget, "@Dribble", true);
	return Kicker::instance().KickBall(mAgent, dribble.mAngle, dribble.mKickSpeed, KM_Quick);
}

BehaviorDribblePlanner::BehaviorDribblePlanner(Agent & agent) :
    BehaviorPlannerBase <BehaviorAttackData>(agent)
{
}


BehaviorDribblePlanner::~BehaviorDribblePlanner(void)
{
}


void BehaviorDribblePlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	if (!mSelfState.IsKickable()) return;
	if (mWorldState.GetPlayMode() != PM_Play_On) return;

	double speed = mSelfState.GetEffectiveSpeedMax();

	for (AngleDeg dir = -90.0; dir < 90.0; dir += 2.5) {
		ActiveBehavior dribble(mAgent, BT_Dribble, BDT_Dribble_Normal);

		dribble.mAngle = dir;
		dribble.mKickSpeed = speed;

		dribble.mEvaluation = 0.0;
		for (int i = 1; i <= 10; ++i) {
			dribble.mEvaluation += Evaluation::instance().EvaluatePosition(mBallState.GetPos() + Polar2Vector(dribble.mKickSpeed * i, dribble.mAngle), true);
		}
		dribble.mEvaluation /= 10;

		mActiveBehaviorList.push_back(dribble);
	}

	if (!mActiveBehaviorList.empty()) {
		mActiveBehaviorList.sort(std::greater<ActiveBehavior>());
		behavior_list.push_back(mActiveBehaviorList.front());
	}
}
