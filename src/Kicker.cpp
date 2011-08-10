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

#include <fstream>
#include <cstdlib>
#include "Kicker.h"
#include "Logger.h"
#include "Parser.h"
#include "Utilities.h"

using namespace std;

/**
 * Constructor
 */
Kicker::Kicker()
{
	/** POINTS_NUM = 81 = 18 + 27 + 36 */
	mNlayer[0]  = 18;
	mNlayer[1]  = 27;
	mNlayer[2]  = 36;

	mD1         = 0.585; /** (mDlayer[0] + mDlayer[1]) / 2.0 */
	mD2         = 0.835; /** (mDlayer[1] + mDlayer[2]) / 2.0 */
	mDlayer[0]  = 0.46;
	mDlayer[1]  = 0.71;
	mDlayer[2]  = 0.96; /** 球员的kick_area是0.985到1.185之间 */

	/** 计算所有的点 */
	int k           = 0;
	AngleDeg angle  = 0.0;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < mNlayer[i]; ++j)
		{
			angle       = 360.0 * j / mNlayer[i];
			mPoint[k]   = Polar2Vector(mDlayer[i], angle);
			++k;
		}
	}

	mOppCurve.SetOutMinMax(0.0, 1.0);
	mOppCurve.Interpolate(0.0, 0.0, 1.0, 0.8, 2.0, 1.0);    /** 输入为近身对手离球的距离减去球员的kick_area */
	mRandCurve.SetOutMinMax(0.6, 1.0);
	mRandCurve.Interpolate(0.0, 1.0, 1.0, 0.9, 2.0, 0.6);   /** 输入为max_rand中属于球位置因素的部分 */
	mSpeedCurve.SetOutMinMax(0.0, 1.0);
	mSpeedCurve.Interpolate(0.0, 0.0, 1.6, 0.8, ServerParam::instance().ballSpeedMax(), 1.0);

	//==========================================================================

	mInput.mPlayerType  = -1;
	mKickSpeed          = 0.0;
	mKickTarget         = Vector(0.0, 0.0);

	for (int i = 0; i < POINTS_NUM; ++i)
	{
		mKickRate[i]    = 0.0;
        mMaxAccel[i]    = 0.0;
		mRandEva[i]     = 0.0;
		mPointEva[i]    = 0.0;
	}

	mMaxRandFactor  = 0.0;

	for (int i = 0; i < 360; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			mMaxSpeed[i][j]     = 0.0;
		}
	}


	/** 将要读取或计算mKickerValue表 */
	memset(mKickerValue, 0, sizeof(mKickerValue));

    if (PlayerParam::instance().KickerMode() == 0)
    {
        ReadUtilityTable();
    }
}


/**
 * Destructor.
 */
Kicker::~Kicker()
{
}


/**
 * Instrance.
 */
Kicker & Kicker::instance()
{
	static Kicker kicker;
	return kicker;
}


/**
 * Calculate maximum random error after kick action.
 * \param agent performer of the kick action.
 * \param power power used to kick.
 * \return maximum random error after kick action.
 */
double Kicker::GetMaxRandAfterKick(const Agent & agent, double power)
{
	UpdateKickData(agent);
	return (mMaxRandFactor * (power / ServerParam::instance().maxPower()));
}


/**
 * Calculate power need of an kick action according to initial and objective velocity. All parameters
 * should be in exactly the same coordinate system.
 * \param ball_vel initial velocity.
 * \param ball_goal_vel objective velocity.
 * \param kick_rate kick_rate.
 * \return power need for the kick action.
 */
double Kicker::GetOneKickPower(const Vector & ball_vel, const Vector & ball_goal_vel, const double kick_rate)
{
	double power = (ball_goal_vel - ball_vel).Mod() / kick_rate;//需要的power
	return (GetNormalizeKickPower(power));
}

/**
 * Calculate the maximum speed the ball can be kicked to in given cycles.
 * \param agent which player it is calculated for.
 * \param kick_angle direction the target lies at (in field's coordinate system).
 * \param cycle
 * \param kick_out_pos if valid, will be set to the position the ball was kicked out from.
 * \return maximum speed.
 */
double Kicker::GetMaxSpeed(const Agent & agent, const double & kick_angle, const int & cycle, Vector * kick_out_pos)
{
	Vector target = agent.GetWorldState().GetBall().GetPos() + Polar2Vector(1000.0, kick_angle);

	return GetMaxSpeed(agent, target, cycle, kick_out_pos);
}

/**
 * Calculate the maximum speed the ball can be kicked to in given cycles.
 * \param agent which player it is calculated for.
 * \param kick_pos target (in field's coordinate system).
 * \param cycle
 * \param kick_out_pos if valid, will be set to the position the ball was kicked out from.
 * \return maximum speed.
 */
