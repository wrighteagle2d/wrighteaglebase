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

#include <vector>
#include "Strategy.h"
#include "Formation.h"
#include "InfoState.h"
#include "PositionInfo.h"
#include "InterceptInfo.h"
#include "Tackler.h"
#include "Dasher.h"
#include "Logger.h"
#include "Utilities.h"
using namespace std;

//==============================================================================
Strategy::Strategy(Agent & agent):
	DecisionData(agent)
{
    mSituation = ST_Defense;
    mLastController = 0;

    mIsBallActuralKickable = false;
    mIsBallFree = false;
    mController = 0;
    mChallenger = 0;

    mForbiddenDribble = false;

    mIsPenaltyFirstStep = false;
    mPenaltyTaker = 0;
    mPenaltySetupTime = 0;
}

Strategy::~Strategy()
{
}

bool Strategy::IsMyControl() const
{
	return mController == mAgent.GetSelfUnum();
}

bool Strategy::IsLastMyControl() const
{
    return mLastController == mAgent.GetSelfUnum();
}

bool Strategy::IsTmKickable() const
{
	return (mInfoState.GetPositionInfo().GetTeammateWithBall() > 0);
}

void Strategy::UpdateRoutine()
{
	StrategyAnalyze();
	PenaltyAnalyze();
}

void Strategy::StrategyAnalyze()
{
    mIsLastBallFree = mIsBallFree;
    if (mIsBallFree == false)//记录上次不free时的状态，参考WE2008
    {
        mLastController = mController;
		mLastChallenger = mChallenger;
    }

	mIsBallActuralKickable = false;
	mIsBallFree = true;
	mController = 0;

	mMyInterCycle = 150;
	mMinTmInterCycle = 150;
	mMinOppInterCycle = 150;
	mMinIntercCycle = 150;
	mSureTmInterCycle = 150;
	mSureOppInterCycle = 150;
	mSureInterCycle = 150;
	mFastestTm = 0;
	mFastestOpp = 0;
	mSureTm = 0;
	mSureOpp = 0;

	mBallInterPos = mWorldState.GetBall().GetPos();
	mBallFreeCycleLeft = 0;
	mBallOutCycle = 1000;

	mController = 0;
	mChallenger = 0;

	if (mForbiddenDribble) {
		if (!mSelfState.IsKickable()) {
			mForbiddenDribble = false;
		}
	}

	BallPossessionAnalyse();

	SituationAnalyse();

    //And once they are over, roll back formation. Formation will be updated by BehaviorData when needed.
    mFormation.Rollback("Strategy");

	if (mSelfState.GetTackleProb(false) < FLOAT_EPS) {
		if ( mIsBallFree) {
			if (mMyTackleInfo.mMinCycle >= mSureInterCycle || (mController > 0 && mController != mSelfState.GetUnum())) {
				mMyTackleInfo.mRes = IR_Failure;
			}
		}
	}
	else {
		mMyTackleInfo.mRes = IR_Success;
		mMyTackleInfo.mMinCycle = 0;
	}
}

/**
 * 球权分析
 */
