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

#ifndef __Dasher_H__
#define __Dasher_H__

#include "Geometry.h"
#include "Agent.h"

struct AtomicAction;
class PlayerState;

class Dasher
{
    Dasher();

public:
    ~Dasher();

    static Dasher & instance();

    static Array<double, 8> DASH_DIR;
    static Array<int, 8> ANTI_DIR_IDX;
    static Array<double, 8> DIR_RATE;

    static int GetDashDirIdx(const AngleDeg & dir) {
    	for (int i = 0; i < 8; ++i) {
    		if (GetAngleDegDiffer(dir, DASH_DIR[i]) < FLOAT_EPS) return i;
    	}
    	Assert(0);
    	return -1;
    }

    /** 以最快的方式跑到目标点
     * Run to the destination point with the fastest method.
     * \param agent the agent itself.
     * \param pos the destination point.
     * \param buffer point buffer, means the real destinantion is a cycle with the certer 
     * 		of pos and radius of buffer.
     * \param power the intend power to use.
     * \param can_inverse true means can run in the inverse direction of the agent's body.
     * \param turn_first true means that the agent should turn first to adjust the direction.
     * \return true if the action excuted successfully.
     */
    bool GoToPoint(Agent & agent, Vector pos, double buffer  = 0.5, double power = 100.0, bool can_inverse = true, bool turn_first = false);

    /** 以最快的方式跑到目标点
     * Run to the destination point with the fastest method.
     * \param agent the agent itself.
     * \param act an atomic action caculated to go to pos for this cycle.
     * \param pos the destination point.
     * \param buffer point buffer, means the real destinantion is a cycle with the certer 
     * 		of pos and radius of buffer.
     * \param power the intend power to use.
     * \param can_inverse true means can run in the inverse direction of the agent's body.
     * \param turn_first true means that the agent should turn first to adjust the direction.
     */
    void GoToPoint(Agent & agent, AtomicAction & act, Vector pos, double buffer  = 0.5, double power = 100.0, bool can_inverse = true, bool turn_first = false);

    /** 以确定的姿势（指倒着跑和正跑），跑到目标点
     * Run to the destination point with a certain posture, forward or backword.
     * \param agent the agent itself.
     * \param act an atomic action caculated to go to pos for this cycle.
     * \param pos the destination point.
     * \param buffer point buffer, means the real destinantion is a cycle with the center
     * 		of pos and radius of buffer.
     * \param power the intend power to use.
     * \param inverse true means run backward.
     * \param turn_first true means that the agent should turn first to adjust the direction.
     */
    void GoToPointWithCertainPosture(Agent & agent, AtomicAction & act, Vector pos, double buffer  = 0.5, double power = 100.0, const bool inverse = false, bool turn_first = false);

    /**
     * player 跑到 target 的所需的最小周期数
     * This function returns the minimum cycles for a player to go to a target position.
     * @param player the player to caculate.
     * @param target the target position to go to.
     * @param can_inverse true means consider running backwards.
     * @param buffer
     * @return an integer to show the minimum cycles caculated.
     */
    int CycleNeedToPoint(const PlayerState & player, Vector target, bool can_inverse = true, double *buf = 0);

     /**
     * player 跑到 target 的所需的最小周期数, 返回实数周期
     * This function returns the minimum cycles for a player to go to a target position.
     * @param player the player to caculate.
     * @param target the target position to go to.
     * @param can_inverse true means consider running backwards.
     * @return a double to show the minimum cycles caculated.
     */
    double RealCycleNeedToPoint(const PlayerState & player, Vector target, bool can_inverse = true)
    {
    	double buf;
    	int cyc = CycleNeedToPoint(player, target, can_inverse, & buf);
    	return cyc - buf;
    }

    /**
     * player 以确定得姿势（指倒着跑和正跑），跑到 target 所需要的周期数
     * This function returns the minimum cycles for a player to go to a target position with 
     * a certain posture, forward or backward.
     * @param player the player to caculate.
     * @param target the target position to go to.
     * @param inverse true means running backwards.
     * @param buffer 
     * @return an integer to show the minimum cycles caculated.
     */
    int CycleNeedToPointWithCertainPosture(const PlayerState & player, Vector target, const bool inverse, double *buf = 0);

