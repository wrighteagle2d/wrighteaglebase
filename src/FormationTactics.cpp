/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2010                                                    *
 * Copyright (c) 1998-2010 WrightEagle 2D Soccer Simulation Team,                   *
 *                         Multi-Agent Systems Lab.,                                *
 *                         School of Computer Science and Technology,               *
 *                         University of Science and Technology of China            *
 * All rights reserved.                                                             *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without               *
 * modification, are permitted provided that the following conditions are met:      *
 *     * Redistributions of source code must retain the above copyright             *
 *       notice, this list of conditions and the following disclaimer.              *
 *     * Redistributions in binary form must reproduce the above copyright          *
 *       notice, this list of conditions and the following disclaimer in the        *
 *       documentation and/or other materials provided with the distribution.       *
 *     * Neither the name of the WrightEagle 2D Soccer Simulation Team nor the      *
 *       names of its contributors may be used to endorse or promote products       *
 *       derived from this software without specific prior written permission.      *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED    *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
 * DISCLAIMED. IN NO EVENT SHALL WrightEagle 2D Soccer Simulation Team BE LIABLE    *
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL       *
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR       *
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER       *
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,    *
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF *
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                *
 ************************************************************************************/

#include "FormationTactics.h"
#include "Types.h"
#include "Utilities.h"
#include "Formation.h"
#include <sstream>
#include <utility>
#include <functional>
#include <cstdlib>
using namespace std;

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

