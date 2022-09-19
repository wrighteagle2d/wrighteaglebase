/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D) * BASE SOURCE CODE RELEASE 2016 *
 * Copyright (c) 1998-2016 WrightEagle 2D Soccer Simulation Team, * Multi-Agent
 *Systems Lab.,                                * School of Computer Science and
 *Technology,               * University of Science and Technology of China *
 * All rights reserved. *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without *
 * modification, are permitted provided that the following conditions are met: *
 *     * Redistributions of source code must retain the above copyright *
 *       notice, this list of conditions and the following disclaimer. *
 *     * Redistributions in binary form must reproduce the above copyright *
 *       notice, this list of conditions and the following disclaimer in the *
 *       documentation and/or other materials provided with the distribution. *
 *     * Neither the name of the WrightEagle 2D Soccer Simulation Team nor the *
 *       names of its contributors may be used to endorse or promote products *
 *       derived from this software without specific prior written permission. *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED    * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *PURPOSE ARE           * DISCLAIMED. IN NO EVENT SHALL WrightEagle 2D Soccer
 *Simulation Team BE LIABLE    * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *EXEMPLARY, OR CONSEQUENTIAL       * DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *PROCUREMENT OF SUBSTITUTE GOODS OR       * SERVICES; LOSS OF USE, DATA, OR
 *PROFITS; OR BUSINESS INTERRUPTION) HOWEVER       * CAUSED AND ON ANY THEORY OF
 *LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,    * OR TORT (INCLUDING
 *NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF * THIS SOFTWARE,
 *EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                *
 ************************************************************************************/

#include "BehaviorPosition.h"
#include "Agent.h"
#include "BehaviorPass.h"
#include "Dasher.h"
#include "Evaluation.h"
#include "InterceptInfo.h"
#include "Logger.h"
#include "Strategy.h"
#include "TimeTest.h"
#include "VisualSystem.h"
#include <sstream>

using namespace std;

const BehaviorType BehaviorPositionExecuter::BEHAVIOR_TYPE = BT_Position;

namespace {
bool ret = BehaviorExecutable::AutoRegister<BehaviorPositionExecuter>();
}

BehaviorPositionExecuter::BehaviorPositionExecuter(Agent &agent)
    : BehaviorExecuterBase<BehaviorAttackData>(agent) {
  Assert(ret);
}

BehaviorPositionExecuter::~BehaviorPositionExecuter(void) {}

bool BehaviorPositionExecuter::Execute(const ActiveBehavior &beh) {
  Logger::instance().LogGoToPoint(mSelfState.GetPos(), beh.mTarget,
                                  "@Position");

  return Dasher::instance().GoToPoint(mAgent, beh.mTarget, beh.mBuffer,
                                      beh.mPower, false, true);
}

BehaviorPositionPlanner::~BehaviorPositionPlanner() {}

BehaviorPositionPlanner::BehaviorPositionPlanner(Agent &agent)
    : BehaviorPlannerBase<BehaviorAttackData>(agent) {}

void BehaviorPositionPlanner::Plan(ActiveBehaviorList &behavior_list) {
  if (!behavior_list.empty())
    return;

  if (mSelfState.IsGoalie())
    return;
  if (mStrategy.IsOppControl())
    return;
  if (mWorldState.GetPlayMode() > PM_Opp_Mode)
    return;

  ActiveBehavior position(mAgent, BT_Position);
  Vector target;
  position.mBuffer = 1.0;
  target = mStrategy.GetTeammateSBSPPosition(mSelfState.GetUnum(),
                                             mBallState.GetPos());
  if (mPositionInfo.GetTeammateWithBall() &&
      mPositionInfo.GetClosestPlayerToTeammate(
          mPositionInfo.GetTeammateWithBall()) == mSelfState.GetUnum()) {
    if ((mWorldState
             .GetOpponent(mPositionInfo.GetClosestOpponentToTeammate(
                 mSelfState.GetUnum()))
             .GetPos() -
         mSelfState.GetPos())
            .Mod() >= 1.0)
      position.mTarget = target;
    else {
      position.mTarget.SetX(target.X() + 1.0);
      position.mTarget.SetY(target.Y());
    }

  } else
    position.mTarget = target;

  if (mWorldState.GetPlayMode() == PM_Our_Goalie_Free_Kick &&
      mAgent.GetFormation().GetMyRole().mLineType == LT_Forward) {
    if (mAgent.GetFormation().GetMyRole().mPositionType == PT_Left) {
      position.mTarget.SetY(26);
    }
    if (mAgent.GetFormation().GetMyRole().mPositionType == PT_Right) {
      position.mTarget.SetY(-26);
    }
  }

  Unum opp = mPositionInfo.GetClosestOpponentToPoint(position.mTarget);
  if (mWorldState.GetOpponent(opp).GetPos().Dist(position.mTarget) < 1.5) {
    int p =
        mWorldState.GetOpponent(mPositionInfo.GetClosestOpponentToPlayer(opp))
                    .GetPos()
                    .Y() > mWorldState.GetOpponent(opp).GetPos().X()
            ? -1
            : 1;
    position.mTarget.SetY(target.Y() + p * 2);
  }

  if (mPositionInfo.GetOpponentOffsideLine() > mBallState.GetPos().X()) {
    position.mTarget.SetX(
        Min(target.X(), (mPositionInfo.GetOpponentOffsideLine())));
  }
  if (position.mTarget.X() > mSelfState.GetPos().X() &&
      mAgent.GetFormation().GetMyRole().mLineType == LT_Forward) {
    position.mPower = mSelfState.CorrectDashPowerForStamina(
        ServerParam::instance().maxDashPower());
  } else if (position.mTarget.X() < mSelfState.GetPos().X() &&
             mAgent.GetFormation().GetMyRole().mLineType == LT_Defender) {
    position.mPower = mSelfState.CorrectDashPowerForStamina(
        ServerParam::instance().maxDashPower());
  } else
    position.mPower = mSelfState.CorrectDashPowerForStamina(
                          ServerParam::instance().maxDashPower()) /
                      2;
  position.mEvaluation =
      Evaluation::instance().EvaluatePosition(position.mTarget, false);

  behavior_list.push_back(position);
}
