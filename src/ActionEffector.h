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

#ifndef ACTIONEFFECTOR_H_
#define ACTIONEFFECTOR_H_

#include "Observer.h"
#include "BasicCommand.h"
#include "ServerParam.h"
#include "PlayerParam.h"
#include <list>
#include <deque>
#include <ostream>

class WorldState;
class PlayerState;
class BallState;

class Agent;


/**
 * 通过实际转身角度计算需要向server发送的turn命令的参数，即虚拟转身角度
 * Calculate angle which should be sent so server as parameter when doing turn action.
 * \param actual_turn_angle angle the player what to turn.
 * \param player_type hetero type of the player.
 * \param player_speed current speed of the player.
 * \return angle which should be sent so server as parameter.
 */
inline AngleDeg GetTurnMoment(const AngleDeg actual_turn_angle, const int player_type, const double player_speed)
{
	return (actual_turn_angle * (1.0 +
			PlayerParam::instance().HeteroPlayer(player_type).inertiaMoment() * player_speed));
}

/**
 * 通过实际转身角度计算需要向server发送的turn命令的参数，即虚拟转身角度
 * Calculate angle the player actually turns from the angle parameter sent to server.
 * \param virtual_turn_angle the angle parameter sent to server.
 * \param player_type hetero type of the player.
 * \param player_speed current speed of the player.
 * \return angle the player actually will turn.
 */
inline AngleDeg GetTurnAngle(const AngleDeg moment, const int player_type, const double player_speed)
{
	return (GetNormalizeMoment(moment) / (1.0 +
			PlayerParam::instance().HeteroPlayer(player_type).inertiaMoment() * player_speed));
}

/**
 * 得到当前状态下可以转身的最大角度
 * Get maximum angle the player can turn with certain state.
 * \param player_type hetero type of the player.
 * \param player_speed current speed of the player.
 * \return maximum angle the player can turn with certain state.
 */
inline AngleDeg GetMaxTurnAngle(const int player_type, const double player_speed)
{
	return (ServerParam::instance().maxMoment() / (1.0 +
			PlayerParam::instance().HeteroPlayer(player_type).inertiaMoment() * player_speed));
}

/**
 * 得到顺时针转脖子的最大角度，肯定为正值，参数的脖子角度是相对量
 * Get maximum angle the player can turn neck by clockwise.
 * \param player_head_angle current neck angle of the player, relative to body angle.
 * \return maximum angle the player can turn neck by.
 */
inline AngleDeg GetMaxTurnNeckAngle(const AngleDeg player_head_angle)
{
	return (ServerParam::instance().maxNeckAngle() - player_head_angle);
}

/**
 * 得到顺时针转脖子的最大角度，肯定为正值，参数的脖子角度是相对量
 * Get maximum angle the player can turn neck by counter-clockwise.
 * \param player_head_angle current neck angle of the player, relative to body angle.
 * \return maximum angle the player can turn neck by.
 */
inline AngleDeg GetMinTurnNeckAngle(const AngleDeg player_head_angle)
{
	return (ServerParam::instance().minNeckAngle() - player_head_angle);
}

/**
 * 得到kick_rate，注意这里的ball_2_player是球相对于球员的相对坐标，以球员身体方向为x正方向
 * Get kick_rate by relative position of the ball.
 * \param ball_2_player ball position in the player's coordinate system.
 * \param player_type hetero type of the player.
 * \return kick_rate.
 */
inline double GetKickRate(const Vector & ball_2_player, const int player_type)
{
	double dir_diff = fabs(ball_2_player.Dir());
	double dist_ball = ball_2_player.Mod() - PlayerParam::instance().HeteroPlayer(player_type).playerSize() - ServerParam::instance().ballSize();
	return PlayerParam::instance().HeteroPlayer(player_type).kickPowerRate() *
			(1.0 - 0.25 * dir_diff / 180.0 -
					0.25 * dist_ball / PlayerParam::instance().HeteroPlayer(player_type).kickableMargin());
}

