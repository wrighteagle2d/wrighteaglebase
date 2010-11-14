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

#ifndef __PositionInfo_H__
#define __PositionInfo_H__

#include "InfoState.h"
#include <cstdlib>
#include <vector>
#include <utility>
#include <deque>
#include <boost/tuple/tuple.hpp>

class WorldState;

/**
 * 有关场上球和球员位置的Info
 */
class PositionInfo: public InfoStateBase
{
public:
	PositionInfo(WorldState *pWorldState, InfoState *pInfoState);

	const double & GetBallDistToPlayer(Unum unum) const;
	const double & GetPlayerDistToPlayer(Unum unum1, Unum unum2) const;
	const double & GetBallDistToTeammate(Unum unum) const { Assert(unum > 0); return GetBallDistToPlayer(unum); }
    const double & GetBallDistToOpponent(Unum unum) const { Assert(unum > 0); return GetBallDistToPlayer(-unum); }

    const std::list<KeyPlayerInfo> & GetXSortTeammate();
    const std::list<KeyPlayerInfo> & GetXSortOpponent();

	std::vector<Unum> GetClosePlayerToPoint(const Vector & bp, const Unum & exclude_unum = 0) const;

	const std::vector<Unum> & GetClosePlayerToBall();
	const std::vector<Unum> & GetCloseTeammateToBall();
	const std::vector<Unum> & GetCloseOpponentToBall();

	const std::vector<Unum> & GetClosePlayerToPlayer(Unum i);
	const std::vector<Unum> & GetCloseTeammateToPlayer(Unum i);
	const std::vector<Unum> & GetCloseOpponentToPlayer(Unum i);

	const std::vector<Unum> & GetClosePlayerToTeammate(Unum i) { Assert(i > 0); return GetClosePlayerToPlayer(i); }
	const std::vector<Unum> & GetCloseTeammateToTeammate(Unum i) { Assert(i > 0); return GetCloseTeammateToPlayer(i); }
	const std::vector<Unum> & GetCloseOpponentToTeammate(Unum i) { Assert(i > 0); return GetCloseOpponentToPlayer(i); }

	const std::vector<Unum> & GetClosePlayerToOpponent(Unum i) { Assert(i > 0); return GetClosePlayerToPlayer(-i); }
	const std::vector<Unum> & GetCloseTeammateToOpponent(Unum i) { Assert(i > 0); return GetCloseTeammateToPlayer(-i); }
	const std::vector<Unum> & GetCloseOpponentToOpponent(Unum i) { Assert(i > 0); return GetCloseOpponentToPlayer(-i); }

	Unum GetClosestPlayerToBall()   { return GetClosePlayerToBall().empty()? 0: GetClosePlayerToBall()[0]; }
	Unum GetClosestTeammateToBall() { return GetCloseTeammateToBall().empty()? 0: GetCloseTeammateToBall()[0]; }
	Unum GetClosestOpponentToBall() { return GetCloseOpponentToBall().empty()? 0: GetCloseOpponentToBall()[0]; }

	Unum GetClosestPlayerToPlayer(Unum i) { return GetClosePlayerToPlayer(i).empty()? 0: GetClosePlayerToPlayer(i)[0]; }
	Unum GetClosestTeammateToPlayer(Unum i) { return GetCloseTeammateToPlayer(i).empty()? 0: GetCloseTeammateToPlayer(i)[0]; }
	Unum GetClosestOpponentToPlayer(Unum i) { return GetCloseOpponentToPlayer(i).empty()? 0: GetCloseOpponentToPlayer(i)[0]; }

	Unum GetClosestPlayerToTeammate(Unum i) { return GetClosePlayerToTeammate(i).empty()? 0: GetClosePlayerToTeammate(i)[0]; }
	Unum GetClosestTeammateToTeammate(Unum i) { return GetCloseTeammateToTeammate(i).empty()? 0: GetCloseTeammateToTeammate(i)[0]; }
	Unum GetClosestOpponentToTeammate(Unum i) { return GetCloseOpponentToTeammate(i).empty()? 0: GetCloseOpponentToTeammate(i)[0]; }

	Unum GetClosestPlayerToOpponent(Unum i) { return GetClosePlayerToOpponent(i).empty()? 0: GetClosePlayerToOpponent(i)[0]; }
	Unum GetClosestTeammateToOpponent(Unum i) { return GetCloseTeammateToOpponent(i).empty()? 0: GetCloseTeammateToOpponent(i)[0]; }
	Unum GetClosestOpponentToOpponent(Unum i) { return GetCloseOpponentToOpponent(i).empty()? 0: GetCloseOpponentToOpponent(i)[0]; }

	double GetClosestPlayerDistToBall() { return GetClosestPlayerToBall() == 0? HUGE_VALUE: mpWorldState->GetPlayer(GetClosestPlayerToBall()).GetPos().Dist(mpWorldState->GetBall().GetPos()); }
	double GetClosestTeammateDistToBall() { return GetClosestTeammateToBall() == 0? HUGE_VALUE: mpWorldState->GetTeammate(GetClosestTeammateToBall()).GetPos().Dist(mpWorldState->GetBall().GetPos()); }
	double GetClosestOpponentDistToBall() { return GetClosestOpponentToBall() == 0? HUGE_VALUE: mpWorldState->GetOpponent(GetClosestOpponentToBall()).GetPos().Dist(mpWorldState->GetBall().GetPos()); }

