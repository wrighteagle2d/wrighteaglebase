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

#ifndef BEHAVIORBASE_H_
#define BEHAVIORBASE_H_

#include <list>
#include <string>
#include "Geometry.h"
#include "ActionEffector.h"
#include "Formation.h"

class WorldState;
class BallState;
class PlayerState;
class InfoState;
class PositionInfo;
class InterceptInfo;
class Strategy;
class Analyser;
class Agent;

enum BehaviorType {
	BT_None,

	BT_Penalty,
	BT_Goalie,
	BT_Setplay,
	BT_Position,
	BT_Dribble,
	BT_Pass,
	BT_Shoot,
	BT_Intercept,
	BT_Formation,
	BT_Block,
	BT_Mark,

	BT_Max
};


enum BehaviorDetailType {
	BDT_None,

	BDT_Intercept_Normal,
    BDT_Pass_Ahead,
    BDT_Pass_Direct,
	BDT_Dribble_Fast,
	BDT_Dribble_Normal,
    BDT_Position_Normal,
    BDT_Position_Rush,
	BDT_Shoot_Tackle,
    BDT_Goalie_Position,
    BDT_Goalie_Catch,
    BDT_Goalie_Tackle,
    BDT_Setplay_Move,
    BDT_Setplay_Scan,
    BDT_Setplay_GetBall
};

class BehaviorExecutable;

class BehaviorFactory {
	BehaviorFactory();
	virtual ~BehaviorFactory();

public:
	typedef BehaviorExecutable * (*BehaviorCreator)(Agent & agent);
	static BehaviorFactory & instance();

	/**
	* 创建行为
	* @param agent
	* @param type
	* @return
	*/
	BehaviorExecutable * CreateBehavior(Agent & agent, BehaviorType type);

	/**
	* 注册行为创建函数，只有具有执行效果的行为需要注册，比如 BehaviorAttackPlanner, BehaviorDefensePlanner 不需要注册
	* @param type 行为类型
	* @param creator 创建函数
	* @return
	*/
	bool RegisterBehavior(BehaviorType type, BehaviorCreator creator, const char *behavior_name);

	const char *GetBehaviorName(BehaviorType type) {
		return mNameMap[type].c_str();
	}

private:
	Array<BehaviorCreator, BT_Max, true> mCreatorMap;
	Array<std::string, BT_Max> mNameMap;
};

class ActiveBehavior
{
	/** behavior type and agent pointer */
	BehaviorType mType;
	Agent * mpAgent;

public:

	ActiveBehavior(Agent & agent, BehaviorType type, BehaviorDetailType detail_type = BDT_None) :
		mType(type), mpAgent(&agent), mEvaluation(0.0),
		mKickCycle(0), mAngle(0), mTarget(Vector(0.0, 0.0)),
		mPower(0.0), mDistance(0.0), mKickSpeed(0.0),
		mFoul(false), mDetailType(detail_type),
		mBuffer(0.0)
	{

	}

	void Clear() {
		mEvaluation = 0.0;
	}

	const BehaviorType & GetType() const {
		return mType;
	}
	void SetType(const BehaviorType &type) {
		mType = type;
	}

	Agent & GetAgent() const {
		return *mpAgent;
	}

	bool Execute(); //不依赖于指向 behavior 的指针，但要求 BehaviorType 无误
	void SubmitVisualRequest(double plus);

	bool operator>(const ActiveBehavior & ab) const {
		return this->mEvaluation > ab.mEvaluation;
	}
	bool operator ==(const ActiveBehavior & ab) const {
		return this->mEvaluation == ab.mEvaluation;
	}
	bool operator <(const ActiveBehavior & ab) const {
		return this->mEvaluation < ab.mEvaluation;
	}

	double mEvaluation;

	//behavior detail
	int mKickCycle; // 踢球的周期，可能多脚踢球
	AngleDeg mAngle; // 角度
	Vector mTarget; // 目标点地位置，可以是把球踢到该点和自己跑到该点等
	double mPower; // dash的power
	double mDistance; // 目标点到自己的距离
	double mKickSpeed; // kick的球速
	bool mFoul; //是否要故意犯规铲球