/**
 * 得到kick的最大误差，ball_2_player和上面函数一样为相对，ball_vel为球的绝对速度
 * Get maximum random error by relative position and absolute velocity.
 * \param ball_2_player ball position in the player's coordinate system.
 * \param ball_vel velocity of the ball in field coordinate system.
 * \param player_type hetero type of the player.
 * \param kick_power power used by kick action.
 * \return maximum random error in this state.
 */
inline double GetMaxKickRand(const Vector & ball_2_player, const Vector & ball_vel, const int player_type, const double kick_power)
{
	double dir_diff = fabs(ball_2_player.Dir());
	double dist_ball = ball_2_player.Mod() - PlayerParam::instance().HeteroPlayer(player_type).playerSize() - ServerParam::instance().ballSize();

	double pos_rate = 0.5 + 0.25 * (dir_diff / 180.0 +
			dist_ball / PlayerParam::instance().HeteroPlayer(player_type).kickableMargin());

	double speed_rate = 0.5 + 0.5 * (ball_vel.Mod() /
			(ServerParam::instance().ballSpeedMax() * ServerParam::instance().ballDecay()));

	return (PlayerParam::instance().HeteroPlayer(player_type).kickRand() *
			(kick_power / ServerParam::instance().maxPower()) *
			(pos_rate + speed_rate));
}

/**
 * 得到tackle的概率，注意这里的ball_2_player是球相对于球员的相对坐标，以球员身体方向为x正方向
 * Get tackle success probability.
 * \param ball_2_player ball position in the player's coordinate system.
 * \return tackle success probability.
 */
inline double GetTackleProb(const Vector & ball_2_player, const bool foul)
{
	double tackle_dist = (ball_2_player.X() > 0.06 /*this is buffer*/ ?
        ServerParam::instance().tackleDist() :
        ServerParam::instance().tackleBackDist());

	if (fabs(tackle_dist) < FLOAT_EPS) {
		return 0.0;
	}
	else {
		double dx = fabs(ball_2_player.X()) / tackle_dist;
		double dy = fabs(ball_2_player.Y()) / ServerParam::instance().tackleWidth();

		if (dx > 1.0 || dy > 1.0 || ball_2_player.Mod() > ServerParam::instance().maxTackleArea()) {
			return 0.0;
		}

		double exponent = ( foul ) ? ServerParam::instance().foulExponent() : ServerParam::instance().tackleExponent();
		// tackle failure probability
		double prob = pow(dx, exponent) + pow(dy, exponent);
		prob = MinMax(0.0, prob, 1.0);
		return 1.0 - prob;
	}
}

inline double GetMaxTackleRand(const Vector & ball_2_player, const Vector & ball_vel, const int player_type, const bool & foul)
{
	double prob = 1.0 - GetTackleProb(ball_2_player, foul);
	double pos_rate =  0.5 + 0.5 * prob;

	double speed_rate = 0.5 + 0.5 * (ball_vel.Mod() /
			(ServerParam::instance().ballSpeedMax() * ServerParam::instance().ballDecay()));

	return (PlayerParam::instance().HeteroPlayer(player_type).kickRand()
			* ServerParam::instance().tackleRandFactor()
			* (pos_rate + speed_rate));
}

/**
 * 铲球 max_rand
 * @param prob 铲球成功率
 * @param ball_vel 球的当前速度
 * @param player_type 球员异构
 * @return
 */
inline double GetMaxTackleRand(const double & prob, const Vector & ball_vel, const int player_type)
{
	double pos_rate =  0.5 + 0.5 * prob;

	double speed_rate = 0.5 + 0.5 * (ball_vel.Mod() /
			(ServerParam::instance().ballSpeedMax() * ServerParam::instance().ballDecay()));

	return (PlayerParam::instance().HeteroPlayer(player_type).kickRand()
			* ServerParam::instance().tackleRandFactor()
			* (pos_rate + speed_rate));
}

