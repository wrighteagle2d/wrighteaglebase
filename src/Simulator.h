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

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include "Geometry.h"
#include "ServerParam.h"
#include "PlayerParam.h"
#include "PlayerState.h"
#include "ActionEffector.h"
#include <vector>

struct AtomicAction;

class Simulator {
	Simulator();

public:
	virtual ~Simulator();

	static Simulator & instance();

public:
	struct Ball {
		Vector mPos;
		Vector mVel;

	public:
		Ball(const Vector & pos, const Vector & vel): mPos(pos), mVel(vel) { }

		Vector noise() {
		    return Polar2Vector( drand( 0.0, ServerParam::instance().ballRand() * mVel.Mod() ), drand( -180.0, 180.0 ) );
		}

		void Step() {
			mPos += mVel;
			mVel *= ServerParam::instance().ballDecay();
		}

		void RandomizedStep() {
			mVel += noise();
			mPos += mVel;
			mVel *= ServerParam::instance().ballDecay();
		}
	};

	struct Player {
		Vector mPos;
		Vector mVel;
		AngleDeg mBodyDir;

		int mStamina;
		double mEffort;

		int mPlayerType;

	public:
		Player(const PlayerState & player):
			mPos(player.GetPos()),
			mVel(player.GetVel()),
			mBodyDir(player.GetBodyDir()),
			mStamina(player.GetStamina()),
			mEffort(player.GetEffort()),
			mPlayerType(player.GetPlayerType())
		{
		}

		Player(const Vector & pos, const Vector & vel, const AngleDeg & body_dir, const int & player_type, int stamina = 8000, double effort = 1.0):
			mPos(pos),
			mVel(vel),
			mBodyDir(body_dir),
			mStamina(stamina),
			mEffort(effort),
			mPlayerType(player_type)
		{
		}

		double GetControlBallProb(const Vector & ball_pos, const PlayerState & real_player, const bool foul = false) const {
			const double dist = mPos.Dist(ball_pos);

			if (dist < real_player.GetKickableArea()) {
				return 1.0;
			}

			if (real_player.IsGoalie()) {
				return Max(real_player.GetCatchProb(dist), GetTackleProb(ball_pos, mPos, mBodyDir, foul));
			}
			else {
				return GetTackleProb(ball_pos, mPos, mBodyDir, foul);
			}
		}

		void Dash(double power, int dir_idx);

		void Turn(const AngleDeg & moment) {
	        mBodyDir = GetNormalizeAngleDeg( mBodyDir + GetNormalizeMoment( moment ) / ( 1.0 + PlayerParam::instance().HeteroPlayer(mPlayerType).inertiaMoment() * mVel.Mod() ) );
	        Step();
		}

		void Step() {
			mPos += mVel;
			mVel *= PlayerParam::instance().HeteroPlayer(mPlayerType).playerDecay();

			UpdateStamina();
		}

#ifndef WIN32
		void Radomize() {
			const double x = drand48() * ServerParam::instance().PITCH_LENGTH - ServerParam::instance().PITCH_LENGTH * 0.5;
			const double y = drand48() * ServerParam::instance().PITCH_WIDTH - ServerParam::instance().PITCH_WIDTH * 0.5;
			const double speed = drand48() * PlayerParam::instance().HeteroPlayer(mPlayerType).effectiveSpeedMax() * PlayerParam::instance().HeteroPlayer(mPlayerType).playerDecay();
			const AngleDeg speed_dir = GetNormalizeAngleDeg(360.0 * drand48());
			const AngleDeg body_dir = GetNormalizeAngleDeg(360.0 * drand48());

			mPos = Vector(x, y);
			mVel = Polar2Vector(speed, speed_dir);
			mBodyDir = body_dir;
		}
#endif

		void RecoverAll() {
			mEffort = 1.0;
			mStamina = ServerParam::instance().staminaMax();
		}

		void Act(const AtomicAction & act);

		friend std::ostream & operator<<(std::ostream & os, const Player & player) {
			return os << "(" <<  player.mPos << " " << player.mVel << " " << player.mBodyDir << ")";
		}

	private:
		void UpdateStamina() {
		    if ( mStamina <= ServerParam::instance().effortDecThr() * ServerParam::instance().staminaMax() )  {
		        if ( mEffort > PlayerParam::instance().HeteroPlayer(mPlayerType).effortMin() )  {
		        	mEffort -= ServerParam::instance().effortDec();
		        }

		        if ( mEffort < PlayerParam::instance().HeteroPlayer(mPlayerType).effortMin() ) {
		        	mEffort = PlayerParam::instance().HeteroPlayer(mPlayerType).effortMin();
		        }
		    }

		    if ( mStamina >= ServerParam::instance().effortIncThr() * ServerParam::instance().staminaMax() )  {
		        if ( mEffort < PlayerParam::instance().HeteroPlayer(mPlayerType).effortMax() ) {
		        	mEffort += ServerParam::instance().effortInc();
		            if ( mEffort > PlayerParam::instance().HeteroPlayer(mPlayerType).effortMax() ) {
		            	mEffort = PlayerParam::instance().HeteroPlayer(mPlayerType).effortMax();
		            }
		        }
		    }

			double stamina_inc = Min( PlayerParam::instance().HeteroPlayer(mPlayerType).staminaIncMax(), ServerParam::instance().staminaMax() - mStamina );
			mStamina += stamina_inc;

			if (mStamina > ServerParam::instance().staminaMax()) {
				mStamina = ServerParam::instance().staminaMax();
			}
		}
	};
};

#endif /* SIMULATOR_H_ */
