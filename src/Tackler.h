/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D) * BASE SOURCE CODE RELEASE 2016 *
 * Copyright (c) 1998-2016 WrightEagle 2D Soccer Simulation Team, * Multi-Agent
 *Systems Lab.,                                * School of Computer Science and
 *Technology,               * University of Science and Technology of China *
 * All rights reserved. *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without *
 * modification, are permitted provided that the following conditions are met: *
 *     * Redistributions of source code must retain the above copyright *
 *       notice, this list of conditions and the following disclaimer. *
 *     * Redistributions in binary form must reproduce the above copyright *
 *       notice, this list of conditions and the following disclaimer in the *
 *       documentation and/or other materials provided with the distribution. *
 *     * Neither the name of the WrightEagle 2D Soccer Simulation Team nor the *
 *       names of its contributors may be used to endorse or promote products *
 *       derived from this software without specific prior written permission. *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED    * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *PURPOSE ARE           * DISCLAIMED. IN NO EVENT SHALL WrightEagle 2D Soccer
 *Simulation Team BE LIABLE    * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *EXEMPLARY, OR CONSEQUENTIAL       * DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *PROCUREMENT OF SUBSTITUTE GOODS OR       * SERVICES; LOSS OF USE, DATA, OR
 *PROFITS; OR BUSINESS INTERRUPTION) HOWEVER       * CAUSED AND ON ANY THEORY OF
 *LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,    * OR TORT (INCLUDING
 *NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF * THIS SOFTWARE,
 *EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                *
 ************************************************************************************/

#ifndef __Tackler_H__
#define __Tackler_H__

#include "Agent.h"

/**
 * Tackler
 */
class Tackler {
  Tackler();

public:
  ~Tackler();

  static Tackler &instance();

  int ang2idx(const AngleDeg &angle) {
    return GetNormalizeAngleDeg(angle, 0.0);
  }
  int dir2idx(const AngleDeg &dir) {
    return Rint(GetNormalizeAngleDeg(dir, 0.0));
  }

  /**
   * Update data used by tackle.
   */
  void UpdateTackleData(const Agent &agent);

  /**
   * Get ball velocity after tackle.
   */
  Vector GetBallVelAfterTackle(const Agent &agent, AngleDeg tackle_angle);

  /**
   * Get ball velocity after tackle to a certain direction.
   */
  bool GetBallVelAfterTackleToDir(const Agent &agent, const AngleDeg &dir,
                                  Vector *ball_vel);

  bool GetTackleInfoToDir(const Agent &agent, AngleDeg dir,
                          AngleDeg *tackle_angle, Vector *ball_vel);

  /**
   * If possible to tackle the ball to a certain direction.
   */
  bool CanTackleToDir(const Agent &agent, const AngleDeg &dir);

  /**
   * Get tackle angle to tackle the ball to a certain direction.
   */
  bool GetTackleAngleToDir(const Agent &agent, const AngleDeg &dir,
                           AngleDeg *tackle_angle);

  /**
   * Maximum ball speed after tackle.
   */
  double GetMaxTackleSpeed(const Agent &agent);

  /**
   * Tackle ball.
   */
  bool TackleToDir(Agent &agent, const AngleDeg &dir, bool foul = false);

  /**
   * If possible to stop ball via tackle.
   */
  bool CanTackleStopBall(Agent &agent);

  /**
   * Perform a tackle to stop ball.
   */
  bool TackleStopBall(Agent &agent);

public:
  static bool MayDangerousIfTackle(const PlayerState &tackler,
                                   const WorldState &world_state);

private:
  /** 用来节省时间的记录量 */
  AgentID mAgentID;

  Array<double, 361> mTackleAngle;        // -180.0 -> 180.0
  Array<Vector, 361> mBallVelAfterTackle; // -180.0 -> 180.0

  double mMaxTackleSpeed;
  bool mCanTackleStopBall;
  AngleDeg mTackleStopBallAngle;

  std::map<int, std::vector<std::pair<int, int>>>
      mDirMap; //记录铲到某一方向所需铲球角度的上界和下届，后面会根据这个上下界结算出所需铲球角度（局部线性估计）
};

#endif
