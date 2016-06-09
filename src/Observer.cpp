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

#include <cstring>
#include "Observer.h"
#include "PlayerParam.h"
#include "Logger.h"

Observer::Observer()
{
	mCurrentTime = Time(-1, 0);
    mOurInitSide = '?';
	mOurSide = '?';
	mOppSide = '?';
	mSelfUnum = 0;
	mOppGoalieUnum = 0;
	mOurScore = 0;
	mOppScore = 0;

	mPlayMode = PM_Before_Kick_Off;

	mLastCycleBeginRealTime = RealTime(0, 0);
	mLastSightRealTime      = RealTime(0, 0);

	Reset();

    mIsBeginDecision = false;
	mSenseArrived = false;
	mSightArrived = false;
	mThinkArrived = false;

	mBallKickTime = Time(-3, 0);
	mBallPosByKick = Vector(0.0, 0.0);
	mBallVelByKick = Vector(0.0, 0.0);

	mPlayerMoveTime = Time(-3, 0);
	mPlayerPosByMove = Vector(0.0, 0.0);
	mPlayerVelByMove = Vector(0.0, 0.0);

	mPlayerDashTime = Time(-3, 0);
	mPlayerPosByDash = Vector(0.0, 0.0);
	mPlayerVelByDash = Vector(0.0, 0.0);

	mPlayerTurnTime = Time(-3, 0);
	mPlayerBodyDirByTurn = 0.0;

	mPlayerTurnNeckTime = Time(-3, 0);
	mPlayerNeckDirByTurnNeck = 0.0; // 脖子角度，绝对量

	mIsNewOppType = false;
}

Observer::~Observer()
{

}

void Observer::Initialize()
{
	if (mOurSide == '?'){
		PRINT_ERROR("my side unknown, can not initialize");
		return;
	}

	/* if true, multiply all coords by -1 inside initialize */
	bool rotate = (mOurSide == 'l') ? false : true;

	InitializeFlags(rotate);
	InitializePlayers();
}

