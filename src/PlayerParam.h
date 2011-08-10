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

#ifndef PLAYERPARAM_H_
#define PLAYERPARAM_H_

#include "ParamEngine.h"
#include "Types.h"
#include "Parser.h"

/**
 * 将dash power离散为100个，存储一些数据
 * Dash power will be divided into 100, some data storage.
 */
#define DASH_POWER_NUM 100

class HeteroParam: public ParamEngine {
	HeteroParam( const HeteroParam & ); // not used
	HeteroParam & operator=( const HeteroParam & ); // not used
	void ParseFromCmdLine(int argc, char **argv);
	bool ParseFromConfigFile(const char *file_name);

public:
	HeteroParam();
	virtual ~HeteroParam(){};

private:
	void AddParams();

private:
	int    M_type;

	double player_speed_max;
	double stamina_inc_max;
	double player_decay;
	double inertia_moment;
	double dash_power_rate;
	double player_size;
	double kickable_margin;
	double kick_rand;
	double extra_stamina;
	double effort_max;
	double effort_min;

	//v14
	double kick_power_rate;
	double catchable_area_l_stretch;
	double foul_detect_probability;

	//private
	double kickable_area;
	double min_catch_area;
	double max_catch_area;
    // 下面4个量都是以effort_max来计算的，使用时要注意
    double effective_speed_max;
    double acceleration_front_rate;
    double acceleration_side_rate;

	Array<double, DASH_POWER_NUM + 1> acceleration_front;
	Array<double, DASH_POWER_NUM + 1> acceleration_side;

public:
	const int & type() const { return M_type; }

	//standard
	const double & playerSpeedMax() const { return player_speed_max; }
	const double & staminaIncMax() const { return stamina_inc_max; }
	const double & playerDecay() const { return player_decay; }
	const double & inertiaMoment() const { return inertia_moment; }
	const double & dashPowerRate() const { return dash_power_rate; }
	const double & playerSize() const { return player_size; }
	const double & kickableMargin() const { return kickable_margin; }
	const double & kickRand() const { return kick_rand; }
	const double & extraStamina() const { return extra_stamina; }
	const double & effortMax() const { return effort_max; }
	const double & effortMin() const { return effort_min; }

	//v14
	const double & kickPowerRate() const { return kick_power_rate; }
	const double & catchableAreaLStretch() const { return catchable_area_l_stretch; }
	const double & foulDetectProbability() const { return foul_detect_probability; }

	//private
	const double & kickableArea() const { return kickable_area; }
	const double & effectiveSpeedMax() const { return effective_speed_max; }
	const double & minCatchArea() const { return min_catch_area; }
	const double & maxCatchArea() const { return max_catch_area; }

    const double & accelerationFrontRate() const { return acceleration_front_rate; }
    const double & accelerationSideRate() const { return acceleration_side_rate; }
    const double & accelerationFrontMax() const { return acceleration_front[DASH_POWER_NUM]; }
    const double & accelerationSideMax() const { return acceleration_side[DASH_POWER_NUM]; }
    const double & accelerationFront(const double & power) const;
    const double & accelerationSide(const double & power) const;

	double accelerationRateOnDir(const double & dir) const;

	/**
	*  一些有依赖关系的变量的初始化和赋值
	*/
	void MaintainConsistency();
};

class PlayerParam: public ParamEngine {
	PlayerParam(); // private
	PlayerParam( const PlayerParam & ); // not used
	PlayerParam & operator=( const PlayerParam & ); // not used

public:
	virtual ~PlayerParam();
	static PlayerParam &instance();
	void init(int argc, char **argv);

private:
	//default values
	static const char CONFIG_FILE[];
	static const char LOG_DIR[];
	static const char TEAM_NAME[];
	static const char OPPONENT_TEAM_NAME[];
	static const char HETERO_TEST_MODEL[];

	static const int DEFAULT_PLAYER_TYPES;
	static const int DEFAULT_SUBS_MAX;
	static const int DEFAULT_PT_MAX;

	static const double DEFAULT_PLAYER_SPEED_MAX_DELTA_MIN;
	static const double DEFAULT_PLAYER_SPEED_MAX_DELTA_MAX;
	static const double DEFAULT_STAMINA_INC_MAX_DELTA_FACTOR;

