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

