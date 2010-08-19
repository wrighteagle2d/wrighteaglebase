#include "DecisionData.h"
#include "Agent.h"

DecisionData::DecisionData(Agent & agent):
	mAgent ( agent ),
	mWorldState ( agent.World() ),
	mBallState ( agent.World().Ball() ),
	mSelfState ( agent.Self() ),
	mInfoState ( agent.Info()),
	mFormation ( agent.GetFormation())
{
}

DecisionData::~DecisionData() {
}

void DecisionData::Update()
{
	UpdateAtTime(mAgent.GetWorldState().CurrentTime());
}