double Kicker::GetMaxSpeed(const Agent & agent, const Vector & kick_pos, const int & cycle, Vector * kick_out_pos, Vector *last_vel)
{
	if (cycle < 1) {
		Assert(0);
		return 0.0;
	}

	UpdateKickData(agent);

	// absolute position of agent before the ball was kicked out
	Vector player_pos_final = mInput.mPlayerPos + mInput.mPlayerVel.Rotate(mInput.mPlayerBodyDir) * ((1 - pow(PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).playerDecay(), cycle-1)) / (1 - PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).playerDecay()));

	if (cycle > 3) /** 4脚肯定能踢到最快球速了 */
	{
		// 不出意外，4脚踢球最后出球位置应该是mPoint[0]。 From Shi Ke.
		if (kick_out_pos) *kick_out_pos = mPoint[0].Rotate(mInput.mPlayerBodyDir) + player_pos_final;
		if (last_vel) *last_vel = Vector(0.0, 0.0); //认为球停在身前，速度为0
		return ServerParam::instance().ballSpeedMax();
	}

	Vector target = (kick_pos-player_pos_final).Rotate(-mInput.mPlayerBodyDir);

	int idx = (int)GetNormalizeAngleDeg((target-mInput.mBallPos).Dir(), -FLOAT_EPS); /** 得到0到359的一个整数 */

	if (mMaxSpeedFlag[idx][cycle - 1] != agent.GetAgentID())
	{
		double max_speed = 0.0;
		Vector max_speed_pos = mInput.mBallPos;
		Vector max_speed_first_pos = mInput.mBallPos;
		Vector max_speed_last_vel = mInput.mBallVel;

		if (cycle == 1)
		{
			max_speed = GetOneKickMaxSpeed(mInput.mBallVel, (target-mInput.mBallPos).Dir(), agent.GetSelf().GetKickRate() * ServerParam::instance().maxPower());
		}
		else
		{
			Vector ball_vel = Vector(0.0, 0.0);

			for (int k = 0; k < POINTS_NUM; ++k)
			{
				if (mPointEva[k] > 0.001)
				{
					ball_vel = (mPoint[k]+mInput.mPlayerVel - mInput.mBallPos) * ServerParam::instance().ballDecay();//踢到k后的球速
					if (cycle == 2) /** 2周期踢球，重新算一下 */
					{
						double speed = GetOneKickMaxSpeed(ball_vel, (target-mPoint[k]).Dir(), mMaxAccel[k]);
						if (max_speed < speed)
						{
							max_speed = speed;
							max_speed_pos = mPoint[k];
							max_speed_first_pos = mPoint[k];
							max_speed_last_vel = ball_vel;
						}
					}
					else // 前向搜索2步
					{
						Vector ball_next = mPoint[k] + ball_vel;
						for (int l = 0; l < POINTS_NUM; ++l) // 从j踢到k，再踢到l
						{
							if (mPoint[l].Dist(ball_next) < mMaxAccel[k]) // 保证理论上可以从k踢到l
							{
								Vector ball_vel_next = (mPoint[l]+mInput.mPlayerVel*PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).playerDecay() - mPoint[k]) * ServerParam::instance().ballDecay();
								double speed = GetOneKickMaxSpeed(ball_vel_next, (target-mPoint[l]).Dir(), mMaxAccel[l]);
								if (max_speed < speed)
								{
									max_speed = speed;
									max_speed_pos = mPoint[l];
									max_speed_first_pos = mPoint[k];
									max_speed_last_vel = ball_vel_next;
								}
							}
						}
					}
				}
			}
		}

		mMaxSpeedFlag[idx][cycle - 1]   = agent.GetAgentID();
		mMaxSpeed[idx][cycle - 1]       = max_speed;
		mMaxSpeedOutPos[idx][cycle -1]  = max_speed_pos.Rotate(mInput.mPlayerBodyDir) + player_pos_final;
		mMaxSpeedOutFirstPos[idx][cycle-1] = max_speed_first_pos;
		mMaxSpeedOutLastVel[idx][cycle-1] = max_speed_last_vel.Rotate(mInput.mPlayerBodyDir);
	}

	if (kick_out_pos) *kick_out_pos = mMaxSpeedOutPos[idx][cycle -1];
	if (last_vel) *last_vel = mMaxSpeedOutLastVel[idx][cycle - 1];

	return Min(mMaxSpeed[idx][cycle - 1], ServerParam::instance().ballSpeedMax());
}


/**
 * Kick ball to target at "speed_out" speed in "cycle" cycles.
 * \param agent which player is kicking the ball.
 * \param target (in field's coordinate system).
 * \param speed_out the speed the ball will be kicked out at.
 * \param cycle cycles this kick procedure cost in total.
 * \param is_shoot if this kick action is performed for shoot, default is false.
 * \return true if the whole procedure can be executed favorably or any other remedial measures
 *         can be performed.
 */
bool Kicker::KickBall(Agent & agent, const Vector & target, double speed_out, int cycle, bool is_shoot)
{
	UpdateKickData(agent);

	mKickSpeed  = speed_out;
	mKickTarget = (target-mInput.mPlayerPos).Rotate(-mInput.mPlayerBodyDir);

	ActionPlan p;
	switch (cycle)
	{
	case 1:
		p = OneCycleKick(agent, is_shoot);
		break;
	case 2:
	case 3:
	case 4:
		p = MultiCycleKick(agent, cycle);
		break;
	default:
		p.mSucceed = false;
		break;
	}

	if (p.mSucceed == false) // 后备处理
	{
		p.mActionQueue.clear();
		AtomicAction a;
		a.mSucceed = false;
		if((mInput.mBallPos + mInput.mBallVel).Dist(mInput.mPlayerPos + mInput.mPlayerVel) <
				PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).kickableArea() - 0.05)
		{
			//a.mTurnAngle = mKickTarget.Dir();
			a.mTurnAngle = GetNormalizeAngleDeg(mKickTarget.Dir());
			if(fabs(a.mTurnAngle) > 10)
			{
				a.mType = CT_Turn;
				a.mSucceed = true;
			}
			else//try kick to body
			{
				mKickTarget = Vector(0, 0);
				mKickSpeed = mInput.mBallPos.Mod();
				a = GetKickOutAction(agent, mInput.mBallPos, mInput.mBallVel);
			}
		}
		if(a.mSucceed == false) // 实在不行先停球
		{
			a = GetStopBallAction(agent);
		}
		p.mSucceed = a.mSucceed;
		p.mActionQueue.push_back(a);
	}

	if (p.mActionQueue.size() > 0)
	{
		Execute(agent, p); // 执行动作
	}
	return p.mSucceed;
}