void Observer::InitializeFlags(bool rotation)
{
	double pitch_length = ServerParam::instance().PITCH_LENGTH;
	double pitch_width  = ServerParam::instance().PITCH_WIDTH;
	double pitch_margin = ServerParam::instance().PITCH_MARGIN;
	double goal_width   = ServerParam::instance().goalWidth();
	double penalty_area_length = ServerParam::instance().PENALTY_AREA_LENGTH;
	double penalty_area_width  = ServerParam::instance().PENALTY_AREA_WIDTH;

	// goals
	mMarkerObservers[Goal_L ].Initialize(Goal_L,  Vector( -pitch_length/2.0, 0.0 ), rotation);  /* Goal_L */
	mMarkerObservers[Goal_R ].Initialize(Goal_R,  Vector( pitch_length/2.0, 0.0 ), rotation);  /* Goal_R */

	// center
	mMarkerObservers[Flag_C ].Initialize(Flag_C,  Vector( 0.0, 0.0 ), rotation);  /* Flag_C */
	mMarkerObservers[Flag_CT].Initialize(Flag_CT, Vector( 0.0, -pitch_width/2.0 ), rotation);  /* Flag_CT */
	mMarkerObservers[Flag_CB].Initialize(Flag_CB, Vector( 0.0, pitch_width/2.0 ), rotation);  /* Flag_CB */

	// field corner
	mMarkerObservers[Flag_LT].Initialize(Flag_LT, Vector( -pitch_length/2.0, -pitch_width/2.0 ), rotation);  /* Flag_LT */
	mMarkerObservers[Flag_LB].Initialize(Flag_LB, Vector( -pitch_length/2.0,  pitch_width/2.0 ), rotation);  /* Flag_LB */
	mMarkerObservers[Flag_RT].Initialize(Flag_RT, Vector(  pitch_length/2.0, -pitch_width/2.0 ), rotation);  /* Flag_RT */
	mMarkerObservers[Flag_RB].Initialize(Flag_RB, Vector(  pitch_length/2.0,  pitch_width/2.0 ), rotation);  /* Flag_RB */

	// penalty area
	mMarkerObservers[Flag_PLT].Initialize(Flag_PLT, Vector( -pitch_length/2.0+penalty_area_length,-penalty_area_width/2.0 ), rotation);  /* Flag_PLT */
	mMarkerObservers[Flag_PLC].Initialize(Flag_PLC, Vector( -pitch_length/2.0+penalty_area_length, 0 ), rotation);  /* Flag_PLC */
	mMarkerObservers[Flag_PLB].Initialize(Flag_PLB, Vector( -pitch_length/2.0+penalty_area_length, penalty_area_width/2.0 ), rotation);  /* Flag_PLB */
	mMarkerObservers[Flag_PRT].Initialize(Flag_PRT, Vector(  pitch_length/2.0-penalty_area_length,-penalty_area_width/2.0 ), rotation);  /* Flag_PRT */
	mMarkerObservers[Flag_PRC].Initialize(Flag_PRC, Vector(  pitch_length/2.0-penalty_area_length, 0 ), rotation);  /* Flag_PRC */
	mMarkerObservers[Flag_PRB].Initialize(Flag_PRB, Vector(  pitch_length/2.0-penalty_area_length, penalty_area_width/2.0 ), rotation);  /* Flag_PRB */

	// goal area
	mMarkerObservers[Flag_GLT].Initialize(Flag_GLT, Vector( -pitch_length/2.0, -goal_width/2.0 ), rotation);  /* Flag_GLT */
	mMarkerObservers[Flag_GLB].Initialize(Flag_GLB, Vector( -pitch_length/2.0,  goal_width/2.0 ), rotation);  /* Flag_GLB */
	mMarkerObservers[Flag_GRT].Initialize(Flag_GRT, Vector(  pitch_length/2.0, -goal_width/2.0 ), rotation);  /* Flag_GRT */
	mMarkerObservers[Flag_GRB].Initialize(Flag_GRB, Vector(  pitch_length/2.0,  goal_width/2.0 ), rotation);  /* Flag_GRB */

	// top field flags
	mMarkerObservers[Flag_TL50].Initialize(Flag_TL50, Vector( -50.0, -pitch_width/2.0-pitch_margin ), rotation);  /* Flag_TL50 */
	mMarkerObservers[Flag_TL40].Initialize(Flag_TL40, Vector( -40.0, -pitch_width/2.0-pitch_margin ), rotation);  /* Flag_TL40 */
	mMarkerObservers[Flag_TL30].Initialize(Flag_TL30, Vector( -30.0, -pitch_width/2.0-pitch_margin ), rotation);  /* Flag_TL30 */
	mMarkerObservers[Flag_TL20].Initialize(Flag_TL20, Vector( -20.0, -pitch_width/2.0-pitch_margin ), rotation);  /* Flag_TL20 */
	mMarkerObservers[Flag_TL10].Initialize(Flag_TL10, Vector( -10.0, -pitch_width/2.0-pitch_margin ), rotation);  /* Flag_TL10 */
	mMarkerObservers[Flag_T0  ].Initialize(Flag_T0  , Vector(   0.0, -pitch_width/2.0-pitch_margin ), rotation);  /* Flag_T0   */
	mMarkerObservers[Flag_TR10].Initialize(Flag_TR10, Vector(  10.0, -pitch_width/2.0-pitch_margin ), rotation);  /* Flag_TR10 */
	mMarkerObservers[Flag_TR20].Initialize(Flag_TR20, Vector(  20.0, -pitch_width/2.0-pitch_margin ), rotation);  /* Flag_TR20 */
	mMarkerObservers[Flag_TR30].Initialize(Flag_TR30, Vector(  30.0, -pitch_width/2.0-pitch_margin ), rotation);  /* Flag_TR30 */
	mMarkerObservers[Flag_TR40].Initialize(Flag_TR40, Vector(  40.0, -pitch_width/2.0-pitch_margin ), rotation);  /* Flag_TR40 */
	mMarkerObservers[Flag_TR50].Initialize(Flag_TR50, Vector(  50.0, -pitch_width/2.0-pitch_margin ), rotation);  /* Flag_TR50 */

	// bottom field flags
	mMarkerObservers[Flag_BL50].Initialize(Flag_BL50, Vector( -50.0, pitch_width/2.0+pitch_margin ), rotation);  /* Flag_BL50 */
	mMarkerObservers[Flag_BL40].Initialize(Flag_BL40, Vector( -40.0, pitch_width/2.0+pitch_margin ), rotation);  /* Flag_BL40 */
	mMarkerObservers[Flag_BL30].Initialize(Flag_BL30, Vector( -30.0, pitch_width/2.0+pitch_margin ), rotation);  /* Flag_BL30 */
	mMarkerObservers[Flag_BL20].Initialize(Flag_BL20, Vector( -20.0, pitch_width/2.0+pitch_margin ), rotation);  /* Flag_BL20 */
	mMarkerObservers[Flag_BL10].Initialize(Flag_BL10, Vector( -10.0, pitch_width/2.0+pitch_margin ), rotation);  /* Flag_BL10 */
	mMarkerObservers[Flag_B0  ].Initialize(Flag_B0  , Vector(   0.0, pitch_width/2.0+pitch_margin ), rotation);  /* Flag_B0 */
	mMarkerObservers[Flag_BR10].Initialize(Flag_BR10, Vector(  10.0, pitch_width/2.0+pitch_margin ), rotation);  /* Flag_BR10 */
	mMarkerObservers[Flag_BR20].Initialize(Flag_BR20, Vector(  20.0, pitch_width/2.0+pitch_margin ), rotation);  /* Flag_BR20 */
	mMarkerObservers[Flag_BR30].Initialize(Flag_BR30, Vector(  30.0, pitch_width/2.0+pitch_margin ), rotation);  /* Flag_BR30 */
	mMarkerObservers[Flag_BR40].Initialize(Flag_BR40, Vector(  40.0, pitch_width/2.0+pitch_margin ), rotation);  /* Flag_BR40 */
	mMarkerObservers[Flag_BR50].Initialize(Flag_BR50, Vector(  50.0, pitch_width/2.0+pitch_margin ), rotation);  /* Flag_BR50 */

	// left field flags
	mMarkerObservers[Flag_LT30].Initialize(Flag_LT30, Vector( -pitch_length/2.0-pitch_margin, -30 ), rotation);  /* Flag_LT30 */
	mMarkerObservers[Flag_LT20].Initialize(Flag_LT20, Vector( -pitch_length/2.0-pitch_margin, -20 ), rotation);  /* Flag_LT20 */
	mMarkerObservers[Flag_LT10].Initialize(Flag_LT10, Vector( -pitch_length/2.0-pitch_margin, -10 ), rotation);  /* Flag_LT10 */
	mMarkerObservers[Flag_L0  ].Initialize(Flag_L0  , Vector( -pitch_length/2.0-pitch_margin,   0 ), rotation);  /* Flag_L0 */
	mMarkerObservers[Flag_LB10].Initialize(Flag_LB10, Vector( -pitch_length/2.0-pitch_margin,  10 ), rotation);  /* Flag_LB10 */
	mMarkerObservers[Flag_LB20].Initialize(Flag_LB20, Vector( -pitch_length/2.0-pitch_margin,  20 ), rotation);  /* Flag_LB20 */
	mMarkerObservers[Flag_LB30].Initialize(Flag_LB30, Vector( -pitch_length/2.0-pitch_margin,  30 ), rotation);  /* Flag_LB30 */

	// right field flags
	mMarkerObservers[Flag_RT30].Initialize(Flag_RT30, Vector( pitch_length/2.0+pitch_margin, -30 ), rotation);  /* Flag_RT30 */
	mMarkerObservers[Flag_RT20].Initialize(Flag_RT20, Vector( pitch_length/2.0+pitch_margin, -20 ), rotation);  /* Flag_RT20 */
	mMarkerObservers[Flag_RT10].Initialize(Flag_RT10, Vector( pitch_length/2.0+pitch_margin, -10 ), rotation);  /* Flag_RT10 */
	mMarkerObservers[Flag_R0  ].Initialize(Flag_R0  , Vector( pitch_length/2.0+pitch_margin,   0 ), rotation);  /* Flag_R0 */
	mMarkerObservers[Flag_RB10].Initialize(Flag_RB10, Vector( pitch_length/2.0+pitch_margin,  10 ), rotation);  /* Flag_RB10 */
	mMarkerObservers[Flag_RB20].Initialize(Flag_RB20, Vector( pitch_length/2.0+pitch_margin,  20 ), rotation);  /* Flag_RB20 */
	mMarkerObservers[Flag_RB30].Initialize(Flag_RB30, Vector( pitch_length/2.0+pitch_margin,  30 ), rotation); /* Flag_RB30 */

	mLineObservers[SL_Left  ].Initialize(SL_Left  , Vector( -pitch_length/2.0,  0.0 ), rotation); /* SL_Left */
	mLineObservers[SL_Right ].Initialize(SL_Right , Vector( pitch_length/2.0,  0.0 ), rotation); /* SL_Right */
	mLineObservers[SL_Top   ].Initialize(SL_Top   , Vector( 0.0, -pitch_width/2.0 ), rotation); /* SL_Top */
	mLineObservers[SL_Bottom].Initialize(SL_Bottom, Vector( 0.0,  pitch_width/2.0 ), rotation); /* SL_Bottom */
}

