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

#include <cstdlib>
#include "WorldState.h"
#include "ActionEffector.h"
#include "Formation.h"
#include "Observer.h"
#include "Logger.h"
#include "Tackler.h"
#include <fstream>

const double WorldStateUpdater::KICKABLE_BUFFER = 0.04;
const double WorldStateUpdater::CATCHABLE_BUFFER = 0.04;

WorldState::WorldState(HistoryState* history_state):
	mpHistory( history_state),
	mCurrentTime( Time(-3, 0)),
	mPlayMode( PM_No_Mode),
	mPlayModeTime( Time(-3,0)),
	mIsBallDropped( false),
	mTeammateGoalieUnum( 0),
	mOpponentGoalieUnum( 0),
	mTeammateScore( 0),
	mOpponentScore( 0),
	mIsCycleStopped( false)
{
	//为各个球员分配号码
	for ( Unum i = 1; i <= TEAMSIZE; i++) {
		mTeammate[i].UpdateUnum(i);
		mOpponent[i].UpdateUnum(-i);

		mPlayerList.push_back(& mTeammate[i]);
		mPlayerList.push_back(& mOpponent[i]);
	}
}

WorldState * WorldState::GetHistory(int i) const
{
	if (i == 0) {
		return const_cast<WorldState *>(this);
	}
	else if (i > HistoryState::HISTORY_SIZE) {
		return 0;
	}

	return mpHistory->GetHistory(i);
}

Time WorldState::GetTimeBeforeCurrent(int cycle) const
{
	Assert(cycle <= HistoryState::HISTORY_SIZE && cycle >= 1);

	return mpHistory->GetHistory(cycle)->mCurrentTime;
}

void WorldState::UpdateFromObserver(Observer *observer)
{
	WorldStateUpdater(observer, this).Run();
}

void WorldState::GetReverseFrom(WorldState *world_state)
{
	mCurrentTime = world_state->CurrentTime();
	mPlayMode    = world_state->GetPlayMode();
	mIsCycleStopped = world_state->mIsCycleStopped;
	mPlayModeTime = world_state->mPlayModeTime;

	mTeammateGoalieUnum = world_state->mOpponentGoalieUnum;
	mOpponentGoalieUnum = world_state->mTeammateGoalieUnum;

	mTeammateScore = world_state->mOpponentScore;
	mOpponentScore = world_state->mTeammateScore;

	Ball().GetReverseFrom(world_state->Ball());

	for (Unum i = 1; i <= TEAMSIZE; ++i){
		Opponent(i).GetReverseFrom(world_state->Teammate(i));
		Teammate(i).GetReverseFrom(world_state->Opponent(i));
	}
}

BallState & WorldStateUpdater::Ball()
{
	return mpWorldState->mBall;
}

PlayerState & WorldStateUpdater::Teammate(Unum i)
{
	return mpWorldState->Teammate(i);
}

PlayerState & WorldStateUpdater::Opponent(Unum i)
{
	return mpWorldState->Opponent(i);
}

PlayerState & WorldStateUpdater::SelfState()
{
	return Teammate(mSelfUnum);
}

const BallState & WorldStateUpdater::GetBall()
{
	return mpWorldState->mBall;
}

const PlayerState & WorldStateUpdater::GetTeammate(Unum i)
{
	return mpWorldState->Teammate(i);
}

const PlayerState & WorldStateUpdater::GetOpponent(Unum i)
{
	return mpWorldState->Opponent(i);
}

const PlayerState & WorldStateUpdater::GetSelf()
{
	return Teammate(mSelfUnum);
}

AngleDeg WorldStateUpdater::GetNeckGlobalDirFromSightDelay(int sight_delay)
{
	Assert(sight_delay >= 0 && sight_delay <= HistoryState::HISTORY_SIZE);

	if (sight_delay == 0)
	{
		return GetSelf().GetNeckGlobalDir();
	}
	else
	{
		sight_delay = Min(sight_delay, int(HistoryState::HISTORY_SIZE));
		double neck_dir = mpWorldState->GetHistory(sight_delay)->GetTeammate(mSelfUnum).GetNeckDir();
		return GetSelf().GetBodyDir() + neck_dir;
	}
}

Vector WorldStateUpdater::GetSelfVelFromSightDelay(int sight_delay)
{
	Assert(sight_delay >= 0 && sight_delay <= HistoryState::HISTORY_SIZE);

	if (sight_delay == 0)
	{
		return GetSelf().GetVel();
	}
	else
	{
		sight_delay = Min(sight_delay, int(HistoryState::HISTORY_SIZE));
		const PlayerState& player = mpWorldState->GetHistory(sight_delay)->GetTeammate(mSelfUnum);
		return player.GetVel().Rotate(-player.GetNeckGlobalDir()).Rotate(GetNeckGlobalDirFromSightDelay(sight_delay));
	}
}


Unum WorldStateUpdater::GetSelfUnum()
{
	return mSelfUnum;
}

char WorldStateUpdater::GetSelfSide()
{
	return mSelfSide;
}


void WorldStateUpdater::Run()
{
	UpdateWorldState();
}

//==============================================================================
void WorldStateUpdater::UpdateActionInfo()
{
	const Vector & ball_pos = GetBall().GetPos();

	for (unsigned i = 0; i < mpWorldState->GetPlayerList().size(); ++i) {
		PlayerState & player = const_cast<PlayerState &>(*mpWorldState->GetPlayerList()[i]);
		if (!player.IsAlive()) continue;

		Vector ball_2_player = (ball_pos - player.GetPos()).Rotate(-player.GetBodyDir());

		if (!player.IsIdling() && ball_2_player.Mod() < player.GetKickableArea() - KICKABLE_BUFFER * Sign(player.GetUnum())) {
			player.UpdateKickable(true);
			player.UpdateKickRate(GetKickRate(ball_2_player, player.GetPlayerType()));
		}
		else {
			player.UpdateKickable(false);
			player.UpdateKickRate(0.0);
		}

		player.UpdateBallCatchable(false);
		if (player.GetUnum() == mpWorldState->GetTeammateGoalieUnum() || player.GetUnum() == -mpWorldState->GetOpponentGoalieUnum()) {
			if ((mpWorldState->GetPlayMode() == PM_Play_On || mpWorldState->GetPlayMode() == PM_Opp_Penalty_Taken) && ball_2_player.Mod() < ServerParam::instance().maxCatchableArea()  - CATCHABLE_BUFFER) {
				if (player.GetUnum() == mpWorldState->GetTeammateGoalieUnum()) {
					if (ServerParam::instance().ourPenaltyArea().IsWithin(ball_pos) && ServerParam::instance().ourPenaltyArea().IsWithin(player.GetPos())) {
						player.UpdateBallCatchable(true);
					}
				}
				else {
					if (ServerParam::instance().oppPenaltyArea().IsWithin(ball_pos) && ServerParam::instance().oppPenaltyArea().IsWithin(player.GetPos())) {
						player.UpdateBallCatchable(true);
					}
				}
			}
		}

		player.UpdateMaxTurnAngle(GetMaxTurnAngle(player.GetPlayerType(), player.GetVel().Mod()));
	}

	for (unsigned i = 0; i < mpWorldState->GetPlayerList().size(); ++i) { //这里要等到可踢信息更新完才能更新
		PlayerState & player = const_cast<PlayerState &>(*mpWorldState->GetPlayerList()[i]);
		if (!player.IsAlive()) continue;

		player.UpdateTackleProb(ComputeTackleProb(player.GetUnum(), false), false);
		player.UpdateTackleProb(ComputeTackleProb(player.GetUnum(), true), true);

		if (PlayerParam::instance().playerVersion() > 14.0) {
			player.UpdateDangerousTackleCondition(Tackler::MayDangerousIfTackle(player, *mpWorldState));
		}
	}
}

/**
 * 从observer里面更新世界状态
 */
void WorldStateUpdater::UpdateWorldState()
{
	/**更新场上的状态*/
	UpdateFieldInfo();

	if (PlayerParam::instance().isCoach()){
		if (mpObserver->OurSide() == 'l'){ //server发过来的信息坐标与自己一致
			Ball().UpdatePos(mpObserver->Ball_Coach().GetPos(), 0, 1.0);
			Ball().UpdateVel(mpObserver->Ball_Coach().GetVel(), 0, 1.0);

			for (Unum i = 1;i <= TEAMSIZE;i++)
			{
				Teammate(i).SetIsAlive(mpObserver->Teammate_Coach(i).IsAlive());
				Teammate(i).UpdatePlayerType(mpObserver->GetTeammateType(i));
				Teammate(i).UpdatePos(mpObserver->Teammate_Coach(i).GetPos(), 0, 1.0);
				Teammate(i).UpdateVel(mpObserver->Teammate_Coach(i).GetVel(), 0, 1.0);
				Teammate(i).UpdateBodyDir(mpObserver->Teammate_Coach(i).GetBodyDir(), 0, 1.0);
				Teammate(i).UpdateNeckDir(mpObserver->Teammate_Coach(i).GetNeckDir(), 0, 1.0);
				Teammate(i).UpdateCardType(mpObserver->Teammate_Coach(i).GetCardType());
				//TODO: arm, tackle lying等，暂时不用 -- TODO

				Opponent(i).SetIsAlive(mpObserver->Opponent_Coach(i).IsAlive());
				Opponent(i).UpdatePos(mpObserver->Opponent_Coach(i).GetPos(), 0, 1.0);
				Opponent(i).UpdateVel(mpObserver->Opponent_Coach(i).GetVel(), 0, 1.0);
				Opponent(i).UpdateBodyDir(mpObserver->Opponent_Coach(i).GetBodyDir(), 0, 1.0);
				Opponent(i).UpdateNeckDir(mpObserver->Opponent_Coach(i).GetNeckDir(), 0, 1.0);
				Opponent(i).UpdateCardType(mpObserver->Opponent_Coach(i).GetCardType());
			}
		}
		else {//server发过来的信息坐标与自己相反，因为代码里总是假设自己在左边
			Ball().UpdatePos(mpObserver->Ball_Coach().GetPos() * -1.0, 0, 1.0);
			Ball().UpdateVel(mpObserver->Ball_Coach().GetVel() * -1.0, 0, 1.0);

			for (Unum i = 1;i <= TEAMSIZE;i++)
			{
				Teammate(i).SetIsAlive(mpObserver->Teammate_Coach(i).IsAlive());
				Teammate(i).UpdatePlayerType(mpObserver->GetTeammateType(i));
				Teammate(i).UpdatePos(mpObserver->Teammate_Coach(i).GetPos() * -1.0, 0, 1.0);
				Teammate(i).UpdateVel(mpObserver->Teammate_Coach(i).GetVel() * -1.0, 0, 1.0);
				Teammate(i).UpdateBodyDir(GetNormalizeAngleDeg(mpObserver->Teammate_Coach(i).GetBodyDir() + 180.0), 0, 1.0);
				Teammate(i).UpdateNeckDir(GetNormalizeAngleDeg(mpObserver->Teammate_Coach(i).GetNeckDir() + 180.0), 0, 1.0);
				Teammate(i).UpdateCardType(mpObserver->Teammate_Coach(i).GetCardType());
				//TODO: arm, tackle 等，暂时不用

				Opponent(i).SetIsAlive(mpObserver->Opponent_Coach(i).IsAlive());

				Opponent(i).UpdatePos(mpObserver->Opponent_Coach(i).GetPos() * -1.0, 0, 1.0);
				Opponent(i).UpdateVel(mpObserver->Opponent_Coach(i).GetVel() * -1.0, 0, 1.0);
				Opponent(i).UpdateBodyDir(GetNormalizeAngleDeg(mpObserver->Opponent_Coach(i).GetBodyDir() + 180.0), 0, 1.0);
				Opponent(i).UpdateNeckDir(GetNormalizeAngleDeg(mpObserver->Opponent_Coach(i).GetNeckDir() + 180.0), 0, 1.0);
				Opponent(i).UpdateCardType(mpObserver->Opponent_Coach(i).GetCardType());
			}
		}

		if (mpWorldState->GetTeammateGoalieUnum() > 0) Teammate(mpWorldState->GetTeammateGoalieUnum()).UpdateIsGoalie(true);
		if (mpWorldState->GetOpponentGoalieUnum() > 0) Opponent(mpWorldState->GetOpponentGoalieUnum()).UpdateIsGoalie(true);
		UpdateActionInfo();
	}
	else
	{
		// this procedure updates history state and there is a similar procedure in UpdateFromAudio
		// but updates current state.
		if (mpObserver->Ball_Coach().GetPosDelay() == 0)
		{
			WorldState & history = *mpWorldState->mpHistory->GetHistory(1);
			history.Ball().UpdatePos(mpObserver->Ball_Coach().GetPos(), 0, 1.0);
			history.Ball().UpdateGuessedTimes(0);
			history.Ball().UpdateVel(mpObserver->Ball_Coach().GetVel(), 0, 1.0);
			history.Ball().UpdatePosEps(0);
			history.Ball().UpdateVelEps(0);

			for (Unum i = 1;i <= TEAMSIZE;i++)
			{
				history.Teammate(i).SetIsAlive(mpObserver->Teammate_Coach(i).IsAlive());
				history.Teammate(i).UpdatePos(mpObserver->Teammate_Coach(i).GetPos(), 0, 1.0);
				history.Teammate(i).UpdateGuessedTimes(0);
				history.Teammate(i).UpdateVel(mpObserver->Teammate_Coach(i).GetVel(), 0, 1.0);
				history.Teammate(i).UpdatePosEps(0);
				history.Teammate(i).UpdateVelEps(0);
				history.Teammate(i).UpdateBodyDir(mpObserver->Teammate_Coach(i).GetBodyDir(), 0, 1.0);
				history.Teammate(i).UpdateNeckDir(mpObserver->Teammate_Coach(i).GetNeckDir(), 0, 1.0);
				if (i != mSelfUnum)
				{
					history.Teammate(i).UpdateStamina(mpObserver->Teammate_Coach(i).GetStamina());
					history.Teammate(i).UpdateCapacity(mpObserver->Teammate_Coach(i).GetCapacity());
				}

				history.Opponent(i).SetIsAlive(mpObserver->Opponent_Coach(i).IsAlive());
				history.Opponent(i).UpdatePlayerType(mpObserver->Opponent_Coach(i).GetPlayerType());
				history.Opponent(i).UpdatePos(mpObserver->Opponent_Coach(i).GetPos(), 0, 1.0);
				history.Opponent(i).UpdateGuessedTimes(0);
				history.Opponent(i).UpdateVel(mpObserver->Opponent_Coach(i).GetVel(), 0, 1.0);
				history.Opponent(i).UpdatePosEps(0);
				history.Opponent(i).UpdateVelEps(0);
				history.Opponent(i).UpdateBodyDir(mpObserver->Opponent_Coach(i).GetBodyDir());
				history.Opponent(i).UpdateNeckDir(mpObserver->Opponent_Coach(i).GetNeckDir());
				history.Opponent(i).UpdateStamina(mpObserver->Opponent_Coach(i).GetStamina());
				history.Opponent(i).UpdateCapacity(mpObserver->Opponent_Coach(i).GetCapacity());
			}
		}

		//更新所有的delay delay加上1 conf乘以1
		AutoDelay(1);

		//更新所有人的player_type和is_goalie
		for (int i = 1;i <= TEAMSIZE;i++)
		{
			Teammate(i).UpdatePlayerType(mpObserver->GetTeammateType(i));
//			Opponent(i).UpdatePlayerType(mpObserver->GetOpponentType(i)); // 在UpdateFromAudio中更新,在此不更新

			Teammate(i).UpdateIsGoalie(false);
			Opponent(i).UpdateIsGoalie(false);
		}

		if (mpWorldState->GetTeammateGoalieUnum() > 0) Teammate(mpWorldState->GetTeammateGoalieUnum()).UpdateIsGoalie(true);
		if (mpWorldState->GetOpponentGoalieUnum() > 0) Opponent(mpWorldState->GetOpponentGoalieUnum()).UpdateIsGoalie(true);
//	int id =	TimeTest::instance().Begin("before");
		//更新自己的听觉信息
		UpdateFromAudio();

		//预测一周期
		EstimateWorld();

		if (mpObserver->IsNewSight())
		{
			UpdateSightDelay();
		}
/*	TimeTest::instance().End(id);
	id = TimeTest::instance().Begin("test");
	TimeTest::instance().End(id);
	id = TimeTest::instance().Begin("middle");*/
		//更新自己的信息
		UpdateSelfInfo();

		if (mpObserver->IsNewSight())
		{
			//更新所有可见的队员的信息
			UpdateKnownPlayers();

			//更新所有未知号码的球员
			UpdateUnknownPlayers();

			//更新球的信息
			UpdateBallInfo();

			EstimateToNow();
		}
/*	TimeTest::instance().End(id);

	id = TimeTest::instance().Begin("end");*/
		/**处理碰撞*/
		UpdateInfoWhenCollided();

		//根据playmode更新确定的位置
		UpdateInfoFromPlayMode();

		//重新评估各球员的置信度 及置球员的生死
		EvaluateConf();

		//更新Action的信息
		UpdateActionInfo();

		MaintainPlayerStamina();

		MaintainConsistency();
//	TimeTest::instance().End(id);
//
	}
}