/**
 * Kick ball to a certain direction.
 * \param agent who is kicking the ball.
 * \param angle direction (in field's coordinate system) the ball will be kicked to.
 * \param speed_out the speed the ball will be kicked out at.
 * \param mode
 * \param cycle_left will be set to cycles left after first action.
 * \param is_shoot if this kick action is performed for shoot, default is false.
 * \return true if the whole procedure can be executed favorably or any other remedial measures
 *         can be performed.
 */
bool Kicker::KickBall(Agent & agent, double angle, double speed_out, KickMode mode, int *cycle_left, bool is_shoot)
{
	Vector target = agent.GetWorldState().GetBall().GetPos() + Polar2Vector(1000.0, angle);
	return KickBall(agent, target, speed_out, mode, cycle_left, is_shoot);
}


/**
 * Kick ball to target.
 * \param agent who is kicking the ball.
 * \param target (in field's coordinate system).
 * \param speed_out the speed the ball will be kicked out at.
 * \param mode
 * \param cycle_left will be set to cycles left after first action.
 * \param is_shoot if this kick action is performed for shoot, default is false.
 * \return true if the whole procedure can be executed favorably or any other remedial measures
 *         can be performed.
 */
bool Kicker::KickBall(Agent & agent, const Vector & target, double speed_out, KickMode mode, int * cycle_left, bool is_shoot)
{
	UpdateKickData(agent);

	double speed_buf = 0.0;
	switch (mode)
	{
	case KM_Hard:
		speed_buf = 0.04;
		break;
	case KM_Quick:
		speed_buf = 0.08; // 要快速踢出去，速度小一点也没关系
		break;
	default:
		speed_buf = 0.06;
		break;
	}

	double speed[] = {0.0, 0.0, 0.0, 0.0, ServerParam::instance().ballSpeedMax()};
	int kick_cycle  = 1;
	double kick_speed = speed_out;

	for (int i = 1; i <= 4; ++i)
	{
		speed[i] = GetMaxSpeed(agent, target, i);

		if (speed[i] < 0.01) // 能产生的速度太小，不考虑
		{
			continue;
		}
		if (speed[i] + speed_buf > speed_out)
		{
			kick_cycle = i;
			kick_speed = Min(speed_out, speed[i]);
			break;
		}
	}

	if(cycle_left != 0)
	{
		*cycle_left = kick_cycle - 1;
	}

	bool rt = KickBall(agent, target, kick_speed, kick_cycle, is_shoot); // 花kick_cycle周期踢球，目标点为target，出球速度为SpeedOut
	return rt;
}


/**
 * Execute a kick action plan.
 */
void Kicker::Execute(Agent &agent, const ActionPlan &plan)
{
	if (plan.mActionQueue.size() > 0)
	{
		plan.mActionQueue.front().Execute(agent);
	}
}

/**
 * 读取mKickerValue表
 */
void Kicker::ReadUtilityTable()
{
	std::ifstream in_file("data/kicker_value", std::ios::binary);
	if (!in_file)
	{
		PRINT_ERROR("open file error");
		return;
	}
	in_file.read((char *)mKickerValue, sizeof(mKickerValue));
	in_file.close();
}


/**
 * Compute kicker value table.
 */
void Kicker::ComputeUtilityTable()
{
	Assert(Parser::IsPlayerTypesReady());

    mInput.mPlayerType = 0;//离线计算只考虑0号类型
    for (int k = 0; k < POINTS_NUM; ++k)
    {
        mKickRate[k] = GetKickRate(mPoint[k], mInput.mPlayerType);
        mMaxAccel[k] = ServerParam::instance().maxPower() * mKickRate[k];
    }

	AngleDeg kick_angle = 0.0;//踢球角度
	Vector ball_vel     = Vector(0.0, 0.0);
	Vector ball_next    = Vector(0.0, 0.0);
	double max_speed    = 0.0;

	for (int v = 0; v < 3; ++v)//3次迭代，分别算v[0], v[1], v[2]
	{
		for (int i = 0; i < 36; ++i)//踢球角度
		{
			kick_angle = STEP_KICK_ANGLE * i;

			for (int j = 0; j < POINTS_NUM; ++j)//初始位置，即第1脚kick前的位置
			{
				for (int k = 0; k < POINTS_NUM; ++k)//第1脚kick后的位置
				{
					ball_vel = (mPoint[k] - mPoint[j]) * ServerParam::instance().ballDecay();

					if (v == 0)//v[0]直接算即可
					{
						mKickerValue[v][i][j][k] = (float)GetOneKickMaxSpeed(ball_vel, kick_angle, mMaxAccel[k]);
					}
					else//v[1]和v[2]要迭代
					{
						max_speed   = 0.0;
						ball_next   = mPoint[k] + ball_vel;

						for (int l = 0; l < POINTS_NUM; ++l)//从j踢到k，再踢到l
						{
							if (mPoint[l].Dist(ball_next) < mMaxAccel[k])//保证理论上可以从k踢到l
							{
								max_speed = Max(max_speed, (double)mKickerValue[v - 1][i][k][l]);
							}
						}

						mKickerValue[v][i][j][k] = (float)max_speed;
					}
				}
			}
		}
	}

	//写文件
	std::ofstream out_file("kicker_value", std::ios::binary);
	if (!out_file)
	{
		PRINT_ERROR("open file error");
		return;
	}
	out_file.write((char *)mKickerValue, sizeof(mKickerValue));
	out_file.close();

    std::cerr << "compute kicker value over ..." << std::endl;
	exit(0);
}


