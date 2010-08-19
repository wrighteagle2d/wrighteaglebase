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

