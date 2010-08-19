#ifndef BEHAVIORDEFENSE_H_
#define BEHAVIORDEFENSE_H_

#include "BehaviorBase.h"
#include "Analyser.h"
#include <vector>

class BehaviorDefensePlanner : public BehaviorPlannerBase <BehaviorDefenseData> {
public:
	BehaviorDefensePlanner(Agent & agent);
	virtual ~BehaviorDefensePlanner();

	void Plan(std::list<ActiveBehavior> & behavior_list);
};

#endif /* BEHAVIORDEFENSE_H_ */
