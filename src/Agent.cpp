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

#include <cstdlib>
#include "Agent.h"
#include "WorldModel.h"

/**
 * Constructor.
 */
Agent::Agent(Unum unum, WorldModel *world_model, bool reverse):
	mSelfUnum( abs(unum) ),
	mReverse( reverse ),
	mpWorldModel( world_model ),
	mpWorldState( &(world_model->World(reverse)) ),
	mpInfoState( new InfoState( mpWorldState)),
	mIsNewSight (false),
	mpStrategy(0),
	mpAnalyser(0),
    mpActionEffector(0),
    mpFormation(0)
{
}

/**
 * Destructor.
 */
Agent::~Agent()
{
	delete mpInfoState;
    delete mpFormation;
	delete mpActionEffector;
	delete mpStrategy;
	delete mpAnalyser;
}

/**
 * Interface to create an agent which represents a team mate.
 * \param unum positive number represents the uniform number of the team mate.
 * \return an agent created by "new" operator, which should be manually deleted when there will be no
 *         use any more.
 */
Agent * Agent::CreateTeammateAgent(Unum unum) ///反算队友
{
    Assert(unum != 0);
    return new Agent(unum, mpWorldModel, mReverse); //reverse属性不变
}

/**
 * Interface to create an agent which represents an opponent.
 * \param unum positive number represents the uniform number of the opponent.
 * \return an agent created by "new" operator, which should be manually deleted when there will be no
 *         use any more.
 */
Agent * Agent::CreateOpponentAgent(Unum unum) ///反算对手
{
	return new Agent(unum, mpWorldModel, !mReverse); //reverse属性相反
}

