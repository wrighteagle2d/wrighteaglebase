#include <vector>
#include "Analyser.h"
#include "Agent.h"
#include "InterceptInfo.h"
#include "PositionInfo.h"
#include "Formation.h"
#include "CommunicateSystem.h"
#include "Logger.h"
#include "VisualSystem.h"
#include "Dasher.h"
using namespace std;

Analyser::Analyser(Agent & agent):
	DecisionData (agent)
{

}

Analyser::~Analyser()
{
}

void Analyser::UpdateRoutine()
{
	mLightHouse = mAgent.GetStrategy().GetBallInterPos();

	for (Unum i = 1; i <= TEAMSIZE; ++i) {
		mHome[i] = mFormation.GetTeammateFormationPoint(i, mLightHouse);
	}
}

