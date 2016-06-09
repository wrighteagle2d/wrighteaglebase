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

#include "ActionEffector.h"
#include "Agent.h"
#include "UDPSocket.h"
#include "Observer.h"
#include "WorldState.h"
#include "VisualSystem.h"
#include "NetworkTest.h"

ThreadMutex ActionEffector::CMD_QUEUE_MUTEX;

bool AtomicAction::Execute(Agent & agent) const
{
	if (mSucceed || !mSucceed){ //先不管succeed为false的情况
		switch (mType){
		case CT_Turn: return agent.Turn(mTurnAngle);
		case CT_Dash: return agent.Dash(mDashPower, mDashDir);
		case CT_Kick: return agent.Kick(mKickVel.Mod(), mKickVel.Dir());
		default: return true;
		}
	}
	else return false;
}

ActionEffector::ActionEffector(Agent & agent):
	mAgent( agent ),
	mWorldState( agent.GetWorldState() ),
	mBallState( agent.GetWorldState().GetBall()),
    mSelfState( agent.GetSelf()),
	mTurn( agent ),
	mDash( agent ),
	mTurnNeck( agent ),
	mSay( agent ),
	mAttentionto( agent ),
	mKick( agent ),
	mTackle( agent ),
	mPointto( agent ),
	mCatch( agent ),
	mMove( agent ),
	mChangeView( agent ),
	mCompression( agent ),
	mSenseBody( agent ),
	mScore( agent ),
	mBye( agent ),
	mDone( agent ),
	mClang( agent ),
	mEar( agent ),
	mSynchSee( agent ),
	mChangePlayerType( agent ),
	mStart( agent ),
	mChangePlayMode( agent ),
	mMovePlayer( agent ),
	mMoveBall( agent ),
	mLook( agent ),
	mTeamNames( agent ),
	mRecover( agent ),
	mCheckBall( agent )
{
	mTurnCount          = 0;
	mDashCount          = 0;
	mTurnNeckCount      = 0;
	mSayCount           = 0;
	mAttentiontoCount   = 0;
	mKickCount          = 0;
	mTackleCount        = 0;
	mPointtoCount       = 0;
	mCatchCount         = 0;
	mMoveCount          = 0;
	mChangeViewCount    = 0;
	mCompressionCount   = 0;
	mSenseBodyCount     = 0;
	mScoreCount         = 0;
	mByeCount           = 0;
	mDoneCount          = 0;
	mClangCount         = 0;
	mEarCount           = 0;
	mSynchSeeCount      = 0;
	mChangePlayerTypeCount = 0;

	mIsMutex        = false;

	mIsTurn         = false;
	mIsDash         = false;
	mIsTurnNeck     = false;
	mIsSay          = false;
	mIsAttentionto  = false;
	mIsKick         = false;
	mIsTackle       = false;
	mIsPointto      = false;
	mIsCatch        = false;
	mIsMove         = false;
	mIsChangeView   = false;
	mIsCompression  = false;
	mIsSenseBody    = false;
	mIsScore        = false;
	mIsBye          = false;
	mIsDone         = false;
	mIsClang        = false;
	mIsEar          = false;
	mIsSynchSee     = false;
	mIsChangePlayerType = false;

	mIsSayMissed	= false;
    mLastCommandType = CT_None;
}

bool ActionEffector::SetTurnAction(AngleDeg turn_angle)
{
	if (mIsTurn == true || mIsMutex == true)
	{
		return false;
	}

	AngleDeg moment = GetTurnMoment(turn_angle, mSelfState.GetPlayerType(), mSelfState.GetVel().Mod());

	moment = GetNormalizeMoment(moment);

	if (fabs(moment) < FLOAT_EPS){
		return false;
	}

	mTurn.Plan(moment);
	mTurn.Execute(mCommandQueue);
	++mTurnCount;
	mIsTurn = true;
	mIsMutex = true;
	return true;
}

bool ActionEffector::SetDashAction(double power, AngleDeg dir)
{
	Assert(dir > -180.0 - FLOAT_EPS && dir < 180.0 + FLOAT_EPS);

	if (mIsDash == true || mIsMutex == true)
	{
		return false;
	}

	power = GetNormalizeDashPower(power);
    dir = GetNormalizeDashAngle(dir);

    // 注意，在rcssserver13.1.0版本下，dash只需要考虑下面四种情况即可：
    // @ -power 0.0
    // @ +power 0.0
    // @ +power 90.0
    // @ +power -90.0
    //
    // from rcssserver13.1.0

    // 注意，在rcssserver v14 下，dash需要考虑的情况包括：
    // @ +power, 0
    // @ +power, 45
    // @ +power, 90
    // @ +power, 135 | -power, -45
    // @ -power, 0 | +power, 180
    // @ +power, -45
    // @ +power, -90
    // @ -power, 45 | +power, -135
    if (ServerParam::instance().dashAngleStep() < FLOAT_EPS )
    {
        // players can dash in any direction.
    }
    else
    {
        // The dash direction is discretized by server::dash_angle_step
        dir = ServerParam::instance().dashAngleStep() * Rint(dir / ServerParam::instance().dashAngleStep());
    }

    TransformDash(power, dir);

	double max_stamina = mSelfState.GetStamina() + mSelfState.GetExtraStamina();
	if (power < 0.0)
	{
		if ((-2.0 * power) > max_stamina)
		{
			power = -max_stamina * 0.5 + FLOAT_EPS;
		}
	}
	else
	{
		if (power > max_stamina)
		{
			power = max_stamina - FLOAT_EPS;
		}
	}

	if (!mAgent.GetSelf().IsOutOfStamina()) {
		power = mAgent.GetSelf().CorrectDashPowerForStamina(power); // 保证不降到最低体力以下
	}

    if (std::fabs(power) < FLOAT_EPS) return false;

    TransformDash(power, dir);

	mDash.Plan(power, dir);
	mDash.Execute(mCommandQueue);
	++mDashCount;
	mIsDash = true;
	mIsMutex = true;
	return true;
}

