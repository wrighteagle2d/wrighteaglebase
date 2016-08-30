/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2016                                                    *
 * Copyright (c) 1998-2016 WrightEagle 2D Soccer Simulation Team,                   *
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

#ifndef __PLAYERSTATE_H__
#define __PLAYERSTATE_H__

#include "BaseState.h"
#include "PlayerParam.h"
#include "BallState.h"

class PlayerState : public MobileState
{
public:
	/**手臂指向动作的类 注：自己队员和自己获得的信息不一样，
	* 暂时以自己可获得的信息为标准构建类，其它人员只能获得信息的一部分*/
	class ArmPoint
	{
	public:
		ArmPoint(int = 0):
			mMovableBan(0),
			mExpireBan(0),
			mTargetDist(0),
			mTargetDir(0)
		{
		}

		/**到下次手臂能动的剩余周期数*/
		int mMovableBan;

		/**直到手臂动作失效剩余的周期数*/
		int mExpireBan;

		/**指向目标的距离*/
		double mTargetDist;

		/**指向目标的方向*/
		AngleDeg mTargetDir;
	};

	/**注视对象的类 注：暂时只有自己可以获得注视的信息，这样只是为以后留接口*/
	class FocusOn
	{
	public:
		FocusOn(int = 0):
			mFocusSide('?'),
			mFocusNum(0)
		{
		}

		/**注视的对象为哪一队*/
		char mFocusSide;

		/**注视的球员号*/
		Unum mFocusNum;
	};

public:
	PlayerState();
	virtual ~PlayerState() {}

	/**更新体力
	* @param 体力
	* @param 时间
	* @param 置信度*/
	void UpdateStamina(double stamina);

	/**获得当前或前几次的体力
	*/
	const double & GetStamina() const { return mStamina; }

	/**更新effort
	* @param 体力
	* @param 时间
	* @param 置信度*/
	void UpdateEffort(double effort);
	void UpdateCapacity(double capacity);

	/**获得当前或前几次的effort
	*/
	const double & GetEffort() const { return mEffort; }

	const double & GetCapacity() const { return mCapacity; }

	/**
	 * Update recovery
	 * @param recovery
	 */
	void UpdateRecovery(double recovery) { mRecovery = recovery;}

	/** get recovery*/
	const double & GetRecovery() const { return mRecovery;}

	/**更新脖子朝向
	* @param 朝向
	* @param 时间
	* @param 置信度*/
	void UpdateNeckDir(double dir , int delay = 0, double conf = 1);

	/**获得当前或前几次的脖子全局朝向
	*/
	double GetNeckGlobalDir() const { return GetNormalizeAngleDeg(GetNeckDir() + GetBodyDir()); }

	/**获得当前脖子相对于身体的角度
	* 注意区别与GetGlobalNeckDir（）
	*/
	double GetNeckDir() const { return GetNormalizeNeckAngle(mNeckDir.mValue); }

	/**获得当前或前几次的脖子朝向的时间
	*/
	int GetNeckDirDelay() const { return mNeckDir.mCycleDelay; }

	/**获得当前或前几次的脖子朝向的置信度
	*/
	double GetNeckDirConf() const { return mNeckDir.mConf; }

	/**更新身体朝向
	* @param 朝向
	* @param 时间
	* @param 置信度*/
	void UpdateBodyDir(double dir , int delay = 0, double conf = 1);

	/**获得当前或前几次的身体朝向
	*/
    const double & GetBodyDir() const { return mBodyDir.mValue; }

	/**获得当前或前几次的身体朝向的时间
	*/
	int GetBodyDirDelay() const { return mBodyDir.mCycleDelay; }

	/**获得当前或前几次的身体朝向的置信度
	*/
	double GetBodyDirConf() const { return mBodyDir.mConf; }

	void UpdateTackleBan(int ban);

	int GetTackleBan() const { return mTackleBan; }

	/**更新可扑球的周期
	* @param 还剩多少周期可以扑球
	*/
	void UpdateCatchBan(int ban);

	/**获得当前周期还有多少周期可以扑球 暂时无法用*/
	int GetCatchBan() const { return mCatchBan; }

