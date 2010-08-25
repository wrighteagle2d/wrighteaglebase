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

#include "ServerParam.h"
#include "PlayerParam.h"
#include "Parser.h"
#include "ActionEffector.h"
#include <fstream>

const char PlayerParam::CONFIG_FILE[] = "./conf/player.conf";
const char PlayerParam::LOG_DIR[] = "Logfiles";
const char PlayerParam::TEAM_NAME[] = "WE2010";
const char PlayerParam::OPPONENT_TEAM_NAME[] = "";
const char PlayerParam::HETERO_TEST_MODEL[] = "./conf/hetero.conf";

const int PlayerParam::DEFAULT_PLAYER_TYPES = 18; // [12.0.0] 7 -> 18
const int PlayerParam::DEFAULT_SUBS_MAX = 3;
const int PlayerParam::DEFAULT_PT_MAX = 1; // [12.0.0] 3 -> 1

const double PlayerParam::DEFAULT_PLAYER_SPEED_MAX_DELTA_MIN = 0.0;
const double PlayerParam::DEFAULT_PLAYER_SPEED_MAX_DELTA_MAX = 0.0;
const double PlayerParam::DEFAULT_STAMINA_INC_MAX_DELTA_FACTOR = 0.0;

const double PlayerParam::DEFAULT_PLAYER_DECAY_DELTA_MIN = -0.1; // [12.0.0] 0.0 -> -0.05
const double PlayerParam::DEFAULT_PLAYER_DECAY_DELTA_MAX = 0.1; // [12.0.0] 0.2 -> 0.1
const double PlayerParam::DEFAULT_INERTIA_MOMENT_DELTA_FACTOR = 25.0;

const double PlayerParam::DEFAULT_DASH_POWER_RATE_DELTA_MIN = 0.0;
const double PlayerParam::DEFAULT_DASH_POWER_RATE_DELTA_MAX = 0.0;
const double PlayerParam::DEFAULT_PLAYER_SIZE_DELTA_FACTOR = -100.0;

const double PlayerParam::DEFAULT_KICKABLE_MARGIN_DELTA_MIN = -0.1; // [12.0.0] 0.0 -> -0.1
const double PlayerParam::DEFAULT_KICKABLE_MARGIN_DELTA_MAX = 0.1; // [12.0.0] 0.2 -> 0.1
const double PlayerParam::DEFAULT_KICK_RAND_DELTA_FACTOR = 1.0; // [12.0.0] 0.5 -> 1.0

const double PlayerParam::DEFAULT_EXTRA_STAMINA_DELTA_MIN = 0.0;
const double PlayerParam::DEFAULT_EXTRA_STAMINA_DELTA_MAX = 50.0;
const double PlayerParam::DEFAULT_EFFORT_MAX_DELTA_FACTOR = -0.002;
const double PlayerParam::DEFAULT_EFFORT_MIN_DELTA_FACTOR = -0.002;

const double PlayerParam::DEFAULT_NEW_DASH_POWER_RATE_DELTA_MIN = -0.0012; // [12.0.0] 0 -> -0.0005
const double PlayerParam::DEFAULT_NEW_DASH_POWER_RATE_DELTA_MAX = 0.0008; // [12.0.0] 0.002 -> 0.0015
const double PlayerParam::DEFAULT_NEW_STAMINA_INC_MAX_DELTA_FACTOR = -6000.0; // [12.0.0] -10000.0 -> -6000.0

// v14
const double PlayerParam::DEFAULT_KICK_POWER_RATE_DELTA_MIN = 0.0;
const double PlayerParam::DEFAULT_KICK_POWER_RATE_DELTA_MAX = 0.0;
const double PlayerParam::DEFAULT_FOUL_DETECT_PROBABILITY_DELTA_FACTOR = 0.0;

const double PlayerParam::DEFAULT_CATCHABLE_AREA_L_STRETCH_MIN = 1.0;
const double PlayerParam::DEFAULT_CATCHABLE_AREA_L_STRETCH_MAX = 1.3;

//private
const double PlayerParam::MAX_SHOOT_DISTANCE = 32.5;
const double PlayerParam::SHOOT_EVALUATION = 5.0;
const double PlayerParam::SHOOT_MAX_CONSIDER_PROB = 0.97;
const double PlayerParam::SHOOT_MIN_PROB = 0.56;
const double PlayerParam::SHOOT_MIN_PROB1 = 0.60;
const double PlayerParam::SHOOT_MIN_PROB2 = 0.60;
const int PlayerParam::SHOOT_MAX_SEARCH_COUNT = 128;
const double PlayerParam::SHOOT_DIR_SEARCH_STEP = 0.6;