void Observer::Reset()
{
	//some reset work
	mIsCommandSend = false;
	mIsNewHear = false;
	mIsNewSense = false;
	mIsNewSight = false;
mIsNewThink = false;
/*
	mUnknownPlayerCount = 0;

	//current bug info just come from that left side players' info is sent before right side players' info
	mCurrentBugInfo.mLeastNum = 1;
	mCurrentBugInfo.mSide = 'l';*/
	ResetSight();

	mAudioObserver.Reset();

	mIsBallDropped = false;
	mIsPlanned = false;

 mReceiveFullstateMsg = false; 
}

void Observer::ResetSight()
{
	mUnknownPlayerCount = 0;
	mBugInfoRanged = 0;
	mLastLeftPlayer = -1;
	mFirstRightPlayer = -1;

	//current bug info just come from that left side players' info is sent before right side players' info
	mCurrentBugInfo.mLeastNum = 1;
	mCurrentBugInfo.mSide = 'l';
	mCurrentBugInfo.mSupSide = 'r';
	mCurrentBugInfo.mSupNum = TEAMSIZE;
}

void Observer::InitializePlayers()
{
	for (int i = 1; i <= ServerParam::TEAM_SIZE; ++i){
		mTeammateObservers[i].SetSide(mOurSide);
		mTeammateObservers[i].SetUnum(i);

		mOpponentObservers[i].SetSide(mOppSide);
		mOpponentObservers[i].SetUnum(i);
	}
}