void Strategy::BallPossessionAnalyse()
{
	const std::vector<Unum> & p2b = mInfoState.GetPositionInfo().GetClosePlayerToBall();
	const PlayerState & self = mAgent.GetSelf();
	BallState & ball = mAgent.World().Ball();

	//假设ball_free，拦截计算
	mIsBallFree = true;
	if (ServerParam::instance().pitchRectanglar().IsWithin(ball.GetPos())){
		Ray ballcourse(ball.GetPos(), ball.GetVel().Dir());
		Vector outpos;
		if (ServerParam::instance().pitchRectanglar().Intersection(ballcourse, outpos)){
			double distance = outpos.Dist(ball.GetPos());
			mBallOutCycle = (int)ServerParam::instance().GetBallCycle(ball.GetVel().Mod(), distance);
		}
        else
        {
            mBallOutCycle = 0; // 可能没有交点，比如ball位置的x坐标正好为52.5
        }
	}
	else {
		mBallOutCycle = 0;
	}
	mController = self.GetUnum();

	//分析谁拿球
	//这里对场上所有其他队员计算拦截时都考虑了cyc_delay,得到的拦截周期是最小拦截周期,
	//认为自己该去拿球的情况是,最快的对手不比自己快太多,最快的队友是自己(暂时不考虑buf)

	const std::vector<OrderedIT> & OIT = mInfoState.GetInterceptInfo().GetOIT();
	std::vector<OrderedIT>::const_iterator it, itr_NULL = OIT.end(), pMyInfo = itr_NULL, pTmInfo = itr_NULL, pOppInfo = itr_NULL;

	mBallFreeCycleLeft = 150;

	for (it = OIT.begin(); it != OIT.end(); ++it){
		if (it->mUnum < 0){
			if (it->mUnum == -mWorldState.GetOpponentGoalieUnum()) continue; //这里认为对方守门员不会去抢球，否则截球里面就不去抢打身后的球了
			if (it->mpInterceptInfo->mMinCycle < mMinOppInterCycle){
				if (it->mCycleDelay < 16){
					mMinOppInterCycle = it->mpInterceptInfo->mMinCycle;
					mBallFreeCycleLeft = Min(mBallFreeCycleLeft, int(mMinOppInterCycle + it->mCycleDelay * 0.5)); //mOppMinInterCycle + it->cd * 0.5是对截球周期的一个权衡的估计
					mFastestOpp = -it->mUnum;
				}
			}
			if(it->mpInterceptInfo->mMinCycle + it->mCycleDelay < mSureOppInterCycle){
				mSureOppInterCycle = int(it->mpInterceptInfo->mMinCycle + it->mCycleDelay);
				mSureOpp = -it->mUnum;
				pOppInfo = it;
				if (!mWorldState.GetOpponent(mSureOpp).IsBodyDirValid() && mWorldState.GetOpponent(mSureOpp).GetVel().Mod() < 0.26){ //无法估计身体方向
					mSureOppInterCycle += 1;
				}
			}
		}
		else {
			if (it->mUnum == self.GetUnum()){
				mMyInterCycle = it->mpInterceptInfo->mMinCycle;
				mBallFreeCycleLeft = Min(mBallFreeCycleLeft, mMyInterCycle);
				pMyInfo = it;
			}
			else {
				if (it->mpInterceptInfo->mMinCycle < mMinTmInterCycle){
					mMinTmInterCycle = it->mpInterceptInfo->mMinCycle;
					mBallFreeCycleLeft = Min(mBallFreeCycleLeft, int(mMinTmInterCycle + it->mCycleDelay * 0.5));
					mFastestTm = it->mUnum;
				}
				if(it->mpInterceptInfo->mMinCycle + it->mCycleDelay < mSureTmInterCycle){
					mSureTmInterCycle = int(it->mpInterceptInfo->mMinCycle + it->mCycleDelay);
					mSureTm = it->mUnum;
					pTmInfo = it;
					if (!mWorldState.GetTeammate(mSureTm).IsBodyDirValid() && mWorldState.GetTeammate(mSureTm).GetVel().Mod() < 0.26){ //无法估计身体方向
						mSureTmInterCycle += 1;
					}
				}
			}
		}
	}

	mSureInterCycle = Min(mSureOppInterCycle, mSureTmInterCycle);
	mSureInterCycle = Min(mSureInterCycle, mMyInterCycle);

	mMinIntercCycle = Min(mMinOppInterCycle, mMinTmInterCycle);
	mMinIntercCycle = Min(mMyInterCycle, mMinIntercCycle);

	if (mSureTm == 0) {
		mSureTm = mSelfState.GetUnum();
		mSureTmInterCycle = mMyInterCycle;
		mMinTmInterCycle = mMyInterCycle;
		mFastestTm = mSureTm;
	}

	if (mAgent.IsLastActiveBehaviorInActOf(BT_Dribble) && mAgent.GetLastActiveBehaviorInAct()->mDetailType == BDT_Dribble_Fast &&
			( !mIsLastBallFree || mMyInterCycle < mSureInterCycle + 6 || mWorldState.CurrentTime() - mLastBallFreeTime < 8)){
		mController = mSelfState.GetUnum();
		if (pMyInfo != itr_NULL){
			mBallInterPos = ball.GetPredictedPos(mMyInterCycle);
		}
	}
	else if( /*mMyInterCycle < mBallOutCycle + 2 &&*/ mMyInterCycle <= mSureInterCycle ){ //自己是最快截到球的人
		mController = self.GetUnum();
		if(pTmInfo != itr_NULL && pTmInfo->mCycleDelay < 3){//2004_10
			if (IsMyControl()) {
				if (mSureTmInterCycle <= mMyInterCycle && mMyInterCycle <= mSureOppInterCycle) {
					if (mWorldState.GetTeammate(mSureTm).GetVelDelay() == 0 || (mWorldState.GetTeammate(mSureTm).GetPosDelay() == 0 && mInfoState.GetPositionInfo().GetPlayerDistToPlayer(mSureTm, mSelfState.GetUnum()) < ServerParam::instance().visibleDistance() - 0.5)) {
						Vector ball_int_pos = mBallState.GetPredictedPos(mSureTmInterCycle);
						Vector pos = mWorldState.GetTeammate(mSureTm).GetPos();
						if (pos.Dist(ball_int_pos) < mWorldState.GetTeammate(mSureTm).GetKickableArea() - Dasher::GETBALL_BUFFER) {
							mController = mSureTm;
						}
						else {
							Vector vel = mWorldState.GetTeammate(mSureTm).GetVel();
							double speed = vel.Mod() * Cos(vel.Dir() - (ball_int_pos - pos).Dir());
							if (speed > mWorldState.GetTeammate(mSureTm).GetEffectiveSpeedMax() * mWorldState.GetTeammate(mSureTm).GetDecay() * 0.9) { //队友正在截球
								mController = mSureTm;
							}
						}
					}
				}
				else if (mSureTmInterCycle < mMyInterCycle && mMyInterCycle <= mMinOppInterCycle) {
					mController = mSureTm;
				}
			}
		}

		if (mController == mSelfState.GetUnum()) {
			if (pMyInfo != itr_NULL){
				mBallInterPos = ball.GetPredictedPos(mMyInterCycle);
			}
            else if (mWorldState.CurrentTime().T() > 0) {
				PRINT_ERROR("bug here?");
			}
		}
		else {
			mBallInterPos = ball.GetPredictedPos(mSureTmInterCycle);
			mController = mSureTm;
		}
	}
	else {//自己拿不到球了,看看队友如何
		if(pTmInfo != itr_NULL && mSureTmInterCycle <= mSureInterCycle){//有可能拿到
			mBallInterPos = ball.GetPredictedPos(mSureTmInterCycle);
			mController = mSureTm;
		}
		else {
			if(pOppInfo != itr_NULL){
				mBallInterPos = ball.GetPredictedPos(mSureOppInterCycle);
				mController = -mSureOpp;
			}
			else {
				if (mMinIntercCycle > mBallOutCycle + 5) { //because ball will be out of field
					mBallInterPos = ball.GetPredictedPos(mBallOutCycle);
					mController = 0;
				}
				else {
					mController = mSelfState.GetUnum();
					mBallInterPos = ball.GetPredictedPos(mMyInterCycle);
				}
			}
		}
	}

	int kickable_player = mInfoState.GetPositionInfo().GetTeammateWithBall(); //这里没有考虑buffer
	if (kickable_player == 0){
		kickable_player = -mInfoState.GetPositionInfo().GetOpponentWithBall(); //这里没有考虑buffer
	}

	if (kickable_player != 0) {
		mController = kickable_player;
		mIsBallFree = false;
		mBallOutCycle = 1000;
	}

	//下面判断可踢情况
	//先判断能踢到球的队员
	//过程:看能踢到球的自己人有几个,多于一个按规则决定是谁踢,得到是否自己可以踢球,存为_pMem->ball_kickable
	//规则:球离谁基本战位点谁踢
    if (self.IsKickable()){
		mController = self.GetUnum();
		mIsBallFree = false;
		mSureInterCycle = 0;
		mMinIntercCycle = 0;
		mMyInterCycle = 0;
		mMinTmInterCycle = 0;
		mSureTmInterCycle = 0;
		mBallInterPos = ball.GetPos();
		mBallFreeCycleLeft = 0;
		mIsBallActuralKickable = true;
		mChallenger = mInfoState.GetPositionInfo().GetOpponentWithBall();

		if (!mSelfState.IsGoalie()) {
			double self_pt_dis = mAgent.GetFormation().GetTeammateFormationPoint(self.GetUnum(), ball.GetPos()).Dist2(ball.GetPos());

			for(unsigned int i = 0; i < p2b.size(); ++i){
				Unum unum = p2b[i];
				if(unum > 0 && unum != self.GetUnum()){
					if (mWorldState.GetPlayer(unum).IsKickable()){
						if(mWorldState.GetPlayMode() != PM_Play_On && mWorldState.GetPlayer(unum).IsGoalie()/*&& unum == PlayerParam::instance().ourGoalieUnum()*/){
							mAgent.Self().UpdateKickable(false); //非playon时如果守门员可踢把自己强行设置成不可踢
							mController = unum;
							break;
						}
						double tm_pt_dis = mAgent.GetFormation().GetTeammateFormationPoint(unum, ball.GetPos()).Dist2(ball.GetPos());
						if(tm_pt_dis < self_pt_dis){
							mAgent.Self().UpdateKickable(false);
							mController = unum;
							break;
						}
					}
					else { //可以认为其他人踢不到了
						break;
					}
				}
			}
		}
	}
	else if (kickable_player != 0 && kickable_player != self.GetUnum()){ //自己踢不到球,但有人可以
		mIsBallFree = false;
		if (kickable_player > 0){ //自己人可踢
			mChallenger = mInfoState.GetPositionInfo().GetOpponentWithBall();
		}
	}

	SetPlayAnalyse(); //最后分析，作为修正
}