bool ActionEffector::SetTurnNeckAction(AngleDeg angle)
{
	if (mIsTurnNeck == true)
	{
		return false;
	}

	if (angle < ServerParam::instance().minNeckMoment() || angle > ServerParam::instance().maxNeckMoment())
	{
		return false;
	}

	mTurnNeck.Plan(angle);
	mTurnNeck.Execute(mCommandQueue);
	++mTurnNeckCount;
	mIsTurnNeck = true;
	return true;
}

bool ActionEffector::SetSayAction(std::string msg)
{
	if (mIsSay == true)
	{
		return false;
	}

    if (PlayerParam::instance().isCoach())
    {
        if (msg.length() > ServerParam::instance().freeformMsgSize())
        {
            //false;
        }
    }
    else
    {
        if (msg.length() > ServerParam::instance().sayMsgSize())
        {
            return false;
        }
    }

	mSay.Plan(msg);
	mSay.Execute(mCommandQueue);
	++mSayCount;
	mIsSay = true;
	return true;
}

bool ActionEffector::SetAttentiontoAction(Unum num)
{
	if (mIsAttentionto == true)
	{
		return false;
	}

	if (num == 0 || num == mAgent.GetSelfUnum() || num < -TEAMSIZE || num > TEAMSIZE)
	{
		return false;
	}

    if (mAgent.GetSelf().GetFocusOnUnum() == num)
    {
        mIsAttentionto = true; // 本周期不再attentionto
        return false;
    }

	mAttentionto.Plan(true, num);
	mAttentionto.Execute(mCommandQueue);
	++mAttentiontoCount;
	mIsAttentionto = true;
	return true;
}

bool ActionEffector::SetAttentiontoOffAction()
{
	if (mIsAttentionto == true)
	{
		return false;
	}

    if (mAgent.GetSelf().GetFocusOnUnum() == 0)
    {
        mIsAttentionto = true; // 本周期不再attentionto
        return false;
    }

	mAttentionto.Plan(false);
	mAttentionto.Execute(mCommandQueue);
	++mAttentiontoCount;
	mIsAttentionto = true;
	return true;
}

bool ActionEffector::SetKickAction(double power, AngleDeg angle)
{
	if (mIsKick == true || mIsMutex == true)
	{
		return false;
	}

	power = GetNormalizeKickPower(power);
	angle = GetNormalizeMoment(angle);

	if (mWorldState.GetPlayMode() == PM_Before_Kick_Off ||
			mWorldState.GetPlayMode() == PM_Goal_Ours ||
			mWorldState.GetPlayMode() == PM_Goal_Opps ||
			mWorldState.GetPlayMode() == PM_Opp_Offside_Kick ||
			mWorldState.GetPlayMode() == PM_Our_Offside_Kick ||
			mWorldState.GetPlayMode() == PM_Our_Foul_Charge_Kick || 
            mWorldState.GetPlayMode() == PM_Opp_Foul_Charge_Kick ||
            mWorldState.GetPlayMode() == PM_Our_Back_Pass_Kick ||
			mWorldState.GetPlayMode() == PM_Opp_Back_Pass_Kick ||
			mWorldState.GetPlayMode() == PM_Our_Free_Kick_Fault_Kick ||
			mWorldState.GetPlayMode() == PM_Opp_Free_Kick_Fault_Kick ||
			mWorldState.GetPlayMode() == PM_Our_CatchFault_Kick ||
			mWorldState.GetPlayMode() == PM_Opp_CatchFault_Kick ||
			mWorldState.GetPlayMode() == PM_Time_Over)
	{
		return false;
	}

	if (mSelfState.IsKickable() == false)
	{
		return false;
	}

    if (power < 1.0) // 避免不必要的kick
    {
        return false;
    }

    if (power > ServerParam::instance().maxPower() - 1.0)
    {
        power = ServerParam::instance().maxPower();
    }

	mKick.Plan(power, angle);
	mKick.Execute(mCommandQueue);
	++mKickCount;
	mIsKick = true;
	mIsMutex = true;
	return true;
}

