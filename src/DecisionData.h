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
