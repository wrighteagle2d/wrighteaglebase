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

#ifndef __Tackler_H__
#define __Tackler_H__

#include "Agent.h"

/**
 * Tackler
 */
class Tackler
{
    Tackler();

public:
    ~Tackler();

    static Tackler & instance();

    int ang2idx(const AngleDeg & angle) { return GetNormalizeAngleDeg(angle, 0.0); }
    int dir2idx(const AngleDeg & dir) { return Rint(GetNormalizeAngleDeg(dir, 0.0)); }

    /**
     * Update data used by tackle.
     */
    void UpdateTackleData(const Agent & agent);

    /**
     * Get ball velocity after tackle.
     */
    Vector GetBallVelAfterTackle(const Agent & agent, AngleDeg tackle_angle);

    /**
     * Get ball velocity after tackle to a certain direction.
     */
    bool GetBallVelAfterTackleToDir(const Agent & agent, const AngleDeg & dir, Vector *ball_vel);

    bool GetTackleInfoToDir(const Agent & agent, AngleDeg dir, AngleDeg *tackle_angle, Vector *ball_vel);

    /**
     * If possible to tackle the ball to a certain direction.
     */
    bool CanTackleToDir(const Agent & agent, const AngleDeg & dir);

    /**
     * Get tackle angle to tackle the ball to a certain direction.
     */
    bool GetTackleAngleToDir(const Agent & agent, const AngleDeg & dir, AngleDeg *tackle_angle);

    /**
     * Maximum ball speed after tackle.
     */
    double GetMaxTackleSpeed(const Agent & agent);

    /**
     * Tackle ball.
     */
    bool TackleToDir(Agent & agent, const AngleDeg & dir, bool foul = false);

    /**
     * If possible to stop ball via tackle.
     */
    bool CanTackleStopBall(Agent & agent);

    /**
     * Perform a tackle to stop ball.
     */
    bool TackleStopBall(Agent & agent);

public:
    static bool MayDangerousIfTackle(const PlayerState & tackler, const WorldState & world_state);

private:
    /** 用来节省时间的记录量 */
	AgentID mAgentID;

	Array<double, 361> mTackleAngle; // -180.0 -> 180.0
    Array<Vector, 361> mBallVelAfterTackle; // -180.0 -> 180.0

    double mMaxTackleSpeed;
    bool mCanTackleStopBall;
    AngleDeg mTackleStopBallAngle;

    std::map<int, std::vector<std::pair<int, int> > > mDirMap; //记录铲到某一方向所需铲球角度的上界和下届，后面会根据这个上下界结算出所需铲球角度（局部线性估计）
};


#endif

