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

#include "PositionInfo.h"
#include "WorldState.h"
#include "Utilities.h"
#include <algorithm>
#include <cmath>
using namespace std;

PositionInfo::PositionInfo(WorldState *pWorldState, InfoState *pInfoState):
	InfoStateBase(pWorldState, pInfoState),
	mPlayerWithBallList_UpdateTime(Time(-3, 0))
{
}

void PositionInfo::UpdateRoutine()
{
	UpdateDistMatrix();
	UpdateOffsideLine();
    UpdateOppGoalInfo();

	/** 每个周期clear一次 */
	mPlayer2BallList.clear();
	mTeammate2BallList.clear();
	mOpponent2BallList.clear();

	for (int i = 0; i < 1 + 2 * TEAMSIZE; ++i) {
		mPlayer2PlayerList[i].clear();
		mTeammate2PlayerList[i].clear();
		mOpponent2PlayerList[i].clear();
	}

	mXSortTeammateList.clear();
	mXSortOpponentList.clear();
}

void PositionInfo::UpdateDistMatrix()
{
	for (int i = 0; i < 1 + 2 * TEAMSIZE; ++i){
		for (int j = 0; j < 1 + 2 * TEAMSIZE; ++j){
			mDistMatrix[i][j] = -1.0;
		}
	}

    for (int i = 0; i < TEAMSIZE; ++i)
    {
        mTeammateDir2Ball[i] = 0.0;
        mOpponentDir2Ball[i] = 0.0;
    }

	const vector<PlayerState*> & player_list = mpWorldState->GetPlayerList();

	for (vector<PlayerState*>::const_iterator it = player_list.begin(); it != player_list.end(); ++it){
		if ((*it)->IsAlive()){
			int index = Unum2Index((*it)->GetUnum());
            Vector rel_pos = (*it)->GetPos() - mpWorldState->GetBall().GetPos();
            mDistMatrix[0][index] = rel_pos.Mod();
			mDistMatrix[index][index] = 0.0;

            // 更新球员相对于球的角度
            if ((*it)->GetUnum() > 0)
            {
                mTeammateDir2Ball[(*it)->GetUnum() - 1] = rel_pos.Dir();
            }
            else
            {
                mOpponentDir2Ball[-(*it)->GetUnum() - 1] = rel_pos.Dir();
            }
		}
	}

	const uint player_list_size = player_list.size();
	for (unsigned int i = 0; i < player_list_size; ++i){
		const PlayerState *pi = player_list[i];
		if (pi->IsAlive()){
			for (unsigned int j = i + 1; j < player_list_size; ++j){
				const PlayerState *pj = player_list[j];
				if (pj->IsAlive()){
					int index1 = Unum2Index(pi->GetUnum());
					int index2 = Unum2Index(pj->GetUnum());
					mDistMatrix[index1][index2] = mDistMatrix[index2][index1] = pi->GetPos().Dist(pj->GetPos());
				}
			}
		}
	}
}

const double & PositionInfo::GetBallDistToPlayer(Unum unum) const
{
	return mDistMatrix[0][Unum2Index(unum)];
}

const double & PositionInfo::GetPlayerDistToPlayer(Unum unum1, Unum unum2) const
{
	return mDistMatrix[Unum2Index(unum1)][Unum2Index(unum2)];
}