void WorldStateUpdater::UpdateSightDelay()
{
	mSightDelay = mpObserver->CurrentTime() - mpObserver->LatestSightTime();
	mBallConf   = ComputeConf(PlayerParam::instance().ballConfDecay() , mSightDelay);
	mPlayerConf = ComputeConf(PlayerParam::instance().playerConfDecay() , mSightDelay);
}

double WorldStateUpdater::ComputeConf(double decay , int cycle)
{
	if (cycle <= 0)
	{
		return 1;
	}

	double sum = 1;
	for (int i = 0;i < cycle;i++)
	{
		sum *= decay;
	}
	return sum;
}

void WorldStateUpdater::UpdateFromAudio()
{
    // update by our coach say
    if (mpObserver->Audio().IsOurCoachSayValid())
    {
        mpObserver->ResetOurCoachSayValid(); // 恢复
        if(mpObserver->IsNewOppType())
        {
			for (int i = 1; i <= TEAMSIZE; ++i)
			{
				if (mpObserver->Audio().GetOpponentType(i) > 0)
				{
					Opponent(i).UpdatePlayerType(mpObserver->Audio().GetOpponentType(i));
				}
			}
			mpObserver->SetNewOppType(false);
        }

    }

	if (mpObserver->Ball_Coach().GetPosDelay() == 0)
	{
		mpObserver->Ball_Coach().AutoUpdate(1, PlayerParam::instance().ballConfDecay());

		Ball().UpdatePos(mpObserver->Ball_Coach().GetPos(), 1, PlayerParam::instance().ballConfDecay());
		Ball().UpdateGuessedTimes(0);
		Ball().UpdateVel(mpObserver->Ball_Coach().GetVel(), 1, PlayerParam::instance().ballConfDecay());
		Ball().UpdatePosEps(0);
		Ball().UpdateVelEps(0);

		for (Unum i = 1;i <= TEAMSIZE;i++)
		{
			Teammate(i).SetIsAlive(mpObserver->Teammate_Coach(i).IsAlive());
			Teammate(i).UpdatePos(mpObserver->Teammate_Coach(i).GetPos(), 1, PlayerParam::instance().playerConfDecay());
			Teammate(i).UpdateGuessedTimes(0);
			Teammate(i).UpdateVel(mpObserver->Teammate_Coach(i).GetVel(), 1, PlayerParam::instance().playerConfDecay());
			Teammate(i).UpdatePosEps(0);
			Teammate(i).UpdateVelEps(0);
			Teammate(i).UpdateBodyDir(mpObserver->Teammate_Coach(i).GetBodyDir(), 1, PlayerParam::instance().playerConfDecay());
			Teammate(i).UpdateNeckDir(mpObserver->Teammate_Coach(i).GetNeckDir(), 1, PlayerParam::instance().playerConfDecay());
			if (i != mSelfUnum)
			{
				Teammate(i).UpdateStamina(mpObserver->Teammate_Coach(i).GetStamina());
				Teammate(i).UpdateCapacity(mpObserver->Teammate_Coach(i).GetCapacity());
			}

			Opponent(i).SetIsAlive(mpObserver->Opponent_Coach(i).IsAlive());
			Opponent(i).UpdatePlayerType(mpObserver->Opponent_Coach(i).GetPlayerType());
			Opponent(i).UpdatePos(mpObserver->Opponent_Coach(i).GetPos(), 1, PlayerParam::instance().playerConfDecay());
			Opponent(i).UpdateGuessedTimes(0);
			Opponent(i).UpdateVel(mpObserver->Opponent_Coach(i).GetVel(), 1, PlayerParam::instance().playerConfDecay());
			Opponent(i).UpdatePosEps(0);
			Opponent(i).UpdateVelEps(0);
			Opponent(i).UpdateBodyDir(mpObserver->Opponent_Coach(i).GetBodyDir());
			Opponent(i).UpdateNeckDir(mpObserver->Opponent_Coach(i).GetNeckDir());
			Opponent(i).UpdateStamina(mpObserver->Opponent_Coach(i).GetStamina());
			Opponent(i).UpdateCapacity(mpObserver->Opponent_Coach(i).GetCapacity());
		}
	}
	else if (mpObserver->Audio().IsTeammateSayValid())
	{
        // update by teammate say
        mpObserver->ResetTeammateSayValid();

		int type = mpObserver->Audio().GetHearInfoType();
		Unum sender = mpObserver->Audio().GetHearUnum();

		//更新听到球的信息
		int cmp = type &  AudioObserver::HearInfo_Ball;
		if (cmp != 0)
		{
			if (Ball().GetGuessedTimes() > 0 ||Ball().GetPosConf() < PlayerParam::instance().minValidConf()  - FLOAT_EPS || (Teammate(sender).IsAlive() && Teammate(sender).GetPos().Dist2(Ball().GetPos()) < SelfState().GetPos().Dist2(Ball().GetPos()))) {
				if (mpObserver->Audio().GetBallPos().time() == mpObserver->CurrentTime())
				{
					Ball().UpdatePos(mpObserver->Audio().GetBallPos().value(), 1, PlayerParam::instance().ballConfDecay() - FLOAT_EPS);
					Ball().UpdateGuessedTimes(0);
					//0.3为经验值 0.75 * 4
					double eps = 1.5 + PlayerParam::instance().GetEpsInSight((Ball().GetPos() - Teammate(sender).GetPos()).Mod());
					Ball().UpdatePosEps(eps);
				}

				if (mpObserver->Audio().GetBallVel().time() == mpObserver->CurrentTime())
				{
					Ball().UpdateVel(mpObserver->Audio().GetBallVel().value(), 1, PlayerParam::instance().ballConfDecay() - FLOAT_EPS);

					//统计得到看见的速度一般误差不大于1
					Ball().UpdateVelEps(3);
				}
			}
		}

		//更新听到队友的信息
		cmp = type & AudioObserver::HearInfo_Teammate;
		if (cmp != 0)
		{
			int teammate_count = mpObserver->Audio().GetTeammateCount();
			for (int i = 0;i < teammate_count;i++)
			{
				if (mpObserver->Audio().GetTeammate(i).time() == mpObserver->CurrentTime())
				{
					int unum = mpObserver->Audio().GetTeammate(i).value().mUnum;
					if (Teammate(unum).GetGuessedTimes() > 0 || Teammate(unum).GetPosConf() < PlayerParam::instance().playerConfDecay()  - FLOAT_EPS || (Teammate(sender).IsAlive() && Teammate(sender).GetPos().Dist2(Teammate(unum).GetPos()) < SelfState().GetPos().Dist2(Teammate(unum).GetPos()))) {
						Teammate(unum).UpdateGuessedTimes(0);
						Teammate(unum).SetIsAlive(true);
						const Vector & pos = mpObserver->Audio().GetTeammate(i).value().mPos;

						Teammate(unum).UpdatePos(pos, 1, PlayerParam::instance().playerConfDecay() - FLOAT_EPS);

						double eps = 0.1 + PlayerParam::instance().GetEpsInSight( (pos - Teammate(sender).GetPos()).Mod());
						Teammate(unum).UpdatePosEps(eps);
					}
				}
			}
		}

		//更新听到的对手的信息
		cmp = type & AudioObserver::HearInfo_Opponent;
		if (cmp != 0)
		{
			int Opponent_count = mpObserver->Audio().GetOpponentCount();
			for (int i = 0;i < Opponent_count;i++)
			{
				if (mpObserver->Audio().GetOpponent(i).time() == mpObserver->CurrentTime())
				{
					int unum = mpObserver->Audio().GetOpponent(i).value().mUnum;
					if (Opponent(unum).GetGuessedTimes() > 0 || Opponent(unum).GetPosConf() < PlayerParam::instance().playerConfDecay() - FLOAT_EPS || (Teammate(sender).IsAlive() && Teammate(sender).GetPos().Dist2(Opponent(unum).GetPos()) < SelfState().GetPos().Dist2(Opponent(unum).GetPos())))
					{
						Opponent(unum).UpdateGuessedTimes(0);
						Opponent(unum).SetIsAlive(true);
						const Vector & pos = mpObserver->Audio().GetOpponent(i).value().mPos;

						Opponent(unum).UpdatePos(pos, 1, PlayerParam::instance().playerConfDecay() - FLOAT_EPS);
						double eps = 0.1 + PlayerParam::instance().GetEpsInSight( (pos - Teammate(sender).GetPos()).Mod());
						Opponent(unum).UpdatePosEps(eps);
					}
				}
			}
		}
	}
}

void WorldStateUpdater::UpdateFieldInfo()
{
	mpWorldState->mCurrentTime = mpObserver->CurrentTime();
	mpWorldState->mIsBallDropped = mpObserver->IsBallDropped();
	mpWorldState->mKickOffMode  = mpObserver->GetKickOffMode();

	if (mpWorldState->mPlayMode != mpObserver->GetPlayMode())
    {
		mpWorldState->mLastPlayMode = mpWorldState->mPlayMode;
        mpWorldState->mPlayMode = mpObserver->GetPlayMode();
        mpWorldState->mPlayModeTime = mpObserver->CurrentTime();
    }

    Formation::instance.SetOpponentGoalieUnum(mpObserver->OppGoalieUnum()); // 设置OpponentFormation的守门员号码
    mpWorldState->mTeammateGoalieUnum = PlayerParam::instance().ourGoalieUnum();
	mpWorldState->mOpponentGoalieUnum = mpObserver->OppGoalieUnum();

    mpWorldState->mTeammateScore = mpObserver->OurScore();
	mpWorldState->mOpponentScore = mpObserver->OppScore();

	if (mpWorldState->GetHistory(1) && mpWorldState->GetHistory(1)->CurrentTime().T() == mpWorldState->mCurrentTime.T())
	{
		mpWorldState->mIsCycleStopped = true;
	}
	else
	{
		mpWorldState->mIsCycleStopped = false;
	}
}

namespace
{
	double calcEps(double a, double b , double angle_deg)
	{
		return Sqrt(a * a + b * b - 2 * a * b * Cos(angle_deg));
	}
}



void WorldStateUpdater::UpdateSelfInfo()
{

	SelfState().SetIsAlive(true);
    SelfState().UpdateIsSensed(true); // “真正”的自己

	//=============================更新自己的sense========================
	UpdateSelfSense();
/*
	//==========================according to speed of this cycle estimate position of this cycle=======================
	if (mpObserver->GetPlayerMoveTime() != mpObserver->CurrentTime())
	{
		Vector vel = SelfState().GetVel();

		if (mpObserver->Sense().IsCollideWithBall() || mpObserver->Sense().IsCollideWithPlayer() || mpObserver->Sense().IsCollideWithPost())
		{
			//when collided ,velocity should be changed to normal;
			vel /= -0.1;
		}

		Vector last_pos = mpWorldState->GetHistory(1)->Teammate(GetSelfUnum()).GetPos();
		SelfState().UpdatePos(last_pos + vel / ServerParam::instance().ballDecay() , SelfState().GetPosDelay() ,SelfState().GetPosConf());
	}
*/

	//到此为止自己的身体角度和脖子角度更新对了
	//计算公式见USTC_Material 第七章
	//playerVel = polar2vector(speedValue, neckGlobalAngle + speedAngle)
	Vector vec = Polar2Vector(mpObserver->Sense().GetSpeed(), mpObserver->Sense().GetSpeedDir() + GetSelf().GetNeckGlobalDir());
	SelfState().UpdateVel(vec);

	//用自己更新后的速度预测自己的位置,比之前的预测准.见08
	if (!mpObserver->Sense().IsCollideWithBall() && !mpObserver->Sense().IsCollideWithPlayer() && mpObserver->GetPlayerMoveTime() != mpObserver->CurrentTime())
	{
		const Vector & pre_pos = mpWorldState->GetHistory(1)->GetTeammate(mSelfUnum).GetPos();
		double conf = mpWorldState->GetHistory(1)->GetTeammate(mSelfUnum).GetPosConf();

		if (conf > PlayerParam::instance().minValidConf())
		{

			Vector pos = pre_pos + vec / SelfState().GetPlayerDecay();
			SelfState().UpdatePos(pos , SelfState().GetPosDelay() , SelfState().GetPosConf());
		}
	}


	//评估自己位置误差 ,目前最大问题是server移人,而人不知.目前处理是如果dropball以及playmode模式改变是将位置eps加到极大
	if (mpWorldState->GetPlayModeTime() == mpWorldState->CurrentTime() || mpWorldState->IsBallDropped())
	{
		SelfState().UpdatePosEps(10000);
	}

	//===============================更新自己的视觉======================
	if (mpObserver->IsNewSight())
	{
		//更新自己的头部和身体角度
		double neck_globe_angle = 0.0;


		if (ComputeSelfDir(neck_globe_angle))
		{
			//计算公式见USTC_Material 第七章
			//bodyGlobalAngle = neckGlobalAngle – head_angle
			if (mSightDelay <= HistoryState::HISTORY_SIZE)
			{
				const PlayerState& player = mpWorldState->GetHistory(mSightDelay)->GetTeammate(mSelfUnum);
				SelfState().UpdateBodyDir(neck_globe_angle - player.GetNeckDir() , mSightDelay , mPlayerConf);
			}
			else
			{
				PRINT_ERROR("mSightDelay too big!!" << mSightDelay);
				SelfState().UpdateBodyDir(neck_globe_angle - SelfState().GetNeckDir() , mSightDelay , mPlayerConf);
			}
		}

		if (mSightDelay == 0)
		{
			//修正自己的速度 当且仅当不出现视觉延迟时.
			Vector vec = Polar2Vector(mpObserver->Sense().GetSpeed(), mpObserver->Sense().GetSpeedDir() + GetSelf().GetNeckGlobalDir());
			SelfState().UpdateVel(vec);
		}

		//更新自己的位置
		Vector pos;
		double eps = 0;
		if (ComputeSelfPos(pos , eps))
		{
			if (eps > SelfState().GetPosEps())
			{
				//根据统计值,x,y单个分量差值大于1基本不存在 因此取1
				if (pos.Dist2(SelfState().GetPos()) < 1)
				{
					pos = SelfState().GetPos();
					eps = SelfState().GetPosEps();
				}

			}
			else if (eps == SelfState().GetPosEps())
			{
				if (pos.Dist2(SelfState().GetPos()) < 1)
				{
					pos = (pos + SelfState().GetPos()) / 2;
				}
			}

			SelfState().UpdatePos(pos, mSightDelay, mPlayerConf);
			SelfState().UpdatePosEps(eps);
		}
	}

}

void WorldStateUpdater::UpdateSelfSense()
{
	SelfState().UpdateNeckDir(PlayerParam::instance().ConvertAngle(mpObserver->Sense().GetNeckDir())); //这个即使没有视觉也要更新

	SelfState().UpdateStamina(mpObserver->Sense().GetStamina()); //更新体力
	SelfState().UpdateEffort(mpObserver->Sense().GetEffort()); //更新effort
	SelfState().UpdateCapacity(mpObserver->Sense().GetCapacity()); //更新就体力容量

	//update recovery
	double buff = Min(SelfState().GetRecovery() * SelfState().GetStaminaIncMax() , SelfState().GetCapacity());
	buff = ((int)(buff * 100)) / 100.0; //精度设置为0.01 太精确了会影响计算
	if (mpObserver->CurrentTime().T() > 0 && mpObserver->Sense().GetStamina() - buff  < ServerParam::instance().staminaMax() * ServerParam::instance().recoverDecThr())
	{
		double recovery = SelfState().GetRecovery() - ServerParam::instance().recoverDec();
		if (recovery < ServerParam::instance().recoverMin())
		{
			recovery = ServerParam::instance().recoverMin();
		}

		if (mpObserver->CurrentTime().T() % ServerParam::instance().halfTime() == 0 && ServerParam::instance().nrNormalHalfs() >= 0 && mpObserver->CurrentTime().T() < ServerParam::instance().halfTime() * ServerParam::instance().nrNormalHalfs())
		{
			recovery = 1.0;
		}

		SelfState().UpdateRecovery(recovery);
	}

	//更新自己的tackleban
	SelfState().UpdateTackleBan(mpObserver->Sense().GetTackleExpires());
	SelfState().UpdateFoulChargedCycle(mpObserver->Sense().GetFoulChargedCycle());

	//更新自己的catchban 暂时没有

	//更新自己的armpoint
	if (mpObserver->Sense().GetArmMovableBan() > 0)
	{
		SelfState().UpdateArmPoint(mpObserver->Sense().GetArmTargetDir(),0,1,mpObserver->Sense().GetArmTargetDist(),mpObserver->Sense().GetArmMovableBan(),mpObserver->Sense().GetArmExpires());
	}
	else
	{
		SelfState().UpdateArmPoint(mpObserver->Sense().GetArmTargetDir(),SelfState().GetArmPointDelay(),SelfState().GetArmPointConf(),mpObserver->Sense().GetArmTargetDist(),mpObserver->Sense().GetArmMovableBan(),mpObserver->Sense().GetArmExpires());
	}


	//更新自己的FocusOn
	SelfState().UpdateFocusOn(mpObserver->Sense().GetFocusSide(), mpObserver->Sense().GetFocusUnum());

	//更新自己的视角
	SelfState().UpdateViewWidth(mpObserver->Sense().GetViewWidth());

	SelfState().UpdateCardType( mpObserver->Sense().GetCardType() );

}

