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