	/**更新手指向朝向
	* @param 手朝向
	* @param 时间
	* @param 置信度
	* @param 距离
	* @param 下次可动时间
	* @param 有效时间剩余*/
	void UpdateArmPoint(AngleDeg dir , int delay = 0, double conf = 1 , double  dist = 0 , int move_ban = -1 , int expire_ban = -1);

	/**是否在指*/
	bool IsPointing() const {return mIsPointing;}
	void SetIsPointing(bool is_pointing) {mIsPointing = is_pointing;}

	/**获得手指向的朝向*/
	AngleDeg GetArmPointDir() const { return GetNormalizeAngleDeg(mArmPoint.mValue.mTargetDir); }

	/**获得手指向的距离 暂时非自己不可用*/
	double  GetArmPointDist() const { return mArmPoint.mValue.mTargetDist; }

	/**获得手还剩多少时间才可动 暂时非自己不可用*/
	int GetArmPointMovableBan() const { return mArmPoint.mValue.mMovableBan; }

	/**获得手动的有效时间剩余 暂时非自己不可用*/
	int GetArmPointExpireBan() const { return mArmPoint.mValue.mExpireBan; }

	/**获得手指向的时间延迟*/
	int GetArmPointDelay() const { return mArmPoint.mCycleDelay; }

	/**获得手指向的置信度*/
	double GetArmPointConf() const { return mArmPoint.mConf; }

	/**更新注视目标
	* @param 注视哪方
	* @param 注视队员号
	* @param 时间
	* @param 置信度*/
	void UpdateFocusOn(char side , Unum num , int delay = 0 , double conf = 1);

	/**获得注视的队员方位*/
	char GetFocusOnSide() const { return mFocusOn.mValue.mFocusSide; }

	/**获得注视的队员号码*/
	Unum GetFocusOnUnum() const { return mFocusOn.mValue.mFocusNum; }

	/**获得注视的时间延迟*/
	int GetFocusOnDelay() const { return mFocusOn.mCycleDelay; }

	/**获得注视的置信度*/
	double GetFocusOnConf() const { return mFocusOn.mConf; }

	/**更新是否本周期踢球*/
	void UpdateKicked(bool is_kicked);

	/**更新本周期是否铲球*/ //  ***********************现在该函数无效**********************
	void UpdateTackling(bool is_tackling);

	/**更新本周期是否被铲倒*/
	void UpdateLying(bool is_lying);

	/**获得上周期是否踢球 暂时对自己估计无法用*/
	bool IsKicked() const { return mIsKicked; }

	/**获得本周期是否活着 暂时对自己估计无法用*/
	bool IsAlive() const { return mIsAlive; }

	void SetIsAlive(bool alive);

	/**获得本周期是否由于铲球而处于tackle_ban中*/
	bool IsTackling() const { return mTackleBan > 0; }

	bool IsLying() const { return mFoulChargedCycle > 0; }

	/**更新球员犯规记录*/
	CardType GetCardType() const { return mCardType; }
	void UpdateCardType(CardType card_type) { if ( mCardType == CR_None || ( mCardType == CR_Yellow && card_type == CR_Red ) ) mCardType = card_type; }

	/**获得与更新球员铲倒时间*/
	int GetFoulChargedCycle () const { return mFoulChargedCycle; }

	void UpdateFoulChargedCycle(int x)
	{
		mFoulChargedCycle = x;
	}

	bool IsIdling() const {
		Assert(!(IsTackling() && IsLying()));
		return IsTackling() || IsLying();
	}

	int GetIdleCycle() const {
		Assert(!(IsTackling() && IsLying()));
		return GetTackleBan() + GetFoulChargedCycle();
	}

	/**获得球员的类型*/
	int GetPlayerType() const { return mPlayerType; }

	/**设置球员的类型*/
	void UpdatePlayerType(int type);

	/**设置球员的视角宽度*/
	void UpdateViewWidth(ViewWidth width);

	/**获得视角宽度*/
	ViewWidth GetViewWidth() const { return mViewWidth; }

	/**get view angle*/
	AngleDeg GetViewAngle() const { return sight::ViewAngle(mViewWidth);}

	/**内部自更新
	* @param 每周期delay加上的数据
	* @param 每周期conf衰减的数据*/
	void AutoUpdate(int delay_add = 1 , double conf_dec_factor = 1);

	/**设置球员边和Unum*/
	void UpdateUnum(Unum num);

	/**获得Unum*/
	Unum GetUnum() const { return mUnum; }