void WorldStateUpdater::UpdateKnownPlayers()
{
	//暂时定队友的位置为最新的时间表示看到

	//更新看到的队友
	for (int i = 1;i <= TEAMSIZE;i++)
	{
		if (i == mSelfUnum)
		{
			continue;
		}

		if (mpObserver->Teammate(i).GetDir().time() == mpObserver->LatestSightTime())
		{
			UpdateSpecificPlayer(mpObserver->Teammate(i) , i , true);
		}
	}

	//更新看到的对手
	for (int i = 1;i <= TEAMSIZE;i++)
	{
		if (mpObserver->Opponent(i).GetDir().time() == mpObserver->LatestSightTime())
		{
			UpdateSpecificPlayer(mpObserver->Opponent(i) , i , false);
		}
	}
}

void WorldStateUpdater::UpdateSpecificPlayer(const PlayerObserver &player, Unum unum, bool is_teammate)
{
	//根据底层的parser类的信息 更新的有：位置，速度，身体角度，头部角度，手指向，是否铲球
	//原则是：只有结果是最新时才更新

	if (is_teammate)
	{
		Teammate(unum).SetIsAlive(true);
		Teammate(unum).UpdateGuessedTimes(0);
		//位置更新
		if (player.GetDir().time() == mpObserver->LatestSightTime())
		{
			Vector vec = Polar2Vector(PlayerParam::instance().ConvertSightDist(player.Dist()) , GetNeckGlobalDirFromSightDelay(mSightDelay) + player.Dir());
			vec = GetSelf().GetPos() + vec;
			Teammate(unum).UpdatePos(vec , mSightDelay , mPlayerConf);

			double eps = PlayerParam::instance().GetEpsInSight(player.Dist()) + SelfState().GetPosEps();
			Teammate(unum).UpdatePosEps(eps);
		}

		//速度更新
		if (player.GetDirChg().time() == mpObserver->LatestSightTime())
		{
			//公式：relPos = polar2vector(1.0, ballAngle)
			Vector vec = Polar2Vector(1.0 , player.Dir());
			double distChg = player.DistChg();
			double dirChg  = player.DirChg();
			double dist    = player.Dist();
			//计算公式见USTC_Material 第七章
			//公式：relVel = (distChg * relPos.x() - (dirChg * PI / 180 * ballDist * relPos.y()),distChg * rpos.y() + (dirChg * PI / 180 * ballDist * relPos.x())
			Vector relvel = Vector(distChg * vec.X() - (dirChg * M_PI / 180 * dist * vec.Y()) , distChg * vec.Y() + (dirChg * M_PI / 180 * dist * vec.X()));
			//公式：ballVel = playerVel() + relVel.rotate(neckGlobalAngle)
			relvel = GetSelfVelFromSightDelay(mSightDelay) + relvel.Rotate(GetNeckGlobalDirFromSightDelay(mSightDelay));
			Teammate(unum).UpdateVel(relvel , mSightDelay , mPlayerConf);
		}
		else if(player.GetDir().time() == mpObserver->LatestSightTime()) //只看到位置但是看不到速度的情况
		{
		//	if (player.Dist() < ServerParam::instance().visibleDistance() * 2)

			if (mpWorldState->GetHistory(1 + mSightDelay)) {
				int delay = mpWorldState->GetHistory(1 + mSightDelay)->Teammate(unum).GetPosDelay();
				if (delay  < HistoryState::HISTORY_SIZE - 1 - mSightDelay)
				{
					Vector 	vel = Teammate(unum).GetPos() - mpWorldState->GetHistory(1 + mSightDelay + delay)->Teammate(unum).GetPos();
					vel /= (delay + 1);

					//use  speed of self sense instead of pos of self sight to reduce the error
					Vector self_vel = GetSelfVelFromSightDelay(mSightDelay);
					if (mpObserver->Sense().IsCollideWithBall() || mpObserver->Sense().IsCollideWithPlayer() || mpObserver->Sense().IsCollideWithPost())
					{
						self_vel /= -0.1;
					}
					self_vel /= PlayerParam::instance().HeteroPlayer(SelfState().GetPlayerType()).playerDecay();
					Vector self_vel_pos = SelfState().GetPos() - mpWorldState->GetHistory(1 + mSightDelay)->Teammate(GetSelfUnum()).GetPos();
					vel += self_vel - self_vel_pos;

					double max_speed = PlayerParam::instance().HeteroPlayer(mpObserver->GetTeammateType(unum)).effectiveSpeedMax();
					//根据速度大致猜测朝向 有视觉时会覆盖
					if (vel.Mod() > max_speed * mpWorldState->GetTeammate(unum).GetPlayerDecay() * 0.8 && GetTeammate(unum).IsBodyDirMayChanged())
					{
						//					Logger::instance().GetTextLogger("bodydir") << mpWorldState->CurrentTime().T() << "\t" << mpWorldState->CurrentTime().S() << "\t" << unum << "\t" << vel.Dir() << "\t" << Teammate(unum).GetBodyDir() << "\n";
						Teammate(unum).UpdateBodyDir(vel.Dir() , GetTeammate(unum).GetBodyDirDelay() , GetTeammate(unum).GetBodyDirConf());
						//					Teammate(unum).UpdateBodyDirMayChanged(false);
					}
					else if ( vel.Mod() <= max_speed * mpWorldState->GetTeammate(unum).GetPlayerDecay())
					{
						Teammate(unum).UpdateBodyDirMayChanged(true);
					}


					//				Logger::instance().GetTextLogger("bodydir") << mpWorldState->CurrentTime().T() << "\t" << mpWorldState->CurrentTime().S() << "\t" << unum << "\t" << Teammate(unum).GetBodyDir() << "\n";
					if (vel.Mod() > max_speed)
					{
						vel = Polar2Vector(max_speed , vel.Dir());
					}

					vel *= PlayerParam::instance().HeteroPlayer(mpObserver->GetTeammateType(unum)).playerDecay();


					Teammate(unum).UpdateVel(vel ,mSightDelay , mPlayerConf);
				}
			}
		}


		//身体角度
		if (player.GetBodyDir().time() == mpObserver->LatestSightTime())
		{
			AngleDeg angle = player.BodyDir();
			angle = angle + GetNeckGlobalDirFromSightDelay(mSightDelay);
			Teammate(unum).UpdateBodyDir(angle , mSightDelay , mPlayerConf);
			Teammate(unum).UpdateBodyDirMayChanged(false);
		}

		//头部角度
		if (player.GetHeadDir().time() == mpObserver->LatestSightTime())
		{
			AngleDeg angle = player.HeadDir() - player.BodyDir();
			Teammate(unum).UpdateNeckDir(angle , mSightDelay , mPlayerConf);
		}

		//手指向
		if (player.GetIsPointing().time() == mpObserver->LatestSightTime())
		{
			if (player.IsPointing())
			{
				Teammate(unum).UpdateArmPoint(player.PointDir() + GetNeckGlobalDirFromSightDelay(mSightDelay) , mSightDelay , mPlayerConf);
				Teammate(unum).SetIsPointing(true);
			}
			else
			{
				Teammate(unum).SetIsPointing(false);
			}
		}

		//是否铲球
		if (player.GetIsTackling().time() == mpObserver->LatestSightTime())
		{
			if (player.IsTackling()) {
				if (Teammate(unum).GetTackleBan() == 0) {
					Teammate(unum).UpdateTackleBan(ServerParam::instance().tackleCycles() - 1);
				}
				//tacle速度一定为零
				Teammate(unum).UpdateVel(Vector(0,0) , mSightDelay , mPlayerConf);
			}
			else {
				Teammate(unum).UpdateTackleBan(0);
			}
		}

		//是否被铲倒
		if ( player.GetIsLying().time() == mpObserver->LatestSightTime() )
		{
			if ( player.IsLying() )
			{
				if ( Teammate( unum ).GetFoulChargedCycle() == 0 )
				{
					Teammate( unum ).UpdateFoulChargedCycle( ServerParam::instance().foulCycles() - 1 );
				}
				Teammate( unum ).UpdateVel( Vector( 0, 0 ), mSightDelay, mPlayerConf );
			}
			else
			{
				Teammate( unum ).UpdateFoulChargedCycle( 0 );
			}
		}

		Teammate( unum ).UpdateCardType( mpObserver->GetTeammateCardType( unum ) );

		//是否上周期踢过球
		if (player.GetIsKicked().time() == mpObserver->LatestSightTime())
		{
			Teammate(unum).UpdateKicked(player.IsKicked());
		}
	}
	else
	{
		Opponent(unum).SetIsAlive(true);
		Opponent(unum).UpdateGuessedTimes(0);
		//位置更新
		if (player.GetDir().time() == mpObserver->LatestSightTime())
		{
			Vector vec = Polar2Vector(PlayerParam::instance().ConvertSightDist(player.Dist()) , GetNeckGlobalDirFromSightDelay(mSightDelay) + player.Dir());
			vec = GetSelf().GetPos() + vec;
			Opponent(unum).UpdatePos(vec,mSightDelay , mPlayerConf);

			double eps = PlayerParam::instance().GetEpsInSight(player.Dist()) + SelfState().GetPosEps();
			Opponent(unum).UpdatePosEps(eps);
		}

		//速度更新
		if (player.GetDirChg().time() == mpObserver->LatestSightTime())
		{
			//公式：relPos = polar2vector(1.0, ballAngle)
			Vector vec = Polar2Vector(1.0 , player.Dir());
			double distChg = player.DistChg();
			double dirChg  = player.DirChg();
			double dist    = player.Dist();
			//计算公式见USTC_Material 第七章
			//公式：relVel = (distChg * relPos.x() - (dirChg * PI / 180 * ballDist * relPos.y()),distChg * rpos.y() + (dirChg * PI / 180 * ballDist * relPos.x())
			Vector relvel = Vector(distChg * vec.X() - (dirChg * M_PI / 180 * dist * vec.Y()) , distChg * vec.Y() + (dirChg * M_PI / 180 * dist * vec.X()));

			//公式：ballVel = playerVel() + relVel.rotate(neckGlobalAngle)
			relvel = GetSelfVelFromSightDelay(mSightDelay) + relvel.Rotate(GetNeckGlobalDirFromSightDelay(mSightDelay));
			Opponent(unum).UpdateVel(relvel , mSightDelay , mPlayerConf);
		}
		else if(player.GetDir().time() == mpObserver->LatestSightTime()) //只看到球的位置但是看不到速度的情况
		{
			if (mpWorldState->GetHistory(1 + mSightDelay)) {
				int delay = mpWorldState->GetHistory(1 + mSightDelay)->Opponent(unum).GetPosDelay();
				if (delay < HistoryState::HISTORY_SIZE - 1 - mSightDelay)
				{
					Vector vel = Opponent(unum).GetPos() - mpWorldState->GetHistory(1 + mSightDelay + delay)->Opponent(unum).GetPos();
					vel /= (delay + 1);

					//use  speed of self sense instead of pos of self sight to reduce the error
					Vector self_vel = GetSelfVelFromSightDelay();
					if (mpObserver->Sense().IsCollideWithBall() || mpObserver->Sense().IsCollideWithPlayer() || mpObserver->Sense().IsCollideWithPost())
					{
						self_vel /= -0.1;
					}
					self_vel /= PlayerParam::instance().HeteroPlayer(SelfState().GetPlayerType()).playerDecay();
					Vector self_vel_pos = SelfState().GetPos() - mpWorldState->GetHistory(1 + mSightDelay)->Teammate(GetSelfUnum()).GetPos();
					vel += self_vel - self_vel_pos;


					double max_speed = GetOpponent(unum).GetEffectiveSpeedMax();

					//根据速度预测对手身体朝向
					if (vel.Mod() > max_speed * GetOpponent(unum).GetPlayerDecay()  * 0.8 && GetOpponent(unum).IsBodyDirMayChanged())
					{
						//					Logger::instance().GetTextLogger("bodydir") << mpWorldState->CurrentTime().T() << "\t" << mpWorldState->CurrentTime().S() << "\t" << unum + TEAMSIZE << "\t" << vel.Dir() << "\t" << Opponent(unum).GetBodyDir() << "\n";
						Opponent(unum).UpdateBodyDir(vel.Dir() , GetOpponent(unum).GetBodyDirDelay() , GetOpponent(unum).GetBodyDirConf());
						//					Opponent(unum).UpdateBodyDirMayChanged(false);
					}
					else if (vel.Mod() <= max_speed * GetOpponent(unum).GetPlayerDecay())
					{
						Opponent(unum).UpdateBodyDirMayChanged(true);
					}

					//				Logger::instance().GetTextLogger("bodydir") << mpWorldState->CurrentTime().T() << "\t" << mpWorldState->CurrentTime().S() << "\t" << unum + TEAMSIZE << "\t" << Opponent(unum).GetBodyDir() << "\n";
					if (vel.Mod() > max_speed)
					{
						vel = Polar2Vector(max_speed , vel.Dir());
					}
					vel *= GetOpponent(unum).GetPlayerDecay();

					Opponent(unum).UpdateVel(vel , mSightDelay , mPlayerConf);
				}
			}
		}


		//身体角度
		if (player.GetBodyDir().time() == mpObserver->LatestSightTime())
		{
			AngleDeg angle = player.BodyDir();
			angle = angle + GetNeckGlobalDirFromSightDelay(mSightDelay);
			Opponent(unum).UpdateBodyDir(angle , mSightDelay , mPlayerConf);
			Opponent(unum).UpdateBodyDirMayChanged(false);
		}

		//头部角度
		if (player.GetHeadDir().time() == mpObserver->LatestSightTime())
		{
			AngleDeg angle = player.HeadDir() - player.BodyDir();
			Opponent(unum).UpdateNeckDir(angle , mSightDelay , mPlayerConf);
		}

		//手指向
		if (player.GetIsPointing().time() == mpObserver->LatestSightTime())
		{
			if (player.IsPointing())
			{
				Opponent(unum).UpdateArmPoint(player.PointDir() + GetNeckGlobalDirFromSightDelay(mSightDelay) , mSightDelay , mPlayerConf);
			}
		}

		//是否铲球
		if (player.GetIsTackling().time() == mpObserver->LatestSightTime())
		{
			if (player.IsTackling())
			{
				if (Opponent(unum).GetTackleBan() == 0)
				{
					Opponent(unum).UpdateTackleBan(ServerParam::instance().tackleCycles() - 1);
				}
				//tackle速度一定为零
				Opponent(unum).UpdateVel(Vector(0,0) , mSightDelay , mPlayerConf);
			}
			else {
				Opponent(unum).UpdateTackleBan(0);
			}
		}

		//是否被铲倒
		if ( player.GetIsLying().time() == mpObserver->LatestSightTime() )
		{
			if ( player.IsLying() )
			{
				if ( Opponent( unum ).GetFoulChargedCycle() == 0 )
				{
					Opponent( unum ).UpdateFoulChargedCycle( ServerParam::instance().foulCycles() - 1 );
				}
				Opponent( unum ).UpdateVel( Vector( 0, 0 ), mSightDelay, mPlayerConf );
			}
			else
			{
				Opponent( unum ).UpdateFoulChargedCycle( 0 );
			}
		}

		Opponent( unum ).UpdateCardType( mpObserver->GetOpponentCardType( unum ) );

		//是否上周期踢过球
		if (player.GetIsKicked().time() == mpObserver->LatestSightTime())
		{
			Opponent(unum).UpdateKicked(player.IsKicked());
		}
	}
}