void Observer::SeeLine(SideLineType line, double dist, double dir)
{
	mLineObservers[line].SetDist(dist, mCurrentTime);
	mLineObservers[line].SetDir(dir, mCurrentTime);
}

void Observer::SeeMarker(MarkerType marker, double dist, double dir)
{
	mMarkerObservers[marker].SetDist(dist, mCurrentTime);
	mMarkerObservers[marker].SetDir(dir, mCurrentTime);
}

void Observer::SeeMarker(MarkerType marker, double dist, double dir, double dist_chg, double dir_chg)
{
	mMarkerObservers[marker].SetDist(dist, mCurrentTime);
	mMarkerObservers[marker].SetDir(dir, mCurrentTime);
	mMarkerObservers[marker].SetDistChg(dist_chg, mCurrentTime);
	mMarkerObservers[marker].SetDirChg(dir_chg, mCurrentTime);
}

void Observer::SeeBall(double dist, double dir)
{
	mBallObserver.SetDist(dist, mCurrentTime);
	mBallObserver.SetDir(dir, mCurrentTime);
}
void Observer::SeeBall(double dist, double dir, double dist_chg, double dir_chg)
{
	mBallObserver.SetDist(dist, mCurrentTime);
	mBallObserver.SetDir(dir, mCurrentTime);
	mBallObserver.SetDistChg(dist_chg, mCurrentTime);
	mBallObserver.SetDirChg(dir_chg, mCurrentTime);
}

void Observer::SeePlayer(double dist, double dir)
{
	if (mUnknownPlayerCount < MAX_UNKNOWN_PLAYES){
		mUnknownPlayers[mUnknownPlayerCount].SetIsKnownSide(false);
		mUnknownPlayers[mUnknownPlayerCount].SetDist(dist, mCurrentTime);
		mUnknownPlayers[mUnknownPlayerCount].SetDir(dir, mCurrentTime);
		mUnknownPlayersBugInfo[mUnknownPlayerCount] = mCurrentBugInfo;
		mUnknownPlayerCount += 1;

		mCurrentBugInfo.mLeastNum++;
		if (mCurrentBugInfo.mLeastNum > TEAMSIZE)
		{
			mCurrentBugInfo.mSide = 'r';
			mCurrentBugInfo.mLeastNum = 1;
		}
		AdjustUnum('?' , -1);
	}
}

