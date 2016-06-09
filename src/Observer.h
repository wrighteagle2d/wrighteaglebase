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

#ifndef OBSERVER_H_
#define OBSERVER_H_

#include "Thread.h"
#include "Geometry.h"
#include "ServerParam.h"
#include "BallState.h"
#include "PlayerState.h"
#include "Parser.h"
#include <vector>

class HearMessage;

template < class T >
class ObserverRecord {
	T    mValue;
	Time mTime;

public:
	ObserverRecord():
		mValue(T()),
		mTime(Time(-1, 0))

	{
	}

	const T & value() const { return mValue; }
	const Time & time() const { return mTime; }

	void SetValue(const T & value) { mValue = value; }
	void SetTime(const Time & time) { mTime = time; }
	void Set(const T & value, const Time & time) { SetValue(value); SetTime(time); }
};

//======================================================================================================================
/**
* 基本的视觉观察类
*/
class SightObserver {
	ObserverRecord<double>   mDist;						///相对距离
	ObserverRecord<AngleDeg> mDir;						///相对自己头部的方向 -- 头部角度指全局的脖子角度
	ObserverRecord<double>   mDistChg;					///相对距离的变化量 -- 径向速度
	ObserverRecord<AngleDeg> mDirChg;					///相对角度的变化 --- 法向速度

public:
	const double & Dist() const { return mDist.value(); }
	const AngleDeg & Dir() const { return mDir.value(); }
	const double & DistChg() const { return mDistChg.value(); }
	const AngleDeg & DirChg() const { return mDirChg.value(); }

	const ObserverRecord<double>& GetDist() const { return mDist;}
	const ObserverRecord<AngleDeg>& GetDir() const { return mDir;}
	const ObserverRecord<double>& GetDistChg() const { return mDistChg;}
	const ObserverRecord<AngleDeg>& GetDirChg() const { return mDirChg;}

	void SetDist(double dist, const Time & time) { mDist.Set(dist, time); }
	void SetDir(AngleDeg dir, const Time & time) { mDir.Set(dir, time); }
	void SetDistChg(double dist_chg, const Time & time) { mDistChg.Set(dist_chg, time); }
	void SetDirChg(AngleDeg dir_chg, const Time & time) { mDirChg.Set(dir_chg, time); }
};

//======================================================================================================================
class BallObserver: public SightObserver {
};

//======================================================================================================================
class FlagObserver: public SightObserver {
	Vector mGlobalPosition;

public:
	const Vector & GlobalPosition() const{ return mGlobalPosition; }
	void SetGlobalPosition(const Vector & global_position) { mGlobalPosition = global_position; }
};

//======================================================================================================================
class MarkerObserver: public FlagObserver {
	MarkerType mType;											///marker的类型
	void SetType(MarkerType type) { mType = type; }

public:
	MarkerObserver() { mType = FLAG_NONE; }

	const MarkerType & Type() const { return mType; }
	void Initialize(MarkerType type, const Vector & global_pos, bool rotate)
	{
		SetGlobalPosition(rotate? -global_pos: global_pos);  /* If on the right side of the field, flip all coords */
		SetType(type);
	}
};

//======================================================================================================================
class LineObserver: public FlagObserver {
	SideLineType mType;										 		///line的类型
	void SetType(SideLineType type) { mType = type; }
	const double & DistChg();  // not used
	const AngleDeg & DirChg(); // not used

public:
	LineObserver() { mType = SL_NONE; }

	const SideLineType & Type() const { return mType; }
	void Initialize(SideLineType type, const Vector & global_pos, bool rotate)
	{
		SetGlobalPosition(rotate? -global_pos: global_pos);  /* If on the right side of the field, flip all coords */
		SetType(type);
	}
};

//======================================================================================================================
class PlayerObserver: public SightObserver {
	char mSide;
	int  mUnum;
	ObserverRecord<AngleDeg> mBodyDir;					///对方身体相对自己头部的角度
	ObserverRecord<AngleDeg> mHeadDir;					///对方头部相对自己头部的角度
	ObserverRecord<bool>     mIsTackling;				///标记是否在铲球
	ObserverRecord<bool>     mIsKicked;				    ///标记是否在上周期踢过球
	ObserverRecord<bool>     mIsPointing;				///标记是否在指
	ObserverRecord<bool>     mIsLying;					///标记是否被铲倒
	CardType				 mCardType;					///标记犯规记录
	AngleDeg                 mPointDir;					///相对头部的指向角度

public:
	PlayerObserver() { mSide = '?'; mUnum = 0; }