/**
 * Update data needed by Kicker.
 */
void Kicker::UpdateKickData(const Agent & agent)
{
	if (mAgentID == agent.GetAgentID()) {
		return; /** no need to update */
	}

	mAgentID = agent.GetAgentID();

	const BallState &ball_state     = agent.GetWorldState().GetBall();
	const PlayerState &player_state = agent.GetSelf();

	/** absolute values */
	mInput.mBallPos         = ball_state.GetPos();
	mInput.mBallVel         = ball_state.GetVel();
	mInput.mPlayerPos       = player_state.GetPos();
	mInput.mPlayerVel       = player_state.GetVel();
	mInput.mPlayerBodyDir   = player_state.GetBodyDir();

	/** relative values */
	mInput.mBallPos     = (mInput.mBallPos - mInput.mPlayerPos).Rotate(-mInput.mPlayerBodyDir);
	mInput.mBallVel     = mInput.mBallVel.Rotate(-mInput.mPlayerBodyDir);
	mInput.mPlayerVel   = mInput.mPlayerVel.Rotate(-mInput.mPlayerBodyDir);

	/** player type is changed */
	if (mInput.mPlayerType != player_state.GetPlayerType())
	{
		mInput.mPlayerType = player_state.GetPlayerType();

		for(int k = 0; k < POINTS_NUM; ++k)
		{
            mKickRate[k] = GetKickRate(mPoint[k], mInput.mPlayerType);
            mMaxAccel[k] = ServerParam::instance().maxPower() * mKickRate[k];//计算出对应该点最大的eff_power
		}

		for (int i = 0; i < POINTS_NUM; ++i)//考虑kick_rand
		{
			AngleDeg angle_diff     = mPoint[i].Dir();
			angle_diff              = GetNormalizeAngleDeg(angle_diff);
			double dir_diff         = Deg2Rad(fabs(angle_diff));
			double dist_2_ball      = mPoint[i].Mod() -
			    PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).playerSize() - ServerParam::instance().ballSize();
			double kick_pos_rand    = dir_diff / M_PI +
			    dist_2_ball / PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).kickableMargin();
			mRandEva[i] = mRandCurve.GetOutput(kick_pos_rand);
		}
	}

	/** initial */
	for (int i = 0; i < POINTS_NUM; i++)
	{
		mPointEva[i] = mRandEva[i];
	}

	/** 排除掉一定不能踢的点 */
	Vector ball_next_pos    = mInput.mBallPos + mInput.mBallVel - mInput.mPlayerVel;
    double eff_power        = ServerParam::instance().maxPower() * agent.GetSelf().GetKickRate(); //球在当前位置下的实际可以踢到的最大加速度
	double outside_buf      = 0.3;
	Vector temp_pos         = Vector(0.0, 0.0);

	double x_max    = ServerParam::instance().PITCH_LENGTH * 0.5 - mInput.mPlayerPos.X() - outside_buf;
	double x_min    = -ServerParam::instance().PITCH_LENGTH * 0.5 - mInput.mPlayerPos.X() + outside_buf;
	double y_max    = ServerParam::instance().PITCH_WIDTH * 0.5 - mInput.mPlayerPos.Y() - outside_buf;
	double y_min    = -ServerParam::instance().PITCH_WIDTH * 0.5 - mInput.mPlayerPos.Y() + outside_buf;

	for (int i = 0; i < POINTS_NUM; i++)
	{
		temp_pos = (mInput.mPlayerVel + mPoint[i]).Rotate(mInput.mPlayerBodyDir); /** 把x正方向变为和球场一样 */
		if (ball_next_pos.Dist(mPoint[i]) > eff_power /** 踢不到该点 */
				|| temp_pos.X() > x_max || temp_pos.X() < x_min || temp_pos.Y() > y_max || temp_pos.Y() < y_min) /** 会出界 */
		{
			mPointEva[i] = 0.0;
			continue;
		}
	}

	/** 考虑近身对手，不用InfoState的东西，接口更简单 */
	int min_unum    = 0;
	double min_dist = 100.0;
	for (int i = 1; i <= TEAMSIZE; ++i)
	{
		double opp_kick_area;
		if(i == agent.GetWorldState().GetOpponentGoalieUnum())
		{
			opp_kick_area = ServerParam::instance().maxCatchableArea();
		}
		else
		{
            opp_kick_area = agent.GetWorldState().GetOpponent(i).GetKickableArea();
		}
		double dist = agent.GetWorldState().GetOpponent(i).GetPos().Dist(ball_state.GetPos()) - opp_kick_area;

		if (dist < min_dist)
		{
			min_dist    = dist;
			min_unum    = i;
		}
	}

	if (min_unum != 0)
	{
		double opp_kick_area    = 0.0;
		Vector opp_next_pos     = agent.GetWorldState().GetOpponent(min_unum).GetPos();
		opp_next_pos            = (opp_next_pos - (mInput.mPlayerPos + mInput.mPlayerVel)).Rotate(-mInput.mPlayerBodyDir);
		if(min_unum == agent.GetWorldState().GetOpponentGoalieUnum())
		{
			opp_kick_area = ServerParam::instance().maxCatchableArea();
		}
		else
		{
            opp_kick_area = agent.GetWorldState().GetOpponent(min_unum).GetKickableArea();
		}

		if(opp_next_pos.Mod() < opp_kick_area + 2.0)
		{
			double dist = 0.0;
			for (int i = 0; i < POINTS_NUM; i++)
			{
				dist = opp_next_pos.Dist(mPoint[i]) - opp_kick_area;
				mPointEva[i] *= mOppCurve.GetOutput(dist);
			}
		}
	}

	/** max kick rand factor */
	mMaxRandFactor  = GetMaxKickRand(mInput.mBallPos, ball_state.GetVel(), player_state.GetPlayerType(), ServerParam::instance().maxPower());
}


