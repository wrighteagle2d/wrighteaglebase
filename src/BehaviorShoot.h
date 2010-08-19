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

#ifndef BehaviorShoot_H_
#define BehaviorShoot_H_

#include "BehaviorBase.h"
#include "Types.h"
#include "Agent.h"
#include "PlayerState.h"
#include "Dasher.h"
#include "VisualSystem.h"
#include "Logger.h"

class BehaviorShootExecuter: public BehaviorExecuterBase<BehaviorAttackData> {
public:
	BehaviorShootExecuter(Agent & agent);
	virtual ~BehaviorShootExecuter();

	bool Execute(const ActiveBehavior &);

	void SubmitVisualRequest(const ActiveBehavior & shoot, double plus = 0.0) {
		Assert(shoot.mKeyOppGT.mUnum >= 0);
		Assert(shoot.mKeyOppGB.mUnum >= 0);

		VisualSystem::instance().RaisePlayer(mAgent, -mWorldState.GetOpponentGoalieUnum(), 1.0 + plus);
		VisualSystem::instance().RaisePlayer(mAgent, -shoot.mKeyOppGT.mUnum, 2.0 + plus);
		VisualSystem::instance().RaisePlayer(mAgent, -shoot.mKeyOppGB.mUnum, 2.0 + plus);
	}

	static const BehaviorType BEHAVIOR_TYPE;
};

class BehaviorShootPlanner: public BehaviorPlannerBase<BehaviorAttackData>
{
public:
	BehaviorShootPlanner(Agent & agent);
	virtual ~BehaviorShootPlanner();

	void Plan(std::list<ActiveBehavior> & behavior_list);
};

#endif /* BehaviorShoot_H_ */