	const char & Side() const { return mSide; }
	const int & Unum() const { return mUnum; }
	const AngleDeg & BodyDir() const { return mBodyDir.value(); }
	const AngleDeg & HeadDir() const { return mHeadDir.value(); }
	const bool & IsTackling() const { return mIsTackling.value(); }
	const bool & IsKicked() const { return mIsKicked.value(); }
	const bool & IsLying()  const { return mIsLying.value(); }
	const bool & IsPointing() const { return mIsPointing.value(); }
	const AngleDeg & PointDir() const { return mPointDir; }
	const CardType & GetCardType() const { return mCardType; }

	const ObserverRecord<AngleDeg> & GetBodyDir() const { return mBodyDir; }
	const ObserverRecord<AngleDeg> & GetHeadDir() const { return mHeadDir; }
	const ObserverRecord<bool>     & GetIsTackling() const { return mIsTackling; }
	const ObserverRecord<bool>     & GetIsKicked() const { return mIsKicked; }
	const ObserverRecord<bool>     & GetIsLying() const { return mIsLying; }
	const ObserverRecord<bool>     & GetIsPointing() const { return mIsPointing; }

	void SetSide(char side) { mSide = side; }
	void SetUnum(int unum) {mUnum = unum; }
	void SetBodyDir(AngleDeg body_dir, const Time & time) { mBodyDir.Set(body_dir, time); }
	void SetHeadDir(AngleDeg head_dir, const Time & time) { mHeadDir.Set(head_dir, time); }
	void SetIsTcakling(bool is_tackling, const Time & time) { mIsTackling.Set(is_tackling, time); }
	void SetIsKicked(bool is_kicked, const Time & time) { mIsKicked.Set(is_kicked, time); }
	void SetIsLying(bool is_lying, const Time & time) { mIsLying.Set(is_lying, time); }
	void SetCardType(CardType card_type) { mCardType = card_type; }
	void SetIsPointing(bool is_pointing, const Time & time) { mIsPointing.Set(is_pointing, time); }
	void SetPointDir(AngleDeg point_dir) { mPointDir = point_dir; }
};

//======================================================================================================================
/**
* 未能识别出unum和side的球员
*/
//============================use the server's bug  add by oldtai==========================
class ServerBugInfo
{
public:
    char mSide; //the most probable side;
    Unum mLeastNum; //the least num;
	char mSupSide;
	Unum mSupNum;

    ServerBugInfo()
    {
	    mSide = 'l'; //server send left player as default
	    mLeastNum = 1; //the default least num is 1;
		mSupSide = 'r';
		mSupNum = TEAMSIZE;//THE DEFUALT 
	}
};

//======================================================================================================================
/**
* 未能识别出的球员
*/
class UnknownPlayerObserver: public SightObserver {
	bool mIsKnownSide;			//tell whether it know the side
	char mSide;
	ObserverRecord<bool>     mIsTackling;				///标记是否在铲球
	ObserverRecord<bool>     mIsKicked;				///标记是否在铲球
	ObserverRecord<bool>     mIsLying;
	const double & DistChg();  // not used
	const AngleDeg & DirChg(); // not used
	CardType mCardType;

public:
	UnknownPlayerObserver() { mSide = '?'; mIsKnownSide = false;}

	bool  IsKnownSide() const { return mIsKnownSide;}
	const char & Side() const { return mSide; }
	const bool & IsTackling() const { return mIsTackling.value(); }
	const bool & IsKicked() const { return mIsKicked.value(); }
	const bool & IsLying() const { return mIsLying.value(); }
	const ObserverRecord<bool> & GetIsTackling() const { return mIsTackling; }
	const ObserverRecord<bool> & GetIsKicked() const { return mIsKicked; }
	const ObserverRecord<bool> & GetIsLying() const { return mIsLying; }

	void SetSide(char side) { mSide = side; }
	void SetIsTcakling(bool is_tackling, const Time & time) { mIsTackling.Set(is_tackling, time); }
	void SetIsKicked(bool is_kicked, const Time & time) { mIsKicked.Set(is_kicked, time); }
	void SetIsLying(bool is_lying, const Time & time) { mIsKicked.Set(is_lying, time); }
	void SetCardType(CardType card_type) { mCardType = card_type; }
	void SetIsKnownSide(bool known) { mIsKnownSide = known;}
};

//======================================================================================================================
/**
* 球员自己的sense观察类
*/
class SenseObserver {
	ViewWidth mViewWidth; //同步模式下忽略 ViewQuality

	double    mStamina;
	double    mEffort;
	double    mCapacity;

	double    mSpeed;
	AngleDeg  mSpeedDir;
	AngleDeg  mNeckDir;