bool ActionEffector::SetTackleAction(AngleDeg angle, const bool foul)
{
	if (mIsTackle == true || mIsMutex == true)
	{
		return true;
	}

	if (angle < ServerParam::instance().minMoment() || angle > ServerParam::instance().maxMoment())
	{
		return false;
	}

	if (mSelfState.GetIdleCycle() > 0)
	{
		return false;
	}

	double tackle_prob = mSelfState.GetTackleProb(foul);
	if (tackle_prob < FLOAT_EPS && 
        mWorldState.GetPlayMode() != PM_Opp_Penalty_Taken) // 守门员防点球时能tackle就tackle
	{
		return false;
	}

	if (mWorldState.GetPlayMode() == PM_Before_Kick_Off ||
			mWorldState.GetPlayMode() == PM_Goal_Ours ||
			mWorldState.GetPlayMode() == PM_Goal_Opps ||
			mWorldState.GetPlayMode() == PM_Opp_Offside_Kick ||
			mWorldState.GetPlayMode() == PM_Our_Offside_Kick ||
			mWorldState.GetPlayMode() == PM_Our_Foul_Charge_Kick || 
            mWorldState.GetPlayMode() == PM_Opp_Foul_Charge_Kick || 
            mWorldState.GetPlayMode() == PM_Our_Back_Pass_Kick ||
			mWorldState.GetPlayMode() == PM_Opp_Back_Pass_Kick ||
			mWorldState.GetPlayMode() == PM_Our_Free_Kick_Fault_Kick ||
			mWorldState.GetPlayMode() == PM_Opp_Free_Kick_Fault_Kick ||
			mWorldState.GetPlayMode() == PM_Our_CatchFault_Kick ||
			mWorldState.GetPlayMode() == PM_Opp_CatchFault_Kick ||
			mWorldState.GetPlayMode() == PM_Time_Over)
	{
		return false;
	}

	mTackle.Plan(angle, foul);
	mTackle.Execute(mCommandQueue);
	++mTackleCount;
	mIsTackle = true;
	mIsMutex = true;
	return true;
}

bool ActionEffector::SetPointtoAction(double dist, AngleDeg angle)
{
	if (mIsPointto == true)
	{
		return false;
	}

	if (mSelfState.GetArmPointMovableBan() > 0)
	{
		return false;
	}

	mPointto.Plan(true, dist, angle);
	mPointto.Execute(mCommandQueue);
	++mPointtoCount;
	mIsPointto = true;
	return true;
}

bool ActionEffector::SetPointtoOffAction()
{
	if (mIsPointto == true)
	{
		return false;
	}

	if (mSelfState.GetArmPointMovableBan() > 0)
	{
		return false;
	}

	mPointto.Plan(false);
	mPointto.Execute(mCommandQueue);
	++mPointtoCount;
	mIsPointto = true;
	return true;
}

bool ActionEffector::SetCatchAction(AngleDeg angle)
{
    if (mIsCatch == true || mIsMutex == true)
    {
        return false;
    }

    if (angle < ServerParam::instance().minMoment() || angle > ServerParam::instance().maxMoment())
    {
        return false;
    }

//    if (mSelfState.IsGoalie() == false) //FIXME
//    {
//        return false;
//    }

    if (mSelfState.GetCatchBan() > 0)
    {
        return false;
    }

    if (mWorldState.GetPlayMode() != PM_Play_On &&
    		mWorldState.GetPlayMode() != PM_Our_Penalty_Setup &&
    		mWorldState.GetPlayMode() != PM_Opp_Penalty_Setup &&
    		mWorldState.GetPlayMode() != PM_Our_Penalty &&
    		mWorldState.GetPlayMode() != PM_Opp_Penalty_Ready &&
    		mWorldState.GetPlayMode() != PM_Our_Penalty_Taken &&
    		mWorldState.GetPlayMode() != PM_Opp_Penalty_Taken &&
    		mWorldState.GetPlayMode() != PM_Our_Penalty_Miss &&
    		mWorldState.GetPlayMode() != PM_Opp_Penalty_Miss &&
    		mWorldState.GetPlayMode() != PM_Our_Penalty_Score &&
    		mWorldState.GetPlayMode() != PM_Opp_Penalty_Score)
    {
        return false;
    }

    /*if (mSelfState.IsBallCatchable() == false)
    {
        return false;
    }*/

    mCatch.Plan(angle);
    mCatch.Execute(mCommandQueue);
    ++mCatchCount;
    mIsCatch = true;
    mIsMutex = true;
    return true;
}

bool ActionEffector::SetMoveAction(Vector pos)
{
	if (mIsMove == true || mIsMutex == true)
	{
		return false;
	}

	//goalie move, see SoccerServer player.cpp

	mMove.Plan(pos);
	mMove.Execute(mCommandQueue);
	++mMoveCount;
	mIsMove = true;
	mIsMutex = true;
	return true;
}

bool ActionEffector::SetChangeViewAction(ViewWidth view_width)
{
	if (view_width != VW_Narrow &&
			view_width != VW_Normal &&
			view_width != VW_Wide)
	{
		return false;
	}

	VisualSystem::instance().ChangeViewWidth(mAgent, view_width); //与视觉系统同步

	if (view_width == mSelfState.GetViewWidth())
	{
		return true;
	}

	mChangeView.Plan(view_width);
	mChangeView.Execute(mCommandQueue);
	++mChangeViewCount;
	mIsChangeView = true;
	return true;
}