void Observer::SeePlayer(char side, double dist, double dir, bool is_tackling, bool is_kicked, bool is_lying, CardType card_type)
{
	if (mUnknownPlayerCount < MAX_UNKNOWN_PLAYES){
		mUnknownPlayers[mUnknownPlayerCount].SetIsKnownSide(true);
		mUnknownPlayers[mUnknownPlayerCount].SetSide(side);
		mUnknownPlayers[mUnknownPlayerCount].SetDist(dist, mCurrentTime);
		mUnknownPlayers[mUnknownPlayerCount].SetDir(dir, mCurrentTime);
		mUnknownPlayers[mUnknownPlayerCount].SetIsTcakling(is_tackling, mCurrentTime);
		mUnknownPlayers[mUnknownPlayerCount].SetIsKicked(is_kicked, mCurrentTime);
		mUnknownPlayers[mUnknownPlayerCount].SetIsLying(is_lying, mCurrentTime);
		mUnknownPlayers[mUnknownPlayerCount].SetCardType(card_type);
		mUnknownPlayersBugInfo[mUnknownPlayerCount] = mCurrentBugInfo;
		mUnknownPlayerCount += 1;

		mCurrentBugInfo.mLeastNum++;
		if (mCurrentBugInfo.mLeastNum > TEAMSIZE || (OurInitSide() == OurSide() ? side : (side == 'l'? 'r' : 'l')) != mCurrentBugInfo.mSide)
		{
			mCurrentBugInfo.mSide = 'r';
			mCurrentBugInfo.mLeastNum = 1;
		}

		AdjustUnum(OurInitSide() == OurSide() ? side : (side == 'l'? 'r' : 'l') , -1);
	}
}

void Observer::SeePlayer(char side, int num, double dist, double dir, double dist_chg, double dir_chg, double body_dir,
		double head_dir, bool is_pointing, double point_dir, bool is_tackling, bool is_kicked, bool is_lying, CardType card_type)
{
	if (side == mOurSide){
		mTeammateObservers[num].SetDist(dist, mCurrentTime);
		mTeammateObservers[num].SetDir(dir, mCurrentTime);
		mTeammateObservers[num].SetDistChg(dist_chg, mCurrentTime);
		mTeammateObservers[num].SetDirChg(dir_chg, mCurrentTime);
		mTeammateObservers[num].SetBodyDir(body_dir, mCurrentTime);
		mTeammateObservers[num].SetHeadDir(head_dir, mCurrentTime);
		mTeammateObservers[num].SetIsPointing(is_pointing, mCurrentTime);
		mTeammateObservers[num].SetPointDir(point_dir);
		mTeammateObservers[num].SetIsTcakling(is_tackling, mCurrentTime);
		mTeammateObservers[num].SetIsKicked(is_kicked, mCurrentTime);
		mTeammateObservers[num].SetIsLying(is_lying, mCurrentTime);
		mTeammateObservers[num].SetCardType(card_type);
	}
	else {
		mOpponentObservers[num].SetDist(dist, mCurrentTime);
		mOpponentObservers[num].SetDir(dir, mCurrentTime);
		mOpponentObservers[num].SetDistChg(dist_chg, mCurrentTime);
		mOpponentObservers[num].SetDirChg(dir_chg, mCurrentTime);
		mOpponentObservers[num].SetBodyDir(body_dir, mCurrentTime);
		mOpponentObservers[num].SetHeadDir(head_dir, mCurrentTime);
		mOpponentObservers[num].SetIsPointing(is_pointing, mCurrentTime);
		mOpponentObservers[num].SetPointDir(point_dir);
		mOpponentObservers[num].SetIsTcakling(is_tackling, mCurrentTime);
		mOpponentObservers[num].SetIsKicked(is_kicked, mCurrentTime);
		mOpponentObservers[num].SetIsLying(is_lying, mCurrentTime);
		mOpponentObservers[num].SetCardType(card_type);
	}
//	std::cout << "seeeee: " << side << "   and : " << num  << "  real_side: " << mOurSide << std::endl;
	mCurrentBugInfo.mSide = OurInitSide() == OurSide() ? side : (side == 'l'? 'r' : 'l');
	mCurrentBugInfo.mLeastNum = num + 1;
	if (mCurrentBugInfo.mLeastNum > TEAMSIZE)
	{
		mCurrentBugInfo.mSide = 'r';
		mCurrentBugInfo.mLeastNum = 1;
	}

	AdjustUnum(OurInitSide() == OurSide() ? side : (side == 'l'? 'r' : 'l'), num);

}