/**
 * Get an atomic kick which moves the ball from initial position to objective position. Positions and
 * velocity should all relative to exactly one coordinate system.
 * \param agent which player this action is calculated for.
 * \param ball_pos initial position of the ball.
 * \param ball_vel velocity of the ball.
 * \param target objective position of the ball.
 * \param is_self if ball_pos represents agent.GetWorldState().GetBall().GetPos(),
 *                agent.GetSelf().GetKickRate() would be the real kick_rate to the ball,
 *                and this parameter should be set to true, otherwise set it to false, default is true.
 * \return an AtomicAction.
 */
AtomicAction Kicker::GetOneKickAction(const Agent & agent, const Vector & ball_pos, const Vector & ball_vel, const Vector & target, bool is_self)
{
	UpdateKickData(agent);

	AtomicAction a;
	a.mType     = CT_Kick;
	a.mKickVel  = (target - ball_pos) - ball_vel;
    a.mKickVel  = a.mKickVel / (is_self ? agent.GetSelf().GetKickRate() : GetKickRate(ball_pos, mInput.mPlayerType));
	a.mSucceed  = (a.mKickVel.Mod() < ServerParam::instance().maxPower());
	return a;
}


/**
 * Kick the ball to mKickTarget at mKickSpeed.
 * \param agent which player this action is calculated for.
 * \param ball_pos position of the ball (in agent's coordinate system).
 * \param ball_vel velocity of the ball (in agent's coordinate system).
 * \param is_self if ball_pos represents agent.GetWorldState().GetBall().GetPos(),
 *                agent.GetSelf().GetKickRate() would be the real kick_rate to the ball,
 *                and this parameter should be set to true, otherwise set it to false, default is true.
 * \return an AtomicAction.
 */
AtomicAction Kicker::GetKickOutAction(const Agent & agent, Vector & ball_pos, Vector & ball_vel, bool is_self)
{
	UpdateKickData(agent);

	AtomicAction a;
	a.mType       = CT_Kick;
	a.mKickVel    = (mKickTarget-ball_pos).SetLength(mKickSpeed) - ball_vel; /** 希望kicker产生的球速 */
    a.mKickVel    = a.mKickVel / (is_self ? agent.GetSelf().GetKickRate() : GetKickRate(ball_pos, mInput.mPlayerType));
	a.mSucceed    = (a.mKickVel.Mod() < ServerParam::instance().maxPower());
	return a;
}


/**
 * Kick the ball to mKickTarget at maximum speed (by 1 kick action).
 * \param agent which player this action is calculated for.
 * \param ball_pos position of the ball in (agent's coordinate system).
 * \param ball_vel velocity of the ball in (agent's coordinate system).
 * \param is_self if ball_pos represents agent.GetWorldState().GetBall().GetPos(),
 *                agent.GetSelf().GetKickRate() would be the real kick_rate to the ball,
 *                and this parameter should be set to true, otherwise set it to false, default is true.
 * \return an AtomicAction.
 */
AtomicAction Kicker::GetMaxKickOutAction(const Agent & agent, Vector & ball_pos, Vector & ball_vel, bool is_self)
{
	UpdateKickData(agent);

	AngleDeg kick_angle = (mKickTarget-ball_pos).Dir();
	AtomicAction a;
	a.mType             = CT_Kick;
	double y            = (Sin(ball_vel.Dir() - kick_angle)) * ball_vel.Mod();
    double kick_rate    = (is_self ? agent.GetSelf().GetKickRate() : GetKickRate(ball_pos, mInput.mPlayerType));
	double max_speed    = ServerParam::instance().maxPower() * kick_rate;
	a.mSucceed          = (fabs(y) < max_speed);
	a.mKickVel          = Polar2Vector(max_speed, ASin(-y/max_speed) + kick_angle);
    a.mKickVel          = a.mKickVel / kick_rate;
	return a;
}