/**
 * 得到tackle的概率，参数均为绝对坐标
 * Get tackle success probability with all parameters are absolute values.
 * \param ball_pos.
 * \prram player_pos.
 * \param player_body_dir
 *
 */
inline double GetTackleProb(const Vector & ball_pos, const Vector & player_pos, const AngleDeg & player_body_dir, const bool & foul)
{
    Vector ball_2_player = (ball_pos - player_pos).Rotate(-player_body_dir);
    return GetTackleProb(ball_2_player, foul);
}


inline double GetDashDirRate(double dir)
{
	dir = GetNormalizeAngleDeg(dir);

	if ( ServerParam::instance().dashAngleStep() < FLOAT_EPS ) {
		// players can dash in any direction.
	}
	else {
		// The dash direction is discretized by server::dash_angle_step
		dir = ServerParam::instance().dashAngleStep() * Rint( dir / ServerParam::instance().dashAngleStep() );
	}

	double dir_rate = ( std::fabs( dir ) > 90.0
			? ServerParam::instance().backDashRate() - ( ( ServerParam::instance().backDashRate() - ServerParam::instance().sideDashRate() )
					* ( 1.0 - ( std::fabs( dir ) - 90.0 ) / 90.0 ) )
					: ServerParam::instance().sideDashRate() + ( ( 1.0 - ServerParam::instance().sideDashRate() )
							* ( 1.0 - std::fabs( dir ) / 90.0 ) )
	);
	dir_rate = MinMax( 0.0, dir_rate, 1.0 );

	return dir_rate;
}

/**
 * 换用更省体力的等效的dash方式
 * @param dash_power
 * @param dash_dir
 */
inline void TransformDash(double & dash_power, AngleDeg & dash_dir)
{
    if (dash_power < 0.0) {
    	double anti_dash_power = -dash_power * GetDashDirRate(dash_dir) / GetDashDirRate(dash_dir + 180.0);
    	if (anti_dash_power < ServerParam::instance().maxDashPower()) {
    		dash_dir = GetNormalizeAngleDeg(dash_dir + 180.0);
    		dash_power = anti_dash_power;
    	}
    }
}

/**
 * Action的输入
 * Input of an Action.
 */
struct ActionInput
{
	int     mPlayerType;
	Vector  mPlayerPos;
	Vector  mPlayerVel;
	double  mPlayerBodyDir;
	Vector  mBallPos;
	Vector  mBallVel;
};

/**
 * 描述原子动作
 * Describes an atomic action.
 */
struct AtomicAction
{
	bool        mSucceed;
	CommandType mType;

	Vector      mKickVel;
	AngleDeg    mTurnAngle;
	double      mDashPower;
    AngleDeg    mDashDir;

	AtomicAction(bool succeed = false):
		mSucceed (succeed),
		mType (CT_None),
		mTurnAngle (0.0),
		mDashPower (0.0),
        mDashDir (0.0)
	{
	}

	bool Execute(Agent & agent) const;

	void Clear() {
		mSucceed = false;
		mType = CT_None;
		mKickVel = Vector(0, 0);
		mTurnAngle = 0.0;
		mDashPower = 0.0;
		mDashDir = 0.0;
	}

	friend std::ostream & operator<<(std::ostream & os, const AtomicAction & act) {
		switch (act.mType) {
		case CT_Turn: return os << "(turn <" << act.mTurnAngle << ">)";
		case CT_Dash: return os << "(dash " << act.mDashPower << " " << act.mDashDir << ")";
		case CT_Kick: return os << "(kick " << act.mKickVel.Mod() << " " << act.mKickVel.Dir() << ")";
		case CT_None: return os << "(none)";
		default: return os << "";
		}
	}
};

/**
 * 计划执行的Action的信息
 * Stores a queue of atomic actions planed to be executed.
 */
struct ActionPlan
{
	bool    mSucceed;
	int     mCycle;