const bool PlayerParam::DYNAMIC_DEBUG_MODE = false;
const bool PlayerParam::SAVE_SERVER_MESSAGE = false;
const bool PlayerParam::SAVE_SIGHT_LOG = false;
const bool PlayerParam::SAVE_DEC_LOG = false;
const bool PlayerParam::SAVE_TEXT_LOG = false;
const bool PlayerParam::USE_PLOTTER = false;
const bool PlayerParam::USE_TEAM_GRAPHIC = true;
const bool PlayerParam::TIME_TEST = false;
const bool PlayerParam::NETWORK_TEST = false;
const int PlayerParam::WAIT_SIGHT_BUFFER = 40; // 每周期最多等视觉40毫秒
const int PlayerParam::WAIT_HEAR_BUFFER = 40; // 每周期最多等听觉40毫秒
const int PlayerParam::WAIT_TIME_OUT = 10; // 每场比赛最多等server10秒
const double PlayerParam::ROUTE_ANGLE_DIFF = 1.0;
const double PlayerParam::OPP_TACKLE_THRESHOLD_FORWARD = 0.69;
const double PlayerParam::OPP_TACKLE_THRESHOLD_MID = 0.75;
const double PlayerParam::OPP_TACKLE_THRESHOLD_BACK = 0.85;
const double PlayerParam::TIRED_BUFFER = 10.0;
const double PlayerParam::AT_POINT_BUFFER = 1.0;
const int PlayerParam::KICKER_MODE = 0;
const int PlayerParam::MARKOV_DRIBBLER_MODE = 0;
const int PlayerParam::MARKOV_DRIBBLER_HORIZON = 3;
const int PlayerParam::MARKOV_DRIBBLER_METHOD = 1;
const double PlayerParam::MAX_CONF           = 1.0;
const double PlayerParam::MIN_VALID_CONF       = 0.5;
const double PlayerParam::CONF_DECAY          = 0.98;
const double PlayerParam::PLAYER_CONF_DECAY    = 0.99;
const double PlayerParam::BALL_CONF_DECAY      = 0.95;
const double PlayerParam::MIN_APPEARANCE_POSS = 0.05;
const double PlayerParam::GOALIE_LOW_DELAY = 0.25;
const double PlayerParam::GOALIE_HIGH_DELAY = 0.25;
const int PlayerParam::A_STAR_GO_TO_POINT_MAX_DEPTH = 128;
const int PlayerParam::A_STAR_GO_TO_POINT_MAX_NODES = 1024;
const int PlayerParam::VELOCITY_RANDOMIZED_DIRS = 8;
const int PlayerParam::VELOCITY_RANDOMIZED_SAMPLES = 1;
const double PlayerParam::LOW_STAMINA_POINT_THR = 2600.0;//这个以下dash时就会控制了
const int PlayerParam::COACH_SEND_HETERO_INFO_CONTROL = 6;
const double PlayerParam::SETPLAY_REINFORCE_DIST = 8.0;
const int PlayerParam::SETPLAY_REINFORCE_PLAYERS = 2;

HeteroParam::HeteroParam()
{
	AddParams();
	MaintainConsistency();
}

void HeteroParam::AddParams()
{
	AddParam( "id", & M_type, 0);
	AddParam( "player_speed_max", & player_speed_max, ServerParam::instance().playerSpeedMax());
	AddParam( "stamina_inc_max", & stamina_inc_max, ServerParam::instance().staminaInc());
	AddParam( "player_decay", & player_decay, ServerParam::instance().playerDecay());
	AddParam( "inertia_moment", & inertia_moment, ServerParam::instance().inertiaMoment());
	AddParam( "dash_power_rate", & dash_power_rate, ServerParam::instance().dashPowerRate());
	AddParam( "player_size", & player_size, ServerParam::instance().playerSize());
	AddParam( "kickable_margin", & kickable_margin, ServerParam::instance().kickableMargin());
	AddParam( "kick_rand", & kick_rand, ServerParam::instance().kickRand());
	AddParam( "extra_stamina", & extra_stamina, ServerParam::instance().extraStamina());
	AddParam( "effort_max", & effort_max, ServerParam::instance().effortInit());
	AddParam( "effort_min", & effort_min, ServerParam::instance().effortMin());

	//v14
	AddParam( "kick_power_rate", & kick_power_rate, ServerParam::instance().kickPowerRate());
	AddParam( "foul_detect_probability", & foul_detect_probability, ServerParam::instance().foulDetectProbability());
	AddParam( "catchable_area_l_stretch", & catchable_area_l_stretch, 1.0);
}

