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

#include "FormationTactics.h"
#include "Types.h"
#include "Utilities.h"
#include "Formation.h"
#include <sstream>
#include <utility>
#include <functional>
#include <boost/regex.hpp>
#include <cstdlib>
using namespace std;
using namespace boost;

FormationTacticBase::FormationTacticBase():
	Index2Unum(IndexIsUnum),
	Unum2Index(IndexIsUnum)
{
}

int FormationTacticBase::IndexIsUnum[TEAMSIZE+1] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

void FormationTacticBase::Initial(vector<string> & config, Unum * index2unum, int * unum2index)
{
	(void) config;

	Index2Unum = index2unum ? index2unum : IndexIsUnum;
	Unum2Index = unum2index ? unum2index : IndexIsUnum;
}

FormationTacticKickOffPosition::FormationTacticKickOffPosition()
{
	for ( int i(0); i < 2; ++i )
	{
		for ( int j(0); j < TEAMSIZE; ++j )
		{
			mKickOffPosition[j][i] = Vector(0,0);
		}
	}
}

void FormationTacticKickOffPosition::Initial(vector<string> & config, Unum * index2unum, int * unum2index)
{
	FormationTacticBase::Initial( config, index2unum, unum2index );

	int i(0);
	for ( vector<string>::iterator config_iter(config.begin()); config_iter != config.end() && i<TEAMSIZE; ++config_iter, ++i )
	{
		stringstream sin(*config_iter);
		double p(0);
		sin >> p;
		mKickOffPosition[i][0].SetX(p);
		sin >> p;
		mKickOffPosition[i][0].SetY(p);
		sin >> p;
		mKickOffPosition[i][1].SetX(p);
		sin >> p;
		mKickOffPosition[i][1].SetY(p);
	}
}

/**
 * Get players' kick off position.
 * @param player unum of player to get position.
 * @param is_our_kickoff true if is our kick off.
 * @return the kick off position of the player.
 */
Vector & FormationTacticKickOffPosition::operator()( Unum player, bool is_our_kickoff )
{
	return mKickOffPosition[Unum2Index[player] - 1][is_our_kickoff? 0: 1];
}