	//counter
	int       mKickCount;
	int       mDashCount;
	int       mTurnCount;
	int       mSayCount;
	int       mTurnNeckCount;
	int       mCatchCount;
	int       mMoveCount;
	int       mChangeViewCount;

	//arm
	int       mArmMovableBan;
	int       mArmExpires;
	double    mArmTargetDist;
	double    mArmTargetDir;
	int       mArmCount;

	//focus
	char      mFocusSide;
	int       mFocusUnum;
	int       mFocusCount;

	//tackle
	int       mTackleExpires;
	int       mTackleCount;

	//foul record
	CardType mCardType;

	//foul charged cycle
	int mFoulChargedCycle; //被铲倒的周期

	//collision
	bool mIsCollideWithPost;
	bool mIsCollideWithPlayer;
	bool mIsCollideWithBall;

	Time mSenseTime;		//感知信息到达的周期
public:
	SenseObserver()
	{
		mViewWidth = VW_None;

		mStamina = ServerParam::instance().staminaMax();
		mEffort = ServerParam::instance().effortInit();
		mCapacity = ServerParam::instance().staminaCapacity();

		mSpeed = 0.0;
		mSpeedDir = 0.0;
		mNeckDir = 0.0;

		mKickCount = 0;
		mDashCount = 0;
		mTurnCount = 0;
		mSayCount = 0;
		mTurnNeckCount = 0;
		mCatchCount = 0;
		mMoveCount = 0;
		mChangeViewCount = 0;

		mArmMovableBan = 0;
		mArmExpires = 0;
		mArmTargetDir = 0.0;
		mArmTargetDist = 0.0;
		mArmCount = 0;

		mFocusSide = '?';
		mFocusUnum = 0;
		mFocusCount = 0;

		mTackleExpires = 0;
		mTackleCount = 0;

		mCardType = CR_None;
		mFoulChargedCycle = 0;
		ClearCollisionState();
	}

	const ViewWidth & GetViewWidth() const { return mViewWidth; }
	const double & GetStamina() const { return mStamina; }
	const double & GetEffort() const { return mEffort; }
	const double & GetCapacity() const { return mCapacity; }
	const double & GetSpeed() const { return mSpeed; }
	const AngleDeg & GetSpeedDir() const { return mSpeedDir; }
	const AngleDeg & GetNeckDir() const { return mNeckDir; }

	const int & GetKickCount() const { return mKickCount; }
	const int & GetDashCount() const { return mDashCount; }
	const int & GetTurnCount() const { return mTurnCount; }
	const int & GetSayCount() const { return mSayCount; }
	const int & GetTurnNeckCount() const { return mTurnNeckCount; }
	const int & GetCatchCount() const { return mCatchCount; }
	const int & GetMoveCount() const { return mMoveCount; }
	const int & GetChangeViewCount() const { return mChangeViewCount; }

	const int & GetArmMovableBan() const { return mArmMovableBan; }
	const int & GetArmExpires() const { return mArmExpires; }
	const double & GetArmTargetDist() const { return mArmTargetDist; }
	const double & GetArmTargetDir() const { return mArmTargetDir; }
	const int & GetArmCount() const { return mArmCount; }

	const char & GetFocusSide() const { return mFocusSide; }
	const int & GetFocusUnum() const { return mFocusUnum; }
	const int & GetFocusCount() const { return mFocusCount; }

	const int & GetTackleExpires() const { return mTackleExpires; }
	const int & GetTackleCount() const { return mTackleCount; }

	const CardType & GetCardType() const { return mCardType; }
	const int & GetFoulChargedCycle() const { return mFoulChargedCycle; }

	bool IsCollideNone() const { return (mIsCollideWithPost || mIsCollideWithPlayer || mIsCollideWithBall) == false; }
	bool IsCollideWithPost() const { return mIsCollideWithPost; }
	bool IsCollideWithPlayer() const { return mIsCollideWithPlayer; }
	bool IsCollideWithBall() const { return mIsCollideWithBall; }

	const Time & GetSenseTime() const { return mSenseTime; }

	void SetViewWidth(ViewWidth view_width) { mViewWidth = view_width; }
	void SetStamina(double stamina) { mStamina = stamina; }
	void SetEffort(double effort) { mEffort = effort; }
	void SetCapacity(double capacity) { mCapacity = capacity; }
	void SetSpeed(double speed) { mSpeed = speed; }
	void SetSpeedDir(AngleDeg speed_dir) { mSpeedDir = speed_dir; }
	void SetNeckDir(AngleDeg neck_dir) { mNeckDir = neck_dir; }

