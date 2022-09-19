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

#include "BehaviorSetplay.h"
#include "BehaviorIntercept.h"
#include "BehaviorPass.h"
#include "BehaviorShoot.h"
#include "Dasher.h"
#include "Evaluation.h"
#include "Formation.h"
#include "Geometry.h"
#include "Kicker.h"
#include "ServerParam.h"
#include "Utilities.h"
#include "VisualSystem.h"
#include <algorithm>
#include <stdio.h>
using namespace std;

const BehaviorType BehaviorSetplayExecuter::BEHAVIOR_TYPE = BT_Setplay;

namespace {
bool ret = BehaviorExecutable::AutoRegister<BehaviorSetplayExecuter>();
}

BehaviorSetplayExecuter::BehaviorSetplayExecuter(Agent &agent)
    : BehaviorExecuterBase<BehaviorAttackData>(agent) {
  Assert(ret);
}

BehaviorSetplayExecuter::~BehaviorSetplayExecuter(void) {}

bool BehaviorSetplayExecuter::Execute(const ActiveBehavior &setplay) {
  if (mWorldState.GetPlayMode() == PM_Before_Kick_Off) {
    if (setplay.mDetailType == BDT_Setplay_Move) {
      return mAgent.Move(setplay.mTarget);
    } else if (setplay.mDetailType == BDT_Setplay_Scan) {
      VisualSystem::instance().ForbidDecision(mAgent);
      return mAgent.Turn(50.0);
    } else {
      PRINT_ERROR("Setplay Detail Type Error");
      return false;
    }
  } else {
    if (setplay.mDetailType == BDT_Setplay_Scan) {
      return true; //交给视觉决策
    } else if (setplay.mDetailType == BDT_Setplay_GetBall) {
      return Dasher::instance().GetBall(mAgent);
    }
    if (setplay.mDetailType == BDT_Setplay_Move) {
      return mAgent.Move(setplay.mTarget);
    } else {
      PRINT_ERROR("Setplay Detail Type Error");
      return false;
    }
  }
}

BehaviorSetplayPlanner::BehaviorSetplayPlanner(Agent &agent)
    : BehaviorPlannerBase<BehaviorAttackData>(agent) {}

BehaviorSetplayPlanner::~BehaviorSetplayPlanner(void) {}

void BehaviorSetplayPlanner::Plan(std::list<ActiveBehavior> &behavior_list) {
  ActiveBehavior setplay(mAgent, BT_Setplay);

  setplay.mBuffer = 0.5;

  if (mWorldState.GetPlayMode() != PM_Play_On) {
    if (mWorldState.GetPlayMode() == PM_Before_Kick_Off) {
      setplay.mDetailType = BDT_Setplay_Move;
      setplay.mTarget = TeammateFormationTactic(KickOffPosition)(
          mSelfState.GetUnum(), mWorldState.GetKickOffMode() == KO_Ours);

      if (setplay.mTarget.Dist(mSelfState.GetPos()) < setplay.mBuffer) {
        setplay.mDetailType = BDT_Setplay_Scan;
      }

      setplay.mEvaluation =
          Evaluation::instance().EvaluatePosition(setplay.mTarget, true);

      behavior_list.push_back(setplay);
    } else if (mWorldState.GetPlayMode() < PM_Our_Mode) {
      if (mPositionInfo.GetClosestTeammateToBall() == mSelfState.GetUnum()) {
        if (!mSelfState.IsKickable()) {
          setplay.mDetailType = BDT_Setplay_GetBall;
          setplay.mTarget = mBallState.GetPos();
          setplay.mEvaluation =
              Evaluation::instance().EvaluatePosition(setplay.mTarget, true);

          behavior_list.push_back(setplay);
        } else {
          mStrategy.SetForbidenDribble(true); //禁止带球

          if (mWorldState.GetLastPlayMode() != PM_Before_Kick_Off) {
            if (mWorldState.CurrentTime().T() -
                    mWorldState.GetPlayModeTime().T() <
                20) {
              setplay.mDetailType = BDT_Setplay_Scan;
              setplay.mEvaluation = Evaluation::instance().EvaluatePosition(
                  setplay.mTarget, true);

              behavior_list.push_back(setplay);
            } else if (mWorldState.CurrentTime().T() -
                               mWorldState.GetPlayModeTime().T() ==
                           20 &&
                       mSelfState.IsGoalie()) {
              setplay.mDetailType = BDT_Setplay_Move;
              for (list<KeyPlayerInfo>::const_iterator it =
                       mPositionInfo.GetXSortTeammate().begin();
                   it != mPositionInfo.GetXSortTeammate().end(); ++it) {
                if (mWorldState.GetTeammate((*it).mUnum).GetPos().X() >
                        ServerParam::instance().ourPenaltyArea().Right() &&
                    (mWorldState
                         .GetOpponent(
                             mPositionInfo.GetClosestOpponentToTeammate(
                                 (*it).mUnum))
                         .GetPos() -
                     mWorldState.GetTeammate((*it).mUnum).GetPos())
                            .Mod() >= 1.0) {
                  double y = MinMax(
                      ServerParam::instance().ourPenaltyArea().Top() + 1,
                      mWorldState.GetTeammate((*it).mUnum).GetPos().Y(),
                      ServerParam::instance().ourPenaltyArea().Bottom() - 1);
                  setplay.mTarget = Vector(
                      ServerParam::instance().ourPenaltyArea().Right() - 1, y);
                  setplay.mEvaluation = Evaluation::instance().EvaluatePosition(
                      setplay.mTarget, true);
                  behavior_list.push_back(setplay);
                  break;
                }
              }
            }
          }
        }
      }
    }
  }
}