double HeteroParam::accelerationRateOnDir(const double & dir) const
{
	return GetDashDirRate(dir) * dash_power_rate * effort_max;
}

const double & HeteroParam::accelerationFront(const double & power) const
{
    Assert(power >= ServerParam::instance().minDashPower() && power <= ServerParam::instance().maxDashPower());
    return acceleration_front[(int)Rint(power)];
}

const double & HeteroParam::accelerationSide(const double & power) const
{
    Assert(power >= ServerParam::instance().minDashPower() && power <= ServerParam::instance().maxDashPower());
    return acceleration_side[(int)Rint(power)];
}

void HeteroParam::MaintainConsistency()
{
	kickable_area = player_size + ServerParam::instance().ballSize() + kickable_margin;

    effective_speed_max = ServerParam::instance().maxDashPower() * effort_max * dash_power_rate / (1 - player_decay); // 实际可能达到的最大速度 -- 对dash模型求解可得
    acceleration_front_rate = effort_max * dash_power_rate;
    acceleration_side_rate = acceleration_front_rate * ServerParam::instance().sideDashRate();
    for (int i = 0; i <= DASH_POWER_NUM; ++i)
    {
        acceleration_front[i] = (double)i * acceleration_front_rate;
        acceleration_side[i] = (double)i * acceleration_side_rate;
    }
	min_catch_area = Sqrt( ServerParam::instance().catchAreaLength() * ( 2.0 - catchable_area_l_stretch ) 
		* ServerParam::instance().catchAreaLength() * ( 2.0 - catchable_area_l_stretch ) + ServerParam::instance().catchAreaWidth() * ServerParam::instance().catchAreaWidth() / 4 );
	max_catch_area = Sqrt( ServerParam::instance().catchAreaLength() * catchable_area_l_stretch
		* ServerParam::instance().catchAreaLength() * catchable_area_l_stretch + ServerParam::instance().catchAreaWidth() * ServerParam::instance().catchAreaWidth() / 4 );
}

PlayerParam &PlayerParam::instance()
{
	static PlayerParam player_param;
	return player_param;
}

PlayerParam::PlayerParam()
{
	mHeteroPlayer = new HeteroParam[DEFAULT_PLAYER_TYPES];
	AddParams();
	MaintainConsistency();
}

PlayerParam::~PlayerParam()
{
	delete[] mHeteroPlayer;
}