bool Kicker::KickBallCloseToBody(Agent & agent , AngleDeg ang, double KickRatio )
{
	BallState 	bs = agent.GetWorldState().GetBall();
	PlayerState ms = agent.GetSelf();
	AngleDeg    angBody    = ms.GetBodyDir();
	Vector	  	posAgent   = ms.GetPredictedPos(1);
	double      dist      = ms.GetKickableMargin() * KickRatio + ServerParam::instance().ballSize() + ms.GetPlayerSize();
	AngleDeg    angGlobal  = GetNormalizeAngleDeg( angBody + ang );
	Vector	  	posDesBall = posAgent   + Polar2Vector(dist, angGlobal);
  if( fabs( posDesBall.Y() ) > ServerParam::PITCH_WIDTH/2.0 ||
      fabs( posDesBall.X() ) > ServerParam::PITCH_LENGTH/2.0 )
  {
    Ray lineBody (posAgent, angGlobal);
    Ray lineSide;
    if( fabs( posDesBall.Y() ) > ServerParam::PITCH_WIDTH/2.0 ){
    	lineSide = Ray(Vector(0,(posDesBall.Y() >0? 1:-1)* ServerParam::PITCH_WIDTH/2.0 ), 0 );
    }

    else{
    	lineSide = Ray(Vector(0,(posDesBall.Y() >0? 1:-1)* ServerParam::PITCH_WIDTH/2.0 ), 90 );
    }
    Vector posIntersect;
    lineSide.Intersection( Line(lineBody), posIntersect );
    posDesBall = posAgent + Polar2Vector(posIntersect.Dist( posAgent ) - 0.2,angGlobal);

  }

  	  Vector	  vecDesired = posDesBall - bs.GetPos();
  return Kicker::KickBall(agent, posDesBall,vecDesired.Mod(),1,false);

}

/**
 * Turn to the ball.
 * \param agent which player this action is calculated for.
 * \return an AtomicAction.
 */
AtomicAction Kicker::GetTurnAction(const Agent & agent)
{
	UpdateKickData(agent);

	AtomicAction a;
	a.mSucceed = false;

	Vector ball_pos = mInput.mBallPos + mInput.mBallVel - mInput.mPlayerVel;
	Vector ball_vel = mInput.mBallVel * ServerParam::instance().ballDecay();
	Vector target   = mKickTarget - mInput.mPlayerVel;
	if (ball_pos.Mod() < PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).kickableArea() - 0.05 &&
        ball_pos.Mod() > PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).playerSize() + ServerParam::instance().ballSize() + 0.01)
	{
        double max_turn_angle = GetMaxTurnAngle(mInput.mPlayerType, mInput.mPlayerVel.Mod());
		if (max_turn_angle > fabs(ball_pos.Dir()))
		{
			max_turn_angle = ball_pos.Dir();
		}
		else
		{
			max_turn_angle = (ball_pos.Dir() > 0) ? max_turn_angle : -max_turn_angle;
		}

		/** turn之后的状态 */
		ball_pos = ball_pos.Rotate(-max_turn_angle);
		ball_vel = ball_vel.Rotate(-max_turn_angle);
		target   = target.Rotate(-max_turn_angle);

        double kick_angle = (target-ball_pos).Dir();
        if (fabs(kick_angle) < 90.0) // 尽量转身以后要面对要踢的方向
        {
		    double max_speed = GetOneKickMaxSpeed(ball_vel, kick_angle,
                GetKickRate(ball_pos, mInput.mPlayerType) * ServerParam::instance().maxPower());
		    if (max_speed > mKickSpeed + 0.16) /** 这里要苛刻一点，否则可能由于误差导致转身过后达不到所需速度 */
		    {
			    a.mType         = CT_Turn;
			    a.mSucceed      = true;
			    a.mTurnAngle    = max_turn_angle;
		    }
        }
	}
	return a;
}


/**
 * Kick the ball to mKickTarget by 1 kick action.
 * \param agent which agent this plan is calculated for.
 * \param is_shoot if this plan was calculated for shoot, default is false.
 * \return an ActionPlan.
 */
ActionPlan Kicker::OneCycleKick(const Agent & agent, bool is_shoot)
{
	UpdateKickData(agent);

	AtomicAction a = GetKickOutAction(agent, mInput.mBallPos, mInput.mBallVel);

	ActionPlan p;
	p.mActionQueue.push_back(a);
	p.mCycle     = 1;
	p.mSucceed   = a.mSucceed;

	if (p.mSucceed == false) /** mKickSpeed太大或者太小，一脚不能达到该速度，如果是射门则以最大速度射，否则先停球 */
	{
		if (is_shoot)
		{
			a = GetMaxKickOutAction(agent, mInput.mBallPos, mInput.mBallVel);
		}
		else
		{
			a = GetStopBallAction(agent);
		}

		if (a.mSucceed == true)
		{
			p.mActionQueue.clear();
			p.mActionQueue.push_back(a);
			p.mSucceed = true;
		}
	}
	return p;
}


/**
 * Kick the ball to mKickTarget by more than 1 kick action.
 * \param agent which agent this plan is calculated for.
 * \param cycle cycles this plan should cost.
 * \return an ActionPlan.
 */
