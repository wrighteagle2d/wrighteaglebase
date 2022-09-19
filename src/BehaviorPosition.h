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

#ifndef __BehaviorPosition_H__
#define __BehaviorPosition_H__

#include "BehaviorBase.h"
#include "BehaviorPass.h"
#include "Formation.h"
#include "PositionInfo.h"
#include "VisualSystem.h"
#include "WorldState.h"

class BehaviorPositionExecuter
    : public BehaviorExecuterBase<BehaviorAttackData> {
public:
  BehaviorPositionExecuter(Agent &agent);
  virtual ~BehaviorPositionExecuter(void);

  bool Execute(const ActiveBehavior &);

  void SubmitVisualRequest(const ActiveBehavior &position, double plus = 0.0) {
    (void)position;
    (void)plus;

    if (fabs(mSelfState.GetPos().X() - mPositionInfo.GetTeammateOffsideLine()) <
        30.0) {
      VisualSystem::instance().RaisePlayer(
          mAgent, -mPositionInfo.GetTeammateOffsideLineOpp(), 3.0);
    }

    if (mSelfState.GetUnum() == mPositionInfo.GetCloseTeammateToBall()[0] ||
        mSelfState.GetUnum() == mPositionInfo.GetCloseTeammateToBall()[1]) {
      VisualSystem::instance().RaiseBall(mAgent, 2);
    }
  }

  static const BehaviorType BEHAVIOR_TYPE;
};

class BehaviorPositionPlanner : public BehaviorPlannerBase<BehaviorAttackData> {
public:
  BehaviorPositionPlanner(Agent &agent);
  virtual ~BehaviorPositionPlanner();
  void Plan(ActiveBehaviorList &behavior_list);
};

#endif