	const Unum & GetTeammateOffsideLineOpp() const     { return mTeammateOffsideLineOpp; }
	const double & GetTeammateOffsideLine() const       { return mTeammateOffsideLine; }
	const double & GetTeammateOffsideLineConf() const   { return mTeammateOffsideLineConf; }
	const double & GetTeammateOffsideLineSpeed() const  { return mTeammateOffsideLineSpeed; }

	const Unum & GetOpponentOffsideLineTm() const     { return mOpponentOffsideLineTm; }
	const double & GetOpponentOffsideLine() const       { return mOpponentOffsideLine; }
	const double & GetOpponentOffsideLineConf() const   { return mOpponentOffsideLineConf; }
	const double & GetOpponentOffsideLineSpeed() const  { return mOpponentOffsideLineSpeed; }

	/** 得到当前可以踢到球的球员列表 */
	const std::vector<Unum> & GetPlayerWithBallList();

	Unum GetPlayerWithBall() { return GetPlayerWithBallList().empty()? 0: GetPlayerWithBallList()[0]; }

    /**
     * Get closest player in a cycle which radius = buffer;
     * If buffer is larger than 2 then the result will be calculated as buffer = 2;
     * @param buffer;
     * return Unum of player. Unum > 0 ? teammate:opponent/unknow.
     */
    Unum GetPlayerWithBall(const double buffer);

	Unum GetTeammateWithBall();

    /**
     * Get closest teammate in a cycle which radius = buffer;
     * If buffer is larger than 2 then the result will be calculated as buffer = 2;
     * @param buffer;
     * return Unum of teammate.
     */
    Unum GetTeammateWithBall(const double buffer);

	Unum GetOpponentWithBall();

    /**
     * Get closest opponent in a cycle which radius = buffer;
     * If buffer is larger than 2 then the result will be calculated as buffer = 2;
     * @param buffer;
     * return Unum of opponent.
     */
    Unum GetOpponentWithBall(const double buffer);

private:
	/** 更新函数 */
	void UpdateRoutine();

	/** 数组下标和球员号码之间的相互转化，球员号码正表示队友，负表示对手 */
	int Unum2Index(Unum unum) const {
		return unum > 0? unum: TEAMSIZE - unum;
	}

	int Index2Unum(int index) const {
		return index <= TEAMSIZE? index: TEAMSIZE - index;
	}

	void UpdateDistMatrix();
	void UpdateOffsideLine();
    void UpdateOppGoalInfo(); /** 暂时这样命名，以后有需要再改 */

private:
	Array<Array<double, 1 + 2 * TEAMSIZE>, 1 + 2 * TEAMSIZE > mDistMatrix; // 22名球员和球相互之间的距离，0为球，1-11为队友，12到22为对手

    std::list<KeyPlayerInfo> mXSortTeammateList;
    std::list<KeyPlayerInfo> mXSortOpponentList;

	std::vector<Unum> mPlayer2BallList;
	std::vector<Unum> mTeammate2BallList;
	std::vector<Unum> mOpponent2BallList;

	Array<std::vector<Unum>, 1 + 2 * TEAMSIZE> mPlayer2PlayerList;
	Array<std::vector<Unum>, 1 + 2 * TEAMSIZE> mTeammate2PlayerList;
	Array<std::vector<Unum>, 1 + 2 * TEAMSIZE> mOpponent2PlayerList;

	Unum mTeammateOffsideLineOpp;
	double mTeammateOffsideLine;
	double mTeammateOffsideLineConf;
	double mTeammateOffsideLineSpeed;

	Unum mOpponentOffsideLineTm;
	double mOpponentOffsideLine;
	double mOpponentOffsideLineConf;
	double mOpponentOffsideLineSpeed;

	std::vector<Unum> mPlayerWithBallList; //当前可以踢球的队员集合 -- 不加buffer的判断
	Time mPlayerWithBallList_UpdateTime;

private:
	class PlayerDistCompare {
	public:
		bool operator()(const std::pair<Unum, double> & i, const std::pair<Unum, double> & j){
			return i.second < j.second;
		}
	};

public:
    const AngleDeg & GetPlayerDir2Ball(Unum unum) const {
        return (unum > 0) ? GetTeammateDir2Ball(unum): GetOpponentDir2Ball(-unum);
    }

    const AngleDeg & GetTeammateDir2Ball(Unum unum) const {
        return mTeammateDir2Ball[unum - 1];
    }

    const AngleDeg & GetOpponentDir2Ball(Unum unum) const {
        return mOpponentDir2Ball[unum - 1];
    }

private:
    Array<AngleDeg, TEAMSIZE> mTeammateDir2Ball; // 队友相对球的角度
    Array<AngleDeg, TEAMSIZE> mOpponentDir2Ball; // 对手相对球的角度

public:
    const Vector & GetOppGoal2Ball() const      { return mOppGoal2Ball; }
    const Vector & GetOppLeftPost2Ball() const  { return mOppLeftPost2Ball; }
    const Vector & GetOppRightPost2Ball() const { return mOppRightPost2Ball; }
    const AngleDeg & GetOppGoal2BallAngle() const       { return mOppGoal2BallAngle; }
    const AngleDeg & GetOppLeftPost2BallAngle() const   { return mOppLeftPost2BallAngle; }
    const AngleDeg & GetOppRightPost2BallAngle() const  { return mOppRightPost2BallAngle; }

private:
    Vector mOppGoal2Ball;       // 球门中心
    Vector mOppLeftPost2Ball;   // 左门柱
	Vector mOppRightPost2Ball;  // 右门柱
	AngleDeg mOppGoal2BallAngle;
    AngleDeg mOppLeftPost2BallAngle;
	AngleDeg mOppRightPost2BallAngle;
};

#endif