	/**update collide with ball*/
	void UpdateCollideWithBall(bool collide) { mCollideWithBall = collide;}

	/**get collide with ball*/
	bool GetCollideWithBall() const { return mCollideWithBall;}

	/**update collide with player*/
	void UpdateCollideWithPlayer(bool collide) { mCollideWithPlayer = collide;}

	/** get collide with player*/
	bool GetCollideWithPlayer() const { return mCollideWithPlayer;}

	/** update collide with post*/
	void UpdateCollideWithPost(bool collide) { mCollideWithPost = collide;}

	/** get collide with post*/
	bool GetCollideWithPost() const { return mCollideWithPost;}

	/**转换得到一个用于反算的PlayerState*/
	void GetReverseFrom(const PlayerState & o);

private:
	/**存储数个周期的体力*/
	double mStamina;

	/**存储Effort*/
	double mEffort;

	/**存储capacity*/
	double mCapacity;

	/**store Recovery*/
	double mRecovery;

	/**存储脖子的朝向*/
	StateValue<double> mNeckDir;

	/**存储身体的朝向*/
	StateValue<double> mBodyDir;

	/**还有多少周期可以铲球*/
	int mTackleBan;

	/**还有多少周期可以扑球 暂时无法用*/
	int mCatchBan;

	/**队员指向信息*/
	StateValue<ArmPoint> mArmPoint;

	/**队员注视信息*/
	StateValue<FocusOn>  mFocusOn;

	/**上次看到时是否在指*/
	bool mIsPointing;

	bool mIsKicked;

	/**是否活着 暂时自己估计无法用*/
	bool mIsAlive;

	/**球员类型*/
	int mPlayerType;

	/**视角宽度*/
	ViewWidth mViewWidth;

	/**
	 * 自己的Unum
	 * + 表示是队友，- 表示是对手
	 * */
	Unum mUnum;

	/**
	 * whether collide with ball
	 */
	bool mCollideWithBall;

	/**
	 * whether collide with player
	 */
	bool mCollideWithPlayer;

	/**
	 * whether collide with post
	 */
	bool mCollideWithPost;

	int mFoulChargedCycle; //被铲倒在地的周期

	//==============================================================================
public:
	bool IsKickable() const { return mIsKickable; }
	bool IsKickable(const BallState &, const double & buffer) const;
	void UpdateKickable(bool kickable) { mIsKickable = kickable; }

	double GetKickRate() const { return mKickRate; }
	void UpdateKickRate(double kick_rate) { mKickRate = kick_rate; }

	bool IsBallCatchable() const { return mIsGoalie && mBallCatchable; }
	void UpdateBallCatchable(bool ball_catchable) { mBallCatchable = ball_catchable; }

	bool UnderDangerousTackleCondition() const { return mUnderDangerousTackleCondition; }
	void UpdateDangerousTackleCondition(const bool may) { mUnderDangerousTackleCondition = may; }

	double GetTackleProb(const bool foul) const { return mTackleProb[foul]; }
	void UpdateTackleProb(double tackle_prob, const bool foul = false) { mTackleProb[foul] = tackle_prob; }

	AngleDeg GetMaxTurnAngle() const { return mMaxTurnAngle; }
	void UpdateMaxTurnAngle(AngleDeg max_turn_angle) { mMaxTurnAngle = max_turn_angle; }

	bool IsTired() const {return mIsTired;}
	void UpdateTired(bool is_tired) { mIsTired = is_tired;}

    const double & GetMinStamina() const { return mMinStamina; }
    void UpdateMinStamina(const double min_stamina) { mMinStamina = min_stamina; } 

	bool IsBodyDirMayChanged() const { return mIsBodyDirMayChanged;}
	void UpdateBodyDirMayChanged(bool may_changed) { mIsBodyDirMayChanged = may_changed;}

	//重新设置Pos_delay  主要在BT_Intercept使用;
	void ResetPosDelay(int delay)
	{
		Assert(delay >= 0);
		double conf = 1;
		for (int i = 0;i < delay;i++)
		{
			conf  *= PlayerParam::instance().playerConfDecay();
		}

		UpdatePos(GetPos() , delay , conf);
	}

private:
	bool        mIsKickable;  /** kick */
	double      mKickRate;
	bool        mBallCatchable; /** catch */

