#include "BehaviorSetplay.h"
#include "Formation.h"
#include "Kicker.h"
#include "Dasher.h"
#include "VisualSystem.h"
#include "BehaviorPass.h"
#include "Geometry.h"
#include "ServerParam.h"
#include "BehaviorShoot.h"
#include "BehaviorIntercept.h"
#include "Evaluation.h"
#include <stdio.h>
#include <algorithm>
using namespace std;

const BehaviorType BehaviorSetplayExecuter::BEHAVIOR_TYPE = BT_Setplay;

namespace
{
bool ret = BehaviorExecutable::AutoRegister<BehaviorSetplayExecuter>();
}

BehaviorSetplayExecuter::BehaviorSetplayExecuter(Agent & agent): BehaviorExecuterBase<BehaviorAttackData>( agent )
{
	Assert(ret);
}

BehaviorSetplayExecuter::~BehaviorSetplayExecuter(void)
{
}

bool BehaviorSetplayExecuter::Execute(const ActiveBehavior &setplay)
{
	if (mWorldState.GetPlayMode() == PM_Before_Kick_Off) {
		if (setplay.mDetailType == BDT_Setplay_Move) {
			return mAgent.Move(setplay.mTarget);
		}
		else if (setplay.mDetailType == BDT_Setplay_Scan) {
			VisualSystem::instance().ForbidDecision(mAgent);
			return mAgent.Turn(50.0);
		}
		else {
			PRINT_ERROR("Setplay Detail Type Error");
			return false;
		}
	}
	else {
		if (setplay.mDetailType == BDT_Setplay_GetBall) {
			return Dasher::instance().GetBall(mAgent);
		}
		else {
			PRINT_ERROR("Setplay Detail Type Error");
			return false;
		}
	}
}


BehaviorSetplayPlanner::BehaviorSetplayPlanner(Agent & agent): BehaviorPlannerBase<BehaviorAttackData>( agent )
{
}

BehaviorSetplayPlanner::~BehaviorSetplayPlanner(void)
{
}

void BehaviorSetplayPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	ActiveBehavior setplay(mAgent, BT_Setplay);

	setplay.mBuffer = 0.5;

	if (mWorldState.GetPlayMode() != PM_Play_On) {
		if (mWorldState.GetPlayMode() == PM_Before_Kick_Off) {
			setplay.mDetailType = BDT_Setplay_Move;
			setplay.mTarget = TeammateFormationTactic(KickOffPosition)(mSelfState.GetUnum(), mWorldState.GetKickOffMode() == KO_Ours);

			if (setplay.mTarget.Dist(mSelfState.GetPos()) < setplay.mBuffer) {
				setplay.mDetailType = BDT_Setplay_Scan;
			}

			setplay.mEvaluation = Evaluation::instance().EvaluatePosition(setplay.mTarget, true);

			behavior_list.push_back(setplay);
		}
		else if (mWorldState.GetPlayMode() < PM_Our_Mode) {
			if (mPositionInfo.GetClosestTeammateToBall() == mSelfState.GetUnum()) {
				if (!mSelfState.IsKickable()) {
					setplay.mDetailType = BDT_Setplay_GetBall;
					setplay.mTarget = mBallState.GetPos();
					setplay.mEvaluation = Evaluation::instance().EvaluatePosition(setplay.mTarget, true);

					behavior_list.push_back(setplay);
				}
			}
		}
	}
}