	void SetKickCount(int kick_count) { mKickCount = kick_count; }
	void SetDashCount(int dash_count) { mDashCount = dash_count; }
	void SetTurnCount(int turn_count) { mTurnCount = turn_count; }
	void SetSayCount(int say_count) { mSayCount = say_count; }
	void SetTurnNeckCount(int turn_neck_count) { mTurnNeckCount = turn_neck_count; }
	void SetCatchCount(int catch_count) { mCatchCount = catch_count; }
	void SetMoveCount(int move_count) { mMoveCount = move_count; }
	void SetChangeViewCount(int change_view) { mChangeViewCount = change_view; }

	void SetArmMovableBan(int arm_movable_ban) { mArmMovableBan = arm_movable_ban; }
	void SetArmExpires(int arm_expires) { mArmExpires = arm_expires; }
	void SetArmTargetDist(double arm_target_dist) { mArmTargetDist = arm_target_dist; }
	void SetArmTargetDir(AngleDeg arm_target_dir) { mArmTargetDir = arm_target_dir; }
	void SetArmCount(int arm_count) { mArmCount = arm_count; }

	void SetFocusSide(char focus_side) { mFocusSide = focus_side; }
	void SetFocusUnum(Unum unum) { mFocusUnum = unum; }
	void SetFocusCount(int focus_count) { mFocusCount = focus_count; }

	void SetTackleExpires(int tackle_expires) { mTackleExpires = tackle_expires; }
	void SetTackleCount(int tackle_count) { mTackleCount = tackle_count; }

	void SetCardType( CardType card_type ) { mCardType = card_type; }
	void SetFoulChargedCycle( int x ) { mFoulChargedCycle = x; }
	void ClearCollisionState() { mIsCollideWithPost = false; mIsCollideWithPlayer = false; mIsCollideWithBall = false; }
	void SetCollideWithPost() { mIsCollideWithPost = true; }
	void SetCollideWithPlayer() { mIsCollideWithPlayer = true; }
	void SetCollideWithBall() { mIsCollideWithBall = true; }

	void SetSenseTime(Time sense_time) { mSenseTime = sense_time; }
};

//======================================================================================================================
class AudioObserver {
public:
	enum HearInfoType {
		HearInfo_Null = 0,
		HearInfo_Ball = 1,
		HearInfo_Teammate = 2,
		HearInfo_Opponent = 4
	};

	class AudioPlayer
	{
	public:
		AudioPlayer(): mUnum(-1)
		{
		}

		AudioPlayer(const Vector & pos , Unum unum): mPos(pos), mUnum(unum)
		{
		}

		Vector mPos;
		Unum   mUnum;
	};

public:
    const bool & IsOurCoachSayValid() const { return mIsOurCoachSayValid; }
    const std::vector<std::string> & GetOurCoachSayContent() const { return mOurCoachSayContent; }
    const bool & IsTeammateSayValid() const { return mIsTeammateSayValid; }
    const std::string & GetTeammateSayContent() const { return mTeammateSayContent; }

    void SetOurCoachSayValid(const bool valid) { if (!(mIsOurCoachSayValid = valid)) mOurCoachSayContent.clear(); }
    void SetOurCoachSayConent(const std::string & content) { mOurCoachSayContent.push_back(content); }
    void SetTeammateSayValid(const bool valid) { mIsTeammateSayValid = valid; }
    void SetTeammateSayConent(const std::string & content) { mTeammateSayContent = content; }

private:
    bool mIsOurCoachSayValid;           // 教练说话当前是否合法
    std::vector<std::string> mOurCoachSayContent;    // 教练说话内容
    bool mIsTeammateSayValid;           // 队友说话当前是否合法
    std::string mTeammateSayContent;	// 队友说话内容

public:
    const int & GetOpponentType(const Unum & unum) const { return mOpponentType[unum]; }

    void SetOpponentType(const Unum & unum, const int & type) { mOpponentType[unum] = type; }

private:

    PlayerArray<int> mOpponentType; // 听到教练说的对手类型


private:
	AngleDeg    mHearDir;	//声源方向
	Unum        mHearUnum;		//说话的队员号码

	//下面的量记录听觉信息解析后的信息内容
	ObserverRecord<Vector> mBallPos;
	ObserverRecord<Vector> mBallVel;

	Array<ObserverRecord<AudioPlayer>, TEAMSIZE> mTeammate;
	Array<ObserverRecord<AudioPlayer>, TEAMSIZE> mOpponent;