	static const double DEFAULT_PLAYER_DECAY_DELTA_MIN;
	static const double DEFAULT_PLAYER_DECAY_DELTA_MAX;
	static const double DEFAULT_INERTIA_MOMENT_DELTA_FACTOR;

	static const double DEFAULT_DASH_POWER_RATE_DELTA_MIN;
	static const double DEFAULT_DASH_POWER_RATE_DELTA_MAX;
	static const double DEFAULT_PLAYER_SIZE_DELTA_FACTOR;

	static const double DEFAULT_KICKABLE_MARGIN_DELTA_MIN;
	static const double DEFAULT_KICKABLE_MARGIN_DELTA_MAX;
	static const double DEFAULT_KICK_RAND_DELTA_FACTOR;

	static const double DEFAULT_EXTRA_STAMINA_DELTA_MIN;
	static const double DEFAULT_EXTRA_STAMINA_DELTA_MAX;
	static const double DEFAULT_EFFORT_MAX_DELTA_FACTOR;
	static const double DEFAULT_EFFORT_MIN_DELTA_FACTOR;

	static const int    DEFAULT_RANDOM_SEED;

	static const double DEFAULT_NEW_DASH_POWER_RATE_DELTA_MIN;
	static const double DEFAULT_NEW_DASH_POWER_RATE_DELTA_MAX;
	static const double DEFAULT_NEW_STAMINA_INC_MAX_DELTA_FACTOR;

	//v 14
    static const double DEFAULT_KICK_POWER_RATE_DELTA_MIN;
    static const double DEFAULT_KICK_POWER_RATE_DELTA_MAX;
    static const double DEFAULT_FOUL_DETECT_PROBABILITY_DELTA_FACTOR;

    static const double DEFAULT_CATCHABLE_AREA_L_STRETCH_MIN;
    static const double DEFAULT_CATCHABLE_AREA_L_STRETCH_MAX;

    //private
	static const double MAX_SHOOT_DISTANCE;
	static const double SHOOT_EVALUATION;
	static const double SHOOT_MAX_CONSIDER_PROB;
	static const double SHOOT_MIN_PROB;
	static const double SHOOT_MIN_PROB1;
	static const double SHOOT_MIN_PROB2;
	static const int SHOOT_MAX_SEARCH_COUNT;
	static const double SHOOT_DIR_SEARCH_STEP;

	//conf的常量 参考we2008
	static const double MAX_CONF;
	static const double MIN_VALID_CONF;
	static const double CONF_DECAY;
	static const double PLAYER_CONF_DECAY;
	static const double BALL_CONF_DECAY;

private:
	int player_types;
	int subs_max;
	int pt_max;

	bool M_allow_mult_default_type;

	double player_speed_max_delta_min;
	double player_speed_max_delta_max;
	double stamina_inc_max_delta_factor;

	double player_decay_delta_min;
	double player_decay_delta_max;
	double inertia_moment_delta_factor;

	double dash_power_rate_delta_min;
	double dash_power_rate_delta_max;
	double player_size_delta_factor;

	double kickable_margin_delta_min;
	double kickable_margin_delta_max;
	double kick_rand_delta_factor;

	double extra_stamina_delta_min;
	double extra_stamina_delta_max;
	double effort_max_delta_factor;
	double effort_min_delta_factor;

	double new_dash_power_rate_delta_min;
	double new_dash_power_rate_delta_max;
	double new_stamina_inc_max_delta_factor;

	//v14
	double catchable_area_l_stretch_min;
	double catchable_area_l_stretch_max;

	double kick_power_rate_delta_min;
	double kick_power_rate_delta_max;

	double foul_detect_probability_delta_factor;

	//private
	std::string M_player_conf_file;
	std::string M_log_dir;
	std::string M_team_name;
	int M_team_name_len;
	std::string M_opponent_team_name;
	std::string M_hetero_test_model;

	int M_our_goalie_unum;
	bool M_is_goalie;
	bool M_is_coach;
	double M_player_version;
	double M_coach_version;

	double M_say_pos_x_eps;
	double M_say_pos_y_eps;
	double M_say_ball_speed_eps;
	double M_say_player_speed_eps;
	double M_say_dir_eps;