	std::deque<AtomicAction> mActionQueue;

	ActionPlan() : mSucceed(false), mCycle(0) {}
};

/**
 * Action effector.
 * This class maintains a queue of commands which will be sent to the server. It provides low level implementations
 * of atomic actions such as kick, dash, and so on.
 */
class ActionEffector {
public:
	ActionEffector(Agent & agent);
	virtual ~ActionEffector() {}

	/**
	 * 检查上周期发给server的命令，用来辅助WorldState的更新
	 * Check commands sent to server in last cycle. Help to update WorldState.
	 */
	void CheckCommandQueue(Observer *observer);

	/**
	 * 检查上周期的命令是否漏发，每周期获得信息后被调用
	 * Check commands sent to server in last cycle, and see if there are any commands were lost.
	 * Called after sense information was fetched.
	 */
	void CheckCommands(Observer *observer);

	/**
	 * 向server发送命令队列中的命令
	 * Send commands in queue to server.
	 */
	void SendCommands(char *msg = 0);

	void Reset();
	void ResetForScan();

	/**
	 * 下面这些接口是server提供的所有命令的接口，高层来调用
	 * 这些接口中严格按照server是否会执行来判断，server肯定可以执行的才返回true
	 * Each of the following methods provides an interface of a server command for high level decisions.
	 * Will return true if and only if command can be executed absolutely by server.
	 */
	bool SetTurnAction(AngleDeg angle); ///这里的 angle 是期望转身的角度，不是发给 server 的角度
	bool SetDashAction(double power, AngleDeg dir);
	bool SetTurnNeckAction(AngleDeg angle);
	bool SetSayAction(std::string msg);
	bool SetAttentiontoAction(Unum num); //这里的实现成互斥动作
	bool SetAttentiontoOffAction();      //这里的实现成互斥动作
	bool SetKickAction(double power, AngleDeg angle);
	bool SetTackleAction(AngleDeg angle, const bool foul);
	bool SetPointtoAction(double dist, AngleDeg angle);
	bool SetPointtoOffAction();
	bool SetCatchAction(AngleDeg angle);
	bool SetMoveAction(Vector pos);
	bool SetChangeViewAction(ViewWidth view_width);
	bool SetCompressionAction(int level);
	bool SetSenseBodyAction();
	bool SetScoreAction();
	bool SetByeAction();
	bool SetDoneAction();
	bool SetClangAction(int min_ver, int max_ver);
	bool SetEarOnAction(bool our_side, EarMode ear_mode = EM_All);
	bool SetEarOffAction(bool our_side, EarMode ear_mode = EM_All);
	bool SetSynchSeeAction();
	bool SetChangePlayerTypeAction(Unum num, int player_type);

	bool SetChangePlayerTypeAction(std::string teamname,Unum num, int player_type);
	bool SetStartAction();
	bool SetChangePlayModeAction(ServerPlayMode spm);
	bool SetMoveBallAction(Vector pos, Vector vel);
	bool SetMovePlayerAction(std::string team_name, Unum num, Vector pos, Vector vel, AngleDeg dir);
	bool SetLookAction();
	bool SetTeamNamesAction();
	bool SetRecoverAction();
	bool SetCheckBallAction();

	/**
	 * Each of the following methods provides an interface to get the number of times a certain action
	 * has been executed.
	 */
	int GetTurnCount() const { return mTurnCount; }
	int GetDashCount() const { return mDashCount; }
	int GetTurnNeckCount() const { return mTurnNeckCount; }
	int GetSayCount() const { return mSayCount; }
	int GetAttentiontoCount() const { return mAttentiontoCount; }
	int GetKickCount() const { return mKickCount; }
	int GetTackleCount() const { return mTackleCount; }
	int GetPointtoCount() const { return mPointtoCount; }
	int GetCatchCount() const { return mCatchCount; }
	int GetMoveCount() const { return mMoveCount; }
	int GetChangeViewCount() const { return mChangeViewCount; }
	int GetCompressionCount() const { return mCompressionCount; }
	int GetSenseBodyCount() const { return mSenseBodyCount; }
	int GetScoreCount() const { return mScoreCount; }
	int GetByeCount() const { return mByeCount; }
	int GetDoneCount() const { return mDoneCount; }
	int GetClangCount() const { return mClangCount; }
	int GetEarCount() const { return mEarCount; }
	int GetSynchSeeCount() const { return mSynchSeeCount; }