	int mTeammateCount;
	int mOpponentCount;
	int mHearInfoType; //记录最后一条听到的信息类型

public:
	AudioObserver() {
        mIsOurCoachSayValid = false;
        mIsTeammateSayValid = false;
		mHearDir = 0.0;
		mHearUnum = 0;
		mHearInfoType = HearInfo_Null;
		mTeammateCount = mOpponentCount = 0;
		for (int i = 1; i <= TEAMSIZE; ++i)
		{
			mOpponentType[i] = -1;
		}
	}
	const AngleDeg & GetHearDir() const { return mHearDir; }
	const Unum & GetHearUnum() const { return mHearUnum; }

	void SetHearDir(AngleDeg hear_dir) { mHearDir = hear_dir; }
	void SetHearUnum(int hear_unum) { mHearUnum = hear_unum; }

	int GetHearInfoType() const { return mHearInfoType; }

	void SetBall(const Vector & pos, const Vector & vel, const Time & time) { mBallPos.Set(pos, time); mBallVel.Set(vel, time); mHearInfoType |= HearInfo_Ball; }
	void SetBall(const Vector & pos, const Time & time) { mBallPos.Set(pos, time); mHearInfoType |= HearInfo_Ball; }
	void SetTeammate(Unum num, const Vector & pos, const Time & time) { mTeammate[mTeammateCount].Set(AudioPlayer(pos , num), time); mTeammateCount++; mHearInfoType |= HearInfo_Teammate;}
	void SetOpponent(Unum num, const Vector & pos, const Time & time) { mOpponent[mOpponentCount].Set(AudioPlayer(pos , num) ,time); mOpponentCount++; mHearInfoType |= HearInfo_Opponent;}

	const ObserverRecord<Vector> & GetBallPos() const { return mBallPos; }
	const ObserverRecord<Vector> & GetBallVel() const { return mBallVel; }
	const ObserverRecord<AudioPlayer> & GetTeammate(int i) const { return mTeammate[i]; }
	const ObserverRecord<AudioPlayer> & GetOpponent(int i) const { return mOpponent[i]; }
	int GetTeammateCount() const{ return mTeammateCount;}
	int GetOpponentCount() const{ return mOpponentCount;}

	//初始数组大小设为零
	void Reset()
	{
		mTeammateCount = mOpponentCount = 0;
		mHearInfoType = HearInfo_Null;
	}
};

//======================================================================================================================
/**
* 观察类，存放从 Parser 里得到的第一手数据，其他数据都从此计算得到
*/
class Observer {
public:
	Observer();
	virtual ~Observer();

	void Reset(); //每周期只运行一次
	void ResetSight(); //重新初始化视觉信息,主要是为了身份识别的覆盖,暂时为权宜,近期可能会对此bug改进

	//提供给 Parser 的接口
    void SetOurInitSide(char side) { mOurInitSide = side; }
	void SetOurSide(char side) { mOurSide = side; mOppSide = (mOurSide == 'l'? 'r': 'l'); }
	void SetSelfUnum(char unum) { mSelfUnum = unum; }
	void SetCurrentTime(const Time & time) { mCurrentTime = time;	}
	void SetPlayMode(PlayMode play_mode) { mPlayMode = play_mode; }
	void SetKickOffMode(KickOffMode kickoffmode) {mKickOffMode = kickoffmode;}
	void SetServerPlayMode(ServerPlayMode server_play_mode) { mServerPlayMode = server_play_mode; }
	void Initialize();	//这要在知道自己的side后，才能初始化
	void SetTeammateType(Unum unum, int type) { mTeammateType[unum] = type; }
	void SetOpponentType(Unum unum, int type) { mOpponentType[unum] = type; }
	void SetTeammateCardType(Unum unum, CardType type) { mTeammateCardType[unum] = type; }
	void SetOpponentCardType(Unum unum, CardType type) { mOpponentCardType[unum] = type; }
	void AddOpponentTypeChangedCount(Unum player) { mOpponentTypeChangedCount[player] ++; };

	void SetSenseBody(
			ViewWidth view_width,
			double stamina,
			double effort,
			double capacity,

			double speed,
			AngleDeg speed_dir,
			AngleDeg neck_dir,

			int kicks,
			int dashes,
			int turns,
			int says,
			int turn_necks,
			int catchs,
			int moves,
			int change_views,

			int arm_movable_ban,
			int arm_expires,
			double arm_target_dist,
			AngleDeg arm_target_dir,
			int arm_count,

			char focus_side,
			Unum focus_unum,
			int  focus_count,

			int tackle_expires,
			int tackle_count,

			int foul_charged_cycle,
			CardType card_type,

			Time sense_time
	);

	void SetSensePartialBody(
			ViewWidth view_width,

			int kicks,
			int dashes,
			int turns,
			int says,
			int turn_necks,
			int catchs,
			int moves,
			int change_views,

			int arm_movable_ban,
			int arm_expires,
			double arm_target_dist,
			AngleDeg arm_target_dir,
			int arm_count,

			Time sense_time
	);