void PlayerParam::AddParams()
{
	AddParam( "player_types", & player_types, DEFAULT_PLAYER_TYPES );
	AddParam( "subs_max", & subs_max, DEFAULT_SUBS_MAX );
	AddParam( "pt_max", & pt_max, DEFAULT_PT_MAX );

	AddParam( "player_speed_max_delta_min", & player_speed_max_delta_min, DEFAULT_PLAYER_SPEED_MAX_DELTA_MIN );
	AddParam( "player_speed_max_delta_max", & player_speed_max_delta_max, DEFAULT_PLAYER_SPEED_MAX_DELTA_MAX );
	AddParam( "stamina_inc_max_delta_factor", & stamina_inc_max_delta_factor, DEFAULT_STAMINA_INC_MAX_DELTA_FACTOR );
	AddParam( "player_decay_delta_min", & player_decay_delta_min, DEFAULT_PLAYER_DECAY_DELTA_MIN );
	AddParam( "player_decay_delta_max", & player_decay_delta_max, DEFAULT_PLAYER_DECAY_DELTA_MAX );
	AddParam( "inertia_moment_delta_factor", & inertia_moment_delta_factor, DEFAULT_INERTIA_MOMENT_DELTA_FACTOR );
	AddParam( "dash_power_rate_delta_min", & dash_power_rate_delta_min, DEFAULT_DASH_POWER_RATE_DELTA_MIN );
	AddParam( "dash_power_rate_delta_max", & dash_power_rate_delta_max, DEFAULT_DASH_POWER_RATE_DELTA_MAX );
	AddParam( "player_size_delta_factor", & player_size_delta_factor, DEFAULT_PLAYER_SIZE_DELTA_FACTOR );
	AddParam( "kickable_margin_delta_min", & kickable_margin_delta_min, DEFAULT_KICKABLE_MARGIN_DELTA_MIN );
	AddParam( "kickable_margin_delta_max", & kickable_margin_delta_max, DEFAULT_KICKABLE_MARGIN_DELTA_MAX );
	AddParam( "kick_rand_delta_factor", & kick_rand_delta_factor, DEFAULT_KICK_RAND_DELTA_FACTOR );
	AddParam( "extra_stamina_delta_min", & extra_stamina_delta_min, DEFAULT_EXTRA_STAMINA_DELTA_MIN );
	AddParam( "extra_stamina_delta_max", & extra_stamina_delta_max, DEFAULT_EXTRA_STAMINA_DELTA_MAX );
	AddParam( "effort_max_delta_factor", & effort_max_delta_factor, DEFAULT_EFFORT_MAX_DELTA_FACTOR );
	AddParam( "effort_min_delta_factor", & effort_min_delta_factor, DEFAULT_EFFORT_MIN_DELTA_FACTOR );
	AddParam( "new_dash_power_rate_delta_min", & new_dash_power_rate_delta_min, DEFAULT_NEW_DASH_POWER_RATE_DELTA_MIN );
	AddParam( "new_dash_power_rate_delta_max", & new_dash_power_rate_delta_max, DEFAULT_NEW_DASH_POWER_RATE_DELTA_MAX );
	AddParam( "new_stamina_inc_max_delta_factor", & new_stamina_inc_max_delta_factor, DEFAULT_NEW_STAMINA_INC_MAX_DELTA_FACTOR );

	// v14
	AddParam( "catchable_area_l_stretch_min", & catchable_area_l_stretch_min, DEFAULT_CATCHABLE_AREA_L_STRETCH_MIN);
	AddParam( "catchable_area_l_stretch_max", & catchable_area_l_stretch_max, DEFAULT_CATCHABLE_AREA_L_STRETCH_MAX);
	AddParam( "kick_power_rate_delta_min", & kick_power_rate_delta_min, DEFAULT_KICK_POWER_RATE_DELTA_MIN);
	AddParam( "kick_power_rate_delta_max", & kick_power_rate_delta_max, DEFAULT_KICK_POWER_RATE_DELTA_MAX);
	AddParam( "foul_detect_probability_delta_factor", & foul_detect_probability_delta_factor,DEFAULT_FOUL_DETECT_PROBABILITY_DELTA_FACTOR);

	AddParam( "allow_mult_default_type", & M_allow_mult_default_type, false );

	AddParam( "player_conf", & M_player_conf_file, std::string(CONFIG_FILE));
	AddParam( "log_dir", & M_log_dir, std::string(LOG_DIR));
	AddParam( "team_name", & M_team_name, std::string(TEAM_NAME));
	AddParam( "opponent_team_name", & M_opponent_team_name, std::string(OPPONENT_TEAM_NAME));
	AddParam( "hetero_test_model", & M_hetero_test_model, std::string(HETERO_TEST_MODEL));

	AddParam( "dynamic_debug_mode", & mDynamicDebugMode, DYNAMIC_DEBUG_MODE );
	AddParam( "save_server_message", & mSaveServerMessage, SAVE_SERVER_MESSAGE );
	AddParam( "save_sight_log", & mSaveSightLog, SAVE_SIGHT_LOG );
	AddParam( "save_dec_log", & mSaveDecLog, SAVE_DEC_LOG );
	AddParam( "save_text_log", & mSaveTextLog, SAVE_TEXT_LOG );
	AddParam( "use_plotter", & mUsePlotter, USE_PLOTTER );
    AddParam( "use_team_graphic", & mUseTeamGraphic, USE_TEAM_GRAPHIC );
    AddParam( "time_test", & mTimeTest, TIME_TEST );
    AddParam( "network_test", & mNetworkTest, NETWORK_TEST );
	AddParam( "wait_sight_buffer", & mWaitSightBuffer, WAIT_SIGHT_BUFFER );
    AddParam( "wait_hear_buffer", & mWaitHearBuffer, WAIT_HEAR_BUFFER );
	AddParam( "wait_time_out", & mWaitTimeOut, WAIT_TIME_OUT );

    AddParam( "tired_buffer", & mTiredBuffer, TIRED_BUFFER );
    AddParam( "at_point_buffer", & mAtPointBuffer, AT_POINT_BUFFER );
    AddParam( "kicker_mode", & mKickerMode, KICKER_MODE );

    AddParam( "our_goalie_unum", & M_our_goalie_unum, 1 );
	AddParam( "goalie", & M_is_goalie, false );
	AddParam( "coach", & M_is_coach, false );
	AddParam( "player_version", & M_player_version, 13.1 );
	AddParam( "coach_version", & M_coach_version, 13.1 );

	AddParam( "say_pos_x_eps", & M_say_pos_x_eps, 0.2);
	AddParam( "say_pos_y_eps", & M_say_pos_y_eps, 0.2);
	AddParam( "say_ball_speed_eps", & M_say_ball_speed_eps, 0.1);
	AddParam( "say_player_speed_eps", & M_say_player_speed_eps, 0.1);
	AddParam( "say_dir_eps", & M_say_dir_eps, 1.0);

	AddParam( "max_conf", & M_max_conf, MAX_CONF);
	AddParam( "min_valid_conf", & M_min_valid_conf, MIN_VALID_CONF);
	AddParam( "conf_decay", & M_conf_decay, CONF_DECAY);
	AddParam( "player_conf_decay", & M_player_conf_decay, PLAYER_CONF_DECAY);
	AddParam( "ball_conf_decay", & M_ball_conf_decay, BALL_CONF_DECAY);

    AddParam( "min_appearance_poss", & M_min_appearance_poss, MIN_APPEARANCE_POSS);

	AddParam( "shoot_max_distance", & shoot_max_distance, MAX_SHOOT_DISTANCE );

    AddParam( "low_stamina_point_thr", & mLowStaminaPointThr, LOW_STAMINA_POINT_THR);
}