void Observer::SeePlayer(char side, int num, double dist, double dir, bool is_pointing, double point_dir, bool is_tackling, bool is_kicked, bool is_lying, CardType card_type)
{
	if (side == mOurSide){
		mTeammateObservers[num].SetDist(dist, mCurrentTime);
		mTeammateObservers[num].SetDir(dir, mCurrentTime);
		mTeammateObservers[num].SetIsPointing(is_pointing, mCurrentTime);
		mTeammateObservers[num].SetPointDir(point_dir);
		mTeammateObservers[num].SetIsTcakling(is_tackling, mCurrentTime);
		mTeammateObservers[num].SetIsKicked(is_kicked, mCurrentTime);
		mTeammateObservers[num].SetIsLying(is_lying, mCurrentTime);
		mTeammateObservers[num].SetCardType(card_type);
	}
	else {
		mOpponentObservers[num].SetDist(dist, mCurrentTime);
		mOpponentObservers[num].SetDir(dir, mCurrentTime);
		mOpponentObservers[num].SetIsPointing(is_pointing, mCurrentTime);
		mOpponentObservers[num].SetPointDir(point_dir);
		mOpponentObservers[num].SetIsTcakling(is_tackling, mCurrentTime);
		mOpponentObservers[num].SetIsKicked(is_kicked, mCurrentTime);
		mOpponentObservers[num].SetIsKicked(is_lying, mCurrentTime);
		mOpponentObservers[num].SetCardType(card_type);
	}
//	std::cout << "seeeee: " << side << "   and : " << num << " real_side: " << mOurSide<< std::endl;
	mCurrentBugInfo.mSide = OurInitSide() == OurSide() ? side : (side == 'l'? 'r' : 'l');
	mCurrentBugInfo.mLeastNum = num + 1;
	if (mCurrentBugInfo.mLeastNum > TEAMSIZE)
	{
		mCurrentBugInfo.mSide = 'r';
		mCurrentBugInfo.mLeastNum = 1;
	}

	AdjustUnum(OurInitSide() == OurSide() ? side : (side == 'l'? 'r' : 'l'),num);
}

void Observer::AdjustUnum(char side , Unum unum)
{
	if (unum >= 0)
	{
		if (side == 'l')
		{
			for (int i = mUnknownPlayerCount - 1;i >= mBugInfoRanged;i--)
			{
				mUnknownPlayersBugInfo[i].mSupSide = side;
				mUnknownPlayersBugInfo[i].mSupNum  = unum - (mUnknownPlayerCount - i);

				if ( mUnknownPlayersBugInfo[i].mSupSide == mUnknownPlayersBugInfo[i].mSide && mUnknownPlayersBugInfo[i].mSupNum < mUnknownPlayersBugInfo[i].mLeastNum)
				{
					PRINT_ERROR(CurrentTime() << "bug info compute error!!!!!!!!" );
				}
			}
		}
		else if (side == 'r')
		{
			for (int i = mUnknownPlayerCount - 1;i >= mBugInfoRanged;i--)
			{
				if (mUnknownPlayersBugInfo[i].mSide == side)
				{
					mUnknownPlayersBugInfo[i].mSupSide = side;
	        		mUnknownPlayersBugInfo[i].mSupNum  = unum - (mUnknownPlayerCount - i);
				}
				else
				{
					mUnknownPlayersBugInfo[i].mSupSide = side;
					mUnknownPlayersBugInfo[i].mSupNum = unum - (mUnknownPlayerCount - i);

					if (unum - (mUnknownPlayerCount - i) <= 0)
					{
						mUnknownPlayersBugInfo[i].mSupSide = 'l';
						mUnknownPlayersBugInfo[i].mSupNum = TEAMSIZE;
					}
				}

				if ( mUnknownPlayersBugInfo[i].mSupSide == mUnknownPlayersBugInfo[i].mSide && mUnknownPlayersBugInfo[i].mSupNum < mUnknownPlayersBugInfo[i].mLeastNum)
				{
					PRINT_ERROR(CurrentTime() << "bug info compute error!!!!!!!!" );
				}
			}
		}
		mBugInfoRanged = mUnknownPlayerCount;
	}
	else
	{
		if (side == 'l')
		{
			for (int i = mUnknownPlayerCount - 2;i >= mBugInfoRanged;i--)
			{
				if (mUnknownPlayersBugInfo[i].mSupSide != side)
				{
					mUnknownPlayersBugInfo[i].mSupSide = side;
				}
			}
		}

		for (int i = mUnknownPlayerCount - 2;i >= mBugInfoRanged;i--)
		{
			Unum num = mUnknownPlayersBugInfo[mUnknownPlayerCount - 1].mSupNum - (mUnknownPlayerCount - 1 - i);
			if (num > 0)
			{
				if (mUnknownPlayersBugInfo[i].mSupSide == 'r')
				{
					mUnknownPlayersBugInfo[i].mSupNum = num;
				}
			}
			else
			{
				mUnknownPlayersBugInfo[i].mSupSide = 'l';
				mUnknownPlayersBugInfo[i].mSupNum  = TEAMSIZE + num;
			}
		}
	}
}