    // coach没有sense信息，设置只能通过ok信息
    const int & GetCoachSayCount() const { return mSenseObserver.GetSayCount(); }
    void SetCoachSayCount(const int count) { mSenseObserver.SetSayCount(count); }

	void ClearCollisionState() { mSenseObserver.ClearCollisionState(); }
	void SetCollideWithPost() { mSenseObserver.SetCollideWithPost(); }
	void SetCollideWithPlayer() { mSenseObserver.SetCollideWithPlayer(); }
	void SetCollideWithBall() { mSenseObserver.SetCollideWithBall(); }

    void ResetOurCoachSayValid() { mAudioObserver.SetOurCoachSayValid(false); }
    void ResetTeammateSayValid() { mAudioObserver.SetTeammateSayValid(false); }
	void HearOurCoachSay(const std::string & hear_content);
	void HearTeammateSay(AngleDeg hear_dir, Unum hear_unum, const std::string & hear_content);

	//同步模式下只有高质量的视觉，所以不存在只看到角度看不到距离的情况
	//看到的时间，都默认 mCurrentTime
	void SeeLine  (SideLineType line, double dist, double dir);

	void SeeMarker(MarkerType marker, double dist, double dir);
	void SeeMarker(MarkerType marker, double dist, double dir, double dist_chg, double dir_chg);

	void SeeBall(double dist, double dir);
	void SeeBall(double dist, double dir, double dist_chg, double dir_chg);

	void SeePlayer(double dist, double dir);
	void SeePlayer(char side, double dist, double dir, bool is_tackling, bool is_kicked, bool is_lying, CardType card_type);
	void SeePlayer(char side, int num, double dist, double dir, double dist_chg, double dir_chg, double body_dir,
			double head_dir, bool is_pointing, double point_dir, bool is_tackling, bool is_kicked, bool is_lying, CardType card_type);
	void SeePlayer(char side, int num, double dist, double dir, bool is_pointing, double point_dir, bool is_tackling, bool is_kicked, bool is_lying, CardType card_type);

	void SeeOppGoalie(int unum) { Assert(unum >= 1 && unum <= 11); mOppGoalieUnum = unum; }

	void HearBall(const Vector & pos, const Vector & vel);
	void HearBall(const Vector & pos);
	void HearTeammate(Unum num, const Vector & pos);
	void HearOpponent(Unum num, const Vector & pos);
    void HearOpponentType(const Unum & unum, const int & type) { mAudioObserver.SetOpponentType(unum, type); }

    void HearOppGoalie(int unum) { Assert(unum >= 1 && unum <= 11); mOppGoalieUnum = unum; }
	//提供给 WorldModel 的接口
	const Unum & SelfUnum() const { return mSelfUnum; }
    const char & OurInitSide() const { return mOurInitSide; }
	const char & OurSide() const { return mOurSide; }
	const char & OppSide() const { return mOppSide; }

	const Time & CurrentTime() const { return mCurrentTime; }
	const Time & LatestSightTime() const { return mLatestSightTime; }
	void SetLatestSightTime(const Time & time) { mLatestSightTime = time; }

	const PlayMode & GetPlayMode() const { return mPlayMode; }
	const KickOffMode & GetKickOffMode() const {return mKickOffMode;}
	const ServerPlayMode & GetServerPlayMode() const { return mServerPlayMode; }
	const int  & OppGoalieUnum() const { return mOppGoalieUnum; }
	const int  & OurScore() const { return mOurScore; }
	void OurScoreInc() { ++mOurScore; }
	const int  & OppScore() const { return mOppScore; }
	void OppScoreInc() { ++mOppScore; }
	const MarkerObserver & Marker(MarkerType type) const { return mMarkerObservers[type]; }
	const LineObserver & SideLine(SideLineType type) const { return mLineObservers[type]; }
	const PlayerObserver & Teammate(int num) const { return mTeammateObservers[num]; }
	const PlayerObserver & Opponent(int num) const { return mOpponentObservers[num]; }
	const BallObserver & Ball() const { return mBallObserver; }
	const int & GetTeammateType(Unum unum) { return mTeammateType[unum]; }
	const int & GetOpponentType(Unum unum) { return mOpponentType[unum]; }
	const CardType & GetTeammateCardType(Unum unum) { return mTeammateCardType[unum]; }
	const CardType & GetOpponentCardType(Unum unum) { return mOpponentCardType[unum]; }
	const SenseObserver & Sense() const { return mSenseObserver; }
	const AudioObserver & Audio() const { return mAudioObserver; }
	const UnknownPlayerObserver& UnknownPlayer( int num) { num = num % (TEAMSIZE * 2) ; return mUnknownPlayers[num];}
	int   GetUnknownPlayerCount() { return mUnknownPlayerCount;}