bool Strategy::SetPlayAnalyse()
{
	PlayMode play_mode = mWorldState.GetPlayMode();
	if (play_mode == PM_Before_Kick_Off){
		mIsBallFree = true;
		mController = mInfoState.GetPositionInfo().GetClosestPlayerToBall();
		return true;
	}
	else if (play_mode < PM_Our_Mode && play_mode > PM_Play_On){
		mIsBallFree = true;
		if (mBallState.GetVel().Mod() < 0.16) { //如果有球速，说明已经发球了
			mController = mInfoState.GetPositionInfo().GetClosestTeammateToBall();
		}
		return true;
	}
	else if (play_mode > PM_Opp_Mode){
		mIsBallFree = true;
		if (mBallState.GetVel().Mod() < 0.16) { //如果有球速，说明已经发球了
			mController = -mInfoState.GetPositionInfo().GetClosestOpponentToBall();
		}
		return true;
	}
	return false;
}

void Strategy::SituationAnalyse()
{
	if( mController < 0 && mWorldState.GetBall().GetPos().X() < -10){
		mSituation = ST_Defense;
	}
	else {
		if(!mIsBallFree){
			if(mController >= 0){
				if(mBallInterPos.X() < 32.0){
					if(mInfoState.GetPositionInfo().GetTeammateOffsideLine() > 40.0 && mAgent.GetFormation().GetTeammateRoleType(mController).mLineType == LT_Midfielder && mBallInterPos.X() > 25.0){
						mSituation = ST_Penalty_Attack;
					}
					else {
						mSituation = ST_Forward_Attack;
					}
				}
				else {
					mSituation=ST_Penalty_Attack;
				}
			}
			else {
				mSituation = ST_Defense;
			}
		}
		else{
			if(IsMyControl() || mSureTmInterCycle <= mSureOppInterCycle){
				if(mBallInterPos.X() < 32.0 && mController > 0) {
					if(mInfoState.GetPositionInfo().GetTeammateOffsideLine() > 40.0 && mAgent.GetFormation().GetTeammateRoleType(mController).mLineType == LT_Midfielder && mBallInterPos.X() > 25.0) {
						mSituation = ST_Penalty_Attack;
					}
					else {
						mSituation = ST_Forward_Attack;
					}
				}
				else {
					mSituation=ST_Penalty_Attack;
				}
			}
			else{
				mSituation = ST_Defense;
			}
		}
	}

	mFormation.Update(Formation::Offensive, "Strategy");
}