	KeyPlayerInfo mKeyTm; // teammate
	KeyPlayerInfo mKeyOpp; // opponent
	KeyPlayerInfo mKeyOppGB; // opponent to get ball
	KeyPlayerInfo mKeyOppGT; // opponent to go through

	BehaviorDetailType mDetailType; //细节类型
	double mBuffer; //有些行为执行时的buffer是在plan时算好的，要先存到这个变量里
};

class BehaviorAttackData {
public:
	BehaviorAttackData(Agent & agent);
	virtual ~BehaviorAttackData();

	Agent & mAgent;

	const WorldState & mWorldState;

	const BallState & mBallState;
	const PlayerState & mSelfState;

	PositionInfo & mPositionInfo;
	InterceptInfo & mInterceptInfo;

	Strategy & mStrategy;

    Formation & mFormation;
};

class BehaviorDefenseData: public BehaviorAttackData {
public:
	BehaviorDefenseData(Agent & agent);
	virtual ~BehaviorDefenseData();

	Analyser & mAnalyser;
};

template < class BehaviorDataType >
class BehaviorPlannerBase: public BehaviorDataType {
	BehaviorPlannerBase(const BehaviorPlannerBase &);

public:
	BehaviorPlannerBase(Agent & agent): BehaviorDataType(agent) {}
	virtual ~BehaviorPlannerBase() {}

	/**
	* 做决策，产生最好的ActiveBehavior，存到behavior_list里面
	*/
	virtual void Plan(std::list<ActiveBehavior> & behavior_list) = 0;

public:
	const std::list<ActiveBehavior> & GetActiveBehaviorList() {
		return mActiveBehaviorList;
	}

protected:
	std::list<ActiveBehavior> mActiveBehaviorList; // record the active behaviors for each high level behavior
};

class BehaviorExecutable {
	BehaviorExecutable(const BehaviorExecutable&);

public:
	BehaviorExecutable() {}
	virtual ~BehaviorExecutable() {}

	/**
	* 执行 ActiveBehavior
	* @param act ActiveBehavior
	* @return
	*/
	virtual bool Execute(const ActiveBehavior & act_bhv) = 0; //每周期只有一个行为被执行
	virtual void SubmitVisualRequest(const ActiveBehavior & act_bhv, double plus = 0.0) { //但可以有多个行为提交视觉请求
		(void) act_bhv;
		(void) plus;
	}

	/// behavior factory interfaces
	template <class BehaviorDerived >
	static BehaviorExecutable* Creator(Agent & agent) { return new BehaviorDerived(agent); }

	template<class BehaviorDerived>
	static bool AutoRegister() {
		std::string behavior_name;

#ifndef WIN32
		std::string function_name = __PRETTY_FUNCTION__;

		size_t end = function_name.find("Executer");
		if (end != std::string::npos) {
			size_t begin = end;
			while (begin && isalpha(function_name[begin--])) ;
			begin += 2;
			if (begin < end) {
				behavior_name = function_name.substr(begin, end - begin);
			}
		}
#endif

		return BehaviorFactory::instance().RegisterBehavior(
				BehaviorDerived::BEHAVIOR_TYPE, Creator<BehaviorDerived>, behavior_name.c_str());
	}
};

template < class BehaviorDataType >
class BehaviorExecuterBase: public BehaviorDataType, public BehaviorExecutable {
	BehaviorExecuterBase(const BehaviorExecuterBase &);

public:
	BehaviorExecuterBase(Agent & agent): BehaviorDataType(agent) {}
	virtual ~BehaviorExecuterBase() {}
};


typedef std::list<ActiveBehavior> ActiveBehaviorList;
typedef std::list<ActiveBehavior>::iterator ActiveBehaviorPtr;

#define TeammateFormationTactic(TacticName) (*(FormationTactic##TacticName *)mFormation.GetTeammateTactic(FTT_##TacticName))
#define OpponentFormationTactic(TacticName) (*(FormationTactic##TacticName *)mFormation.GetOpponentTactic(FTT_##TacticName))

#endif /* BEHAVIORBASE_H_ */