bool ActionEffector::SetCompressionAction(int level)
{
	if (mIsCompression == true)
	{
		return false;
	}

	if (level > 9)
	{
		return false;
	}

	mCompression.Plan(level);
	mCompression.Execute(mCommandQueue);
	++mCompressionCount;
	mIsCompression = true;
	return true;
}

bool ActionEffector::SetSenseBodyAction()
{
	if (mIsSenseBody == true)
	{
		return false;
	}

	mSenseBody.Plan();
	mSenseBody.Execute(mCommandQueue);
	++mSenseBodyCount;
	mIsSenseBody = true;
	return true;
}

bool ActionEffector::SetScoreAction()
{
	if (mIsScore == true)
	{
		return false;
	}

	mScore.Plan();
	mScore.Execute(mCommandQueue);
	++mScoreCount;
	mIsScore = true;
	return true;
}

bool ActionEffector::SetByeAction()
{
	if (mIsBye == true)
	{
		return false;
	}

	mBye.Plan();
	mBye.Execute(mCommandQueue);
	++mByeCount;
	mIsBye = true;
	return true;
}

bool ActionEffector::SetDoneAction()
{
	if (mIsDone == true)
	{
		return false;
	}

	mDone.Plan();
	mDone.Execute(mCommandQueue);
	++mDoneCount;
	mIsDone = true;
	return true;
}

bool ActionEffector::SetClangAction(int min_ver, int max_ver)
{
	//    if (mIsClang == true)
	//    {
	//        return false;
	//    }

	mClang.Plan(min_ver, max_ver);
	mClang.Execute(mCommandQueue);
	++mClangCount;
	mIsClang = true;
	return true;
}

bool ActionEffector::SetEarOnAction(bool our_side, EarMode ear_mode)
{
	// ear命令一个周期可以发多个
	if (ear_mode != EM_Partial && ear_mode != EM_Complete)
	{
		return false;
	}

	mEar.Plan(true, our_side, ear_mode);
	mEar.Execute(mCommandQueue);
	++mEarCount;
	mIsEar = true;
	return true;
}

bool ActionEffector::SetEarOffAction(bool our_side, EarMode ear_mode)
{
	// ear命令一个周期可以发多个
	if (ear_mode != EM_Partial && ear_mode != EM_Complete && ear_mode != EM_All)
	{
		return false;
	}

	mEar.Plan(false, our_side, ear_mode);
	mEar.Execute(mCommandQueue);
	++mEarCount;
	mIsEar = true;
	return true;
}

bool ActionEffector::SetSynchSeeAction()
{
	//    if (mIsSynchSee == true)
	//    {
	//        return false;
	//    }

	mSynchSee.Plan();
	mSynchSee.Execute(mCommandQueue);
	++mSynchSeeCount;
	mIsSynchSee = true;
	return true;
}

bool ActionEffector::SetChangePlayerTypeAction(Unum num, int player_type)
{
	mChangePlayerType.Plan(num, player_type);
	mChangePlayerType.Execute(mCommandQueue);
	++mChangePlayerTypeCount;
	mIsChangePlayerType = true;
	return true;
}

/**********************************for trainer*************************************/
bool ActionEffector::SetChangePlayerTypeAction(std::string teamname,Unum num, int player_type)
{
	mChangePlayerType.Plan(teamname,num, player_type);
	mChangePlayerType.Execute(mCommandQueue);
	++mChangePlayerTypeCount;
	mIsChangePlayerType = true;
	return true;
}

//Trainer就不检测是否漏掉了，没有必要
bool ActionEffector::SetStartAction()
{
	mStart.Plan();
	mStart.Execute(mCommandQueue);
	mIsStart = true;
	return true;
}
bool ActionEffector::SetChangePlayModeAction(ServerPlayMode spm)
{
	mChangePlayMode.Plan(spm);
	mChangePlayMode.Execute(mCommandQueue);
	mIsChangePlayMode = true;
	return true;
}
bool ActionEffector::SetMovePlayerAction(std::string team_name, Unum num, Vector pos, Vector vel, AngleDeg dir)
{
	mMovePlayer.Plan(team_name, num, pos, vel, dir);
	mMovePlayer.Execute(mCommandQueue);
	mIsMovePlayer = true ;
	return true;
}
bool ActionEffector::SetMoveBallAction(Vector pos, Vector vel)
{
	mMoveBall.Plan(pos,vel);
	mMoveBall.Execute(mCommandQueue);
	mIsMoveBall = true;
	return true;
}

bool ActionEffector::SetLookAction()
{
	mLook.Plan();
	mLook.Execute(mCommandQueue);
	mIsLook = true;
	return true;
}
bool ActionEffector::SetTeamNamesAction()
{
	mTeamNames.Plan();
	mTeamNames.Execute(mCommandQueue);
	mIsTeamNames = true ;
	return true;
}
bool ActionEffector::SetRecoverAction()
{
	mRecover.Plan();
	mRecover.Execute(mCommandQueue);
	mIsRecover = true;
	return true;
}
bool ActionEffector::SetCheckBallAction()
{
	mCheckBall.Plan();
	mCheckBall.Execute(mCommandQueue);
	mIsCheckBall = true;
	return true;
}

