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