void Observer::SetSenseBody(
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
)
{
	mSenseObserver.SetViewWidth(view_width);

	mSenseObserver.SetStamina(stamina);
	mSenseObserver.SetEffort(effort);
	mSenseObserver.SetCapacity(capacity);

	mSenseObserver.SetSpeed(speed);
	mSenseObserver.SetSpeedDir(speed_dir);
	mSenseObserver.SetNeckDir(neck_dir);

	mSenseObserver.SetKickCount(kicks);
	mSenseObserver.SetDashCount(dashes);
	mSenseObserver.SetTurnCount(turns);
	mSenseObserver.SetSayCount(says);
	mSenseObserver.SetTurnNeckCount(turn_necks);
	mSenseObserver.SetCatchCount(catchs);
	mSenseObserver.SetMoveCount(moves);
	mSenseObserver.SetChangeViewCount(change_views);

	mSenseObserver.SetArmMovableBan(arm_movable_ban);
	mSenseObserver.SetArmExpires(arm_expires);
	mSenseObserver.SetArmTargetDist(arm_target_dist);
	mSenseObserver.SetArmTargetDir(arm_target_dir);
	mSenseObserver.SetArmCount(arm_count);

	mSenseObserver.SetFocusSide(focus_side);
	mSenseObserver.SetFocusUnum(focus_unum);
	mSenseObserver.SetFocusCount(focus_count);

	mSenseObserver.SetTackleExpires(tackle_expires);
	mSenseObserver.SetTackleCount(tackle_count);

	mSenseObserver.SetSenseTime(sense_time);
	mSenseObserver.SetFoulChargedCycle(foul_charged_cycle);
	mSenseObserver.SetCardType(card_type);
}


void Observer::SetSensePartialBody(
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
)
{
	mSenseObserver.SetViewWidth(view_width);

	mSenseObserver.SetKickCount(kicks);
	mSenseObserver.SetDashCount(dashes);
	mSenseObserver.SetTurnCount(turns);
	mSenseObserver.SetSayCount(says);
	mSenseObserver.SetTurnNeckCount(turn_necks);
	mSenseObserver.SetCatchCount(catchs);
	mSenseObserver.SetMoveCount(moves);
	mSenseObserver.SetChangeViewCount(change_views);

	mSenseObserver.SetArmMovableBan(arm_movable_ban);
	mSenseObserver.SetArmExpires(arm_expires);
	mSenseObserver.SetArmTargetDist(arm_target_dist);
	mSenseObserver.SetArmTargetDir(arm_target_dir);
	mSenseObserver.SetArmCount(arm_count);

	mSenseObserver.SetSenseTime(sense_time);
}

void Observer::HearOurCoachSay(const std::string & hear_content)
{
    mAudioObserver.SetOurCoachSayValid(true);
    mAudioObserver.SetOurCoachSayConent(hear_content);
}


void Observer::HearTeammateSay(AngleDeg hear_dir, Unum hear_unum, const std::string & hear_content)
{
	Logger::instance().GetTextLogger("freeform") << "\n#\n" << CurrentTime() << " hear from tm " << hear_unum << std::endl;

    mAudioObserver.SetTeammateSayValid(true);
    mAudioObserver.SetTeammateSayConent(hear_content);
	mAudioObserver.SetHearDir(hear_dir);
	mAudioObserver.SetHearUnum(hear_unum);
}


//==============================================================================
bool Observer::WaitForNewInfo()
{
	Lock();
	Reset();
	UnLock();

	bool flag = false;
	if (ServerParam::instance().synchMode()) {
		flag = WaitForNewThink();
	}
	else {
		if (PlayerParam::instance().isCoach() || PlayerParam::instance().isTrainer()) {
			flag = WaitForNewSight(); //see_global 信息
			WaitForCoachNewHear();
		}
		else {
			flag = WaitForNewSense(); //首先等到sense信息，然后在花40毫秒等一下hear和sight
			WaitForNewSight();
		}
	}

	return flag;
}

bool Observer::WaitForNewThink()
{
	int max_time = PlayerParam::instance().WaitTimeOut() * 1000 * ServerParam::instance().slowDownFactor();

	bool ret = true;
	if (mThinkArrived == false) {
		bool timeout = mCondNewThink.Wait(max_time);
		if (timeout) {
			ret = false;
		}
	}
	mThinkArrived = false;
	return ret;
}


//==============================================================================
bool Observer::WaitForNewSense()
{
	bool ret = true;
	if (mSenseArrived == false || mIsBeginDecision == false){
		bool timeout = mCondNewSense.Wait(PlayerParam::instance().WaitTimeOut() * 1000 * ServerParam::instance().slowDownFactor());
		if (timeout){
			ret = false;
		}

        if (mIsBeginDecision == false){
            mIsBeginDecision = true; // set to true after the first wait and will be never changed
        }
	}
	mSenseArrived = false;  // reset the indication of new visual information
	return ret;
}