void PositionInfo::UpdateOffsideLine()
{
	const BallState &ball_state = mpWorldState->GetBall();
	const PlayerState *pPlayer  = 0;

	double first    = 0.0;
	double second   = 0.0;
	Unum f_unum     = 0;
	Unum s_unum     = 0;

	// teammate offside line
	for (int i = 1; i <= TEAMSIZE; ++i)
	{
		pPlayer = &(mpWorldState->GetOpponent(i));
		if (pPlayer->IsAlive() == false)
		{
			continue;
		}

		if (pPlayer->GetPos().X() > second)
		{
			if (pPlayer->GetPos().X() > first)
			{
				second  = first;
				s_unum  = f_unum;
				first   = pPlayer->GetPos().X();
				f_unum  = i;
			}
			else
			{
				second  = pPlayer->GetPos().X();
				s_unum  = i;
			}
		}
	}

	if (ball_state.GetPosConf() > PlayerParam::instance().minValidConf() &&
			ball_state.GetPos().X() > second)
	{
		mTeammateOffsideLineOpp    = 0;
		mTeammateOffsideLine        = ball_state.GetPos().X();
		mTeammateOffsideLineConf    = ball_state.GetPosConf();
		mTeammateOffsideLineSpeed   = ball_state.GetVel().X();
	}
	else
	{
		mTeammateOffsideLine        = second;
		if (s_unum == 0)
		{
			mTeammateOffsideLineOpp    = 0;
			mTeammateOffsideLineConf    = 0.0;
			mTeammateOffsideLineSpeed   = 0.0;
		}
		else
		{
			mTeammateOffsideLineOpp    = s_unum;
			mTeammateOffsideLineConf    = mpWorldState->GetOpponent(s_unum).GetPosConf();
			mTeammateOffsideLineSpeed   = mpWorldState->GetOpponent(s_unum).GetVel().X();
		}
	}

	// opponent offside line
	first   = 0.0;
	second  = 0.0;
	f_unum  = 0;
	s_unum  = 0;
	for (int i = 1; i <= TEAMSIZE; ++i)
	{
		pPlayer = &(mpWorldState->GetTeammate(i));
		if (pPlayer->IsAlive() == false)
		{
			continue;
		}

		if (pPlayer->GetPos().X() < second)
		{
			if (pPlayer->GetPos().X() < first)
			{
				second  = first;
				s_unum  = f_unum;
				first   = pPlayer->GetPos().X();
				f_unum  = i;
			}
			else
			{
				second  = pPlayer->GetPos().X();
				s_unum  = i;
			}
		}
	}
	if (ball_state.GetPosConf() > PlayerParam::instance().minValidConf() &&
			ball_state.GetPos().X() < second)
	{
		mOpponentOffsideLineTm    = 0;
		mOpponentOffsideLine        = ball_state.GetPos().X();
		mOpponentOffsideLineConf    = ball_state.GetPosConf();
		mOpponentOffsideLineSpeed   = ball_state.GetVel().X();
	}
	else
	{
		mOpponentOffsideLine        = second;
		if (s_unum == 0)
		{
			mOpponentOffsideLineTm    = 0;
			mOpponentOffsideLineConf    = 0.0;
			mOpponentOffsideLineSpeed   = 0.0;
		}
		else
		{
			mOpponentOffsideLineTm    = s_unum;
			mOpponentOffsideLineConf    = mpWorldState->GetTeammate(s_unum).GetPosConf();
			mOpponentOffsideLineSpeed   = mpWorldState->GetTeammate(s_unum).GetVel().X();
		}
	}

}


void PositionInfo::UpdateOppGoalInfo()
{
    mOppGoal2Ball       = Vector(ServerParam::instance().PITCH_LENGTH / 2.0, 0.0) - mpWorldState->GetBall().GetPos();
    mOppLeftPost2Ball   = ServerParam::instance().oppLeftGoalPost() - mpWorldState->GetBall().GetPos();
	mOppRightPost2Ball  = ServerParam::instance().oppRightGoalPost() - mpWorldState->GetBall().GetPos();
    mOppGoal2BallAngle      = mOppGoal2Ball.Dir();
    mOppLeftPost2BallAngle  = mOppLeftPost2Ball.Dir();
    mOppRightPost2BallAngle = mOppRightPost2Ball.Dir();
}