	const ServerBugInfo & UnknownPlayerBugInfo(int num)
	{
		Assert(num >= 0 && num < TEAMSIZE * 2);
		return mUnknownPlayersBugInfo[num];
	}

	bool IsBallDropped() const { return mIsBallDropped; }
	void DropBall() { mIsBallDropped = true; }
	bool IsNewOppType() {return mIsNewOppType;}
	void SetNewOppType(bool NewOppType) {mIsNewOppType = NewOppType; }

    bool mReceiveFullstateMsg; //标记是否接受到了 fullstate 信息

private:
	Time mCurrentTime;							//当前时间
	Time mLatestSightTime;                      //收到最新视觉的时间，在等漏视觉的情况下，比mCurrentTime小

	PlayMode mPlayMode;
	KickOffMode mKickOffMode;
	ServerPlayMode mServerPlayMode; //来自server的playmode，sight_log要用
	char mOurInitSide; // 记录初始side，永远不会变，而mOurSide在点球大战时有可能变
    char mOurSide;
	char mOppSide;
	int  mSelfUnum;
	int  mOppGoalieUnum;
	int  mOurScore;
	int  mOppScore;

	PlayerArray<int, true> mTeammateType;//队友的球员类型 －－ 从server获得
	PlayerArray<int, true> mOpponentType;//对手的球员类型 －－ 从coach获得
	PlayerArray<int, true> mOpponentTypeChangedCount;//对手的球员类型改变的次数
	PlayerArray<CardType, true> mTeammateCardType;//队友犯规记录
	PlayerArray<CardType, true> mOpponentCardType;//对手犯规记录 均从server获得

	bool mIsBallDropped; //球是否被drop
	bool mIsNewOppType;  //教练是否发来新的对手异构

	SenseObserver  mSenseObserver;
	AudioObserver  mAudioObserver;
	Array<MarkerObserver, FLAG_MAX> mMarkerObservers;
	BallObserver   mBallObserver;
	PlayerArray<PlayerObserver> mTeammateObservers;
	PlayerArray<PlayerObserver> mOpponentObservers;
	Array<LineObserver, SL_MAX> mLineObservers;
	Array<UnknownPlayerObserver, TEAMSIZE * 2> mUnknownPlayers;
	int mUnknownPlayerCount;							//每个周期更新时都要设为0

	//for using server bug
	Array<ServerBugInfo, TEAMSIZE * 2> mUnknownPlayersBugInfo;
	ServerBugInfo             mCurrentBugInfo;                              //current record info;
	int mBugInfoRanged;
	int mLastLeftPlayer;
	int mFirstRightPlayer;


	static const int MAX_UNKNOWN_PLAYES = TEAMSIZE * 2;

	void InitializeFlags(bool rotation);
	void InitializePlayers();

	void AdjustUnum(char side , Unum unum);

public:
	/*
	 * synch with parser thread
	 */
	bool WaitForNewInfo();

	bool WaitForNewSense();
	bool WaitForNewSight();
	bool WaitForNewThink();
	bool WaitForCommandSend();
	bool WaitForCoachNewHear(); // coach用来等待hear信息
	void SetNewSense();
	void SetNewSight();
	void SetNewThink();
	void SetCommandSend();
	void Lock() { mUpdateMutex.Lock(); };
	void UnLock() { mUpdateMutex.UnLock(); }
	bool IsNewSight() const { return mIsNewSight;}

	RealTime GetLastCycleBeginRealTime() const      { return mLastCycleBeginRealTime; }
	void SetLastCycleBeginRealTime(RealTime time)   { mLastCycleBeginRealTime = time; }
	RealTime GetLastSightRealTime() const           { return mLastSightRealTime; }
	void SetLastSightRealTime(RealTime time)        { mLastSightRealTime = time; }

	bool WillBeNewSight(); /** whether there will be the sight msg in this cycle */

	void SetPlanned() { mIsPlanned = true; }
	bool IsPlanned() const { return mIsPlanned; }

private:
	RealTime        mLastCycleBeginRealTime;    /** last cycle begin time */
	RealTime        mLastSightRealTime;         /** last sight msg time */

