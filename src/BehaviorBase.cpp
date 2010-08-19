/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2010                                                    *
 * Copyright (C) 1998-2010 WrightEagle 2D Soccer Simulation Team,                   *
 *                         Multi-Agent Systems Lab.,                                *
 *                         School of Computer Science and Technology,               *
 *                         University of Science and Technology of China, China.    *
 *                                                                                  *
 * This program is free software; you can redistribute it and/or                    *
 * modify it under the terms of the GNU General Public License                      *
 * as published by the Free Software Foundation; either version 2                   *
 * of the License, or (at your option) any later version.                           *
 *                                                                                  *
 * This program is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 * GNU General Public License for more details.                                     *
 *                                                                                  *
 * You should have received a copy of the GNU General Public License                *
 * along with this program; if not, write to the Free Software                      *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.  *
 ************************************************************************************/

#include "BehaviorBase.h"
#include "Agent.h"
#include "WorldState.h"
#include "InfoState.h"
#include "Strategy.h"
#include "Analyser.h"
#include "Logger.h"

BehaviorAttackData::BehaviorAttackData(Agent & agent):
	mAgent ( agent ),
    mWorldState ( agent.GetWorldState() ),
    mBallState ( agent.GetWorldState().GetBall() ),
	mSelfState ( agent.Self() ),
	mPositionInfo ( agent.Info().GetPositionInfo()),
	mInterceptInfo ( agent.Info().GetInterceptInfo()),
	mStrategy (agent.GetStrategy()),
    mFormation ( agent.GetFormation() )
{
	mFormation.Update(Formation::Offensive, "Offensive");
}

BehaviorAttackData::~BehaviorAttackData()
{
	mFormation.Rollback("Offensive");
}

BehaviorDefenseData::BehaviorDefenseData(Agent & agent):
	BehaviorAttackData (agent),
	mAnalyser (agent.GetAnalyser())
{
	mFormation.Update(Formation::Defensive, "Defensive");
}

BehaviorDefenseData::~BehaviorDefenseData()
{
	mFormation.Rollback("Defensive");
}

bool ActiveBehavior::Execute()
{
	BehaviorExecutable * behavior = BehaviorFactory::instance().CreateBehavior(GetAgent(), GetType());

	if (behavior){
		Logger::instance().GetTextLogger("executing") << GetAgent().GetWorldState().CurrentTime() << " " << BehaviorFactory::instance().GetBehaviorName(GetType()) << " executing" << std::endl;

		behavior->SubmitVisualRequest(*this);
		bool ret = behavior->Execute(*this);

		delete behavior;
		return ret;
	}
	else {
		return false;
	}
}

void ActiveBehavior::SubmitVisualRequest(double plus)
{
	BehaviorExecutable * behavior = BehaviorFactory::instance().CreateBehavior(GetAgent(), GetType());

	if (behavior){
		Logger::instance().GetTextLogger("executing") << GetAgent().GetWorldState().CurrentTime() << " " << BehaviorFactory::instance().GetBehaviorName(GetType()) << " visual plus: " << plus << std::endl;

		behavior->SubmitVisualRequest(*this, plus);

		delete behavior;
	}
}

BehaviorFactory::BehaviorFactory()
{
}

BehaviorFactory::~BehaviorFactory()
{
}

BehaviorFactory & BehaviorFactory::instance()
{
	static BehaviorFactory factory;

	return factory;
}

BehaviorExecutable * BehaviorFactory::CreateBehavior(Agent & agent, BehaviorType type)
{
	if (type == BT_None) {
		return 0;
	}

	BehaviorCreator creator = mCreatorMap[type];

	if (creator){
		return creator( agent );
	}

	return 0;
}

bool BehaviorFactory::RegisterBehavior(BehaviorType type, BehaviorCreator creator, const char *behavior_name)
{
	if (type > BT_None && type < BT_Max) {
		if (mCreatorMap[type] == 0){
			mCreatorMap[type] = creator;
			mNameMap[type] = behavior_name;
			return true;
		}
	}

	return false;
}