	int GetChangePlayerTypeCount() const { return mChangePlayerTypeCount; }

	/**
	 * If the commands queue includes turn, dash, kick, tackle, catch or move.
	 */
	bool IsMutex() const { return mIsMutex; }

	/**
	 * Each of the following methods checks if the commands queue includes a certain type of command.
	 */
	bool IsTurn() const { return mIsTurn; }
	bool IsDash() const { return mIsDash; }
	bool IsTurnNeck() const { return mIsTurnNeck; }
	bool IsSay() const { return mIsSay; }
	bool IsAttentionto() const { return mIsAttentionto; }
	bool IsKick() const { return mIsKick; }
	bool IsTackle() const { return mIsTackle; }
	bool IsPointto() const { return mIsPointto; }
	bool IsCatch() const { return mIsCatch; }
	bool IsMove() const { return mIsMove; }
	bool IsChangeView() const { return mIsChangeView; }
	bool IsCompression() const { return mIsCompression; }
	bool IsSenseBody() const { return mIsSenseBody; }
	bool IsScore() const { return mIsScore; }
	bool IsBye() const { return mIsBye; }
	bool IsDone() const { return mIsDone; }
	bool IsClang() const { return mIsClang; }
	bool IsEar() const { return mIsEar; }
	bool IsSynchSee() const { return mIsSynchSee; }
	bool IsChangePlayerType() const { return mIsChangePlayerType; }

	bool IsStart() const { return mIsStart; }
	bool IsChangePlayMode() const { return mIsChangePlayMode; }
	bool IsMovePlayer() const { return mIsMovePlayer; }
	bool IsMoveBall() const { return mIsMoveBall; }
	bool IsLook() const { return mIsLook; }
	bool IsTeamNames() const { return mIsTeamNames; }
	bool IsRecover() const { return mIsRecover; }
	bool IsCheckBall() const { return mIsCheckBall; }

	bool IsSayMissed() const { return mIsSayMissed; }
    CommandType GetLastCommandType() const { return mLastCommandType; }
	
    /** 根据当前命令队列里的内容，预测信息*/
	/**
	 * 得到动作执行后的视角宽度
	 * Get view width after queued actions.
	 */
	ViewWidth GetSelfViewWidthWithQueuedActions();

	/**
	 * Get self position after queued actions.
	 */
	Vector GetSelfPosWithQueuedActions();

	/**
	 * Get self velocity after queued actions.
	 */
	Vector GetSelfVelWithQueuedActions();

	/**
	 * Get self body direction after queued actions.
	 * btw, neck direction don't consider, said baj.
	 */
	AngleDeg GetSelfBodyDirWithQueuedActions();

	/**
	 * Get ball position after queued actions.
	 */
	Vector GetBallPosWithQueuedActions();

	/**
	 * Get ball velocity after queued actions.
	 */
	Vector GetBallVelWithQueuedActions();

public:
	/**
	 * 通过传入kick的参数，计算kick后球的位置和速度
	 * Calculate ball position and velocity after a kick action.
	 */
	static void ComputeInfoAfterKick(const double kick_power, const double kick_angle,
			const PlayerState &player_state, const BallState &ball_state, Vector &ball_pos, Vector &ball_vel, bool is_self = true);