/**
 * 通过传入kick的参数，计算kick后球的位置和速度
 * Calculate ball position and velocity after a kick action.
 * \param kick_power.
 * \param kick_angle.
 * \param player_state state of the player who is performing kick.
 * \param ball_state ball state before the kick action is performed.
 * \param ball_pos will be set to ball position after kick.
 * \param ball_vel will be set to ball velocity after kick.
 * \param is_self if the action is performed by the agent this process represents.
 */
void ActionEffector::ComputeInfoAfterKick(const double kick_power, const double kick_angle,
		const PlayerState &player_state, const BallState &ball_state, Vector &ball_pos, Vector &ball_vel, bool is_self)
{
	double power = GetNormalizeKickPower(kick_power);
	double dir = GetNormalizeMoment(kick_angle);

	Vector ball_2_player = (ball_state.GetPos() - player_state.GetPos()).Rotate(-player_state.GetBodyDir());
    double eff_power = power *
        (is_self ? player_state.GetKickRate() : GetKickRate(ball_2_player, player_state.GetPlayerType()));
	Vector accel = Polar2Vector(eff_power, player_state.GetBodyDir() + dir);

	ball_vel = ball_state.GetVel() + accel;
	ball_pos = ball_state.GetPos() + ball_vel;
	ball_vel *= ServerParam::instance().ballDecay();
}

/**
 * 通过传入dash的参数，计算dash后球员的位置和速度
 * Calculate player position and velocity after a dash action. Conflicts or forbidden areas are not
 * considered.
 * \param dash_power.
 * \param dash_dir.
 * \param player_state state of the player who is dashing.
 * \param player_pos will be set to player position after dash.
 * \param player_vel will be set to player velocity after dash.
 */
void ActionEffector::ComputeInfoAfterDash(const double dash_power, double dash_dir,
		const PlayerState &player_state, Vector &player_pos, Vector &player_vel)
{
	double dir_rate = GetDashDirRate(dash_dir);

	if (dash_power < 0.0) {
		dash_dir += 180.0;
	}
    double eff_dash_power = fabs(dash_power) * player_state.GetEffort() * player_state.GetDashPowerRate() * dir_rate;

	Vector accel = Polar2Vector(eff_dash_power, GetNormalizeAngleDeg(player_state.GetBodyDir() + dash_dir));

	player_vel = player_state.GetVel() + accel;
	player_pos = player_state.GetPos() + player_vel;
	player_vel *= player_state.GetPlayerDecay();
}

/**
 * 通过传入move的参数，计算move后球员的位置和速度
 * Calculate player position and velocity after a move action.
 * \param move_pos.
 * \param player_pos will be set to player position after move.
 * \param player_vel will be set to player velocity after move.
 */
void ActionEffector::ComputeInfoAfterMove(const Vector & move_pos, Vector &player_pos, Vector &player_vel)
{
	player_pos = move_pos;
	player_vel.SetValue(0, 0);
}

/**
 * 通过传入turn的参数，计算turn后球员的身体朝向和脖子朝向
 * Calculate player body direction after a turn action.
 * \param turn_angle.
 * \param player_state state of the player who is turning.
 * \param body_dir will be set to player's body direction after turn.
 */
void ActionEffector::ComputeInfoAfterTurn(const AngleDeg moment,
		const PlayerState &player_state, AngleDeg &body_dir)
{
	double turn_angle = GetTurnAngle(moment, player_state.GetPlayerType(), player_state.GetVel().Mod());
	body_dir = GetNormalizeAngleDeg(player_state.GetBodyDir() + turn_angle);
}

/**
 * 通过传入turn_neck的参数，计算turn_neck后球员的脖子朝向
 * Calculate player neck direction after a turn_neck action.
 * \param turn_neck_angle.
 * \param player_state state of the player who is turning.
 * \param neck_dir will be set to player's neck direction after turn_neck.
 */
void ActionEffector::ComputeInfoAfterTurnNeck(const AngleDeg turn_neck_angle,
		const PlayerState &player_state, AngleDeg &neck_dir)
{
	double eff_moment = GetNormalizeNeckMoment(turn_neck_angle);
	neck_dir = GetNormalizeNeckAngle(player_state.GetNeckDir() + eff_moment);
}

/**
 * 检查上周期发给server的命令，用来辅助WorldState的更新
 * Check commands sent to server in last cycle. Help to update WorldState.
 * It predicts information by doing actions in commands queue,, assuming only this one agent influences
 * world state. And it will flush the commands queue.
 */