void WorldStateUpdater::UpdateOtherKick()
{
	mIsOtherKick = false;
	mOtherKickUnum = 0;
	mIsOtherMayKick = true;
	for (int i = 1;i < TEAMSIZE;i++)
	{
		if (i != mSelfUnum)
		{
			if (Teammate(i).IsKicked() || Teammate(i).GetTackleBan() == ServerParam::instance().tackleCycles() - 1)
			{
				mIsOtherKick = true;
				mOtherKickUnum = i;
				break;
			}
		}

		if (Opponent(i).IsKicked() || Opponent(i).GetTackleBan() == ServerParam::instance().tackleCycles() - 1)
		{
			mIsOtherKick = true;
			mOtherKickUnum = -i;
			break;
		}
	}

	double min_dist = 10000;
	int min_index;

	if (mpWorldState->GetBall().GetPosDelay() <= 1 && SelfState().IsKickable())
	{
		Vector ball_pos = mpWorldState->GetBall().GetPos();
		for (int i = 1;i <= TEAMSIZE;i++)
		{
			if (i != mSelfUnum)
			{
				if (Teammate(i).GetPos().Dist(ball_pos) < min_dist)
				{
					min_dist = Teammate(i).GetPos().Dist(ball_pos);
					min_index = i;
				}
			}

			if (Opponent(i).GetPos().Dist(ball_pos) < min_dist)
			{
				min_dist = Opponent(i).GetPos().Dist(ball_pos);
				min_index = i;
			}
		}

		if (min_dist > 2.5)
		{
			mIsOtherMayKick = false;
		}
	}
}
namespace {
	Vector LimitToField(Vector pos)
	{
		if (pos.X() > ServerParam::instance().PITCH_LENGTH / 2)
		{
			pos.SetX(ServerParam::instance().PITCH_LENGTH / 2);
		}
		else if (pos.X() < -ServerParam::instance().PITCH_LENGTH / 2)
		{
			pos.SetX(-ServerParam::instance().PITCH_LENGTH / 2);
		}

		if (pos.Y() > ServerParam::instance().PITCH_WIDTH / 2)
		{
			pos.SetY(ServerParam::instance().PITCH_WIDTH / 2);
		}
		else if (pos.Y() < -ServerParam::instance().PITCH_WIDTH / 2)
		{
			pos.SetY(-ServerParam::instance().PITCH_WIDTH / 2);
		}

		return pos;
	}
}


void WorldStateUpdater::UpdateBallInfo()
{
	//更新别人是否可踢.
	UpdateOtherKick();


	//更新位置
	//计算公式见USTC_Material 第七章
	//公式：ballPos = playerPos + polar2vector(ballDist , neckGlobalAngle + ballAngle)
	if (mpObserver->Ball().GetDist().time() == mpObserver->LatestSightTime())
	{
		double dist = PlayerParam::instance().ConvertSightDist(mpObserver->Ball().Dist());

		if (mpObserver->GetPlayMode() == PM_Opp_Kick_In
				|| mpObserver->GetPlayMode() == PM_Opp_Corner_Kick
				|| mpObserver->GetPlayMode() == PM_Opp_Free_Kick
				|| mpObserver->GetPlayMode() == PM_Opp_Goalie_Free_Kick
				|| mpObserver->GetPlayMode() == PM_Opp_Offside_Kick
				|| mpObserver->GetPlayMode() == PM_Opp_Free_Kick_Fault_Kick
			)
		{
			//对方开球是,己方球员距离球最短为freekickBuffer ,暂时不知道这样做的作用 .
            dist = Max(dist, ServerParam::instance().offsideKickMargin() + FLOAT_EPS);
		}

		//发生碰撞时固定将球与人移开如此距离
		if (mpObserver->Sense().IsCollideWithBall())
		{
			dist = GetSelf().GetPlayerSize() + ServerParam::instance().ballSize();
		}
		Vector pos = GetSelf().GetPos() + Polar2Vector(dist , GetNeckGlobalDirFromSightDelay(mSightDelay) + mpObserver->Ball().Dir());

		//更新eps 球相对误差重要 球近身是不使用, 因为会影响到近身球速计算
		double eps = PlayerParam::instance().GetEpsInSight(mpObserver->Ball().Dist());
		eps = calcEps(dist , dist + eps , 0.1) + SelfState().GetPosEps();
		if (dist >  ServerParam::instance().visibleDistance() * 2 && !(mSightDelay == 0 && mIsOtherKick && !mIsHearBallPos))
		{
			if (mSightDelay == 0 && eps > Ball().GetPosEps() && pos.Dist(Ball().GetPos()) < Ball().GetPosEps())
			{
				eps = Ball().GetPosEps();
				pos = Ball().GetPos();
			}
    		else if (mSightDelay == 0 && eps > Ball().GetPosEps() && pos.Dist(Ball().GetPos()) >=  Ball().GetPosEps() && !mIsOtherKick)
			{
				if (!mpWorldState->IsBallDropped() && mpWorldState->GetPlayMode() == PM_Play_On && mpWorldState->GetHistory(1)->GetBall().GetPosDelay() == 0)
				{
/*
	char filename[25] = {0};
	sprintf(filename , "ballinfo_%d.txt" , mSelfUnum);

	std::ofstream file(filename , std::ios::app);
	file << mpWorldState->CurrentTime().T() << "\t" << mpWorldState->CurrentTime().S() << "\t" << pos.X() << "\t" << pos.Y();*/

					Vector normal = (pos - Ball().GetPos()).Normalize();
					pos = Ball().GetPos() + normal * Ball().GetPosEps();
//	file << "\t" << pos.X() << "\t" << pos.Y() << std::endl;
//	file.close();
				}
			}

			else if (mSightDelay == 0 && eps == Ball().GetPosEps() && pos.Dist(Ball().GetPos()) < eps)
			{
				pos  = (pos + Ball().GetPos())/2;
			}
		}

		// limited to field 仅当位于3米以外，否则球的相对位置不准
		if (dist > ServerParam::instance().visibleDistance())
		{
			pos = LimitToField(pos);
		}

		Ball().UpdatePosEps(eps);
		Ball().UpdatePos(pos , mSightDelay , mBallConf);
		Ball().UpdateGuessedTimes(0);
	}

	//更新速度
	if (mpObserver->Ball().GetDistChg().time() == mpObserver->LatestSightTime())
	{
		//公式：relPos = polar2vector(1.0, ballAngle)
		Vector relPos = Polar2Vector(1.0, mpObserver->Ball().Dir());

		double distChg = mpObserver->Ball().DistChg();
		double dirChg  = mpObserver->Ball().DirChg();
		double ballDist = mpObserver->Ball().Dist();

        ballDist = Max(ballDist, FLOAT_EPS);

		//公式：relVel = (distChg * relPos.x() - (dirChg * PI / 180 * ballDist * relPos.y()),distChg * rpos.y() + (dirChg * PI / 180 * ballDist * relPos.x())
		Vector relVel = Vector(distChg * relPos.X() - (dirChg * M_PI / 180 * ballDist * relPos.Y()) , distChg * relPos.Y() + (dirChg * M_PI / 180 * ballDist * relPos.X()));

		relVel = GetSelfVelFromSightDelay(mSightDelay) + relVel.Rotate(GetNeckGlobalDirFromSightDelay(mSightDelay));

		//compute vel eps;
		double eps = PlayerParam::instance().GetEpsInSight(mpObserver->Ball().Dist());
		Vector mod = Vector(0.02 * ballDist + fabs(distChg) / ballDist * eps + 0.02 * eps , 0.1 * M_PI / 180  * ballDist + fabs(dirChg) * M_PI / 180 * eps + eps * 0.1 * M_PI / 180);
		eps = mod.Mod();


		if ((!(mSightDelay == 0 && mIsOtherKick) )&& Ball().GetVelConf() > 0.9499)//0.95
		{
			Vector vel_estimate = Ball().GetVel();

			if (fabs(relVel.Mod() - vel_estimate.Mod()) <= ServerParam::instance().ballRand() * relVel.Mod() / ServerParam::instance().ballDecay() && fabs(relVel.Dir() - vel_estimate.Dir()) < 1 )
			{
				relVel = (relVel + vel_estimate) / 2;
			}
		}
		//公式：ballVel = playerVel() + relVel.rotate(neckGlobalAngle)
		//Ball().UpdateVel(GetSelf().GetVel() + relVel.Rotate(GetSelf().GetNeckGlobalDir()));
		//
		if (!(mSightDelay == 0 && mIsOtherKick && !mIsHearBallVel))
		{
			if (mSightDelay == 0 && Ball().GetVelEps() <= 0.32 && eps > Ball().GetVelEps() && (relVel.Dist(Ball().GetVel()) < Ball().GetVelEps()))
			{
	/*
		char filename[25] = {0};
		sprintf(filename , "ballinfo_%d.txt" , mSelfUnum);

		std::ofstream file(filename , std::ios::app);
		file << mpWorldState->CurrentTime().T() << "\t" << mpWorldState->CurrentTime().S() << "\t" << relVel.X() << "\t" << relVel.Y() << "\t" << Ball().GetVel().X() << "\t" << Ball().GetVel().Y() << std::endl;

		file.close();
		*/
				relVel = Ball().GetVel();
				eps = Ball().GetVelEps();
			}
			else if (mSightDelay == 0 && Ball().GetVelEps() <= 0.32 && eps == Ball().GetVelEps() && (relVel.Dist(Ball().GetVel()) < Ball().GetVelEps()))
			{
				relVel = (relVel + Ball().GetVel()) / 2;
			}
		}

		Ball().UpdateVelEps(eps);
		Ball().UpdateVel(relVel , mSightDelay , mBallConf);
	}

	//利用位置相减修正.
	if (mpObserver->Ball().GetDist().time() == mpObserver->LatestSightTime()) //看到球的位置情况
	{
	/*		if (//mpObserver->Ball().Dist() < ServerParam::instance().visibleDistance() * 2 &&
				(mpWorldState->GetHistory(1 + mSightDelay)->GetBall().GetPosDelay() == 0 ||
				 (mpWorldState->GetHistory(1 + mSightDelay)->GetBall().GetPos().Dist(SelfState().GetPos()) < ServerParam::instance().visibleDistance() - 0.15 && mpWorldState->GetHistory(1 + mSightDelay)->GetBall().GetPosDelay() < 2)))*/

		if (mpWorldState->GetHistory(1 + mSightDelay)) {
			if (mpWorldState->GetHistory(1 + mSightDelay)->GetBall().GetPosDelay() < 2)//距离近到一定程度才可以用
			{
				Vector vel = GetBall().GetPos() - mpWorldState->GetHistory(1 + mSightDelay)->Ball().GetPos() ;

				//use  speed of self sense instead of pos of self sight to reduce the error
				if (GetBall().GetPosEps() > ServerParam::instance().playerSize()  ||( !mpObserver->Sense().IsCollideWithBall() && !mpObserver->Sense().IsCollideWithPlayer() && !mpObserver->Sense().IsCollideWithPost()))
				{
					//碰撞时server强制移动人,如果用位置相减修正,会引入最大0.3(player size)的误差,08也应该有这个bug
					Vector self_vel = GetSelfVelFromSightDelay(mSightDelay);

					if (mpObserver->Sense().IsCollideWithBall() || mpObserver->Sense().IsCollideWithPlayer() || mpObserver->Sense().IsCollideWithPost())
					{
						self_vel /= -0.1;
					}

					self_vel /= PlayerParam::instance().HeteroPlayer(SelfState().GetPlayerType()).playerDecay();
					Vector self_vel_pos = SelfState().GetPos() - mpWorldState->GetHistory(1 + mSightDelay)->Teammate(GetSelfUnum()).GetPos();
					vel += self_vel - self_vel_pos;
				}

				if (mpObserver->GetBallKickTime() != mpObserver->LatestSightTime() && !mpObserver->Sense().IsCollideWithBall())
				{
					if (mpWorldState->GetHistory(1 + mSightDelay)->GetBall().GetVelConf() > 0.9)
					{
						Vector old_vel = mpWorldState->GetHistory(1 + mSightDelay)->GetBall().GetVel();

						if (fabs(old_vel.X() - vel.X()) + fabs(old_vel.Y() - vel.Y()) < 0.30 && fabs(old_vel.Mod() / (vel.Mod() + FLOAT_EPS) - 1.0) < 0.16)
						{
							vel = (old_vel + vel) / 2;
						}
					}
				}
				else if (!mpObserver->Sense().IsCollideWithBall())
				{
					if (mpWorldState->GetHistory(1 + mSightDelay)->GetBall().GetVelConf() > 0.9)
					{
						if (mpObserver->GetBallKickTime() == mpObserver->LatestSightTime())
						{
							Vector old_vel = mpObserver->GetBallVelByKick() / ServerParam::instance().ballDecay();
							if (fabs(old_vel.X() - vel.X()) + fabs(old_vel.Y() - vel.Y()) < 0.30 && fabs(old_vel.Mod() / (vel.Mod() + FLOAT_EPS) - 1.0) < 0.16)
							{
								vel = (old_vel + vel) / 2;
							}
						}
					}
				}
				else if (mpObserver->Sense().IsCollideWithBall())
				{
					vel *= -0.1;
				}

				vel *= ServerParam::instance().ballDecay();

				if (vel.Mod() > ServerParam::instance().ballSpeedMax())
				{
					vel = Polar2Vector(ServerParam::instance().ballSpeedMax() , vel.Dir());
				}

				double eps1 = PlayerParam::instance().GetEpsInSight((mpWorldState->GetHistory(1 + mSightDelay)->GetBall().GetPos() - mpWorldState->GetHistory(1 + mSightDelay)->GetTeammate(mSelfUnum).GetPos()).Mod());
				double eps2 = PlayerParam::instance().GetEpsInSight(mpObserver->Ball().Dist());
				double eps = eps1 + eps2;


				if (mpObserver->Sense().IsCollideWithPlayer() || mpObserver->Sense().IsCollideWithPost())
				{
					eps += ServerParam::instance().playerSize() * ServerParam::instance().ballDecay();
				}
				else if (mpObserver->Sense().IsCollideWithBall())
				{
					eps += ServerParam::instance().playerSize() * ServerParam::instance().ballDecay() * 0.1;
				}

				//没有视觉事一定更新, 有角度视觉是作为修正
				if (mpObserver->Ball().Dist() < 2 * ServerParam::instance().visibleDistance())
				{
					if ( mpWorldState->GetHistory(1 + mSightDelay)->GetBall().GetPosDelay() == 0 || (mpWorldState->GetHistory(1 + mSightDelay)->GetBall().GetPos().Dist(SelfState().GetPos()) < ServerParam::instance().visibleDistance() - 0.15 && mpWorldState->GetHistory(1 + mSightDelay)->GetBall().GetPosDelay() == 1) || (!mIsHearBallVel && mIsOtherKick))
					{
						if (Ball().GetVelDelay() > mSightDelay + 1 || eps < Ball().GetVelEps() || (Ball().GetVelDelay() != 0 && vel.Dist(Ball().GetVel()) > Ball().GetVelEps() && mIsOtherMayKick))
						{
							/*	char filename[25] = {0};
					sprintf(filename , "ballinfo_%d.txt" , mSelfUnum);

					std::ofstream file(filename , std::ios::app);
					file << mpWorldState->CurrentTime().T() << "\t" << mpWorldState->CurrentTime().S() << "\t" << vel.X() << "\t" << vel.Y() << "\t" << Ball().GetVel().X() << "\t" << Ball().GetVel().Y() << std::endl;

					file.close();
							 */
							Ball().UpdateVelEps(eps);
							Ball().UpdateVel(vel , mSightDelay , mBallConf);
						}
					}
				}
				//测试结果非常不好 先注释掉
				else
				{
					if (Ball().GetVelDelay() > mSightDelay && !mIsHearBallVel && mIsOtherKick)
					{
						/*
					char filename[25] = {0};
					sprintf(filename , "ballinfo_%d.txt" , mSelfUnum);

					std::ofstream file(filename , std::ios::app);
					file << mpWorldState->CurrentTime().T() << "\t" << mpWorldState->CurrentTime().S() << "\t" << vel.X() << "\t" << vel.Y() << "\t" << Ball().GetVel().X() << "\t" << Ball().GetVel().Y() << "\t" << eps << "\t" << Ball().GetVelEps() << std::endl;

					file.close();
						 */
						Ball().UpdateVelEps(eps);
						Ball().UpdateVel(vel , mSightDelay , mBallConf);
					}
				}
			}
		}
	}
}