	double M_max_conf;
	double M_min_valid_conf;
	double M_conf_decay;
	double M_player_conf_decay;
	double M_ball_conf_decay;

	enum
	{
		SIGHTSIZE = 62,
		MARKSIZE = 388
	};
	double mSightChange[SIGHTSIZE][3];
	double mMarkChange[MARKSIZE][3];

	void AddParams();
public:


	//sight dist 主要指视觉中ball player的dist ,其所加的误差加了0.1的截断
    double ConvertSightDist(double dist)
    {
		Assert(dist >= 0);
		int low = 0;
		int middle = 0;
		int high = SIGHTSIZE;

		while(low <=  high)
		{
			middle = (low + high ) / 2;

			if (middle >= SIGHTSIZE || middle < 0)
			{
				break;
			}

			if (fabs(dist - mSightChange[middle][0]) < 0.01)
			{
				return mSightChange[middle][1];
			}

			if (dist < mSightChange[middle][0])
			{
				high = middle;
			}
			else
			{
				low = middle + 1;
			}
		}

		 PRINT_ERROR("can not find this dist " << dist );
		 return dist;
    }

	double GetEpsInSight(double dist)
	{
		 Assert(dist >= 0);
		 int low = 0;
		 int middle = 0;
		 int high = SIGHTSIZE - 1;

		 while(low <= high)
		 {
			middle = (low + high) / 2;

			if (middle + 1 >= SIGHTSIZE)
			{
				return mSightChange[SIGHTSIZE - 1][2];
			}

			if (dist >= mSightChange[middle][0] && dist <= mSightChange[middle + 1][0])
			{
				if (fabs(dist - mSightChange[middle][0]) < fabs (dist - mSightChange[middle + 1][0]))
				{
					return mSightChange[middle][2];
				}
				else
				{
					return mSightChange[middle + 1][2];
				}
			}

			if (middle == 0)
			{
				return mSightChange[middle][2];
			}

			if (dist < mSightChange[middle][0])
			{
				high = middle;
			}
			else
			{
				low = middle + 1;
			}

		 }

		 PRINT_ERROR("can not find this dist " << dist );
		 return 0;
	}

	//mark dist 主要指视觉中marker 的dist的误差 ,其所加的截断为0.01
    double ConvertMarkDist(double dist)
    {
		Assert(dist >= 0);
		int low = 0;
		int middle = 0;
		int high = MARKSIZE;

		while(low <=  high)
		{
			middle = (low + high ) / 2;

			if (middle >= MARKSIZE || middle < 0)
			{
				break;
			}

			if (fabs(dist - mMarkChange[middle][0]) < 0.01)
			{
				return mMarkChange[middle][1];
			}

			if (dist < mMarkChange[middle][0])
			{
				high = middle;
			}
			else
			{
				low = middle + 1;
			}
		}

		 PRINT_ERROR("can not find this dist " << dist );
		 return dist;
    }

	double GetEpsInMark(double dist)
	{
		 Assert(dist >= 0);
		 int low = 0;
		 int middle = 0;
		 int high = MARKSIZE - 1;

		 while(low <= high)
		 {
			middle = (low + high) / 2;

			if (middle + 1 >= MARKSIZE)
			{
				return mMarkChange[MARKSIZE - 1][2];
			}

			if (dist >= mMarkChange[middle][0] && dist <= mMarkChange[middle + 1][0])
			{
				if (fabs(dist - mMarkChange[middle][0]) < fabs (dist - mMarkChange[middle + 1][0]))
				{
					return mMarkChange[middle][2];
				}
				else
				{
					return mMarkChange[middle + 1][2];
				}
			}

			if (middle == 0)
			{
				return mMarkChange[middle][2];
			}

			if (dist < mMarkChange[middle][0])
			{
				high = middle;
			}
			else
			{
				low = middle + 1;
			}
		 }

		 PRINT_ERROR("can not find this dist " << dist );
		 return 0;
	}

	double ConvertAngle(double angle)
	{
		if (angle > 0)
		{
			angle += 0.5;
		}
		else	if (angle < 0)
		{
			angle -= 0.5;
		}

		return angle;
	}

	void MaintainConsistency();
	bool SaveParam();

	int playerTypes() const { return player_types; }
	int subsMax() const { return subs_max; }
	int ptMax() const { return pt_max; }