void ActionEffector::CheckCommandQueue(Observer *observer)
{
    mLastCommandType = CT_None;

	ActionEffector::CMD_QUEUE_MUTEX.Lock();
	if (!mCommandQueue.empty())
	{
		for (std::list<CommandInfo>::iterator it = mCommandQueue.begin(); it != mCommandQueue.end(); ++it)
		{
			switch (it->mType)
			{
			case CT_Kick:
				if (observer->Sense().GetKickCount() == mKickCount)
				{
                    mLastCommandType = CT_Kick;

					Vector ball_pos = Vector(0.0, 0.0);
					Vector ball_vel = Vector(0.0, 0.0);
					ComputeInfoAfterKick(it->mPower, it->mAngle, mSelfState, mBallState, ball_pos, ball_vel);

					observer->SetBallKickTime(observer->CurrentTime());
					observer->SetBallPosByKick(ball_pos);
					observer->SetBallVelByKick(ball_vel);
				}
				break;
			case CT_Dash:
				if (observer->Sense().GetDashCount() == mDashCount)
				{
                    mLastCommandType = CT_Dash;
					
                    Vector player_pos = Vector(0.0, 0.0);
					Vector player_vel = Vector(0.0, 0.0);
					ComputeInfoAfterDash(it->mPower, it->mAngle, mSelfState, player_pos, player_vel);

					observer->SetPlayerDashTime(observer->CurrentTime());
					observer->SetPlayerPosByDash(player_pos);
					observer->SetPlayerVelByDash(player_vel);
				}
				break;
			case CT_Move:
				if (observer->Sense().GetMoveCount() == mMoveCount)
				{
                    mLastCommandType = CT_Move;
					
                    Vector player_pos = Vector(0.0, 0.0);
					Vector player_vel = Vector(0.0, 0.0);
					ComputeInfoAfterMove(it->mMovePos, player_pos, player_vel);

					observer->SetPlayerMoveTime(observer->CurrentTime());
					observer->SetPlayerPosByMove(player_pos);
					observer->SetPlayerVelByMove(player_vel);
				}
				break;
			case CT_Turn:
				if (observer->Sense().GetTurnCount() == mTurnCount)
				{
                    mLastCommandType = CT_Turn;
					
                    AngleDeg body_dir = 0.0;
					ComputeInfoAfterTurn(it->mAngle, mSelfState, body_dir);

					observer->SetPlayerTurnTime(observer->CurrentTime());
					observer->SetPlayerBodyDirByTurn(body_dir);
				}
				break;
			case CT_TurnNeck:
				if (observer->Sense().GetTurnNeckCount() == mTurnNeckCount)
				{
					AngleDeg neck_dir = 0.0;
					ComputeInfoAfterTurnNeck(it->mAngle, mSelfState, neck_dir);

					observer->SetPlayerTurnNeckTime(observer->CurrentTime());
					observer->SetPlayerNeckDirByTurnNeck(neck_dir);
				}
				break;
			default:
				break;
			}
		}

		mCommandQueue.clear(); // 清空命令队列
	}
	ActionEffector::CMD_QUEUE_MUTEX.UnLock();
}

/**
 * 检查上周期的命令是否漏发，每周期获得信息后被调用
 * Check commands sent to server in last cycle, see if there are any commands were lost, and print
 * "miss xxx" if there is one.
 * Called after all information has been fetched in every cycle.
 */