//==============================================================================
bool Observer::WaitForNewSight()
{
	int max_time;

	if (PlayerParam::instance().isCoach() || PlayerParam::instance().isTrainer())
	{
		max_time = PlayerParam::instance().WaitTimeOut() * 1000 * ServerParam::instance().slowDownFactor();
	}
	else
	{
        // TODO: rcssserver13.2.0开始，所有信息均在周期一开始就发，这里暂时不判断是否有视觉，所有周期均等WaitSightBuffer
        if (true /** WillBeNewSight() */)
		{
			max_time = ServerParam::instance().synchSeeOffset() + PlayerParam::instance().WaitSightBuffer();
		}
		else
		{
			max_time = PlayerParam::instance().WaitHearBuffer(); // though there will be no sight msg, take 10ms to wait for the hear msg
		}
		max_time *= ServerParam::instance().slowDownFactor();
	}

	bool ret = true;
	if (mSightArrived == false){
		bool timeout = mCondNewSight.Wait(max_time);
		if (timeout){
			ret = false;
		}
	}
	mSightArrived = false;
	return ret;
}


//==============================================================================
bool Observer::WaitForCommandSend()
{
	bool flag = true;
	if (mIsCommandSend == false)
	{
		bool is_timeout = mCondCommandSend.Wait(PlayerParam::instance().WaitTimeOut() * 1000);
		if (is_timeout == true)
		{
			flag = false;
		}
	}

	mIsCommandSend = false;
	return flag;
}

//==============================================================================
bool Observer::WaitForCoachNewHear()
{
    int max_time = PlayerParam::instance().WaitHearBuffer() * ServerParam::instance().slowDownFactor();
    mCondCoachNewHear.Wait(max_time); // 注意：coach的hear信息每周期有很多，这里只wait，不set
	return true;
}

//==============================================================================
void Observer::SetNewSense()
{
	mIsNewSense = true;
	mSenseArrived = true;
	mSightArrived = false; // sight不可能比sense更早到，在这里暂时修正一下，rcssserver-13.0出来后再改
    mThinkArrived = false;	
    ResetSight();
	mCondNewSense.Set();
}

void Observer::SetNewThink()
{
	mIsNewThink = true;
	mSightArrived = true;
	mThinkArrived = true;
	mCondNewThink.Set();
}

//==============================================================================
void Observer::SetNewSight()
{
	mIsNewSight = true;
	mSightArrived = true;
	mThinkArrived = false;
	mCondNewSight.Set();
}

//==============================================================================
void Observer::SetCommandSend()
{
	mIsCommandSend = true;
	mCondCommandSend.Set();
}


void Observer::HearBall(const Vector & pos, const Vector & vel)
{
	//std::cerr << mCurrentTime << " " << mSelfUnum << " hear ball " << pos << " " << vel << std::endl;
	mAudioObserver.SetBall(pos, vel, mCurrentTime);
}

void Observer::HearBall(const Vector & pos)
{
	//std::cerr << mCurrentTime << " " << mSelfUnum << " hear ball " << pos << " " << std::endl;
	mAudioObserver.SetBall(pos, mCurrentTime);
}

void Observer::HearTeammate(Unum num, const Vector & pos)
{
	//std::cerr << mCurrentTime << " " << mSelfUnum << " hear teammate " << num << " " << pos << std::endl;
	mAudioObserver.SetTeammate(num, pos, mCurrentTime);
}

void Observer::HearOpponent(Unum num, const Vector & pos)
{
	//std::cerr << mCurrentTime << " " << mSelfUnum << " hear opponent " << num << " " << pos << std::endl;
	mAudioObserver.SetOpponent(num, pos, mCurrentTime);
}

bool Observer::WillBeNewSight()
{
	RealTime next_sight_time;
	switch (Sense().GetViewWidth())
	{
	case VW_Narrow: return true;
	case VW_Normal: next_sight_time = GetLastSightRealTime() + 2 * ServerParam::instance().simStep(); break;
	case VW_Wide:   next_sight_time = GetLastSightRealTime() + 3 * ServerParam::instance().simStep(); break;
	default:        PRINT_ERROR("view width error"); return true; /** bug is here, wait anyway */
	}

	if (next_sight_time - GetLastCycleBeginRealTime() <
        ServerParam::instance().synchSeeOffset() + PlayerParam::instance().WaitSightBuffer())
	{
		return true;
	}
	else
	{
		return false;
	}
}