	bool allowMultDefaultType() const { return M_allow_mult_default_type; }

	const double & maxConf() const { return M_max_conf; }
	const double & minValidConf() const { return M_min_valid_conf; }
	const double & confDecay() const { return M_conf_decay; }
	const double & playerConfDecay() const { return M_player_conf_decay; }
	const double & ballConfDecay() const { return M_ball_conf_decay; }

	const double & playerSpeedMaxDeltaMin() const { return player_speed_max_delta_min; }

	const double & playerSpeedMaxDeltaMax() const { return player_speed_max_delta_max; }
	const double & staminaIncMaxDeltaFactor() const { return stamina_inc_max_delta_factor; }

	const double & playerDecayDeltaMin() const { return player_decay_delta_min; }
	const double & playerDecayDeltaMax() const { return player_decay_delta_max; }
	const double & inertiaMomentDeltaFactor() const { return inertia_moment_delta_factor; }

	const double & dashPowerRateDeltaMin() const { return dash_power_rate_delta_min; }
	const double & dashPowerRateDeltaMax() const { return dash_power_rate_delta_max; }
	const double & playerSizeDeltaFactor() const { return player_size_delta_factor; }

	const double & kickableMarginDeltaMin() const { return kickable_margin_delta_min; }
	const double & kickableMarginDeltaMax() const { return kickable_margin_delta_max; }
	const double & kickRandDeltaFactor() const { return kick_rand_delta_factor; }

	const double & extraStaminaDeltaMin() const { return extra_stamina_delta_min; }
	const double & extraStaminaDeltaMax() const { return extra_stamina_delta_max; }
	const double & effortMaxDeltaFactor() const { return effort_max_delta_factor; }
	const double & effortMinDeltaFactor() const { return effort_min_delta_factor; }

    const double & kickPowerRateDeltaMin() const { return kick_power_rate_delta_min; }
    const double & kickPowerRateDeltaMax() const { return kick_power_rate_delta_max; }
    const double & foulDetectProbabilityDeltaFactor() const { return foul_detect_probability_delta_factor; }

	const double & newDashPowerRateDeltaMin() const { return new_dash_power_rate_delta_min; }
	const double & newDashPowerRateDeltaMax() const { return new_dash_power_rate_delta_max; }
	const double & newStaminaIncMaxDeltaFactor() const { return new_stamina_inc_max_delta_factor; }

	const double & shootMaxDistance() const { return shoot_max_distance; }

	const std::string & logDir() const { return M_log_dir; }
	const std::string & teamName() const { return M_team_name; }
	void setTeamName(const char *name) { M_team_name = std::string(name); }
	const int & teamNameLen() const { return M_team_name_len; }
	const std::string & opponentTeamName() const { return M_opponent_team_name; }

	void setOpponentTeamName(const char *name) 
	{ 
		M_opponent_team_name = std::string(name);
	}

	const std::string & heteroTestModel() const { return M_hetero_test_model; }

	const int & ourGoalieUnum() const { return M_our_goalie_unum; } //这个量在决策层不应该使用，否则反算对手时会出错
	const bool & isGoalie() const { return M_is_goalie; } //这个量在决策层不应该使用，否则反算对手时会出错
	const bool & isCoach() const { return M_is_coach; }
	const double & playerVersion() const { return M_player_version; }
	const double & coachVersion() const { return M_coach_version; }

	const double & sayPosXEps() const { return M_say_pos_x_eps; }
	const double & sayPosYEps() const { return M_say_pos_y_eps; }
	const double & sayBallSpeedEps() const { return M_say_ball_speed_eps; }
	const double & sayPlayerSpeedEps() const { return M_say_player_speed_eps; }
	const double & sayDirEps() const { return M_say_dir_eps; }

private:
	HeteroParam *mHeteroPlayer;

public:
	/**
	* add hetero player type from server msg
	* @param line server msg
	*/
	void AddPlayerType(int type, const char *line);