void ActionEffector::CheckCommands(Observer *observer)
{
	CheckCommandQueue(observer);
	Reset();

	/**
	* 下面通过sense中server发来的命令数据和自己的纪录数据进行比较，判断是否漏发命令
	* 按照sense中的顺序排列
	*/
	if (observer->Sense().GetKickCount() != mKickCount)
	{
		if (observer->Sense().GetKickCount() < mKickCount)
		{
			std::cout << observer->CurrentTime() << " " << PlayerParam::instance().teamName()
			<< " " << observer->SelfUnum() << " miss a kick" << std::endl;
		}
		mKickCount = observer->Sense().GetKickCount();
	}

	if (observer->Sense().GetDashCount() != mDashCount)
	{
		if (observer->Sense().GetDashCount() < mDashCount)
		{
			std::cout << observer->CurrentTime() << " " << PlayerParam::instance().teamName()
			<< " " << observer->SelfUnum() << " miss a dash" << std::endl;
		}
		mDashCount = observer->Sense().GetDashCount();
	}

	if (observer->Sense().GetTurnCount() != mTurnCount)
	{
		if (observer->Sense().GetTurnCount() < mTurnCount)
		{
			std::cout << observer->CurrentTime() << " " << PlayerParam::instance().teamName()
			<< " " << observer->SelfUnum() << " miss a turn" << std::endl;
		}
		mTurnCount = observer->Sense().GetTurnCount();
	}

	if (observer->Sense().GetSayCount() != mSayCount)
	{
		if (observer->Sense().GetSayCount() < mSayCount)
		{
			std::cout << observer->CurrentTime() << " " << PlayerParam::instance().teamName()
			<< " " << observer->SelfUnum() << " miss a say" << std::endl;
			mIsSayMissed = true;
		}
		mSayCount = observer->Sense().GetSayCount();
	}

	if (observer->Sense().GetTurnNeckCount() != mTurnNeckCount)
	{
		if (observer->Sense().GetTurnNeckCount() < mTurnNeckCount)
		{
			std::cout << observer->CurrentTime() << " " << PlayerParam::instance().teamName()
			<< " " << observer->SelfUnum() << " miss a turn_neck" << std::endl;
		}
		mTurnNeckCount = observer->Sense().GetTurnNeckCount();
	}

	if (observer->Sense().GetCatchCount() != mCatchCount)
	{
		if (observer->Sense().GetCatchCount() < mCatchCount)
		{
			std::cout << observer->CurrentTime() << " " << PlayerParam::instance().teamName()
			<< " " << observer->SelfUnum() << " miss a catch" << std::endl;
		}
		mCatchCount = observer->Sense().GetCatchCount();
	}

	if (observer->Sense().GetMoveCount() != mMoveCount)
	{
		if (observer->Sense().GetMoveCount() < mMoveCount)
		{
			std::cout << observer->CurrentTime() << " " << PlayerParam::instance().teamName()
			<< " " << observer->SelfUnum() << " miss a move" << std::endl;
		}
		mMoveCount = observer->Sense().GetMoveCount();
	}

	if (observer->Sense().GetChangeViewCount() != mChangeViewCount)
	{
		if (observer->Sense().GetChangeViewCount() < mChangeViewCount)
		{
			std::cout << observer->CurrentTime() << " " << PlayerParam::instance().teamName()
			<< " " << observer->SelfUnum() << " miss a change_view" << std::endl;
		}
		mChangeViewCount = observer->Sense().GetChangeViewCount();
	}

	if (observer->Sense().GetArmCount() != mPointtoCount)
	{
		if (observer->Sense().GetArmCount() < mPointtoCount)
		{
			std::cout << observer->CurrentTime() << " " << PlayerParam::instance().teamName()
			<< " " << observer->SelfUnum() << " miss a pointto" << std::endl;
		}
		mPointtoCount = observer->Sense().GetArmCount();
	}

	if (observer->Sense().GetFocusCount() != mAttentiontoCount)
	{
		if (observer->Sense().GetFocusCount() < mAttentiontoCount)
		{
			std::cout << observer->CurrentTime() << " " << PlayerParam::instance().teamName()
			<< " " << observer->SelfUnum() << " miss a attentionto" << std::endl;
		}
		mAttentiontoCount = observer->Sense().GetFocusCount();
	}

	if (observer->Sense().GetTackleCount() != mTackleCount)
	{
		if (observer->Sense().GetTackleCount() < mTackleCount)
		{
			std::cout << observer->CurrentTime() << " " << PlayerParam::instance().teamName()
			<< " " << observer->SelfUnum() << " miss a tackle" << std::endl;
		}
		mTackleCount = observer->Sense().GetTackleCount();
	}
}

void ActionEffector::Reset()
{
	ActionEffector::CMD_QUEUE_MUTEX.Lock();
	mCommandQueue.clear();
	ActionEffector::CMD_QUEUE_MUTEX.UnLock();

	mIsMutex        = false;

	mIsTurn         = false;
	mIsDash         = false;
	mIsTurnNeck     = false;
	mIsSay          = false;
	mIsAttentionto  = false;
	mIsKick         = false;
	mIsTackle       = false;
	mIsPointto      = false;
	mIsCatch        = false;
	mIsMove         = false;
	mIsChangeView   = false;
	mIsCompression  = false;
	mIsSenseBody    = false;
	mIsScore        = false;
	mIsBye          = false;
	mIsDone         = false;
	mIsClang        = false;
	mIsEar          = false;
	mIsSynchSee     = false;
	mIsChangePlayerType = false;

	mIsSayMissed	= false;
}

void ActionEffector::ResetForScan()
{
	//清空互斥命令
	//清除turn neck命令
	ActionEffector::CMD_QUEUE_MUTEX.Lock();
	if (!mCommandQueue.empty())
	{
		for (std::list<CommandInfo>::iterator it = mCommandQueue.begin(); it != mCommandQueue.end();)
		{
			switch (it->mType)
			{
			case CT_Kick:
				mIsKick = false;
				--mKickCount;
				it = mCommandQueue.erase(it);
				break;
			case CT_Dash:
				mIsDash = false;
				--mDashCount;
				it = mCommandQueue.erase(it);
				break;
			case CT_Move:
				mIsMove = false;
				--mMoveCount;
				it = mCommandQueue.erase(it);
				break;
			case CT_Turn:
				mIsTurn = false;
				--mTurnCount;
				it = mCommandQueue.erase(it);
				break;
			case CT_TurnNeck:
				mIsTurnNeck = false;
				--mTurnNeckCount;
				it = mCommandQueue.erase(it);
				break;
			case CT_Tackle:
				mIsTackle = false;
				--mTackleCount;
				it = mCommandQueue.erase(it);
				break;
			default:
				++it;
				break;
			}
		}
		mIsMutex = false;
	}
	ActionEffector::CMD_QUEUE_MUTEX.UnLock();
}

ViewWidth ActionEffector::GetSelfViewWidthWithQueuedActions()
{
	if (IsChangeView()){
		ViewWidth view_width = mSelfState.GetViewWidth();
		ActionEffector::CMD_QUEUE_MUTEX.Lock();
		for (std::list<CommandInfo>::iterator it = mCommandQueue.begin(); it != mCommandQueue.end(); ++it){
			if (it->mType == CT_ChangeView) {
				view_width = it->mViewWidth;
			}
		}
		ActionEffector::CMD_QUEUE_MUTEX.UnLock();
		return view_width;
	}
	else {
		return mSelfState.GetViewWidth();
	}
}