    /**
     * player 以确定得姿势（指倒着跑和正跑），跑到 target 所需要的周期数, 返回实数周期
     * This function returns the minimum cycles for a player to go to a target position with 
     * a certain posture, forward or backward.
     * @param player the player to caculate.
     * @param target the target position to go to.
     * @param inverse true means running backwards.
     * @return an integer to show the minimum cycles caculated.
     */
    double RealCycleNeedToPointWithCertainPosture(const PlayerState & player, Vector target, const bool inverse)
    {
    	double buf;
    	int cyc = CycleNeedToPointWithCertainPosture(player, target, inverse, & buf);
    	return cyc - buf;
    }

    /**
     * 计算用同一dash_power到达某点需要的时间
     * Caculate cycles needed to a target position with a certain dash power.
     * @param player the player to caculate.
     * @param target the target position to go to.
     * @param dash_power the power for dash to predict.
     * @return an integer to show the cycles caculated.
     */
    int CyclePredictedToPoint(const PlayerState& player , Vector target , double dash_power);


    /**
     * 主要是为了调整power用，为了避免power过大而跑过可
     * 目标点。原则视为了更高层协调，power只可能减小不可
     * 能增大。为了不必要到dash，power也可可能为零。也就
     * dash不执行
     * power 的正负由外面给定
     * This funcition is mainly used to adjust ( usually decrease ) the dash power 
     * to avoid run over the target position by using a big power. The final power 
     * could be 0 to avoid an unnecessary dash.
     * The sign of power is given outside the function.
     * @param player the player to caculate.
     * @param target the target position to go to.
     * @param buffer
     * @param power orignal power given.
     * @return power adjusted.
     */
    double AdjustPowerForDash(const PlayerState & player, Vector target, double buffer, double power);

    /**
     * 考虑转身或直接dash
     * Planing to turn or to dash.
     * @param agent the agent itself.
     * @param act an atomic action to return the caculated decision.
     * @param target the target position to go to.
     * @param buffer
     * @param power the intend power for dash.
     * @param inverse true means running backwards.
     * @param turn_first true means turn first manually.
     */
    void TurnDashPlaning(Agent & agent, AtomicAction & act, Vector target, double buffer, double power, bool inverse, bool turn_first = false);

    /**
     * Caculate the get ball cycles and actions.
     * @param agent the agent itself.
     * @param act the atomic action to execute this cycle to get the ball.
     * @param int_cycle the beginning cycle to do the action, while -1 means now.
     * @param can_inverse true means allow runnning backwards to get the ball.
     * @param turn_first true means the agent turn first to get the ball.
     * @return a double to show the cycles needed and if it less than 0, that is to say impossible.
     */
    double GetBall(Agent & agent, AtomicAction & act, int int_cycle = -1, bool can_inverse = true, bool turn_first = false);

    /**
     * Caculate the get ball action and execute it.
     * @param agent the agent itself.
     * @param int_cycle the beginning cycle to do the action, while -1 means now.
     * @param can_inverse true means allow runnning backwards to get the ball.
     * @param turn_first true means the agent turn first to get the ball.
     * @return true means the action executed successfully.
     */
    bool GetBall(Agent & agent, int int_cycle = -1, bool can_inverse = true, bool turn_first = false);

    /*
     * This function is used to correct the target position when near the ball.
     * @param player the player to consider.
     * @param target the target position to go to.
     * @param fix the distance error to fix.
     * @return position corrected.
     */
    Vector CorrectTurnForDash(const PlayerState & player, const Vector & target, double fix = 0.0);

// 下面提供有关无球agent基本行为的接口，直接返回AtomicAction，供外部调用
// The following funtions are some interfaces for agent to do some basic behaviors 
// without ball. The return value are all AtomicAction. 
public:
    /**
     * 将身体转向特定方向
     * Turn body to a certain direction.
     * @param agent the agent itself.
     * @param ang the angle to turn to.
     * @return an atomic action to turn the body.
     */
    AtomicAction GetTurnBodyToAngleAction(const Agent & agent, AngleDeg ang);

    /*! This method determines the optimal dash power to mantain an optimal speed
	When the current speed is too high and the distance is very small, a
	negative dash is performed. Otherwise the difference with the maximal speed
	is determined and the dash power rate is set to compensate for this
	difference.
	\param posRelTo relative point to which we want to dash
	\param angBody body angle of the agent
	\param vel current velocity of the agent
	\param dEffort current effort of the player
	\param iCycles desired number of cycles to reach this point
	\return dash power that should be sent with dash command */
    bool GetPowerForForwardDash(const Agent &agent, double* dash_power, Vector posRelTo, double angBody, double dEffort, int iCycles );

public:
	static double GETBALL_BUFFER; //拿球里面使用的判断是否可踢的buf，比worldstate里的大
};

#endif