	HeteroParam & HeteroPlayer(const int & type) const
	{
		Assert (type >= 0 && type < PlayerParam::instance().playerTypes());
		Assert(type == 0 || Parser::IsPlayerTypesReady());

		return mHeteroPlayer[type];
	}

private:
	static const bool DYNAMIC_DEBUG_MODE;
	static const bool SAVE_SERVER_MESSAGE;
	static const bool SAVE_SIGHT_LOG;
	static const bool SAVE_DEC_LOG;
	static const bool SAVE_TEXT_LOG;
	static const bool SAVE_STAT_LOG;
	static const bool USE_PLOTTER;
    static const bool USE_TEAM_GRAPHIC;
	static const bool TIME_TEST;
	static const bool NETWORK_TEST;
	static const int WAIT_SIGHT_BUFFER;
	static const int WAIT_HEAR_BUFFER;
	static const int WAIT_TIME_OUT;
	static const double ROUTE_ANGLE_DIFF;
    static const double OPP_TACKLE_THRESHOLD_FORWARD;
    static const double OPP_TACKLE_THRESHOLD_MID;
    static const double OPP_TACKLE_THRESHOLD_BACK;
    static const double TIRED_BUFFER;
    static const double AT_POINT_BUFFER;
    static const int KICKER_MODE;
    static const int MARKOV_DRIBBLER_MODE;
    static const int MARKOV_DRIBBLER_HORIZON;
    static const int MARKOV_DRIBBLER_METHOD;
	static const double MIN_APPEARANCE_POSS;
	static const double GOALIE_LOW_DELAY;
	static const double GOALIE_HIGH_DELAY;
	static const int A_STAR_GO_TO_POINT_MAX_DEPTH;
	static const int A_STAR_GO_TO_POINT_MAX_NODES;
	static const int VELOCITY_RANDOMIZED_DIRS;
	static const int VELOCITY_RANDOMIZED_SAMPLES;
	static const double LOW_STAMINA_POINT_THR;
	static const int COACH_SEND_HETERO_INFO_CONTROL;
	static const double SETPLAY_REINFORCE_DIST;
	static const int SETPLAY_REINFORCE_PLAYERS;

	bool mDynamicDebugMode; // DynamicDebug模式
	bool mSaveServerMessage; // 是否保存server的信息，用于动态调试
	bool mSaveSightLog; // 是否保存sight_log
	bool mSaveDecLog; // 是否保存dec_log
	bool mSaveTextLog;
	bool mUsePlotter;
    bool mUseTeamGraphic;
	bool mTimeTest;
	bool mNetworkTest;
	int mWaitSightBuffer; // 等待视觉到来的最大buffer
	int mWaitHearBuffer; // 等待听觉到来的最大buffer
	int mWaitTimeOut; // 等待server的最大时间

    double mTiredBuffer;
    double mMinStamina;
    double mAtPointBuffer;

    //射门相关
	double shoot_max_distance;

	/**
     * 对应Kicker的几种状态
     *
     * 0表示读取值函数文件，进行正常比赛
     * 1表示离线计算值函数，需要关掉所有log
     */
    int mKickerMode;

    /**
     * 如果视觉部分导致超时严重，就调大这个变量，最大为1
     */
	double M_min_appearance_poss;

    double mLowStaminaPointThr;

public:
	const bool & DynamicDebugMode() const { return mDynamicDebugMode; }
	const bool & SaveServerMessage() const { return mSaveServerMessage; }
	const bool & SaveSightLog() const { return mSaveSightLog; }
	const bool & SaveDecLog() const { return mSaveDecLog; }
	const bool & SaveTextLog() const { return mSaveTextLog; }
	const bool & TimeTest() const { return mTimeTest; }
	const bool & NetworkTest() const { return mNetworkTest; }
	const bool & UsePlotter() const { return mUsePlotter; }
    const bool & UseTeamGraphic() const { return mUseTeamGraphic; }
	const int & WaitSightBuffer() const { return mWaitSightBuffer; }
	const int & WaitHearBuffer() const { return mWaitHearBuffer; }
	const int & WaitTimeOut() const { return mWaitTimeOut; }

	const double & minAppearancePoss() const { return M_min_appearance_poss; }

    const double & TiredBuffer() const { return mTiredBuffer; }
    const double & MinStamina() const { return mMinStamina; }
    const double & AtPointBuffer() const { return mAtPointBuffer; }
    const int & KickerMode() const { return mKickerMode; }

	const double & LowStaminaPointThr() const { return mLowStaminaPointThr; }
};

#endif /* PLAYERPARAM_H_ */
