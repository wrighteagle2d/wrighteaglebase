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

