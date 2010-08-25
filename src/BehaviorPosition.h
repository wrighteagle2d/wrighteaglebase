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

#ifndef __BehaviorPosition_H__
#define __BehaviorPosition_H__


#include "BehaviorBase.h"
#include "Formation.h"
#include "WorldState.h"
#include "BehaviorPass.h"
#include "PositionInfo.h"
#include "VisualSystem.h"


class BehaviorPositionExecuter: public BehaviorExecuterBase<BehaviorAttackData>
{
public:
	BehaviorPositionExecuter(Agent &agent);
	virtual ~BehaviorPositionExecuter(void);

	bool Execute(const ActiveBehavior &);

	void SubmitVisualRequest(const ActiveBehavior & position, double plus = 0.0) {
		(void) position;
		(void) plus;

		if (fabs(mSelfState.GetPos().X() - mPositionInfo.GetTeammateOffsideLine()) < 30.0) {
			VisualSystem::instance().RaisePlayer(mAgent, -mPositionInfo.GetTeammateOffsideLineOpp(), 3.0);
		}
	}

	static const BehaviorType BEHAVIOR_TYPE;
};


class BehaviorPositionPlanner: public BehaviorPlannerBase<BehaviorAttackData>
{
public:
	BehaviorPositionPlanner(Agent &agent);
	virtual ~BehaviorPositionPlanner();
	void Plan(ActiveBehaviorList & behavior_list);
};


#endif

