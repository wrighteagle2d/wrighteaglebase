#ifndef __BehaviorGoaliePlanner_H__
#define __BehaviorGoaliePlanner_H__


#include "BehaviorBase.h"
#include "Tackler.h"
#include "Dasher.h"
#include "Kicker.h"
#include "VisualSystem.h"
#include "InterceptModel.h"
#include "InterceptInfo.h"
#include "BehaviorPass.h"
#include "Formation.h"
#include "ActionEffector.h"

class BehaviorGoalieExecuter : public BehaviorExecuterBase<BehaviorDefenseData>
{
public:
    BehaviorGoalieExecuter(Agent &agent);
    virtual ~BehaviorGoalieExecuter(void);

    bool Execute(const ActiveBehavior & act_bhv);

    static const BehaviorType BEHAVIOR_TYPE;
};

class BehaviorGoaliePlanner : public BehaviorPlannerBase<BehaviorDefenseData>
{
public:
    BehaviorGoaliePlanner(Agent& agent);
    ~BehaviorGoaliePlanner(void);

    void Plan(std::list<ActiveBehavior>& behavior_list);
};
#endif