Vector ActionEffector::GetSelfPosWithQueuedActions()
{
	if (IsDash()) {
		return mSelfState.GetPredictedPosWithDash(1, mDash.GetPower());
	}
	else if (IsMove()){
		Vector player_pos = Vector(0.0, 0.0);
		Vector player_vel = Vector(0.0, 0.0);
		ComputeInfoAfterMove(mMove.GetMovePos(), player_pos, player_vel);
		return player_pos;
	}
	else return mSelfState.GetPredictedPos();
}

Vector ActionEffector::GetSelfVelWithQueuedActions()
{
	if (IsDash()){
		return mSelfState.GetPredictedVelWithDash(1, mDash.GetPower());
	}
	else if (IsMove()){
		Vector player_pos = Vector(0.0, 0.0);
		Vector player_vel = Vector(0.0, 0.0);
		ComputeInfoAfterMove(mMove.GetMovePos(), player_pos, player_vel);
		return player_vel;
	}
	else return mSelfState.GetPredictedVel();
}

AngleDeg ActionEffector::GetSelfBodyDirWithQueuedActions()
{
	if (IsTurn()){
		AngleDeg body_dir = 0.0;
		ComputeInfoAfterTurn(mTurn.GetAngle(), mSelfState, body_dir);
		return body_dir;
	}
	else return mSelfState.GetBodyDir();
}

Vector ActionEffector::GetBallPosWithQueuedActions()
{
	if (IsKick()){
		Vector ball_pos = Vector(0.0, 0.0);
		Vector ball_vel = Vector(0.0, 0.0);
		ComputeInfoAfterKick(mKick.GetPower(), mKick.GetAngle(), mSelfState, mBallState, ball_pos, ball_vel);
		return ball_pos;
	}
	else return mBallState.GetPredictedPos();
}

Vector ActionEffector::GetBallVelWithQueuedActions()
{
	if (IsKick()){
		Vector ball_pos = Vector(0.0, 0.0);
		Vector ball_vel = Vector(0.0, 0.0);
		ComputeInfoAfterKick(mKick.GetPower(), mKick.GetAngle(), mSelfState, mBallState, ball_pos, ball_vel);
		return ball_vel;
	}
	else return mBallState.GetPredictedVel();
}

/**
 * 向server发送命令队列中的命令
 * Send commands in queue to server.
 * \param msg if save_server_msg is on and a c-style string is passed to this method, it will record
 *            this string to msg-log as well.
 */
void ActionEffector::SendCommands(char *msg)
{
	if (PlayerParam::instance().isCoach() || PlayerParam::instance().isTrainer())
	{
		//Coach分条发送命令
		ActionEffector::CMD_QUEUE_MUTEX.Lock();
		for (std::list<CommandInfo>::iterator it = mCommandQueue.begin(); it != mCommandQueue.end(); ++it)
		{
			if (it->mType != CT_None && it->mTime == mWorldState.CurrentTime()) //TODO: seg fault here
			{
				NetworkTest::instance().SetCommandSendCount((*it));
			}
			if (!it->mString.empty())
			{
				if (PlayerParam::instance().DynamicDebugMode())
				{
					std::cerr << std::endl << it->mString.c_str(); // 动态调试模式，直接输出命令即可
				}
				else if (UDPSocket::instance().Send(it->mString.c_str()) < 0) // 发送命令
				{
					PRINT_ERROR("UDPSocket error!");
				}
			}
			if (PlayerParam::instance().SaveServerMessage() && msg != 0) //说明要记录命令信息
			{
				strcat(msg, it->mString.c_str());
			}
		}
		ActionEffector::CMD_QUEUE_MUTEX.UnLock();
	}
	else
	{
		//球员把命令放在一起发
		static char command_msg[MAX_MESSAGE];

		command_msg[0] = '\0';
		ActionEffector::CMD_QUEUE_MUTEX.Lock();
		for (std::list<CommandInfo>::iterator it = mCommandQueue.begin(); it != mCommandQueue.end(); ++it)
		{
			if (it->mType != CT_None && it->mTime == mWorldState.CurrentTime())
			{
				strcat(command_msg, it->mString.c_str());
	            NetworkTest::instance().SetCommandSendCount((*it));
			}
		}
		ActionEffector::CMD_QUEUE_MUTEX.UnLock();

		if (command_msg[0] != '\0')
		{
			if (PlayerParam::instance().DynamicDebugMode())
			{
				std::cerr << std::endl << command_msg; // 动态调试模式，直接输出命令即可
			}
			else if (UDPSocket::instance().Send(command_msg) < 0) // 发送命令
			{
				PRINT_ERROR("UDPSocket error!");
			}
		}
		if (PlayerParam::instance().SaveServerMessage() && msg != 0) //说明要记录命令信息
		{
			strcat(msg, command_msg);
		}
	}
}

// end of ActionEffector.cpp

