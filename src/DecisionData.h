#ifndef DECISIONDATA_H_
#define DECISIONDATA_H_

#include "Utilities.h"

class Agent;
class WorldState;
class BallState;
class PlayerState;
class InfoState;
class Formation;

class DecisionData: public Updatable {
	DecisionData(const DecisionData &);
	const DecisionData & operator=(const DecisionData &);

public:
	DecisionData(Agent & agent);
	virtual ~DecisionData();

	void Update();

protected:
	Agent & mAgent;

	const WorldState  & mWorldState;
	const BallState   & mBallState;
	const PlayerState & mSelfState;

	InfoState   & mInfoState;
	Formation   & mFormation;
};

#endif /* DECISIONDATA_H_ */
