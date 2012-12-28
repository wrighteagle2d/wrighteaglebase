/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2013                                                    *
 * Copyright (c) 1998-2013 WrightEagle 2D Soccer Simulation Team,                   *
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

#ifndef __Kicker_H__
#define __Kicker_H__

#include "Agent.h"

enum KickMode
{
    KM_Null,
    KM_Hard,
    KM_Quick
};

class Kicker
{
    Kicker();

public:
    ~Kicker();

    static Kicker & instance();

    /**
     * 计算mKickerValue表
     * Compute kicker value table.
     */
    void ComputeUtilityTable();

    /**
     * 通过kick的误差模型计算踢球后的max_rand
     * Calculate maximum random error after kick action.
     */
    double GetMaxRandAfterKick(const Agent & agent, double power);

    /**
     * 根据球的初速度和目标速度，得到kick需要的power，参数都为绝对量或都为相对量
     * Calculate power need for an kick action according to initial and objective velocity.
     */
    double GetOneKickPower(const Vector & ball_vel, const Vector & ball_goal_vel, const double kick_rate);

    /**
     * 下面2个重载函数计算在给定目标点或踢球角度和踢球周期的前提下，使球能达到的最大球速，参数均为绝对量
     * The following 2 functions calculate maximum speed the ball can reach by given conditions.
     */
	double GetMaxSpeed(const Agent & agent, const double & kick_angle, const int & cycle, Vector * kick_out_pos = 0);
    double GetMaxSpeed(const Agent & agent, const Vector & kick_pos, const int & cycle, Vector * kick_out_pos = 0, Vector *last_vel = 0);

    /**
     * 外面用到的接口，里面会计算需要几周期踢球
     * the following 2 functions generate kick action plans.
     */
	bool KickBall(Agent & agent, double angle, double speed_out, KickMode mode = KM_Hard, int *cycle_left = 0, bool is_shoot = false);
    bool KickBall(Agent & agent, const Vector & target, double speed_out, KickMode mode = KM_Hard, int * cycle_left = 0, bool is_shoot = false);

    bool KickBallCloseToBody(Agent & agent , AngleDeg ang, double KickRatio );
    /**
     * 执行函数
     * Execute a kick action plan.
     */
    void Execute(Agent &agent, const ActionPlan &plan);

private:

    /** 在离散点钟寻找最近的一个点 */
    inline int NearestPoint(const Vector & p)
    {
        double ang = 0.0;
	    double value = p.Mod();
	    int idxA;
	    if(value < mD1)
	    {
		    double fix = 180.0 / mNlayer[0];
		    ang = p.Dir() + fix;
		    ang = GetNormalizeAngleDeg(ang, 0.0);
		    idxA = int(ang * mNlayer[0] / 360.0);
		    return idxA;
	    }
	    else if(value > mD2)
	    {
		    double fix = 180.0 / mNlayer[2];
		    ang = p.Dir() + fix;
		    ang = GetNormalizeAngleDeg(ang, 0.0);
		    idxA = int(ang * mNlayer[2] / 360.0);
		    return mNlayer[0] + mNlayer[1] + idxA;
	    }
	    else
	    {
		    double fix = 180.0 / mNlayer[1];
		    ang = p.Dir() + fix;
		    ang = GetNormalizeAngleDeg(ang, 0.0);
		    idxA = int(ang * mNlayer[1] / 360.0);
		    return mNlayer[0] + idxA;
	    }
    }

    /** 读取mKickerValue表 */
    void ReadUtilityTable();

    /** 更新kick的数据，里面有时间控制 */
    void UpdateKickData(const Agent & agent);

    /**
     * 得到一次kick在某方向能达到的最大速度，参数都为绝对量或都为相对量
     * Calculate maximum speed from acceleration. All parameters should be in the same coordinate
     * system. And return value will be in the same system.
     * \param ball_vel velocity of ball.
     * \param kick_angle direction of acceleration.
     * \param max_accel maximum modulus of acceleration.
     * \return maximum speed.
     */
    double GetOneKickMaxSpeed(const Vector & ball_vel, const double & kick_angle, const double & max_accel)
    {
    	SinCosT value = SinCos(ball_vel.Dir() - kick_angle);

    	double y = Sin(value) * ball_vel.Mod();

    	if (fabs(y) < max_accel) {
    		double x = Cos(value) * ball_vel.Mod();
    		return (MinMax(0.0, Sqrt(max_accel * max_accel - y * y) + x, ServerParam::instance().ballSpeedMax()));
    	}
    	else {
    		return 0.0;
    	}
    }