void WorldStateUpdater::UpdateInfoWhenCollided()
{
	if (mpObserver->Sense().IsCollideWithBall() || mpObserver->Sense().IsCollideWithPlayer() || mpObserver->Sense().IsCollideWithPost())
	{
		if (mpObserver->Sense().IsCollideWithBall())
		{
			SelfState().UpdateCollideWithBall(true);
		}
		else if (mpObserver->Sense().IsCollideWithPlayer())
		{
			SelfState().UpdateCollideWithPlayer(true);
		}
		else if (mpObserver->Sense().IsCollideWithPost())
		{
			SelfState().UpdateCollideWithPost(true);
		}
	}

	//如果发生碰撞，速度需要处理 仅在未更新看到速度时使用
	if (mpObserver->Sense().IsCollideWithBall() && Ball().GetVelDelay() != 0)
	{
		Ball().UpdateVel(GetBall().GetVel() * -0.1 ,  GetBall().GetVelDelay() , GetBall().GetVelConf());
		Ball().UpdateVelEps(0.1 * Ball().GetVelEps());
	}

	if (mpObserver->Sense().IsCollideWithPlayer())
	{
		//找到最近的球员
		bool is_teammate = false;
		Unum tmmt = GetSeenClosestTeammate(); //无则返回0
		Unum oppn = GetSeenClosestOpponent(); //无则返回0

		if (tmmt * oppn == 0)
		{
			if (tmmt != 0)
			{
				is_teammate = true;
			}
			else if (oppn != 0)
			{
				is_teammate = false;
			}
			else
			{
				return;
			}
		}
		else
		{
			if (mpObserver->Teammate(tmmt).Dist() <= mpObserver->Opponent(oppn).Dist())
			{
				is_teammate = true;
			}
			else
			{
				is_teammate = false;
			}
		}


		if (is_teammate)
		{
			if (mpObserver->Teammate(tmmt).GetDirChg().time() != mpObserver->LatestSightTime())
			{
				Teammate(tmmt).UpdateVel(Teammate(tmmt).GetVel() * -0.1 , Teammate(tmmt).GetVelDelay() , Teammate(tmmt).GetVelConf());
			}
			Teammate(tmmt).UpdateCollideWithPlayer(true);
		}
		else
		{
			if (mpObserver->Opponent(oppn).GetDirChg().time() != mpObserver->LatestSightTime())
			{
				Opponent(oppn).UpdateVel(Opponent(oppn).GetVel() * -0.1 , Opponent(oppn).GetVelDelay() , Opponent(oppn).GetVelConf());
			}
			Opponent(oppn).UpdateCollideWithPlayer(true);
		}
	}
}

bool WorldStateUpdater::ComputeSelfDir(double& angle)
{
	//计算可以看到的线，如果有两个则表示在球场外
	int sample_line = SL_NONE; //mark
	int num = 0;
	double min = 10000; //初始值
	for (int i = 0;i <  SL_MAX;i++)
	{
		if (mpObserver->SideLine((SideLineType)i).GetDir().time() == mpObserver->LatestSightTime())
		{
			//必须使用最近的一条线
			if (mpObserver->SideLine((SideLineType)i).Dist() < min)
			{
				sample_line = i;
				min = mpObserver->SideLine((SideLineType)i).Dist();
			}
			num++;
		}
	}

	//看不到
	if (num == 0)
	{
		return false;
	}

	//计算公式见USTC_Material 第七章

	//lineRelativeAngle = (line_dir < 0 ) ? (90 + line_dir ) : (line_dir – 90)
	angle = mpObserver->SideLine((SideLineType)sample_line).Dir();

	angle = angle < 0 ? 90 + angle : angle - 90 ;

	//neckGlobalAngle = lineGlobalAngle – lineRelativeAngle
	angle = (mpObserver->SideLine((SideLineType)sample_line)).GlobalPosition().Dir() - angle;

	//如果同时看到两条线就表示在场外 加上180 具体可参加源码MemPosition.c的Update_self_seen函数处理
	if (num >= 2)
	{
		angle = 180 + angle;
	}

	//保证角度在180 到 -180 之间
	if (angle > 180)
	{
		angle -= 360;
	}
	else if (angle < -180)
	{
		angle += 360;
	}

	return true;
}

bool WorldStateUpdater::ComputeSelfPos(Vector &vec ,double& eps)
{
	//寻找最近的标志
	int sample = FLAG_NONE; //最近标志的标示
	double min = 2000.0;      //最近标志的距离 初始值应该不能比这个更小了吧！

	for (int i = 0;i < FLAG_MAX;i++)
	{
		if (mpObserver->Marker((MarkerType)i).GetDir().time() == mpObserver->LatestSightTime())
		{
			if (mpObserver->Marker((MarkerType)i).Dist() < min)
			{
				min = mpObserver->Marker((MarkerType)i).Dist();
				sample = i;
			}
		}
	}

	//没有看到的 则返回false
	if (sample == FLAG_NONE)
	{
		return false;
	}

	//修正及计算eps
	min = PlayerParam::instance().ConvertMarkDist(min);
	eps = PlayerParam::instance().GetEpsInMark(min);

	eps = calcEps(min , min+eps , 1);

	if (GetSelf().GetBodyDirDelay() != mSightDelay)
	{
		//不去计算身体角度误差，没有更新认为其极不准
		eps = 10000;
	}

	//计算公式见USTC_Material 第七章
	//playerPos = flagPos – polar2vector(flagDist, neckGlobalAngle + flagDir)
	vec = mpObserver->Marker((MarkerType)sample).GlobalPosition() - Polar2Vector(min , GetNeckGlobalDirFromSightDelay(mSightDelay) + mpObserver->Marker((MarkerType)sample).Dir());

	return true;
}

bool WorldStateUpdater::ComputePlayerMaySeeOrNot(const PlayerState& state)
{
	double view_angle = 0;
	switch(GetSelf().GetViewWidth())
	{
	case VW_Narrow:
		view_angle = 60;
		break;
	case VW_Normal:
		view_angle = 120;
		break;
	case VW_Wide:
		view_angle = 180;
		break;
	default:
		view_angle = 120;
	}

	Vector dist_vec = state.GetPos() - GetSelf().GetPos();
	double angle = GetNormalizeAngleDeg(dist_vec.Dir());
	angle = ::fabs(::GetNormalizeAngleDeg(angle - GetNeckGlobalDirFromSightDelay(mSightDelay)));

	double anglebuf = ASin((state.GetPosDelay() * 0.6 + 0.05 * dist_vec.Mod() ) / dist_vec.Mod());

	angle -= anglebuf;

	if (angle < view_angle / 2)
	{
		return true;
	}

	if (dist_vec.Mod() < ServerParam::instance().visibleDistance() + ComputePlayerMaxDist(state))
	{
		return true;
	}

	double dist = ComputePlayerMaxDist(state) / dist_vec.Mod();
	if (dist < 1)
	{
		double delta_angle = ASin(ComputePlayerMaxDist(state) / dist_vec.Mod());
		angle -= delta_angle;

		if (angle < 0)
		{
			angle = 0;
		}
	}
	else
	{
		angle = 0;
	}

	if (angle < view_angle / 2)
	{
		return true;
	}

	return false;
}

//#define __UNKNOWN_TEST

#ifdef __UNKNOWN_TEST
#include <fstream>
#endif

void WorldStateUpdater::UpdateUnknownPlayers()
{
	if (!mpObserver->IsNewSight())
	{
		return;
	}

	bool is_special_mode = mpWorldState->GetPlayMode() == PM_Before_Kick_Off || mpWorldState->GetPlayMode() == PM_Goal_Ours || mpWorldState->GetPlayMode() == PM_Goal_Opps;


	////============================================更新不知道球员号码的==========================================================
	bool Unknown[TEAMSIZE * 2][TEAMSIZE * 2 + 1]; //与未知球员接近的已知球员的队列.例如如果己方球员i与第j个未知球员可能相同 则Unknown[j][i] = true;  对每一行 0~TEAMSIZE-1为己方球员 其它为对方球员
	int UnknownCount[TEAMSIZE * 2];               //标记未知球员可能的属于球员的个数

	bool UnknownUpdate[TEAMSIZE*2];

	for (int i = 0; i < TEAMSIZE * 2; ++i){
		UnknownCount[i] = 0;
		UnknownUpdate[i] = false;
		for (int j = 0; j < TEAMSIZE * 2 + 1; ++j){
			Unknown[i][j] = false;
		}
	}

	//不同的两个量
	int player_num = mpObserver->GetUnknownPlayerCount();
	char self_side = mpObserver->OurInitSide();

	//1.对所有pi,oj，当满足raidus_max(pi) > distance(pi,oj)时，将pi 与oj 互相加入对方的匹配列表。
	for (int i = 0;i < player_num;i++)
	{
		Vector pos = Polar2Vector(PlayerParam::instance().ConvertSightDist(mpObserver->UnknownPlayer(i).Dist()) , GetNeckGlobalDirFromSightDelay(mSightDelay) + mpObserver->UnknownPlayer(i).Dir());
		pos = GetSelf().GetPos() + pos;

		if (is_special_mode && !ServerParam::instance().pitchRectanglar().IsWithin(pos))
		{
			continue;
		}

		//double disbuf = PlayerParam::instance().GetEpsInSight(mpObserver->UnknownPlayer(i).Dist()); // please look at line 3301 in MemPosition.C in WE2008a
		double disbuf = 0.1 * mpObserver->UnknownPlayer(i).Dist() + 0.66;
		if (mpObserver->UnknownPlayerBugInfo(i).mSide == mpObserver->UnknownPlayerBugInfo(i).mSupSide)
		{
			if (mpObserver->UnknownPlayerBugInfo(i).mLeastNum == mpObserver->UnknownPlayerBugInfo(i).mSupNum)
			{
				UnknownCount[i] = 1;
				int index = mpObserver->UnknownPlayerBugInfo(i).mSide == self_side ?  mpObserver->UnknownPlayerBugInfo(i).mLeastNum : mpObserver->UnknownPlayerBugInfo(i).mLeastNum  + TEAMSIZE;
				Unknown[i][index] = true;
				continue;
			}

			for (int j = mpObserver->UnknownPlayerBugInfo(i).mLeastNum;j <= mpObserver->UnknownPlayerBugInfo(i).mSupNum ;j++)
			{
					const PlayerState& player = mpObserver->UnknownPlayerBugInfo(i).mSide == self_side ? GetTeammate(j) : GetOpponent(j);
					if (j == mSelfUnum && mpObserver->UnknownPlayerBugInfo(i).mSide == self_side)
					{
						continue;
					}
					double dist = (is_special_mode ? 0 : ComputePlayerMaxDist(player)) + disbuf;
					if (player.GetPosDelay() != 0&& ComputePlayerMaySeeOrNot(player) && (pos - player.GetPos()).Mod() <= dist)
					{
						//可能成为的队员
						UnknownCount[i]++;
						int index = mpObserver->UnknownPlayerBugInfo(i).mSide == self_side ? j : j + TEAMSIZE;
						Unknown[i][index] = true;
					}
				}
		}
		else
		{
			if (mpObserver->UnknownPlayerBugInfo(i).mSide != 'l')
			{
				PRINT_ERROR("mpObserver->UnknownPlayerBugInfo(i).mSide != 'l'");
			}

			for (int j = mpObserver->UnknownPlayerBugInfo(i).mLeastNum;j <= TEAMSIZE;j++)
			{
					const PlayerState& player = mpObserver->UnknownPlayerBugInfo(i).mSide == self_side ? GetTeammate(j) : GetOpponent(j);
					if (j == mSelfUnum && mpObserver->UnknownPlayerBugInfo(i).mSide == self_side)
					{
						continue;
					}
					double dist = (is_special_mode ? 0 :ComputePlayerMaxDist(player) )+ disbuf;
					if (player.GetPosDelay() != 0&& ComputePlayerMaySeeOrNot(player) && (pos - player.GetPos()).Mod() <= dist)
					{
						//可能成为的队员
						UnknownCount[i]++;
						int index = mpObserver->UnknownPlayerBugInfo(i).mSide == self_side ? j : j + TEAMSIZE;
						Unknown[i][index] = true;
					}
			}

			for (int j = 1;j <= mpObserver->UnknownPlayerBugInfo(i).mSupNum ;j++)
			{
					const PlayerState& player = mpObserver->UnknownPlayerBugInfo(i).mSide == self_side ? GetOpponent(j) : GetTeammate(j);
					if (j == mSelfUnum && mpObserver->UnknownPlayerBugInfo(i).mSide !=  self_side)
					{
						continue;
					}
					double dist = (is_special_mode ? 0 : ComputePlayerMaxDist(player) )+ disbuf;
					if (player.GetPosDelay() != 0&& ComputePlayerMaySeeOrNot(player) && (pos - player.GetPos()).Mod() <= dist)
					{
						//可能成为的队员
						UnknownCount[i]++;
						int index = mpObserver->UnknownPlayerBugInfo(i).mSide == self_side ? j + TEAMSIZE : j;
						Unknown[i][index] = true;
					}
			}
		}
	}

	//2.对所有未知球员oj，当oj 的匹配列表只有1 个元素时，把该元素对应的老球员pi 与oj 匹配，从O 集合移除oj，并通过pi 匹配列表查询到
	//其它可能与其匹配的未知球员，将pi 从它们的匹配列表中移除。
	for (int i = 0;i < player_num;i++)
	{
		bool is_update = false; //判断有没有更新
		for (int j = 0;j < player_num;j++)
		{
			if (UnknownCount[j] == 1)
			{
				//更新
				int k = 0;
				for (k = 1;k <= 2*TEAMSIZE;k++)
				{
					//判断是否在已知
					if (Unknown[j][k])
					{
						if (k <= TEAMSIZE)
						{
#ifdef __UNKNOWN_TEST
test_num++;					//更新队友
file << mpObserver->LatestSightTime() << "unknown guess teammate: " << k << std::endl;
#endif
							UpdateSpecificUnknownPlayer(mpObserver->UnknownPlayer(j) , k , true);
						}
						else
						{
#ifdef __UNKNOWN_TEST
test_num++;							//更新对手
file << mpObserver->LatestSightTime() << "unknown guess opponent: " << k - TEAMSIZE << std::endl;
#endif
							UpdateSpecificUnknownPlayer(mpObserver->UnknownPlayer(j) , k - TEAMSIZE , false);
						}
						UnknownUpdate[j] = true;
						break;
					}
				}

				//没有则继续
				if (k > 2*TEAMSIZE )
				{
					UnknownCount[j] = 0;
					std::cout << " error : 数组与值不相符!" << std::endl;
					continue;
				}

				//从原有的数组中去除该配对
				for (int m = 0;m < player_num;m++)
				{
					if (Unknown[m][k])
					{
						Unknown[m][k] = false;
						UnknownCount[m]--;
					}
/*
					//利用server的bug，发回的号码是从大到小的顺序的
					if (m < j)
					{
						int bound = TEAMSIZE;
						if (k > TEAMSIZE)
						{
							bound = TEAMSIZE * 2;
						}

						for (int n = k + 1;n <= bound;n++)
						{
							if (Unknown[m][n])
							{
								Unknown[m][n] = false;
								UnknownCount[m]--;
							}
						}
					}
					else if (m > j)
					{
						int bound = 1;
						if ( k > TEAMSIZE)
						{
							bound = TEAMSIZE + 1;
						}

						for (int n = bound;n < k;n++)
						{
							if (Unknown[m][n])
							{
								Unknown[m][n] = false;
								UnknownCount[m]--;
							}
						}
					}*/
				}
				is_update = true;

			}
		}

		//本周期没有更新 使用distance(pi,oj)最小的原则 保证更新一个 觉得不是太好 改进一下：个数最少中选最近的
		if (!is_update)
		{
			int min = 200 * TEAMSIZE;
			int index = -1;
			bool is_all_zero = true; //
			for (int j = 0;j < player_num;j++)
			{
				if (UnknownCount[j] > 0 && UnknownCount[j] < min)
				{
					min = UnknownCount[j];
					index = j;
				}

				if (is_all_zero && UnknownCount[j] > 0)
				{
					is_all_zero = false;
				}
			}

			//全为零表示更新完毕
			if (is_all_zero)
			{
				break;
			}

			//寻找最近的更新
			if (index != -1)
			{
				//寻找最近的球员
				Vector pos = Polar2Vector(PlayerParam::instance().ConvertSightDist(mpObserver->UnknownPlayer(index).Dist()) , GetNeckGlobalDirFromSightDelay(mSightDelay) + mpObserver->UnknownPlayer(index).Dir()) + GetSelf().GetPos();
				double min_dist = 200000;
				int min_index = -1;
				for (int k = 1;k <= 2*TEAMSIZE;k++)
				{
					if (Unknown[index][k])
					{
						Vector compare_pos;
						if (k <= TEAMSIZE)
						{
							compare_pos = GetTeammate(k).GetPos();
						}
						else
						{
							compare_pos = GetOpponent(k - TEAMSIZE).GetPos();
						}

						if ( (pos - compare_pos).Mod() < min_dist)
						{
							min_dist = (pos - compare_pos).Mod();
							min_index = k;
						}
					}
				}

				if (min_index <= TEAMSIZE && min_index > 0)
				{
#ifdef __UNKNOWN_TEST
test_num++;					//更新队友
file << mpObserver->LatestSightTime() << "unknown guess teammate: " << min_index << std::endl;
#endif
					UpdateSpecificUnknownPlayer(mpObserver->UnknownPlayer(index) , min_index , true);
					UnknownUpdate[index] = true;
				}
				else if (min_index > TEAMSIZE && min_index <= TEAMSIZE * 2)
				{
#ifdef __UNKNOWN_TEST
test_num++;					//更新对手
file << mpObserver->LatestSightTime() << "unknown guess opponent: " << min_index - TEAMSIZE << std::endl;
#endif
					UpdateSpecificUnknownPlayer(mpObserver->UnknownPlayer(index) , min_index - TEAMSIZE , false);
					UnknownUpdate[index] = true;
				}
				else if (min_index == -1) //此时应该没有队员了
				{
					UnknownCount[index] = 0;
					continue;
				}

				//生成的count置为零
				UnknownCount[index] = 0;

				//从原有的数组中去除该配对
				for (int m = 0;m < player_num;m++)
				{
					if (Unknown[m][min_index])
					{
						Unknown[m][min_index] = false;
						UnknownCount[m]--;
					}
/*
					//利用server的bug，发回的号码是从大到小的顺序的
					if (m < index)
					{
						int bound = TEAMSIZE;
						if (min_index > TEAMSIZE)
						{
							bound = TEAMSIZE * 2;
						}

						for (int n = min_index + 1;n <= bound;n++)
						{
							if (Unknown[m][n])
							{
								Unknown[m][n] = false;
								UnknownCount[m]--;
							}
						}
					}
					else if (m > index)
					{
						int bound = 1;
						if ( min_index > TEAMSIZE)
						{
							bound = TEAMSIZE + 1;
						}

						for (int n = bound;n < min_index;n++)
						{
							if (Unknown[m][n])
							{
								Unknown[m][n] = false;
								UnknownCount[m]--;
							}
						}
					}*/
				}

				is_update = true;
			}
		}

	}

	for (int i = 0;i < player_num;i++)
	{
		if (!UnknownUpdate[i])
		{
			Vector pos = Polar2Vector(PlayerParam::instance().ConvertSightDist(mpObserver->UnknownPlayer(i).Dist()) , GetNeckGlobalDirFromSightDelay(mSightDelay) + mpObserver->UnknownPlayer(i).Dir());
			pos = GetSelf().GetPos() + pos;

			if (!is_special_mode)
			{
				UnknownUpdate[i] = UpdateMostSimilarPlayer(pos ,i);
				continue;
			}

			//next just for special mode
			if (is_special_mode && !ServerParam::instance().pitchRectanglar().IsWithin(pos))
			{
				continue;
			}

			//初始识别时最少需要6周期才能看全所有场景
			if (mpObserver->UnknownPlayerBugInfo(i).mSide == mpObserver->UnknownPlayerBugInfo(i).mSupSide)
			{
				for (int j = mpObserver->UnknownPlayerBugInfo(i).mLeastNum;j <= mpObserver->UnknownPlayerBugInfo(i).mSupNum;j++)
				{
					const PlayerState& player = mpObserver->UnknownPlayerBugInfo(i).mSide == self_side ? GetTeammate(j) : GetOpponent(j);

					if (j == mSelfUnum && mpObserver->UnknownPlayerBugInfo(i).mSide == self_side)
					{
						continue;
					}

					if (player.GetPosDelay() > 6)
					{
						this->UpdateSpecificUnknownPlayer(mpObserver->UnknownPlayer(i) , abs(player.GetUnum()) , player.GetUnum() > 0);
						UnknownUpdate[i] = true;
						break;
					}
				}
			}
			else
			{
				bool is_continue = true;
				for (int j = mpObserver->UnknownPlayerBugInfo(i).mLeastNum;j <= TEAMSIZE;j++)
				{
					const PlayerState& player = mpObserver->UnknownPlayerBugInfo(i).mSide == self_side ? GetTeammate(j) : GetOpponent(j);

					if (j == mSelfUnum && mpObserver->UnknownPlayerBugInfo(i).mSide == self_side)
					{
						continue;
					}

					if (player.GetPosDelay() > 6)
					{
						this->UpdateSpecificUnknownPlayer(mpObserver->UnknownPlayer(i) , abs(player.GetUnum()) , player.GetUnum() > 0);
						UnknownUpdate[i] = true;
						is_continue = false;
						break;
					}
				}

				if (is_continue)
				{
					for (int j = 1;j <= mpObserver->UnknownPlayerBugInfo(i).mSupNum;j++)
					{
						const PlayerState& player = mpObserver->UnknownPlayerBugInfo(i).mSide == self_side ? GetOpponent(j) : GetTeammate(j);

						if (j == mSelfUnum && mpObserver->UnknownPlayerBugInfo(i).mSide != self_side)
						{
							continue;
						}

						if (player.GetPosDelay() > 6)
						{
							this->UpdateSpecificUnknownPlayer(mpObserver->UnknownPlayer(i) , abs(player.GetUnum()) , player.GetUnum() > 0);
							UnknownUpdate[i] = true;
							break;
						}
					}
				}
			}
		}

	}

}