/**
 * Situation based strategy position
 * \param t unum of teammate.
 * \param ballpos
 * \param normal true normal situation, and position should be adjusted with offside line,
 *               false setplay, return SBSP directly.
 */
Vector Strategy::GetTeammateSBSPPosition(Unum t,const Vector& ballpos)
{
	Vector position;

	if (mController > 0 ||
        (mController == 0 && mBallInterPos.X() > 10.0))
    {
		position = mAgent.GetFormation().GetTeammateFormationPoint(t, mController, ballpos);
	}
    else
    {
        position = mAgent.GetFormation().GetTeammateFormationPoint(t);
	}

	double x = Min(position.X(),
			mInfoState.GetPositionInfo().GetTeammateOffsideLine() - PlayerParam::instance().AtPointBuffer());
	if (mAgent.GetFormation().GetTeammateRoleType(t).mLineType==LT_Defender){		//后卫不过中场，便于回防
		position.SetX(Min(0.0,x));
	}
	else if (mAgent.GetFormation().GetTeammateRoleType(t).mLineType== LT_Forward){		//前锋不回场，便于进攻…………
		position.SetX(Max( - 1.0,x));
	}
	else position.SetX(x);

	return position;
}


Vector Strategy::GetMyInterPos()
{
	return mBallState.GetPredictedPos(mMyInterCycle);
}

