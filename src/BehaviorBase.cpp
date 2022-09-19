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

#include "BehaviorBase.h"
#include "Agent.h"
#include "Analyser.h"
#include "InfoState.h"
#include "Logger.h"
#include "Strategy.h"
#include "WorldState.h"

BehaviorAttackData::BehaviorAttackData(Agent &agent)
    : mAgent(agent), mWorldState(agent.GetWorldState()),
      mBallState(agent.GetWorldState().GetBall()), mSelfState(agent.Self()),
      mPositionInfo(agent.Info().GetPositionInfo()),
      mInterceptInfo(agent.Info().GetInterceptInfo()),
      mStrategy(agent.GetStrategy()), mFormation(agent.GetFormation()) {
  mFormation.Update(Formation::Offensive, "Offensive");
}

BehaviorAttackData::~BehaviorAttackData() { mFormation.Rollback("Offensive"); }

BehaviorDefenseData::BehaviorDefenseData(Agent &agent)
    : BehaviorAttackData(agent), mAnalyser(agent.GetAnalyser()) {
  mFormation.Update(Formation::Defensive, "Defensive");
}

BehaviorDefenseData::~BehaviorDefenseData() {
  mFormation.Rollback("Defensive");
}

bool ActiveBehavior::Execute() {
  BehaviorExecutable *behavior =
      BehaviorFactory::instance().CreateBehavior(GetAgent(), GetType());

  if (behavior) {
    Logger::instance().GetTextLogger("executing")
        << GetAgent().GetWorldState().CurrentTime() << " "
        << BehaviorFactory::instance().GetBehaviorName(GetType())
        << " executing" << std::endl;

    behavior->SubmitVisualRequest(*this);
    bool ret = behavior->Execute(*this);

    delete behavior;
    return ret;
  } else {
    return false;
  }
}

void ActiveBehavior::SubmitVisualRequest(double plus) {
  BehaviorExecutable *behavior =
      BehaviorFactory::instance().CreateBehavior(GetAgent(), GetType());

  if (behavior) {
    Logger::instance().GetTextLogger("executing")
        << GetAgent().GetWorldState().CurrentTime() << " "
        << BehaviorFactory::instance().GetBehaviorName(GetType())
        << " visual plus: " << plus << std::endl;

    behavior->SubmitVisualRequest(*this, plus);

    delete behavior;
  }
}

BehaviorFactory::BehaviorFactory() {}

BehaviorFactory::~BehaviorFactory() {}

BehaviorFactory &BehaviorFactory::instance() {
  static BehaviorFactory factory;

  return factory;
}

BehaviorExecutable *BehaviorFactory::CreateBehavior(Agent &agent,
                                                    BehaviorType type) {
  if (type == BT_None) {
    return 0;
  }

  BehaviorCreator creator = mCreatorMap[type];

  if (creator) {
    return creator(agent);
  }

  return 0;
}

bool BehaviorFactory::RegisterBehavior(BehaviorType type,
                                       BehaviorCreator creator,
                                       const char *behavior_name) {
  if (type > BT_None && type < BT_Max) {
    if (mCreatorMap[type] == 0) {
      mCreatorMap[type] = creator;
      mNameMap[type] = behavior_name;
      return true;
    }
  }

  return false;
}