bool  WorldStateUpdater::UpdateMostSimilarPlayer(const Vector & pos ,int index)
{
	bool is_known_side = mpObserver->UnknownPlayer(index).IsKnownSide();
	int least_num = mpObserver->UnknownPlayerBugInfo(index).mLeastNum;

	int min_index = 0;
	double min = 100000;


	if (is_known_side)
	{
		if (mpObserver->UnknownPlayer(index).Side() != mpObserver->UnknownPlayerBugInfo(index).mSide)
		{
			least_num = 1;
		}
		for (int i = least_num;i <= TEAMSIZE;i++)
		{
			const PlayerState& player = mpObserver->UnknownPlayer(index).Side() == mSelfSide ? mpWorldState->GetTeammate(i) : mpWorldState->GetOpponent(i);
			if (player.GetPosDelay() != 0 && (player.GetPos() - pos).Mod() < min)
			{
				min = (player.GetPos() - pos).Mod();
				min_index = player.GetUnum();
			}
		}
	}
	else
	{
		for (int i = 1;i <= TEAMSIZE;i++)
		{
			const PlayerState& player = mpObserver->UnknownPlayerBugInfo(index).mSide == mSelfSide ? mpWorldState->GetTeammate(i) : mpWorldState->GetOpponent(i);
			const PlayerState& opp_player = mpObserver->UnknownPlayerBugInfo(index).mSide == mSelfSide ? mpWorldState->GetOpponent(i) : mpWorldState->GetTeammate(i);
			if (i >= least_num)
			{
				if (player.GetPosDelay() != 0 && (player.GetPos() - pos).Mod() < min)
				{
					min = (player.GetPos() - pos).Mod();
					min_index = player.GetUnum();
				}
			}

			if (opp_player.GetPosDelay() != 0 && (opp_player.GetPos() - pos).Mod() < min)
			{
				min = (opp_player.GetPos() - pos).Mod();
				min_index = opp_player.GetUnum();
			}
		}
	}

	if (min_index != 0)
	{
		bool is_teammate = min_index > 0;
		this->UpdateSpecificUnknownPlayer(mpObserver->UnknownPlayer(index) , abs(min_index) , is_teammate);
		return true;
	}
	else
	{
		return false;
	}
}



double WorldStateUpdater::ComputePlayerMaxDist(const PlayerState &state)
{
	//计算最大的速度
	double max_speed = PlayerParam::instance().HeteroPlayer(state.GetPlayerType()).effectiveSpeedMax();

	return max_speed * state.GetPosDelay();
}

void WorldStateUpdater::UpdateSpecificUnknownPlayer(const UnknownPlayerObserver &player, Unum unum, bool is_teammate)
{
	if (is_teammate)
	{
		Teammate(unum).SetIsAlive(true);
		Teammate(unum).UpdateGuessedTimes(0);
		//位置更新
		if (player.GetDir().time() == mpObserver->LatestSightTime())
		{
			Vector vec = Polar2Vector(PlayerParam::instance().ConvertSightDist(player.Dist()) , GetNeckGlobalDirFromSightDelay(mSightDelay) + player.Dir());
			vec = GetSelf().GetPos() + vec;
			Teammate(unum).UpdatePos(vec ,mSightDelay , mPlayerConf);

			double eps = PlayerParam::instance().GetEpsInSight(player.Dist()) + SelfState().GetPosEps();
			Teammate(unum).UpdatePosEps(eps);
		}

		if (mpWorldState->GetHistory(1 + mSightDelay)) {
			//更新近距离位置
			int delay = mpWorldState->GetHistory(1 + mSightDelay)->Teammate(unum).GetPosDelay();
			if (delay < HistoryState::HISTORY_SIZE - 1 - mSightDelay)
			{
				Vector 	vel = Teammate(unum).GetPos() - mpWorldState->GetHistory(1 + mSightDelay + delay)->Teammate(unum).GetPos();
				vel /= (delay + 1);

				//use  speed of self sense instead of pos of self sight to reduce the error
				Vector self_vel = GetSelfVelFromSightDelay(mSightDelay);
				if (mpObserver->Sense().IsCollideWithBall() || mpObserver->Sense().IsCollideWithPlayer() || mpObserver->Sense().IsCollideWithPost())
				{
					self_vel /= -0.1;
				}
				self_vel /= PlayerParam::instance().HeteroPlayer(SelfState().GetPlayerType()).playerDecay();
				Vector self_vel_pos = SelfState().GetPos() - mpWorldState->GetHistory(1 + mSightDelay)->Teammate(GetSelfUnum()).GetPos();
				vel += self_vel - self_vel_pos;

				double max_speed = PlayerParam::instance().HeteroPlayer(mpObserver->GetTeammateType(unum)).effectiveSpeedMax();
				//根据速度大致猜测朝向 有视觉时会覆盖
				if (vel.Mod()  > max_speed * mpWorldState->GetTeammate(unum).GetPlayerDecay() * 0.8 &&  GetTeammate(unum).IsBodyDirMayChanged())
				{
					//				Logger::instance().GetTextLogger("bodydir") << mpWorldState->CurrentTime().T() << "\t" << mpWorldState->CurrentTime().S() << "\t" << unum<< "\t" << vel.Dir() << "\t" << GetTeammate(unum).GetBodyDir() << "\n";
					Teammate(unum).UpdateBodyDir(vel.Dir() , GetTeammate(unum).GetBodyDirDelay() , GetTeammate(unum).GetBodyDirConf());
					//				Teammate(unum).UpdateBodyDirMayChanged(false);
				}
				else if (vel.Mod() <= max_speed * mpWorldState->GetTeammate(unum).GetPlayerDecay())
				{
					Teammate(unum).UpdateBodyDirMayChanged(true);
				}


				//			Logger::instance().GetTextLogger("bodydir") << mpWorldState->CurrentTime().T() << "\t" << mpWorldState->CurrentTime().S() << "\t" << unum<< "\t" << GetTeammate(unum).GetBodyDir() << "\n";
				if (vel.Mod() > max_speed)
				{
					vel = Polar2Vector(max_speed , vel.Dir());
				}

				vel *= PlayerParam::instance().HeteroPlayer(mpObserver->GetTeammateType(unum)).playerDecay();


				Teammate(unum).UpdateVel(vel ,mSightDelay , mPlayerConf);
			}
		}

		//既然不知道这个队员,直接置０
		Teammate( unum ).UpdateFoulChargedCycle( 0 );

		//是否铲球
		if (player.IsKnownSide())
		{
			if (player.GetIsTackling().time() == mpObserver->LatestSightTime())
			{
				if (player.IsTackling())
				{
					if (Teammate(unum).GetTackleBan() == 0)
					{
						Teammate(unum).UpdateTackleBan(ServerParam::instance().tackleCycles() - 1);
					}
					//tackle 速度一定为0
					Teammate(unum).UpdateVel(Vector(0,0), mSightDelay , mPlayerConf);
				}
				else {
					Teammate(unum).UpdateTackleBan(0);
				}
			}
			if (player.GetIsLying().time() == mpObserver->LatestSightTime())
			{
				if (player.IsLying())
				{
					if (Teammate(unum).GetFoulChargedCycle() == 0)
					{
						Teammate(unum).UpdateFoulChargedCycle(ServerParam::instance().foulCycles() - 1);
					}
					Teammate(unum).UpdateVel(Vector(0,0) , mSightDelay ,mPlayerConf);
				}
				else
				{
					Teammate(unum).UpdateFoulChargedCycle(0);
				}
			}
			//是否上周期踢过球
			if (player.GetIsKicked().time() == mpObserver->LatestSightTime())
			{
				Teammate(unum).UpdateKicked(player.IsKicked());
			}

			Teammate( unum ).UpdateCardType( mpObserver->GetTeammateCardType( unum ) );
		}

	}
	else
	{
		Opponent(unum).SetIsAlive(true);
		Opponent(unum).UpdateGuessedTimes(0);
		//位置更新
		if (player.GetDir().time() == mpObserver->LatestSightTime())
		{
			Vector vec = Polar2Vector(PlayerParam::instance().ConvertSightDist(player.Dist()) , GetNeckGlobalDirFromSightDelay(mSightDelay) + player.Dir());
			vec = GetSelf().GetPos() + vec;
			Opponent(unum).UpdatePos(vec , mSightDelay , mPlayerConf);

			double eps = PlayerParam::instance().GetEpsInSight(player.Dist()) + SelfState().GetPosEps();
			Opponent(unum).UpdatePosEps(eps);
		}

		if (mpWorldState->GetHistory(1 + mSightDelay)) {
			//近距离速度更新
			int delay = mpWorldState->GetHistory(1 + mSightDelay)->Opponent(unum).GetPosDelay();
			if (delay < HistoryState::HISTORY_SIZE - 1 - mSightDelay)
			{
				Vector vel = Opponent(unum).GetPos() - mpWorldState->GetHistory(1 + mSightDelay + delay)->Opponent(unum).GetPos();
				vel /= (delay + 1);

				//use  speed of self sense instead of pos of self sight to reduce the error
				Vector self_vel = GetSelfVelFromSightDelay();
				if (mpObserver->Sense().IsCollideWithBall() || mpObserver->Sense().IsCollideWithPlayer() || mpObserver->Sense().IsCollideWithPost())
				{
					self_vel /= -0.1;
				}
				self_vel /= PlayerParam::instance().HeteroPlayer(SelfState().GetPlayerType()).playerDecay();
				Vector self_vel_pos = SelfState().GetPos() - mpWorldState->GetHistory(1 + mSightDelay)->Teammate(GetSelfUnum()).GetPos();
				vel += self_vel - self_vel_pos;


				double max_speed = GetOpponent(unum).GetEffectiveSpeedMax();

				//根据速度预测对手身体朝向
				if (vel.Mod()  > max_speed * GetOpponent(unum).GetPlayerDecay() * 0.8 &&  GetOpponent(unum).IsBodyDirMayChanged())
				{
					//				Logger::instance().GetTextLogger("bodydir") << mpWorldState->CurrentTime().T() << "\t" << mpWorldState->CurrentTime().S() << "\t" << unum + TEAMSIZE<< "\t" << vel.Dir() << "\t" << GetOpponent(unum).GetBodyDir() << "\n";
					Opponent(unum).UpdateBodyDir(vel.Dir() , GetOpponent(unum).GetBodyDirDelay() , GetOpponent(unum).GetBodyDirConf());
					//				Opponent(unum).UpdateBodyDirMayChanged(false);
				}
				else if (vel.Mod() <= max_speed * GetOpponent(unum).GetPlayerDecay())
				{
					Opponent(unum).UpdateBodyDirMayChanged(true);
				}

				//			Logger::instance().GetTextLogger("bodydir") << mpWorldState->CurrentTime().T() << "\t" << mpWorldState->CurrentTime().S() << "\t" << unum + TEAMSIZE<< "\t" << GetOpponent(unum).GetBodyDir() << "\n";
				if (vel.Mod() > max_speed)
				{
					vel = Polar2Vector(max_speed , vel.Dir());
				}
				vel *= GetOpponent(unum).GetPlayerDecay();

				Opponent(unum).UpdateVel(vel , mSightDelay , mPlayerConf);
			}
		}

		//既然不知道这个队员,置０
		Opponent( unum ).UpdateFoulChargedCycle( 0 );

		//是否铲球
		if (player.IsKnownSide())
		{
			if (player.GetIsTackling().time() == mpObserver->LatestSightTime())
			{
				if (player.IsTackling())
				{
					if (Opponent(unum).GetTackleBan() == 0)
					{
						Opponent(unum).UpdateTackleBan(ServerParam::instance().tackleCycles() - 1);
					}
					Opponent(unum).UpdateVel(Vector(0,0) , mSightDelay ,mPlayerConf);
				}
				else
				{
					Opponent(unum).UpdateTackleBan(0);
				}
			}
			if (player.GetIsLying().time() == mpObserver->LatestSightTime())
			{
				if (player.IsLying())
				{
					if (Opponent(unum).GetFoulChargedCycle() == 0)
					{
						Opponent(unum).UpdateFoulChargedCycle(ServerParam::instance().foulCycles() - 1);
					}
					Opponent(unum).UpdateVel(Vector(0,0) , mSightDelay ,mPlayerConf);
				}
				else
				{
					Opponent(unum).UpdateFoulChargedCycle(0);
				}
			}
			//是否上周期踢过球
			if (player.GetIsKicked().time() == mpObserver->LatestSightTime())
			{
				Opponent(unum).UpdateKicked(player.IsKicked());
			}

			Opponent( unum ).UpdateCardType( mpObserver->GetOpponentCardType( unum ) );
		}

	}
}