    bool            mIsBeginDecision;
	bool            mIsNewSense;
	bool            mIsNewSight; //判断是否有视觉 add by tai （9/7/08)
	bool            mIsNewThink;
	bool            mIsCommandSend;
	bool            mIsPlanned;  //本周期已经决策过了
	bool            mIsNewHear;
	bool            mSenseArrived; //有sense信息到来
	bool            mSightArrived;
	bool            mThinkArrived;
	ThreadCondition mCondNewSense;
	ThreadCondition mCondNewSight;
	ThreadCondition mCondNewThink;
	ThreadCondition mCondCommandSend;
	ThreadCondition mCondCoachNewHear; //只有当自己是coach时才用这个量

	ThreadMutex     mUpdateMutex; //更新时与parser互斥

private:
	//从coach的视觉信息更新得到的 或 coach发过来的worldstate 或 fullstate信息
	BallState mBall_Fullstate;
	PlayerArray<PlayerState> mTeammate_Fullstate;
	PlayerArray<PlayerState> mOpponent_Fullstate;

public:
//	const BallState & Ball_Fullstate() const { return mBall_Fullstate; }
//	const PlayerState & Teammate_Fullstate(Unum num) const { return mTeammate_Fullstate[num]; }
//	const PlayerState & Opponent_Fullstate(Unum num) const { return mOpponent_Fullstate[num]; }

	BallState & Ball_Fullstate() { return mBall_Fullstate; }
	PlayerState & Teammate_Fullstate(Unum num) { return mTeammate_Fullstate[num]; }
	PlayerState & Opponent_Fullstate(Unum num) { return mOpponent_Fullstate[num]; }


public:
	Time GetBallKickTime() const { return mBallKickTime; }
	const Vector & GetBallPosByKick() const { return mBallPosByKick; }
	const Vector & GetBallVelByKick() const { return mBallVelByKick; }
	void SetBallKickTime(Time ball_kick_time) { mBallKickTime = ball_kick_time; }
	void SetBallPosByKick(const Vector & ball_pos) { mBallPosByKick = ball_pos; }
	void SetBallVelByKick(const Vector & ball_vel) { mBallVelByKick = ball_vel; }

	Time GetPlayerDashTime() const { return mPlayerDashTime; }
	const Vector & GetPlayerPosByDash() const { return mPlayerPosByDash; }
	const Vector & GetPlayerVelByDash() const { return mPlayerVelByDash; }
	void SetPlayerDashTime(Time player_dash_time) { mPlayerDashTime = player_dash_time; }
	void SetPlayerPosByDash(const Vector & player_pos) { mPlayerPosByDash = player_pos; }
	void SetPlayerVelByDash(const Vector & player_vel) { mPlayerVelByDash = player_vel; }

	Time GetPlayerMoveTime() const { return mPlayerMoveTime; }
	const Vector & GetPlayerPosByMove() const { return mPlayerPosByMove; }
	const Vector & GetPlayerVelByMove() const { return mPlayerVelByMove; }
	void SetPlayerMoveTime(Time player_move_time) { mPlayerMoveTime = player_move_time; }
	void SetPlayerPosByMove(const Vector & player_pos) { mPlayerPosByMove = player_pos; }
	void SetPlayerVelByMove(const Vector & player_vel) { mPlayerVelByMove = player_vel; }

	Time GetPlayerTurnTime() const { return mPlayerTurnTime; }
	AngleDeg GetPlayerBodyDirByTurn() const { return mPlayerBodyDirByTurn; }
	void SetPlayerTurnTime(Time player_turn_time) { mPlayerTurnTime = player_turn_time; }
	void SetPlayerBodyDirByTurn(AngleDeg body_dir) { mPlayerBodyDirByTurn = body_dir; }

	Time GetPlayerTurnNeckTime() const { return mPlayerTurnNeckTime; }
	AngleDeg GetPlayerNeckDirByTurnNeck() const { return mPlayerNeckDirByTurnNeck; }
	void SetPlayerTurnNeckTime(Time player_turn_neck_time) { mPlayerTurnNeckTime = player_turn_neck_time; }
	void SetPlayerNeckDirByTurnNeck(AngleDeg neck_dir) { mPlayerNeckDirByTurnNeck = neck_dir; }

private:
	Time    mBallKickTime;
	Vector  mBallPosByKick;
	Vector  mBallVelByKick;

	Time    mPlayerDashTime;
	Vector  mPlayerPosByDash;
	Vector  mPlayerVelByDash;

	Time    mPlayerMoveTime;
	Vector  mPlayerPosByMove;
	Vector  mPlayerVelByMove;

	Time        mPlayerTurnTime;
	AngleDeg    mPlayerBodyDirByTurn;

	Time        mPlayerTurnNeckTime;
	AngleDeg    mPlayerNeckDirByTurnNeck; // 脖子角度，相对量
};

#endif /* OBSERVER_H_ */