ActionPlan Kicker::MultiCycleKick(const Agent & agent, int cycle)
{
	Assert(cycle > 1);

	UpdateKickData(agent);

	ActionPlan plan;
	AtomicAction act;

	/** 首先考虑能否先转身再踢，可以就不进行后面的搜索了 */
	act = GetTurnAction(agent);
	if (act.mSucceed == true)
	{
		plan.mCycle = 2;
		plan.mSucceed = true;
		plan.mActionQueue.clear();
		plan.mActionQueue.push_back(act);
		return plan;
	}

	/** 其次考虑能否先停再转身，再踢 */
	double speed_buf = 0.1;
	int index = -1;
	double turn_max_speed = 0.0;
	double turn_poss = 0.0;
	if (cycle >= 3)
	{
		act = TurnPlan(agent, index, turn_max_speed);
		if (act.mSucceed == true && index >= 0 && turn_max_speed > mKickSpeed + 0.1)
		{
			turn_poss = mPointEva[index] * mSpeedCurve.GetOutput(turn_max_speed - mKickSpeed + speed_buf);
			turn_poss *= 1.2; /** 这里要仔细调下 */
		}
	}

	/** 下面进行多脚踢球的规划 */
	//target及mPoint都是在踢球的最后一个周期的坐标系中
	Vector target    = mKickTarget
	                   - mInput.mPlayerVel * ((1 - pow(PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).playerDecay(), cycle-1)) / (1 - PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).playerDecay()));
	int best         = -1;
	double poss      = 0.0;
	double max_poss  = 0.0;

	int idx = (int)GetNormalizeAngleDeg((target-mInput.mBallPos).Dir(), -FLOAT_EPS); /** 得到0到359的一个整数 */

	if (cycle < 4 && mMaxSpeedFlag[idx][cycle-1] == mAgentID) {
		best = NearestPoint(mMaxSpeedOutFirstPos[idx][cycle-1]);

		//std::cerr << agent.GetSelfUnum() << "@" << agent.GetWorldState().CurrentTime() << ": good" << std::endl;

		if (best >= 0){
			poss = mPointEva[best] * mSpeedCurve.GetOutput(mMaxSpeed[idx][cycle-1] - mKickSpeed + speed_buf);
		}
	}

	if (poss < 0.001) {
		//std::cerr << agent.GetSelfUnum() << "@" << agent.GetWorldState().CurrentTime() << ": no good" << std::endl;

		for (int k = 0, v = cycle - 2, j = NearestPoint(mInput.mBallPos); k < POINTS_NUM; ++k)
		{
			int i = (int)(GetNormalizeAngleDeg((target-mPoint[k]).Dir(), -FLOAT_EPS) / STEP_KICK_ANGLE);
			if (mPointEva[k] > 0.001 && (double)mKickerValue[v][i][j][k] > mKickSpeed - speed_buf) // 要大于期望的出球速度
			{
				//mPoint[k]+mInput.mPlayerVel是mPoint[k]在踢球的第一个周期的坐标系中的位置
				Vector ball_vel = (mPoint[k]+mInput.mPlayerVel - mInput.mBallPos) * ServerParam::instance().ballDecay(); // 踢到k后的球速
				double speed = 0.0;

				if (cycle == 2) // 前向搜索1步
				{
					speed = GetOneKickMaxSpeed(ball_vel, (target-mPoint[k]).Dir(), mMaxAccel[k]);
				}
				else // 前向搜索2步
				{
					Vector ball_next = mPoint[k] + ball_vel;
					for (int l = 0; l < POINTS_NUM; ++l) // 从j踢到k，再踢到l
					{
						if (mPoint[l].Dist(ball_next) < mMaxAccel[k]) // 保证理论上可以从k踢到l
						{
							//mPoint[l]+PlayerVel*Decay是mPoint[l]在踢球的第二个周期的坐标系中的位置
							Vector ball_vel_next = (mPoint[l]+mInput.mPlayerVel*PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).playerDecay() - mPoint[k]) * ServerParam::instance().ballDecay();
							if (cycle == 3)
							{
								speed = Max(speed, GetOneKickMaxSpeed(ball_vel_next, (target-mPoint[l]).Dir(), mMaxAccel[l]));
							}
							else if (cycle == 4) // 利用mKickerValue[1]的信息，没必要再前向了
							{
								int t = (int)(GetNormalizeAngleDeg((target-mPoint[l]).Dir(), -FLOAT_EPS) / STEP_KICK_ANGLE);
								speed = Max(speed, (double)mKickerValue[1][t][k][l]);
							}
						}
					}
				}

				if (speed > mKickSpeed - speed_buf)
				{
					poss = mPointEva[k] * mSpeedCurve.GetOutput(speed - mKickSpeed + speed_buf);
				}
				else
				{
					poss = 0.0;
				}

				if (poss > max_poss) // 找到poss最大的点
				{
					best        = k;
					max_poss    = poss;
					//max_speed   = speed;
				}
			}
		}
	}

	if (turn_poss > max_poss) // 转身规划的收益较高
	{
		plan.mCycle = 3;
		plan.mSucceed = true;
		plan.mActionQueue.clear();
		plan.mActionQueue.push_back(act);
		return plan;
	}

	// 不进行转身的规划，进行多脚踢球
	plan.mCycle     = cycle;
	plan.mSucceed   = false;
	if (best >= 0)
	{
		act = GetOneKickAction(agent, mInput.mBallPos, mInput.mBallVel, mInput.mPlayerVel + mPoint[best]);
		if (act.mSucceed == true)
		{
			plan.mSucceed = true;
			plan.mActionQueue.clear();
			plan.mActionQueue.push_back(act);
		}
	}
	return plan;
}


/**
 * Stop ball and turn to it.
 * \param agent which agent this plan is calculated for.
 * \param index will be set to the index of the position of the ball in discrete points after it had
 *              been stopped.
 * \param turn_max_speed will be set to the maximum speed the ball can reach after this action was
 *                       done.
 */