void WorldStateUpdater::AutoDelay(int delay_add)
{
	//team均delay一周期
	for (Unum i = 1; i <= TEAMSIZE; i++)
	{
		mpWorldState->mTeammate[i].AutoUpdate(delay_add , PlayerParam::instance().playerConfDecay());
		mpWorldState->mOpponent[i].AutoUpdate(delay_add , PlayerParam::instance().playerConfDecay());
	}

	//球delay一周期
	Ball().AutoUpdate(delay_add , PlayerParam::instance().ballConfDecay());

	//特殊处理
	SelfState().UpdateBodyDir(SelfState().GetBodyDir(), SelfState().GetBodyDirDelay(), 1.0); //认为自己的身体方向肯定是清楚的
	SelfState().UpdateVel(SelfState().GetVel(), SelfState().GetVelDelay(), 1.0);             //认为自己的速度肯定是清楚的
}

void WorldStateUpdater::EstimateToNow()
{
	if (mSightDelay > 0 && mSightDelay <= HistoryState::HISTORY_SIZE)
	{
		//保存与sense相关的信息 主要有 mSelfVel mNeckDir
		const Vector & vel = SelfState().GetVel();

		SelfState().UpdateVel(mpWorldState->GetHistory(mSightDelay)->mTeammate[mSelfUnum].GetVel());
		int cycle = mSightDelay;
		for (int i = 0;i < cycle;i++)
		{
			EstimateWorld(true , cycle - i);
		}

		//恢复现场 此时应该看见了 delay可设置为0
		SelfState().UpdateVel(vel);
	}
}
void WorldStateUpdater::EstimateWorld(bool is_estimate_to_now ,int cycle)
{
	/**预估自己的信息*/
	EstimateSelf(is_estimate_to_now , cycle);

	/**预估球的信息*/
	EstimateBall(is_estimate_to_now , cycle);

	/**预估球员的信息*/
	EstimatePlayers();
}

void WorldStateUpdater::EstimateSelf(bool is_estimate_to_now ,int cycle)
{
	(void)cycle;
	bool is_collided = mpObserver->Sense().IsCollideWithPlayer() || mpObserver->Sense().IsCollideWithBall() || mpObserver->Sense().IsCollideWithPost();
	if (mpObserver->GetPlayerDashTime() == mpObserver->CurrentTime())
	{
		if (is_collided)
		{
			//碰撞由于内部处理太复杂,直接认为不准.
			SelfState().UpdatePosEps(10000);
		}

		if (!is_estimate_to_now)
		{
			if (!is_collided)
			{
				//server内部最大的randp为Vel.Mod()*playerRand()
				double eps = (SelfState().GetPos() - mpObserver->GetPlayerPosByDash()).Mod() * ServerParam::instance().playerRand();
				SelfState().UpdatePosEps(SelfState().GetPosEps() + eps);
			}
			SelfState().UpdatePos(mpObserver->GetPlayerPosByDash(), SelfState().GetPosDelay(), SelfState().GetPosConf());
			SelfState().UpdateVel(mpObserver->GetPlayerVelByDash(), SelfState().GetVelDelay(), SelfState().GetVelConf());
		}
		else
		{
			const PlayerState& player = mpWorldState->GetHistory(1)->GetTeammate(mSelfUnum);
			Vector accel = mpObserver->GetPlayerPosByDash() - player.GetPos() - player.GetVel();
			Vector vel = SelfState().GetVel() + accel;

			if (!is_collided)
			{
				double eps = vel.Mod() * ServerParam::instance().playerRand();
				SelfState().UpdatePosEps(SelfState().GetPosEps() + eps);
			}

			Vector pos = SelfState().GetPos() + vel;
			vel *= SelfState().GetPlayerDecay();
			SelfState().UpdatePos(pos, SelfState().GetPosDelay(), SelfState().GetPosConf());
			SelfState().UpdateVel(vel,SelfState().GetVelDelay(), SelfState().GetVelConf());
		}
	}
	else
	{
		if (is_collided)
		{
			//碰撞由于内部处理太复杂,直接认为不准.
			SelfState().UpdatePosEps(10000);
		}
		else
		{
			SelfState().UpdatePosEps(SelfState().GetPosEps() + SelfState().GetVel().Mod() * ServerParam::instance().playerRand());
		}
	   	ComputeNextCycle(SelfState(), PlayerParam::instance().HeteroPlayer(GetSelf().GetPlayerType()).playerDecay());
	}

	if (mpObserver->GetPlayerMoveTime() == mpObserver->CurrentTime())
	{
		SelfState().UpdatePos(mpObserver->GetPlayerPosByMove() , GetSelf().GetPosDelay() , GetSelf().GetPosConf());
		SelfState().UpdateVel(mpObserver->GetPlayerVelByMove() , GetSelf().GetVelDelay() , GetSelf().GetVelConf());
	}

	//预测身体角度
	if (mpObserver->GetPlayerTurnTime() == mpObserver->CurrentTime())
	{
		if (!is_estimate_to_now)
		{
			SelfState().UpdateBodyDir(mpObserver->GetPlayerBodyDirByTurn() , GetSelf().GetBodyDirDelay() , GetSelf().GetBodyDirConf());
		}
		else
		{
			const PlayerState& player = mpWorldState->GetHistory(1)->GetTeammate(mSelfUnum);
			double eff_moment = mpObserver->GetPlayerBodyDirByTurn() - player.GetBodyDir();
			double body_dir = GetNormalizeAngleDeg(eff_moment + SelfState().GetBodyDir());
			SelfState().UpdateBodyDir(body_dir , GetSelf().GetBodyDirDelay() , GetSelf().GetBodyDirConf());
		}
	}

	//预测头部角度
	if (!is_estimate_to_now && mpObserver->GetPlayerTurnNeckTime() == mpObserver->CurrentTime())
	{
		SelfState().UpdateNeckDir(mpObserver->GetPlayerNeckDirByTurnNeck() , GetSelf().GetNeckDirDelay() , GetSelf().GetNeckDirConf());
	}
}

void WorldStateUpdater::EstimateBall(bool is_estimate_to_now , int cycle)
{
	if (!is_estimate_to_now &&  mpObserver->GetBallKickTime() == mpObserver->CurrentTime())
	{
		double eps = (2 * SelfState().GetKickRand() + mpObserver->GetBallVelByKick().Mod() / ServerParam::instance().ballDecay()) * ServerParam::instance().ballRand();
		Ball().UpdatePosEps(Ball().GetPosEps() + eps);
		Ball().UpdateVelEps(Ball().GetVelEps() + eps  * ServerParam::instance().ballDecay());
		Ball().UpdatePos(mpObserver->GetBallPosByKick() , GetBall().GetPosDelay() , GetBall().GetPosConf());
		Ball().UpdateVel(mpObserver->GetBallVelByKick() , GetBall().GetVelDelay() , GetBall().GetVelConf());
	}
	else if(is_estimate_to_now && cycle == 1 && mpObserver->GetBallKickTime() == mpObserver->CurrentTime())
	{
		Vector pos = mpWorldState->GetHistory(1)->GetBall().GetPos();

		Vector vel = mpObserver->GetBallPosByKick() - pos;
		pos = vel + Ball().GetPos();
		vel  *= ServerParam::instance().ballDecay();

		Ball().UpdatePos(pos ,  GetBall().GetPosDelay() , GetBall().GetPosConf());
		Ball().UpdateVel(vel ,  GetBall().GetVelDelay() , GetBall().GetVelConf());
	}
	else
	{
		double eps = (Ball().GetVel().Mod() + Ball().GetVelEps()) * ServerParam::instance().ballRand() * ServerParam::instance().ballDecay();
		Ball().UpdateVelEps(Ball().GetVelEps() + eps);

		eps = (Ball().GetVel().Mod() + Ball().GetVelEps() ) * ServerParam::instance().ballRand();
		Ball().UpdatePosEps(Ball().GetPosEps() + eps + Ball().GetVelEps());

		ComputeNextCycle(Ball() , ServerParam::instance().ballDecay());
		Ball().UpdatePos(LimitToField(Ball().GetPos()) , Ball().GetPosDelay() ,Ball().GetPosConf()); //limit ball in  field
	}

}

void WorldStateUpdater::EstimatePlayers()
{
	//看到速度的下一周期预测队员将按照前一周期的速度继续运行
	for (Unum i = 1;i <= TEAMSIZE;i++)
	{
		if (i != mSelfUnum){

			if (Teammate(i).GetVelDelay() <= 1)
			{
				Teammate(i).UpdateVel(Teammate(i).GetVel() / Teammate(i).GetPlayerDecay() , Teammate(i).GetVelDelay() , Teammate(i).GetVelConf());
			}
			ComputeNextCycle(Teammate(i), PlayerParam::instance().HeteroPlayer(Teammate(i).GetPlayerType()).playerDecay());
		}

		if (Opponent(i).GetVelDelay() <= 1)
		{
			Opponent(i).UpdateVel(Opponent(i).GetVel() / Opponent(i).GetPlayerDecay(), Opponent(i).GetVelDelay() , Opponent(i).GetVelConf());
		}
		ComputeNextCycle(Opponent(i), PlayerParam::instance().HeteroPlayer(Opponent(i).GetPlayerType()).playerDecay());
	}
}

Unum WorldStateUpdater::GetSeenClosestTeammate()
{
	Unum num = 0;
	double min = 20000;
	for (int i = 1; i <= TEAMSIZE; i++)
	{
		if (i != mSelfUnum){
			if (mpObserver->Teammate(i).GetDist().time() == mpObserver->CurrentTime())
			{
				if (mpObserver->Teammate(i).Dist() < min)
				{
					min = mpObserver->Teammate(i).Dist();
					num = i;
				}
			}
		}
	}

	return num;
}

Unum WorldStateUpdater::GetSeenClosestOpponent()
{
	Unum num = 0;
	double min = 20000;
	for (int i = 1;i <=TEAMSIZE;i++)
	{
		if (mpObserver->Opponent(i).GetDist().time() == mpObserver->CurrentTime())
		{
			if (mpObserver->Opponent(i).Dist() < min)
			{
				min = mpObserver->Opponent(i).Dist();
				num = i;
			}
		}
	}

	return num;
}

bool WorldStateUpdater::ComputeNextCycle(MobileState& ms , double decay)
{
	ms.UpdatePos(ms.GetPos() + ms.GetVel(), ms.GetPosDelay(), ms.GetPosConf());
	ms.UpdateVel(ms.GetVel() * decay, ms.GetVelDelay(), ms.GetVelConf());

	return true;
}

void WorldStateUpdater::EvaluateConf()
{
	for (int i = 1;i <= TEAMSIZE;i++)
	{
		if (i != mSelfUnum) {
			EvaluatePlayer(Teammate(i));
		}

		EvaluatePlayer(Opponent(i));
	}

	//根据delay置球员生死
	int dead_level = 100;
	for (int i = 1;i <= TEAMSIZE;i++)
	{
		if (i != mSelfUnum && GetTeammate(i).IsAlive())
		{
			if (GetTeammate(i).GetPosDelay() > dead_level)
			{
				Teammate(i).SetIsAlive(false);
			}
		}

		if (GetOpponent(i).IsAlive())
		{
			if (GetOpponent(i).GetPosDelay() > dead_level)
			{
				Opponent(i).SetIsAlive(false);
			}
		}
	}

	//evaluate ball
	EvaluateBall();
}


void WorldStateUpdater::EvaluateBall()
{
	//if drop ball just forget
	//
	if (mpWorldState->IsBallDropped() && Ball().GetPosDelay() != 0)
	{
		EvaluateForgetBall(true);
		return;
	}

	if (!mpObserver->IsNewSight())
	{
		return;
	}

	if (Ball().GetPosDelay() != 0 && SelfState().GetPosDelay() == 0)
	{
		if (Ball().GetGuessedTimes() >= 100)
		{
			EvaluateForgetBall(false);
		}
		else {
			if (ShouldSee(Ball().GetPos())) {
				Ball().UpdateGuessedTimes(Ball().GetGuessedTimes() + 1);

				const Vector pos = GetNearSidePos(Ball().GetPos());

				Ball().UpdatePos(
						pos,
						Ball().GetPosDelay(),
						Ball().GetPosConf()
				);

				Logger::instance().GetTextLogger("guess") << mpWorldState->CurrentTime() << ": guess ball out of view" << std::endl;
			}
		}
	}
}

void WorldStateUpdater::EvaluateForgetBall(bool use_memory)
{
	if (!use_memory) {
		Ball().UpdatePos(Vector(0,0), Ball().GetPosDelay(), 0); //as WE2008
		Ball().UpdateVel(Vector(0,0), Ball().GetPosDelay(), 0); //as WE2008
	}

	Ball().UpdateGuessedTimes(100);

	Logger::instance().GetTextLogger("forget") << mpWorldState->CurrentTime() << ": forget ball " << use_memory << std::endl;
}

Vector WorldStateUpdater::GetNearSidePos(const Vector & pos, const Vector *expected_pos)
{
	const ViewWidth view_width = GetSelf().GetViewWidth();
	const AngleDeg view_angle = sight::ViewAngle(view_width);
	const AngleDeg left = GetSelf().GetNeckGlobalDir() - view_angle * 0.5;
	const AngleDeg right = left + view_angle;
	const AngleDeg rel_dir = (pos - GetSelf().GetPos()).Dir();

	Vector pos1 = GetSelf().GetPos() + (pos - GetSelf().GetPos()).Rotate(left - rel_dir - FLOAT_EPS);
	Vector pos2 = GetSelf().GetPos() + (pos - GetSelf().GetPos()).Rotate(right - rel_dir + FLOAT_EPS);

	if (!expected_pos) {
		expected_pos = &pos;
	}

	if (pos1.Dist(*expected_pos) < pos2.Dist(*expected_pos)) {
		return pos1;
	}
	else {
		return pos2;
	}
}

bool WorldStateUpdater::ShouldSee(const Vector & pos)
{
	if (!mpObserver->IsNewSight())
	{
		return false;
	}

	const ViewWidth view_width = GetSelf().GetViewWidth();
	const AngleDeg view_angle = sight::ViewAngle(view_width);
	const AngleDeg left = GetSelf().GetNeckGlobalDir() - view_angle * 0.5;
	const AngleDeg right = left + view_angle;
	const Vector self_pos = SelfState().GetPos();
	const Vector rel_pos = (pos - self_pos);
	const double dist = rel_pos.Mod();

	return dist < ServerParam::instance().visibleDistance() - 0.1 || IsAngleDegInBetween(left, rel_pos.Dir(), right);
}

void WorldStateUpdater::EvaluatePlayer(PlayerState& player)
{
	if (!mpObserver->IsNewSight())
	{
		return;
	}

	if (player.GetPosDelay() != 0 && player.IsAlive() && SelfState().GetPosDelay() == 0)
	{
		const Unum player_unum = player.GetUnum();

		if (player.GetGuessedTimes() >= 5)
		{
			EvaluateForgetPlayer(player);
		}
		else {
			if (ShouldSee(player.GetPos())) {
				player.UpdateGuessedTimes(player.GetGuessedTimes() + 1);

				const Vector *expected_pos = 0;

				if (player.IsGoalie()) {
					if (player_unum > 0) {
						expected_pos = &ServerParam::instance().ourGoal();
					}
					else {
						expected_pos = &ServerParam::instance().oppGoal();
					}
				}

				const Vector pos = GetNearSidePos(player.GetPos(), expected_pos);

				player.UpdatePos(
						pos,
						player.GetPosDelay(),
						player.GetPosConf()
				); //delay和conf在前面更新过

				Logger::instance().GetTextLogger("guess") << mpWorldState->CurrentTime() << ": guess player " << player_unum << " out of view" << std::endl;
			}
		}
	}
}

