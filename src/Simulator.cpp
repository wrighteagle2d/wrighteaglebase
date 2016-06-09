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