	Array<double, 2,true> mTackleProb;    /** tackle */
	bool mUnderDangerousTackleCondition; //如果铲球，是否可能是危险情况

	AngleDeg    mMaxTurnAngle;  /** turn */
	bool		mIsTired;		/** tell is  tired or not*/
    double      mMinStamina; // 最低体力值，考虑了player的extraStamina
	CardType    mCardType;  /** card */

	//just for update body dir other should not use
	bool        mIsBodyDirMayChanged;

public:
	bool IsGoalie() const { return mIsGoalie; }
	void UpdateIsGoalie(bool is_goalie) { mIsGoalie = is_goalie; }

private:
	bool        mIsGoalie;

public:
    bool IsSensed() const { return mIsSensed; }
    void UpdateIsSensed(bool is_sensed) { mIsSensed = is_sensed; }

private:
    bool        mIsSensed; // 为true表示是可以收到sense信息的球员，即“真正”的自己

public:
	/** some useful interfaces */
	const double & GetPlayerSpeedMax() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).playerSpeedMax(); }
	const double & GetStaminaIncMax() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).staminaIncMax(); }
	const double & GetPlayerDecay() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).playerDecay(); }
	const double & GetInertiaMoment() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).inertiaMoment(); }
	const double & GetDashPowerRate() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).dashPowerRate(); }
	const double & GetPlayerSize() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).playerSize(); }
	const double & GetKickableMargin() const {return PlayerParam::instance().HeteroPlayer(mPlayerType).kickableMargin(); }
	const double & GetKickRand() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).kickRand(); }
	const double & GetKickPowerRate() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).kickPowerRate(); }
	const double & GetExtraStamina() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).extraStamina(); }
	const double & GetEffortMax() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).effortMax(); }
	const double & GetEffortMin() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).effortMin(); }
	virtual const double & GetKickableArea() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).kickableArea(); }
	const double & GetCatchAreaLStretch() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).catchableAreaLStretch(); }
	const double & GetMinCatchArea() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).minCatchArea(); }
	const double & GetMaxCatchArea() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).maxCatchArea(); }
	const double & GetFoulDetectProbability() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).foulDetectProbability(); }

	double GetAccelerationRateOnDir(const double dir) const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationRateOnDir(dir); }
    const double & GetAccelerationFrontRate() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationFrontRate(); }
    const double & GetAccelerationSideRate() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationSideRate(); }
    const double & GetAccelerationFrontMax() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationFrontMax(); }
    const double & GetAccelerationSideMax() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationSideMax(); }
    const double & GetAccelerationFront(const double & power) const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationFront(power); }
    const double & GetAccelerationSide(const double & power) const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationSide(power); }

    AngleDeg GetRandAngle(const double & power ,const double & vel ,const BallState & bs ) const;
	double GetCatchProb( const double & dist ) const;
	double GetControlBallProb(const Vector & ball_pos) const; //返回kick、catch、tackle三者中概率最大的一个

	Vector GetPredictedPosWithDash(int steps = 1, double dash_power = 0, AngleDeg dash_dir = 0.0) const;
	Vector GetPredictedVelWithDash(int steps = 1, double dash_power = 0, AngleDeg dash_dir = 0.0) const;

	double CorrectDashPowerForStamina(double dash_power) const;

	AngleDeg GetEffectiveTurn(AngleDeg moment, double my_speed) const;
	AngleDeg GetEffectiveTurn(AngleDeg moment) const
	{ return GetEffectiveTurn(moment, GetVel().Mod()); }
	AngleDeg GetMaxEffectiveTurn(double my_speed) const /* how much we'll actually turn if we try max turn */
	{ return GetEffectiveTurn(ServerParam::instance().maxMoment(), my_speed); }

public:
	bool IsBodyDirValid() const { return GetBodyDirConf() > 0.991; }

	bool IsOutOfStamina() const { return (GetStamina() + GetCapacity() < PlayerParam::instance().MinStamina() + FLOAT_EPS); }

};

//make to be inlined
inline void PlayerState::UpdateStamina(double stamina)
{
	mStamina = stamina;
}

inline void PlayerState::UpdateEffort(double effort)
{
	mEffort = effort;
}

inline void PlayerState::UpdateCapacity(double capacity)
{
	mCapacity = capacity;
}