AtomicAction Kicker::TurnPlan(const Agent & agent, int index, double turn_max_speed)
{
	UpdateKickData(agent);

	AtomicAction a = GetStopBallAction(agent, true);
	if (a.mSucceed == false)
	{
		return a;
	}

	// kick之后的各个量
	Vector ball_vel     = mInput.mBallVel + a.mKickVel;
	Vector ball_pos     = mInput.mBallPos + ball_vel - mInput.mPlayerVel;
	ball_vel            = ball_vel * ServerParam::instance().ballDecay();
	Vector target       = mKickTarget - mInput.mPlayerVel;
	Vector player_vel   = mInput.mPlayerVel * PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).playerDecay();

	// turn-转坐标轴之前的状态
	ball_pos += ball_vel - player_vel;
	ball_vel *= ServerParam::instance().ballDecay();
	target   -= player_vel;
	if (ball_pos.Mod() > PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).kickableArea() - 0.05 || //保证下周期球可踢
			ball_pos.Mod() < PlayerParam::instance().HeteroPlayer(mInput.mPlayerType).playerSize() + ServerParam::instance().ballSize() + 0.01) //保证不碰撞
	{
		a.mSucceed = false;
		return a;
	}

	double max_turn_angle = GetMaxTurnAngle(mInput.mPlayerType, player_vel.Mod());
	if (max_turn_angle > fabs(ball_pos.Dir()))
	{
		max_turn_angle = ball_pos.Dir();
	}
	else
	{
		max_turn_angle = ball_pos.Dir() > 0 ? max_turn_angle : -max_turn_angle;
	}

	// turn-转坐标轴之后的状态
	ball_pos = ball_pos.Rotate(-max_turn_angle);
	ball_vel = ball_vel.Rotate(-max_turn_angle);
	target   = target.Rotate(-max_turn_angle);

	Vector kicked_ball_pos = mInput.mBallPos + mInput.mBallVel + a.mKickVel - mInput.mPlayerVel;
	index = NearestPoint(kicked_ball_pos); // 这里要用kick后的点算
	turn_max_speed = GetOneKickMaxSpeed(ball_vel, (target-ball_pos).Dir(),
        GetKickRate(ball_pos, mInput.mPlayerType) * ServerParam::instance().maxPower());
	return a;
}


/**
 * Get an atomic action to stop ball or minimum ball speed.
 * \param agent which agent this plan is calculated for.
 * \param forecStop if force this method to stop the ball, default is true.
 * \return if forceStop is set to true, this method returns if there's a way to stop the ball, else
 *         reports if it is possible to minimum ball speed.
 */
AtomicAction Kicker::GetStopBallAction(const Agent & agent, bool forceStop)
{
	UpdateKickData(agent);

	int best            = -1;
	double speed        = 0.0;
	double min_speed    = ServerParam::instance().ballSpeedMax();

	for (int i = 0; i < POINTS_NUM ; i++)
	{
		if (mPointEva[i] > FLOAT_EPS)
		{
			speed = (mInput.mPlayerVel + mPoint[i]- mInput.mBallPos).Mod();
			if (speed < min_speed) /** 找出踢完后球速最小的点 */
			{
				best = i;
				min_speed = speed;
			}
		}
	}

	AtomicAction b;
	if(best >= 0)
	{
		b = GetOneKickAction(agent, mInput.mBallPos, mInput.mBallVel, mInput.mPlayerVel + mPoint[best]);
	}

	if (b.mSucceed == false && forceStop)
	{
		b.mType = CT_Kick;
        if (mInput.mBallVel.Mod() > FLOAT_EPS)
        {
            b.mKickVel = mInput.mBallVel.Rotate(180.0).SetLength(ServerParam::instance().maxPower());
            b.mSucceed = true;
        }
        else
        {
            b.mKickVel = Vector(0.0, 0.0);
            b.mSucceed = false;
        }
	}
	return b;
}


/**
 * Accelerate the ball to a certain velocity.
 * \param agent which agent this plan is calculated for.
 * \param goal_vel objective velocity (in field coordinate system).
 * \return an atomic action.
 */
AtomicAction Kicker::GetAccelerateBallAction(const Agent & agent, Vector & goal_vel)
{
    UpdateKickData(agent);

    AtomicAction action;
    action.mType = CT_Kick;

    Vector accel = goal_vel.Rotate(-mInput.mPlayerBodyDir) - mInput.mBallVel;
    action.mKickVel = accel / agent.GetSelf().GetKickRate();
    if (action.mKickVel.Mod() <= ServerParam::instance().maxPower())
    {
        action.mSucceed = true;
    }
    else
    {
        action.mSucceed = false;
        action.mKickVel = action.mKickVel.SetLength(ServerParam::instance().maxPower());
    }

	return action;
}


/**
 * Kick the ball to a certain direction.
 * \param rel_angle direction (in agent's coordinate system).
 * \param rel_dist_rate relative rate for target distance and kickable area, default is 0.9.
 */
AtomicAction Kicker::GetKickBallToAngleAction(const Agent & agent, AngleDeg rel_angle, double rel_dist_rate)
{
    UpdateKickData(agent);

    AtomicAction action;
    action.mType = CT_Kick;

    Vector target = mInput.mPlayerVel + Polar2Vector(agent.GetSelf().GetKickableArea() * rel_dist_rate, rel_angle);
    action.mKickVel = (target - mInput.mBallPos - mInput.mBallVel) / agent.GetSelf().GetKickRate();
    action.mKickVel = action.mKickVel.SetLength(action.mKickVel.Mod() - 0.01);
    if (action.mKickVel.Mod() <= ServerParam::instance().maxPower())
    {
        action.mSucceed = true;
        return action;
    }
    else
    {
        action.mSucceed = false;
        return action;
    }
}


// end of Kicker.cpp

