/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2010                                                    *
 * Copyright (C) 1998-2010 WrightEagle 2D Soccer Simulation Team,                   *
 *                         Multi-Agent Systems Lab.,                                *
 *                         School of Computer Science and Technology,               *
 *                         University of Science and Technology of China, China.    *
 *                                                                                  *
 * This program is free software; you can redistribute it and/or                    *
 * modify it under the terms of the GNU General Public License      ribbl                *
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

#ifndef __BehaviorHold_H__
#define __BehaviorHold_H__


#include <vector>
#include "BehaviorBase.h"
#include "VisualSystem.h"


class BehaviorHoldExecuter : public BehaviorExecuterBase<BehaviorAttackData>
{
public:
    BehaviorHoldExecuter(Agent & agent);
    virtual ~BehaviorHoldExecuter(void);

    bool Execute(const ActiveBehavior &);

	void SubmitVisualRequest(const ActiveBehavior & hold, double plus = 0.0) {
		Assert(hold.mKeyOppGB.mUnum >= 0);
		Assert(hold.mKeyOppGT.mUnum >= 0);

		VisualSystem::instance().RaisePlayer(mAgent, -hold.mKeyOppGB.mUnum, 2.0 + plus);
		VisualSystem::instance().RaisePlayer(mAgent, -hold.mKeyOppGT.mUnum, 2.0 + plus);
	}

    static const BehaviorType BEHAVIOR_TYPE;
};

class BehaviorHoldPlanner: public BehaviorPlannerBase <BehaviorAttackData>{
public:
    BehaviorHoldPlanner(Agent & agent);
    virtual ~BehaviorHoldPlanner(void);

    void Plan(std::list<ActiveBehavior> & behavior_list);
};


#endif