void PlayerParam::init(int argc, char **argv)
{
	ParseFromCmdLine(argc, argv);                       //首先分析命令行，因为有可能命令行里面改了配置文件的路径
	ParseFromConfigFile(M_player_conf_file.c_str());    //分析配置文件
	ParseFromCmdLine(argc, argv);                       //再次分析命令行，因为命令行有权更改配置文件里面的默认设置
	MaintainConsistency();

    std::ifstream file("data/eps0.1");

	memset(mSightChange , 0 , sizeof(mSightChange));
    if (!file)  {
        PRINT_ERROR("date/eps0.1 not exist");
        return;
    }

    int i = 0;
    while(!file.eof())
    {
        if (i >= SIGHTSIZE)
        {
         //    PRINT_ERROR("mSightChange not big enough!!!!!!!");
             break;
        }
        file >> mSightChange[i][0] >> mSightChange[i][1] >> mSightChange[i][2];
        i++;
     }
     file.close();




	 std::ifstream file1("data/eps0.01");
	 memset(mMarkChange ,0 , sizeof(mMarkChange));
	 if (!file1)
	 {
		 PRINT_ERROR("file data/eps0.01 not exist");
		 return;
	 }
	 i = 0;
	 while(!file1.eof())
	 {

		 if (i >= MARKSIZE)
		 {
		//	 PRINT_ERROR("mMarkChange not big enough!!!!!!!!");
			 break;
		 }
		 file1 >> mMarkChange[i][0] >> mMarkChange[i][1] >> mMarkChange[i][2];
		 i++;
	 }
	 file1.close();

}

bool PlayerParam::SaveParam()
{
	std::cout << "Saving Player Param To Config File " << M_player_conf_file.c_str() << "..." << std::endl;
	return SaveToConfigFile(M_player_conf_file.c_str());
}

void PlayerParam::AddPlayerType(int type, const char *line)
{
	Assert (type >= 0 && type < PlayerParam::instance().playerTypes());

	mHeteroPlayer[type].ParseFromServerMsg(line);
	mHeteroPlayer[type].MaintainConsistency();
}

void PlayerParam::MaintainConsistency()
{
	M_team_name_len = M_team_name.length();
    mMinStamina = ServerParam::instance().recoverDecStamina() + TiredBuffer();
}