void WorldStateUpdater::EvaluateForgetPlayer(PlayerState& player)
{
	//i think it will cause trouble with UpdateUnknownPlayer ,it need test.
	player.SetIsAlive(false);
	player.UpdatePos(Vector(0,0) , player.GetPosDelay() , 0);
	player.UpdateVel(Vector(0,0) , player.GetVelDelay() , 0);
	player.UpdateBodyDir(0 , player.GetBodyDirDelay() , 0);
	player.UpdateNeckDir(0 , player.GetNeckDirDelay() , 0);
	player.UpdateGuessedTimes(100);

	Logger::instance().GetTextLogger("forget") << mpWorldState->CurrentTime() << ": forget player " << player.GetUnum() << std::endl;
}

double  WorldStateUpdater::CalcPlayerGotoPoint(PlayerState& player , double dist)
{
	//very simple ,different from WE2008 ,it will be changed if not so good
	double max_speed = player.GetEffectiveSpeedMax() ;

	return dist / max_speed ;
}

Unum  WorldStateUpdater::GetClosestOpponent()
{
	double min_dist = 200000;
	int  min_index = -1;
	for (int i = 1;i <= TEAMSIZE;i++)
	{
		if(GetOpponent(i).IsAlive())
		{
			if ((GetOpponent(i).GetPos() - SelfState().GetPos()).Mod() < min_dist)
			{
				min_dist = (GetOpponent(i).GetPos() - SelfState().GetPos()).Mod();
				min_index = i;
			}
		}
	}
	return min_index;
}

Unum WorldStateUpdater::GetClosestPlayerToBall()
{
	double min_dist = 20000;
	int min_index = 0;
	for (int i = 1;i <= TEAMSIZE;i++)
	{
		if (GetTeammate(i).IsAlive())
		{
			if ((GetTeammate(i).GetPos() - Ball().GetPos()).Mod() < min_dist)
			{
				min_dist = (GetTeammate(i).GetPos() - Ball().GetPos()).Mod();
				min_index = i;
			}
		}
		if (GetOpponent(i).IsAlive())
		{
			if ((GetOpponent(i).GetPos() - Ball().GetPos()).Mod() < min_dist)
			{
				min_dist = (GetOpponent(i).GetPos() - Ball().GetPos()).Mod();
				min_index = - i;
			}
		}
	}


	return min_index;
}

bool WorldStateUpdater::ComputeShouldSeeOrNot(Vector pos)
{
	double view_angle = 0;
	switch(GetSelf().GetViewWidth())
	{
	case VW_Narrow:
		view_angle = 60;
		break;
	case VW_Normal:
		view_angle = 120;
		break;
	case VW_Wide:
		view_angle = 180;
		break;
	default:
		view_angle = 120;
	}

	//获得位置相对自己的夹角
	Vector dist_vec = pos - GetSelf().GetPos();
	double angle = GetNormalizeAngleDeg(dist_vec.Dir());
	angle = ::fabs(::GetNormalizeAngleDeg(angle - GetSelf().GetNeckGlobalDir()));
	double anglebuf = ASin((1 * 0.6 + 0.05 * dist_vec.Mod() ) / dist_vec.Mod());
	angle += anglebuf; //different from ComputePlayerMaySeeOrNot

	//完全视角内可见
	double distbuf = 0.05 * ServerParam::instance().visibleDistance();
	if (dist_vec.Mod() <= ServerParam::instance().visibleDistance() - distbuf)
	{
		return true;
	}

	//角度在视角内
	if (angle > view_angle / 2)
	{
		return false;
	}

	return true;

}
void WorldStateUpdater::UpdateInfoFromPlayMode()
{
	double x ,y;

	switch(mpWorldState->GetPlayMode())
	{
	case PM_Our_Kick_Off:
	case PM_Opp_Kick_Off:
	case PM_Before_Kick_Off:
	case PM_Goal_Opps :
	case PM_Goal_Ours :
		Ball().UpdatePos(Vector(0, 0));
		Ball().UpdateVel(Vector(0, 0));
		break;

	case PM_Our_Penalty_Setup :
	case PM_Opp_Penalty_Setup :
        Ball().UpdatePos(Vector(ServerParam::instance().PITCH_LENGTH / 2.0 - ServerParam::instance().penDistX(), 0.0));
		Ball().UpdateVel(Vector(0, 0));
        break;

	case PM_Our_Goalie_Free_Kick :
        Ball().UpdateVel(Vector(0, 0));
        if (mSelfUnum == mpWorldState->GetTeammateGoalieUnum())
        {
            const PlayerState & self_state = mpWorldState->GetTeammate(mSelfUnum);
            const double dist = self_state.GetPlayerSize() + ServerParam::instance().ballSize();
            Ball().UpdatePos(self_state.GetPos() + Vector(dist, 0.0).Rotate(self_state.GetBodyDir()));
        }
        break;
	
	case PM_Our_Kick_In:
	case PM_Opp_Kick_In:
		x = Ball().GetPos().X();
		y = Sign(mpWorldState->GetHistory(1)->GetBall().GetPos().Y()) * ServerParam::instance().PITCH_WIDTH * 0.5;
		Ball().UpdatePos(Vector(x, y));
		Ball().UpdateVel(Vector(0, 0));
		break;

	case PM_Our_Corner_Kick:
		x = ServerParam::instance().PITCH_LENGTH * 0.5 - 1.0;
		y = Sign(mpWorldState->GetHistory(1)->GetBall().GetPos().Y()) * (ServerParam::instance().PITCH_WIDTH * 0.5 - 1.0);
		Ball().UpdatePos(Vector(x, y));
		Ball().UpdateVel(Vector(0, 0));
		break;

	case PM_Opp_Corner_Kick:
		x = -ServerParam::instance().PITCH_LENGTH * 0.5 - 1.0;
		y = Sign(mpWorldState->GetHistory(1)->GetBall().GetPos().Y()) * (ServerParam::instance().PITCH_WIDTH * 0.5 - 1.0);
		Ball().UpdatePos(Vector(x, y));
		Ball().UpdateVel(Vector(0, 0));
		break;

	default:
		break;
	}

	//以下信息只在playMode改变时更新
	if (mpWorldState->GetPlayModeTime() == mpWorldState->CurrentTime())
	{
		switch(mpWorldState->GetPlayMode())
		{
		case PM_Our_Kick_Off:
		case PM_Opp_Kick_Off:
			//Ball().UpdatePos(Vector(0,0));
			//Ball().UpdateVel(Vector(0,0));
			break;
		case PM_Our_Goal_Kick:
			x = -1.0 * (ServerParam::instance().PITCH_LENGTH * 0.5 - ServerParam::instance().GOAL_AREA_LENGTH);
			y = Sign(mpWorldState->GetHistory(1)->GetBall().GetPos().Y()) * ServerParam::instance().GOAL_AREA_WIDTH * 0.5;
			Ball().UpdatePos(Vector(x ,y));
			Ball().UpdateVel(Vector(0, 0));
			break;
		case PM_Opp_Goal_Kick:
			x = 1.0 * (ServerParam::instance().PITCH_LENGTH * 0.5 - ServerParam::instance().GOAL_AREA_LENGTH);
			y = Sign(mpWorldState->GetHistory(1)->GetBall().GetPos().Y()) * ServerParam::instance().GOAL_AREA_WIDTH * 0.5;
			Ball().UpdatePos(Vector(x ,y));
			Ball().UpdateVel(Vector(0, 0));
			break;
		case PM_Our_Back_Pass_Kick:
		case PM_Opp_Back_Pass_Kick:
        case PM_Our_Foul_Charge_Kick: 
        case PM_Opp_Foul_Charge_Kick: 
			//TODO:we2008算法使用到了截球点,但是WE2009中此结果不支持使用截球信息,因此对此做了简化.
			Ball().UpdateVel(Vector(0, 0));
			break;
		case PM_Our_Offside_Kick:
		case PM_Opp_Offside_Kick:
			Ball().UpdateVel(Vector(0, 0));
			break;
		default:
			break;
		}
	}
}

void WorldStateUpdater::MaintainPlayerStamina()
{
	for (unsigned int i = 0; i < mpWorldState->GetPlayerList().size(); ++i) {
		PlayerState * player = mpWorldState->GetPlayerList()[i];

        // 更新所有球员的mMinStamina
        player->UpdateMinStamina(PlayerParam::instance().MinStamina());
        if (player->GetUnum() == mSelfUnum && // 暂时只考虑自己
            mpWorldState->CurrentTime().T() <= ServerParam::instance().halfTime() * ServerParam::instance().nrNormalHalfs()) // 常规时间
        {
            const int reserve_cycle = ServerParam::instance().halfTime() - mpWorldState->CurrentTime().T() % ServerParam::instance().halfTime(); // 这个半场还剩多少周期
            const int cycle_buffer = (int)((player->GetStamina() + player->GetExtraStamina()) / ServerParam::instance().maxPower());
            if (reserve_cycle <= cycle_buffer) // 近似认为每个半场的最后若干周期可以不考虑体力
            {
                player->UpdateMinStamina(0.0);
            }
        }

		const PlayerState * last_cycle_player = & mpWorldState->GetHistory(1)->GetPlayer(player->GetUnum());
		if (player->GetUnum() == mSelfUnum) continue;
		if (player->IsAlive() && last_cycle_player->IsAlive()) {
			double power = 0.0; //认为没有dash

			if (player->GetVelDelay() == 0 && last_cycle_player->GetVelDelay() == 0) { //一次确定的dash -- 暂时不考虑侧dash
				double acc = (player->GetVel() - last_cycle_player->GetVel() * player->GetDecay()).Mod();
				power = acc / (player->GetDashPowerRate() * last_cycle_player->GetEffort());
			}

			double stamina = last_cycle_player->GetStamina();
			double recovery = last_cycle_player->GetRecovery();
			double effort = last_cycle_player->GetEffort();

			stamina -= (power > 0.0)? power: -2.0 * power;
			if( stamina  <= ServerParam::instance().recoverDecStamina() && recovery > ServerParam::instance().recoverMin() )
				recovery -= ServerParam::instance().recoverDec();
			if( stamina  <= ServerParam::instance().effortDecStamina() && effort > last_cycle_player->GetEffortMin() )
				effort -= ServerParam::instance().effortDec();
			if( stamina  >= ServerParam::instance().effortIncStamina() && effort < 1.0 ) {
				effort += ServerParam::instance().effortInc();
				if ( effort > 1.0 ) {
					effort = 1.0;
				}
			}
			stamina  += recovery * last_cycle_player->GetStaminaIncMax();
			if ( stamina  > ServerParam::instance().staminaMax() ) {
				stamina  = ServerParam::instance().staminaMax();
			}
			// tracer stamina may error, give the min stamina
			if (stamina < ServerParam::instance().recoverDecStamina() + 100) {
				stamina = ServerParam::instance().recoverDecStamina() + 100;
			}

			player->UpdateStamina(stamina);
			player->UpdateEffort(effort);
			player->UpdateRecovery(recovery);
		}
		else {
			player->UpdateStamina(ServerParam::instance().staminaMax());
			player->UpdateEffort(player->GetEffortMax());
			player->UpdateRecovery(1.0);
		}

		if (player->GetUnum() > 0) {
			if (player->GetArmPointDelay() == 0 && fabs(player->GetArmPointDir()) > MAX_POINT_DIR + 30.0) { //队友因为没体力point
//				std::cout << "#" << SelfState().GetUnum() << " see @" << mpWorldState->CurrentTime() << ": teammate " << player->GetUnum() << " point dir 180" << std::endl;
			//	if(mIsZeroStamina[player->GetUnum()])
			//		player->UpdateStamina(0.0);
			//	else
			//		if(!mIsZeroStamina[player->GetUnum()]&&fabs(player->GetStamina())<FLOAT_EPS)
			//			player->UpdateStamina(PlayerParam::instance().LowStaminaPointThr());
			//		else
				player->UpdateStamina(Min(PlayerParam::instance().LowStaminaPointThr(), player->GetStamina()));
			}
		}
	}
}

void WorldStateUpdater::MaintainConsistency()
{
	for (unsigned int i = 0; i < mpWorldState->GetPlayerList().size(); ++i) {
		PlayerState * player = mpWorldState->GetPlayerList()[i];
		if (player->GetUnum() == mSelfUnum) continue;
		if (player->IsAlive()) {
			if (player->IsGoalie() || player->GetPlayerType() != 0) {
				Vector vel = player->GetVel();
				if (vel.Mod() > player->GetEffectiveSpeedMax() * player->GetDecay()) {
					vel = vel.Normalize() * player->GetEffectiveSpeedMax() * player->GetDecay();
					player->UpdateVel(vel, player->GetVelDelay(), player->GetVelConf());
				}
			}
		}
	}

    Vector vel = Ball().GetVel();
	if (vel.Mod() > ServerParam::instance().ballSpeedMax() * ServerParam::instance().ballDecay()) {
		vel = vel.Normalize() * ServerParam::instance().ballSpeedMax() * ServerParam::instance().ballDecay();
		Ball().UpdateVel(vel, Ball().GetVelDelay(), Ball().GetVelConf());
	}

	{
		Vector pos = Ball().GetPos();

		if (pos.X() >= ServerParam::instance().PITCH_LENGTH * 0.5) {
			pos.SetX(ServerParam::instance().PITCH_LENGTH * 0.5 - FLOAT_EPS);
		}
		else if (pos.X() <= -ServerParam::instance().PITCH_LENGTH * 0.5) {
			pos.SetX(-ServerParam::instance().PITCH_LENGTH * 0.5 + FLOAT_EPS);
		}
		if (pos.Y() >= ServerParam::instance().PITCH_WIDTH * 0.5) {
			pos.SetY(ServerParam::instance().PITCH_WIDTH * 0.5 - FLOAT_EPS);
		}
		else if (pos.Y() <= -ServerParam::instance().PITCH_WIDTH * 0.5) {
			pos.SetY(-ServerParam::instance().PITCH_WIDTH * 0.5 + FLOAT_EPS);
		}

		Ball().UpdatePos(pos, Ball().GetPosDelay(), Ball().GetPosConf());
	}
}

double WorldStateUpdater::ComputeTackleProb(const Unum & unum, bool foul)
{
    const PlayerState & player = mpWorldState->GetPlayer(unum);

    if (mpWorldState->GetPlayMode() == PM_Before_Kick_Off ||
        player.IsAlive() == false ||
        player.IsIdling() == true)
    {
        return 0.0;
    }

    const Vector & ball_pos = mpWorldState->GetBall().GetPos();
    const Vector & player_pos = player.GetPos();
    Vector ball_2_player = ball_pos - player_pos;
    if (ball_2_player.Mod() > ServerParam::instance().maxTackleDist())
    {
        return 0.0;
    }

    const Vector & v = player.GetVel();
    double body_dir = player.GetBodyDir();
    if (player.GetBodyDirConf() > 0.9)
    {
        body_dir = player.GetBodyDir();
	}
    else if (v.Mod() > 0.2)
    {
		body_dir = v.Dir();
	}
    else
    {
        Vector posdiff = player.GetPos() - mpWorldState->GetHistory(1)->GetPlayer(unum).GetPos();
		if (posdiff.Mod() > 0.12)
        {
			body_dir = posdiff.Dir();
		}
	}

    ball_2_player = ball_2_player.Rotate(-body_dir);

    if (foul) {
    	foul = false;
    	if (PlayerParam::instance().playerVersion() > 14.0) {
    		for (unsigned i = 0; i < mpWorldState->GetPlayerList().size(); ++i) { //如对手无人可踢，则退化为正常铲球
    			const PlayerState & opp = *mpWorldState->GetPlayerList()[i];

    			if (!opp.IsAlive()) continue; //dead
    			if (opp.GetUnum() * unum > 0) continue; //not opp
    			if (!opp.IsKickable()) continue; //not kickable

    			foul = true;
    			break;
    		}
    	}
    }

	return GetTackleProb(ball_2_player, foul);
}

void HistoryState::UpdateHistory(const WorldState &world)
{
	mNum = mNum % HISTORY_SIZE;
	mRecord[mNum++] = world;
}

WorldState *HistoryState::GetHistory(int num)
{
	Assert(num > 0);

	num = num % HISTORY_SIZE;
	num = (mNum + HISTORY_SIZE - num) % HISTORY_SIZE;

	return &mRecord[num];
}