Vector Strategy::AdjustTargetForSetplay(Vector target)
{
	if (mWorldState.GetPlayMode() > PM_Opp_Mode) {
		while (target.Dist(mBallState.GetPos()) < ServerParam::instance().offsideKickMargin() + 0.5) {
			target.SetX(target.X() - 0.5);
		}

		if (mWorldState.GetPlayMode() == PM_Opp_Kick_Off) {
			target.SetX(Min(target.X(), -0.1));
		}
		else if (mWorldState.GetPlayMode() == PM_Opp_Offside_Kick) {
			target.SetX(Min(target.X(), mBallState.GetPos().X() - 0.5));
		}
		else if (mWorldState.GetPlayMode() == PM_Opp_Goal_Kick) {
			if (ServerParam::instance().oppPenaltyArea().IsWithin(target)) {
				if (mSelfState.GetPos().X() < ServerParam::instance().oppPenaltyArea().Left()) {
					if (mSelfState.GetPos().Y() < ServerParam::instance().oppPenaltyArea().Top()) {
						target = ServerParam::instance().oppPenaltyArea().TopLeftCorner();
					}
					else if (mSelfState.GetPos().Y() > ServerParam::instance().oppPenaltyArea().Bottom()) {
						target = ServerParam::instance().oppPenaltyArea().BottomLeftCorner();
					}
					else {
						target.SetX(Min(target.X(), ServerParam::instance().oppPenaltyArea().Left() - 0.5));
					}
				}
				else {
					if (mSelfState.GetPos().Y() < ServerParam::instance().oppPenaltyArea().Top()) {
						target.SetY(Min(target.Y(), ServerParam::instance().oppPenaltyArea().Top() - 0.5));
					}
					else if (mSelfState.GetPos().Y() > ServerParam::instance().oppPenaltyArea().Bottom()) {
						target.SetY(Max(target.Y(), ServerParam::instance().oppPenaltyArea().Bottom() + 0.5));
					}
					else {
						target = mSelfState.GetPos(); //do nothing
					}
				}
			}
		}
	}

	return target;
}

bool Strategy::IsMyPenaltyTaken() const
{
	return (mWorldState.GetPlayMode() == PM_Our_Penalty_Taken) &&
	       (mAgent.GetSelfUnum() == mPenaltyTaker);
}

bool Strategy::IsPenaltyPlayMode() const
{
    const PlayMode &play_mode = mWorldState.GetPlayMode();
    return (play_mode == PM_Penalty_On_Our_Field) || (play_mode == PM_Penalty_On_Opp_Field) ||
           (play_mode >= PM_Our_Penalty_Setup && play_mode <= PM_Our_Penalty_Miss) ||
           (play_mode >= PM_Opp_Penalty_Setup && play_mode <= PM_Opp_Penalty_Miss);
}

void Strategy::PenaltyAnalyze()
{
    if (IsPenaltyPlayMode() == false)
    {
        return;
    }

    if (mWorldState.GetPlayModeTime() == mWorldState.CurrentTime())
    {
    	static Unum penalty_taker_seq[] = {1, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2};
        const PlayMode &play_mode = mWorldState.GetPlayMode();
        if (play_mode == PM_Our_Penalty_Setup)
        {
            while (penalty_taker_seq[++mPenaltySetupTime%TEAMSIZE] == mWorldState.GetTeammateGoalieUnum());
            mPenaltyTaker = penalty_taker_seq[mPenaltySetupTime%TEAMSIZE];
        }
        else if (play_mode == PM_Opp_Penalty_Setup)
        {
            mPenaltyTaker = -1;
        }
    }
}

// end of file Strategyinfo.cpp