	/**
	 * 通过传入dash的参数，计算dash后球员的位置和速度
	 * Calculate player position and velocity after a dash action.
	 */
	static void ComputeInfoAfterDash(const double dash_power, double dash_dir,
			const PlayerState &player_state, Vector &player_pos, Vector &player_vel);

	/**
	 * 通过传入move的参数，计算move后球员的位置和速度
	 * Calculate player position and velocity after a move action.
	 */
	static void ComputeInfoAfterMove(const Vector & move_pos,
			Vector &player_pos, Vector &player_vel);

	/**
	 * 通过传入turn的参数，计算turn后球员的身体朝向和脖子朝向
	 * Calculate player body direction after a turn action.
	 */
	static void ComputeInfoAfterTurn(const AngleDeg turn_angle,
			const PlayerState &player_state, AngleDeg &body_dir);

	/**
	 * 通过传入turn_neck的参数，计算turn_neck后球员的脖子朝向
	 * Calculate player neck direction after a turn_neck action.
	 */
	static void ComputeInfoAfterTurnNeck(const AngleDeg turn_neck_angle,
			const PlayerState &player_state, AngleDeg &neck_dir);

private:
	const Agent       & mAgent;

	const WorldState  & mWorldState;
	const BallState   & mBallState;
    const PlayerState & mSelfState;

	std::list<CommandInfo> mCommandQueue;

public:
	static ThreadMutex CMD_QUEUE_MUTEX;

	Turn        mTurn;
	Dash        mDash;
	TurnNeck    mTurnNeck;
	Say         mSay;
	Attentionto mAttentionto;
	Kick        mKick;
	Tackle      mTackle;
	Pointto     mPointto;
	Catch       mCatch;
	Move        mMove;
	ChangeView  mChangeView;
	Compression mCompression;
	SenseBody   mSenseBody;
	Score       mScore;
	Bye         mBye;
	Done        mDone;
	Clang       mClang;
	Ear         mEar;
	SynchSee    mSynchSee;
	ChangePlayerType mChangePlayerType;

	Start		mStart;
	ChangePlayMode	mChangePlayMode;
	MovePlayer	mMovePlayer;
	MoveBall	mMoveBall;
	Look		mLook;
	TeamNames	mTeamNames;
	Recover		mRecover;
	CheckBall	mCheckBall;

	int         mTurnCount;
	int         mDashCount;
	int         mTurnNeckCount;
	int         mSayCount;
	int         mAttentiontoCount;
	int         mKickCount;
	int         mTackleCount;
	int         mPointtoCount;
	int         mCatchCount;
	int         mMoveCount;
	int         mChangeViewCount;
	int         mCompressionCount;
	int         mSenseBodyCount;
	int         mScoreCount;
	int         mByeCount;
	int         mDoneCount;
	int         mClangCount;
	int         mEarCount;
	int         mSynchSeeCount;
	int         mChangePlayerTypeCount;

	bool        mIsMutex;

	bool        mIsTurn;
	bool        mIsDash;
	bool        mIsTurnNeck;
	bool        mIsSay;
	bool        mIsAttentionto;
	bool        mIsKick;
	bool        mIsTackle;
	bool        mIsPointto;
	bool        mIsCatch;
	bool        mIsMove;
	bool        mIsChangeView;
	bool        mIsCompression;
	bool        mIsSenseBody;
	bool        mIsScore;
	bool        mIsBye;
	bool        mIsDone;
	bool        mIsClang;
	bool        mIsEar;
	bool        mIsSynchSee;
	bool        mIsChangePlayerType;

	bool		mIsStart;
	bool	 	mIsChangePlayMode;
	bool		mIsMovePlayer;
	bool		mIsMoveBall;
	bool		mIsLook;
	bool		mIsTeamNames;
	bool		mIsRecover;
	bool		mIsCheckBall;

	bool		mIsSayMissed;
    CommandType mLastCommandType; // 上周期执行的互斥原子动作类型，暂时只考虑kick, dash, move, turn
};

#endif /* ACTIONEFFECTOR_H_ */