const list<KeyPlayerInfo> & PositionInfo::GetXSortTeammate()
{
    if (mXSortTeammateList.empty())
    {
		KeyPlayerInfo kp;
		for (int i=1; i<=TEAMSIZE; i++)
        {
            if (mpWorldState->GetTeammate(i).IsAlive() == true &&
                mpWorldState->GetTeammate(i).GetPosConf() > FLOAT_EPS)
            {
                kp.mUnum = i;
                kp.mValue = mpWorldState->GetTeammate(i).GetPos().X();
                mXSortTeammateList.push_back(kp);
            }
		}
		mXSortTeammateList.sort();
    }
    return mXSortTeammateList;
}


const list<KeyPlayerInfo> & PositionInfo::GetXSortOpponent()
{
    if (mXSortOpponentList.empty())
    {
		KeyPlayerInfo kp;
		for (int i=1; i<=TEAMSIZE; i++)
        {
            if (mpWorldState->GetOpponent(i).IsAlive() == true &&
                mpWorldState->GetOpponent(i).GetPosConf() > FLOAT_EPS)
            {
                kp.mUnum = i;
                kp.mValue = mpWorldState->GetOpponent(i).GetPos().X();
			    mXSortOpponentList.push_back(kp);
            }
		}
		mXSortOpponentList.sort();
    }
    return mXSortOpponentList;
}


vector<Unum> PositionInfo::GetClosePlayerToPoint(const Vector & bp, const Unum & exclude_unum) const
{
	vector< pair<Unum, double> > tmp;

	for (vector<PlayerState*>::const_iterator it = mpWorldState->GetPlayerList().begin(); it != mpWorldState->GetPlayerList().end(); ++it){
        if ((*it)->IsAlive() && (*it)->GetPosConf() > FLOAT_EPS && (*it)->GetUnum() != exclude_unum){ // 算距离自己的球员时把自己排除掉
			tmp.push_back(pair<Unum, double>((*it)->GetUnum(), (*it)->GetPos().Dist2(bp)));
		}
	}

	sort(tmp.begin(), tmp.end(), PlayerDistCompare());

	vector<Unum> ret;
	for (vector< pair<Unum, double> >::iterator it = tmp.begin(); it != tmp.end(); ++it) {
		ret.push_back(it->first);
	}

	return ret;
}

const vector<Unum> & PositionInfo::GetClosePlayerToBall()
{
	if (mPlayer2BallList.empty()){
		mPlayer2BallList = GetClosePlayerToPoint(mpWorldState->GetBall().GetPos());
	}
	return mPlayer2BallList;
}

const vector<Unum> & PositionInfo::GetCloseTeammateToBall()
{
	if (mTeammate2BallList.empty()){
		const vector<Unum> & player2ball = GetClosePlayerToBall();
		for (vector<Unum>::const_iterator it = player2ball.begin(); it != player2ball.end(); ++it){
			if ((*it) > 0){
				mTeammate2BallList.push_back(*it);
			}
		}
	}
	return mTeammate2BallList;
}

const vector<Unum> & PositionInfo::GetCloseOpponentToBall()
{
	if (mOpponent2BallList.empty()){
		const vector<Unum> & player2ball = GetClosePlayerToBall();
		for (vector<Unum>::const_iterator it = player2ball.begin(); it != player2ball.end(); ++it){
			if ((*it) < 0){
				mOpponent2BallList.push_back(-(*it));
			}
		}
	}
	return mOpponent2BallList;
}

const vector<Unum> & PositionInfo::GetClosePlayerToPlayer(Unum i)
{
	int index = Unum2Index(i);
	if (mPlayer2PlayerList[index].empty()){
		mPlayer2PlayerList[index] = GetClosePlayerToPoint(mpWorldState->GetPlayer(i).GetPos(), i);
	}
	return mPlayer2PlayerList[index];
}

const vector<Unum> & PositionInfo::GetCloseTeammateToPlayer(Unum i)
{
	int index = Unum2Index(i);
	if (mTeammate2PlayerList[index].empty()){
		const vector<Unum> & player2player = GetClosePlayerToPlayer(i);
		for (vector<Unum>::const_iterator it = player2player.begin(); it != player2player.end(); ++it){
			if ((*it) > 0){
				mTeammate2PlayerList[index].push_back(*it);
			}
		}
	}
	return mTeammate2PlayerList[index];
}

