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

#include "PlayerState.h"
#include "ActionEffector.h"
#include "Simulator.h"
#include "Dasher.h"

PlayerState::PlayerState():
	MobileState(ServerParam::instance().playerDecay(), PlayerParam::instance().HeteroPlayer(0).effectiveSpeedMax())
{
	mBallCatchable = false;
	mCatchBan = 0;
	mCollideWithBall = false;
	mCollideWithPlayer = false;
	mCollideWithPost = false;
	mIsAlive = false;
	mIsGoalie = false;
    mIsSensed = false;
	mIsKicked = false;
	mIsPointing = false;
	mKickRate = 0.0;
	mIsKickable = false;
	mMaxTurnAngle = 0.0;
	mPlayerType = 0;

	mStamina = ServerParam::instance().staminaMax();
	mEffort  = ServerParam::instance().effortInit();
	mCapacity = ServerParam::instance().staminaCapacity();
	mRecovery = ServerParam::instance().recoverInit();

	mTackleBan = 0;
	mUnderDangerousTackleCondition = false;

	mFoulChargedCycle = 0;

	mUnum = 0;

	mViewWidth = VW_Normal;
	mIsTired = false;
    mMinStamina = PlayerParam::instance().MinStamina();

	mCardType = CR_None;
	mIsBodyDirMayChanged = true;
}

bool PlayerState::IsKickable(const BallState & ball_state, const double & buffer) const
{
	if (ball_state.GetPosConf() < FLOAT_EPS || GetPosConf() < FLOAT_EPS) return false;
	double dis2self = GetPos().Dist(ball_state.GetPos());
	return (dis2self <= GetKickableArea() - buffer);
}

void PlayerState::GetReverseFrom(const PlayerState & o)
{
	SetIsAlive(o.IsAlive());

	UpdateIsGoalie(o.IsGoalie());
	UpdateIsSensed(o.IsSensed());

	UpdatePlayerType(o.GetPlayerType());
	UpdateViewWidth(o.GetViewWidth());

	UpdateBallCatchable(o.IsBallCatchable());
	UpdateCatchBan(o.GetCatchBan());

	UpdateKickable(o.IsKickable());
	UpdateKicked(o.IsKicked());
	UpdateCardType(o.GetCardType());

	UpdateStamina(o.GetStamina());
	UpdateEffort(o.GetEffort());
	UpdateCapacity(o.GetCapacity());
	UpdateRecovery(o.GetRecovery());

	UpdateMaxTurnAngle(o.GetMaxTurnAngle());

	UpdateBodyDir(GetNormalizeAngleDeg(o.GetBodyDir() + 180.0), o.GetBodyDirDelay(), o.GetBodyDirConf());
	UpdateNeckDir(GetNormalizeAngleDeg(o.GetNeckDir() + 180.0), o.GetNeckDirDelay(), o.GetNeckDirConf());

	UpdatePos(o.GetPos().Rotate(180.0), o.GetPosDelay(), o.GetPosConf());
	UpdateVel(o.GetVel().Rotate(180.0), o.GetVelDelay(), o.GetVelConf());

	UpdateTackleBan(o.GetTackleBan());
	UpdateTackleProb(o.GetTackleProb(false), false);
	UpdateTackleProb(o.GetTackleProb(true), true);
	UpdateDangerousTackleCondition(o.UnderDangerousTackleCondition());

	UpdateFoulChargedCycle(o.GetFoulChargedCycle());

	UpdateArmPoint(GetNormalizeAngleDeg(o.GetArmPointDir() + 180.0), o.GetArmPointDelay(), o.GetArmPointConf(), o.GetArmPointDist(), o.GetArmPointMovableBan(), o.GetArmPointExpireBan());
	UpdateFocusOn(o.GetFocusOnSide(), o.GetFocusOnUnum(), o.GetFocusOnDelay(), o.GetFocusOnConf());
}

Vector PlayerState::GetPredictedPosWithDash(int steps, double dash_power, AngleDeg dash_dir) const
{
	Simulator::Player self(*this);
	int dash_dir_idx = Dasher::GetDashDirIdx(dash_dir);

	for (int i = 0; i < steps; ++i) {
		self.Dash(dash_power, dash_dir_idx);
	}

	return self.mPos;
}

Vector PlayerState::GetPredictedVelWithDash(int steps, double dash_power, AngleDeg dash_dir) const
{
	Simulator::Player self(*this);
	int dash_dir_idx = Dasher::GetDashDirIdx(dash_dir);

	for (int i = 0; i < steps; ++i) {
		self.Dash(dash_power, dash_dir_idx);
	}

	return self.mVel;
}

double PlayerState::CorrectDashPowerForStamina(double dash_power) const
{
	double stamina = GetStamina();

	if (IsOutOfStamina()) {
		return dash_power; //已经没有容量了，不需要在控制了
	}

	double new_power;

	if (dash_power >= 0) {
		new_power = Min( dash_power, stamina - PlayerParam::instance().MinStamina() );
		if ( new_power < 0 ) new_power = 0;
	}
	else {
		new_power = Min( -dash_power, (stamina - PlayerParam::instance().MinStamina()) / 2.0);
		if ( new_power < 0 ) new_power = 0;

		new_power = -new_power;
	}

	return new_power;
}

AngleDeg PlayerState::GetEffectiveTurn(AngleDeg moment, double my_speed) const
{
	return GetTurnAngle(moment, GetPlayerType(), my_speed);
}

double PlayerState::GetControlBallProb(const Vector & ball_pos) const
{
	const double dist = GetPos().Dist(ball_pos);

	if (dist < GetKickableArea()) {
		return 1.0; //已经可踢了
	}

	if (IsGoalie()) {
		return Max(GetCatchProb(dist), ::GetTackleProb(ball_pos, GetPos(), GetBodyDir(), false));
	}
	else {
		return ::GetTackleProb(ball_pos, GetPos(), GetBodyDir(), false);
	}
}

double PlayerState::GetCatchProb( const double & dist ) const
{
	if ( dist < GetMinCatchArea() ) {
		return ServerParam::instance().catchProb();
	}
	else {
		double min_length = ServerParam::instance().catchAreaLength() * ( 2.0 - GetCatchAreaLStretch() );
		double max_length = ServerParam::instance().catchAreaLength() * GetCatchAreaLStretch();
		if ( max_length < min_length + FLOAT_EPS ) return 0.0;
		double delt = Sqrt( dist * dist - ServerParam::instance().catchAreaWidth() * ServerParam::instance().catchAreaWidth() / 4 );
		if ( delt > max_length ) return 0.0;
		double dx = delt - min_length;
		return MinMax( 0.0, ServerParam::instance().catchProb() - ServerParam::instance().catchProb() * dx / ( max_length - min_length ), ServerParam::instance().catchProb());
	}
}

//end of file pLayerState.cpp
