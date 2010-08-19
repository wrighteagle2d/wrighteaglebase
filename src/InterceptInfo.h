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

#ifndef __InterceptInfo_H__
#define __InterceptInfo_H__

#include "InfoState.h"
#include "InterceptModel.h"

enum InterceptRes { //拦截结果
	IR_None,

	IR_Failure,
	IR_Success
};

//根据简化模型求解得到的截球信息
class PlayerInterceptInfo
{
private:
	InterceptModel::InterceptSolution solution;
	friend class InterceptInfo;

public:
	PlayerInterceptInfo():
		mTime (Time(-3, 0)),
		mpPlayer (0),
		mMinCycle (1000),
		mInterPos (Vector(1000, 1000)),
		mIntervals (1),
		mRes (IR_Failure)
	{

	}

	Time mTime;		//更新时间
	const PlayerState *mpPlayer;
	Array<int, 3> mInterCycle; //修正后的截球区间对应的周期数
	int mMinCycle; //最小截球周期（根据前面的数据计算而得）
	Vector mInterPos; //最小周期对应的截球点
	int mIntervals; //区间个数 {1, 2}
	InterceptRes mRes; //截球成功是指能在场地之内截到球

	friend std::ostream &operator<<(std::ostream &os, const PlayerInterceptInfo &it){
		if (it.solution.interc == 1){
			return os << "[" << it.mInterCycle[0] << ", $]";
		}
		else {
			return os << "[" << it.mInterCycle[0] << ", " << it.mInterCycle[1] << "] [" << it.mInterCycle[2] << ", $]";
		}
	}
};

struct OrderedIT {
	OrderedIT(PlayerInterceptInfo *pinfo, Unum unum, int cd): mpInterceptInfo(pinfo), mUnum(unum), mCycleDelay(cd) {

	}

	PlayerInterceptInfo *mpInterceptInfo; //指向num的player的拦截info,无效时为NULL
	Unum mUnum; //+ 为队友，- 为对手
	int mCycleDelay;   //cycle delay

	bool operator < (const OrderedIT& it) const {
		return (mpInterceptInfo->mMinCycle == it.mpInterceptInfo->mMinCycle)? mCycleDelay < it.mCycleDelay: mpInterceptInfo->mMinCycle < it.mpInterceptInfo->mMinCycle;
	}
};

class InterceptInfo: public InfoStateBase
{
public:
    InterceptInfo(WorldState *pWorldState, InfoState *pInfoState);

    PlayerInterceptInfo *GetPlayerInterceptInfo(Unum unum) const;
    const std::vector<OrderedIT> & GetOIT() const { return mOIT; }
    bool IsPlayerBallInterceptable(Unum unum) const { return GetPlayerInterceptInfo(unum)? GetPlayerInterceptInfo(unum)->mRes == IR_Success: false; } //可以在球出界前拦截到球

	static void CalcTightInterception(const BallState & ball, PlayerInterceptInfo *pInfo, bool can_inverse = true); //求解可踢即可截`紧'截球区间 -- 考虑gotopoint修正
	static void CalcLooseInterception(const BallState & ball, PlayerInterceptInfo *pInfo, const double & buffer); //求解buffer可截的`松‘截球区间 -- 不考虑gotopoint修正

private:
	static void CalcIdealInterception(const BallState & ball, PlayerInterceptInfo *pInfo, const double & buffer);
	static void AnalyseInterceptSolution(const BallState & ball, PlayerInterceptInfo *pInfo);

private:
	void UpdateRoutine();

	void SortIntercerptInfo();
	PlayerInterceptInfo *VerifyIntInfo(Unum unum);

private:
    PlayerArray<PlayerInterceptInfo> mTeammateInterceptInfo;
    PlayerArray<PlayerInterceptInfo> mOpponentInterceptInfo;
    std::vector<OrderedIT> mOIT; //拦截排序链表
};

#endif