inline void PlayerState::UpdateNeckDir(double dir, int delay, double conf)
{
	mNeckDir.mValue = GetNormalizeAngleDeg(dir);
	mNeckDir.mCycleDelay = delay;
	mNeckDir.mConf  = conf;
}

inline void PlayerState::UpdateBodyDir(double dir, int delay, double conf)
{
	mBodyDir.mValue = GetNormalizeAngleDeg(dir);
	mBodyDir.mCycleDelay = delay;
	mBodyDir.mConf = conf;
}

inline void PlayerState::UpdateTackleBan(int ban)
{
	mTackleBan = ban;
}

inline void PlayerState::UpdateCatchBan(int ban)
{
	mCatchBan = ban;
}

inline void PlayerState::UpdateArmPoint(AngleDeg dir, int delay, double conf, double dist, int move_ban , int expire_ban )
{
	ArmPoint arm;
	arm.mTargetDir = GetNormalizeAngleDeg(dir);
	arm.mTargetDist = dist;
	arm.mMovableBan = move_ban;
	arm.mExpireBan  = expire_ban;

	mArmPoint.mValue = arm;
	mArmPoint.mCycleDelay = delay;
	mArmPoint.mConf  = conf;
}

inline void PlayerState::UpdateFocusOn(char side, Unum num, int delay, double conf)
{
	FocusOn focus;
	focus.mFocusSide = side;
	focus.mFocusNum  = num;

	mFocusOn.mValue = focus;
	mFocusOn.mCycleDelay = delay;
	mFocusOn.mConf  = conf;
}

inline void PlayerState::UpdateKicked(bool is_kicked)
{
	mIsKicked = is_kicked;
}

inline void PlayerState::AutoUpdate(int delay_add, double conf_dec_factor)
{
	MobileState::AutoUpdate(delay_add , conf_dec_factor);

	mNeckDir.AutoUpdate(delay_add , conf_dec_factor);
	mBodyDir.AutoUpdate(delay_add , conf_dec_factor);
	mArmPoint.AutoUpdate(delay_add , conf_dec_factor);
	mFocusOn.AutoUpdate(delay_add , conf_dec_factor);

	mCollideWithPost = false;
	mCollideWithPlayer = false;
	mCollideWithBall = false;
	mIsKicked    = false;
	mIsTired     = false;

	if (mTackleBan > 0) {
		mTackleBan -= delay_add;
		mTackleBan = Max(mTackleBan, 0);
	}

	if (mFoulChargedCycle > 0) {
		mFoulChargedCycle -= delay_add;
		mFoulChargedCycle = Max(mFoulChargedCycle, 0);
	}

	if (mCatchBan > 0) {
		mCatchBan -= delay_add;
		mCatchBan = Max(mCatchBan, 0);
	}

	if (mArmPoint.mValue.mExpireBan > 0) {
		mArmPoint.mValue.mExpireBan -= delay_add;
		mArmPoint.mValue.mExpireBan = Max(mArmPoint.mValue.mExpireBan, 0);
	}

	if (mArmPoint.mValue.mMovableBan > 0) {
		mArmPoint.mValue.mMovableBan -= delay_add;
		mArmPoint.mValue.mMovableBan = Max(mArmPoint.mValue.mMovableBan, 0);
	}

	if (GetPosDelay() > delay_add) {
		mIsBodyDirMayChanged = true;
	}
}

inline void PlayerState::UpdatePlayerType(int type)
{
	if (mPlayerType != type) {
		mPlayerType = type;

		SetDecay(PlayerParam::instance().HeteroPlayer(mPlayerType).playerDecay());
		SetEffectiveSpeedMax(PlayerParam::instance().HeteroPlayer(mPlayerType).effectiveSpeedMax());
	}
}

inline void PlayerState::UpdateViewWidth(ViewWidth width)
{
	mViewWidth = width;
}

inline void PlayerState::UpdateUnum(Unum num)
{
	mUnum = num;
}

inline void PlayerState::SetIsAlive(bool alive)
{
	mIsAlive = alive;

	if (!mIsAlive) {
		UpdatePos(GetPos(), GetPosDelay(), 0.0);
		UpdateVel(GetVel(), GetVelDelay(), 0.0);

		mNeckDir.mConf = 0.0;
		mBodyDir.mConf = 0.0;
	}
}

#endif