const vector<Unum> & PositionInfo::GetCloseOpponentToPlayer(Unum i)
{
	int index = Unum2Index(i);
	if (mOpponent2PlayerList[index].empty()){
		const vector<Unum> & player2player = GetClosePlayerToPlayer(i);
		for (vector<Unum>::const_iterator it = player2player.begin(); it != player2player.end(); ++it){
			if ((*it) < 0){
				mOpponent2PlayerList[index].push_back(-(*it));
			}
		}
	}
	return mOpponent2PlayerList[index];
}

const vector<Unum> & PositionInfo::GetPlayerWithBallList()
{
	if (mPlayerWithBallList_UpdateTime != mpWorldState->CurrentTime()){
		mPlayerWithBallList.clear();
		if (GetClosePlayerToBall().size() > 0) {
			for (unsigned int i = 0; i < GetClosePlayerToBall().size(); ++i) {
				Unum unum = GetClosePlayerToBall()[i];
				if (mpWorldState->GetPlayer(unum).IsKickable()) {
					mPlayerWithBallList.push_back(unum);
				}
			}
		}
		mPlayerWithBallList_UpdateTime = mpWorldState->CurrentTime();
	}
	return mPlayerWithBallList;
}

Unum PositionInfo::GetPlayerWithBall(const double buffer)
{
	Unum tm_with_ball = GetTeammateWithBall(buffer);
	Unum opp_with_ball = GetOpponentWithBall(buffer);

	if (tm_with_ball > 0) return tm_with_ball;
	return -opp_with_ball;
}

Unum PositionInfo::GetTeammateWithBall()
{
	if (mpWorldState->GetBall().GetPosConf() < FLOAT_EPS) {
		return 0;
	}

	if (GetPlayerWithBallList().empty()){
		return 0;
	}
	else {
        for (unsigned int i = 0; i < GetPlayerWithBallList().size(); ++i){
            if (GetPlayerWithBallList()[i] > 0){
                return GetPlayerWithBallList()[i];
            }
        }
        return 0;
	}
}

/**
 * @param buffer
 * @return
 */
Unum PositionInfo::GetTeammateWithBall(const double buffer)
{
    if (mpWorldState->GetBall().GetPosConf() < FLOAT_EPS) {
        return 0;
    }

    for (unsigned int i = 0; i < GetCloseTeammateToBall().size(); ++i) {
    	Unum tm = GetCloseTeammateToBall()[i];
    	if (mpWorldState->GetTeammate(tm).IsKickable(mpWorldState->GetBall(), buffer)) {
    		return tm;
    	}
    	if (GetBallDistToTeammate(tm) > 2.0) break;
    }

    return 0;
}

Unum PositionInfo::GetOpponentWithBall()
{
	if (mpWorldState->GetBall().GetPosConf() < FLOAT_EPS) {
		return 0;
	}

	if (GetPlayerWithBallList().empty()){
		return 0;
	}
	else {
		for (unsigned int i = 0; i < GetPlayerWithBallList().size(); ++i){
			if (GetPlayerWithBallList()[i] < 0){
				return -GetPlayerWithBallList()[i];
			}
		}
		return 0;
	}
}

/**
 * @param buffer
 * @return
 */
Unum PositionInfo::GetOpponentWithBall(const double buffer)
{
	if (mpWorldState->GetBall().GetPosConf() < FLOAT_EPS) {
		return 0;
	}

	for (unsigned int i = 0; i < GetCloseOpponentToBall().size(); ++i) {
		Unum opp = GetCloseOpponentToBall()[i];
		if (mpWorldState->GetOpponent(opp).IsKickable(mpWorldState->GetBall(), buffer)) {
			return opp;
		}
		if (GetBallDistToOpponent(opp) > 2.0) break;
	}

	return 0;
}

