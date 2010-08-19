#ifndef ANALYSER_H_
#define ANALYSER_H_

#include <vector>
#include "Utilities.h"
#include "Geometry.h"
#include "ServerParam.h"
#include "DecisionData.h"
#include "InterceptInfo.h"

class Agent;

/**
 * 主要是防守需要使用的一些数据
 * */
class Analyser: public DecisionData {
public:
	Analyser(Agent & agent);
	virtual ~Analyser();

	void UpdateRoutine();

public:
	/**
	 * broadcast the position of ball & opponents
	 */
	void BroadcastPosition();

	Vector mLightHouse;
	PlayerArray<Vector, true> mHome;
};

#endif /* ANALYSER_H_ */