    /** 已知几周期踢球后，该函数被调用 */
    bool KickBall(Agent & agent, const Vector & target, double speed_out, int cycle, bool is_shoot = false);

    /** 下面是得到ActionInfo接口的函数 */
	AtomicAction GetOneKickAction(const Agent & agent, const Vector & ball_pos, const Vector & ball_vel, const Vector & target, bool is_self = true);
	AtomicAction GetKickOutAction(const Agent & agent, Vector & ball_pos, Vector & ball_vel, bool is_self = true);
    AtomicAction GetMaxKickOutAction(const Agent & agent, Vector & ball_pos, Vector & ball_vel, bool is_self = true);
    AtomicAction GetTurnAction(const Agent & agent);

    /** 一周期踢球和多周期踢球，以及转身的规划 */
    ActionPlan OneCycleKick(const Agent & agent, bool is_shoot = false);
    ActionPlan MultiCycleKick(const Agent & agent, int cycle);
    AtomicAction TurnPlan(const Agent & agent, int index, double turn_max_speed);

private:
    enum {
    	STEP_KICK_ANGLE = 10, // Kicker类中搜索角度的步长，共有360 / 10 = 36个角度
    	POINTS_NUM = 81 // Kicker类中搜索的总点数，POINTS_NUM = nlayer[0] + nlayer[1] + nlayer[2]
    };

    AgentID     mAgentID;

    Array<int, 3>  mNlayer;         /** 每层的离散点数 */
    double      mD1;                /** 1,2层半径的均值 */
    double      mD2;                /** 2,3层半径的均值 */
    Array<double, 3> mDlayer;    /** 每层的半径 */
    Array<Vector, POINTS_NUM> mPoint; /** 存储所有点 */

    float mKickerValue[3][36][POINTS_NUM][POINTS_NUM]; /** 2,3,4脚踢球，36个角度，POINTS_NUM个点 */ // float即可，节省所占空间

    ReciprocalCurve mOppCurve;      /** 对手的影响 */
    ReciprocalCurve mRandCurve;     /** 误差的影响 */
    ReciprocalCurve mSpeedCurve;    /** 球速的影响 */

    ActionInput mInput;                 /** 记录kick的状态信息 */
    double      mKickSpeed;             /** kick的期望速度大小 */
    Vector      mKickTarget;            /** kick的目标点，在player坐标系中 */

    Array<double, POINTS_NUM> mKickRate;/** 在每个点球员的kick_rate */
    Array<double, POINTS_NUM> mMaxAccel;/** 在每个点球员能产生的最大加速度eff_power */
    Array<double, POINTS_NUM> mRandEva; /** 在每个点球员kick产生的最大误差 */
    Array<double, POINTS_NUM> mPointEva;/** 在每个点球员kick的eva */

    double      mMaxRandFactor;         /** 记录求kick rand时的一个量 */

    Array<Array<AgentID, 3>, 360 > mMaxSpeedFlag;     /** 记录各个角度的计算标记， */
    Array<Array<double, 3>, 360 > mMaxSpeed;          /** 360个角度，1,2,3脚情况下能达到的最大速度 */
    Array<Array<Vector, 3>, 360 > mMaxSpeedOutPos;    /** 360个角度，1,2,3脚情况下达到最大速度时的出球的位置 */
    Array<Array<Vector, 3>, 360 > mMaxSpeedOutFirstPos;
    Array<Array<Vector, 3>, 360 > mMaxSpeedOutLastVel;/** 最大速度出球时，最后一个周期的速度（kick前） */

public:
    /** 下面提供有关有球agent的接口，直接返回AtomicAction，供外部调用 */

    /**
     * 保证kick完使球速达到最小
     * Get an atomic action to stop ball or minimum ball speed.
     */
    AtomicAction GetStopBallAction(const Agent & agent, bool forceStop = true);

    /**
     * 将球加速到特定的目标速度
     * Accelerate the ball to a certain velocity.
     */
    AtomicAction GetAccelerateBallAction(const Agent & agent, Vector & goal_vel);

    /**
     * 将球踢向特定方向
     * Kick the ball to a certain direction.
     */
    AtomicAction GetKickBallToAngleAction(const Agent & agent, AngleDeg rel_angle, double rel_dist_rate = 0.9);
};


#endif

