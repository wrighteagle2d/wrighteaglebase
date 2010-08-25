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

#include "Simulator.h"
#include "ActionEffector.h"
#include "Dasher.h"

Simulator::Simulator() {
}

Simulator::~Simulator() {
}

Simulator & Simulator::instance()
{
	static Simulator simulator;
	return simulator;
}

void Simulator::Player::Dash(double power, int dir_idx)
{
	power = GetNormalizeDashPower(power);

	AngleDeg dir = Dasher::DASH_DIR[dir_idx];
	double dir_rate = Dasher::DIR_RATE[dir_idx];;

	bool back_dash = power < 0.0;

	double power_need = ( back_dash
			? power * -2.0
					: power );
	power_need = std::min( power_need, mStamina + PlayerParam::instance().HeteroPlayer(mPlayerType).extraStamina() );
	mStamina = std::max( 0.0, mStamina - power_need );

	power = ( back_dash
			? power_need / -2.0
					: power_need );


	double acc = std::fabs(mEffort * power * dir_rate * PlayerParam::instance().HeteroPlayer(mPlayerType).dashPowerRate());

	if ( back_dash ) {
		dir += 180.0;
	}

	mVel += Polar2Vector(acc, GetNormalizeAngleDeg(mBodyDir + dir));
	Step();
}

void Simulator::Player::Act(const AtomicAction & act)
{
	switch (act.mType) {
	case CT_Turn: Turn(GetTurnMoment(act.mTurnAngle, mPlayerType, mVel.Mod())); break;
	case CT_Dash: Dash(act.mDashPower, Dasher::GetDashDirIdx(act.mDashDir)); break;
	case CT_Kick: Step(); break;
	case CT_None: break;
	default: Assert(0); break;
	}
}
