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
