#include "ServerParam.h"
#include "Utilities.h"

const int ServerParam::DEFAULT_PORT_NUMBER = 6000;
const int ServerParam::COACH_PORT_NUMBER = 6001;
const int ServerParam::OLCOACH_PORT_NUMBER = 6002;
const char ServerParam::DEFAULT_HOST[] = "127.0.0.1";

const int ServerParam::SIMULATOR_STEP_INTERVAL_MSEC = 100;
const int ServerParam::UDP_RECV_STEP_INTERVAL_MSEC = 10;
const int ServerParam::UDP_SEND_STEP_INTERVAL_MSEC = 150;
const int ServerParam::SENSE_BODY_INTERVAL_MSEC = 100;
const int ServerParam::SEND_VISUALINFO_INTERVAL_MSEC = 100;

const int ServerParam::HALF_TIME = 300;
const int ServerParam::EXTRA_HALF_TIME = 300;
const int ServerParam::DROP_TIME = 200;

const double ServerParam::PITCH_LENGTH = 105.0;
const double ServerParam::PITCH_WIDTH = 68.0;
const double ServerParam::PITCH_MARGIN = 5.0;
const double ServerParam::CENTER_CIRCLE_R = 9.15;
const double ServerParam::PENALTY_AREA_LENGTH = 16.5;
const double ServerParam::PENALTY_AREA_WIDTH = 40.32;
const double ServerParam::GOAL_AREA_LENGTH = 5.5;
const double ServerParam::GOAL_AREA_WIDTH = 18.32;
const double ServerParam::GOAL_WIDTH = 14.02;
const double ServerParam::GOAL_DEPTH = 2.44;
const double ServerParam::PENALTY_SPOT_DIST = 11.0;
const double ServerParam::CORNER_ARC_R = 1.0;
const double ServerParam::KICK_OFF_CLEAR_DISTANCE = CENTER_CIRCLE_R;

const double ServerParam::CORNER_KICK_MARGIN = 1.0;

const int ServerParam::TEAM_SIZE = 11;

const double ServerParam::BALL_SIZE = 0.085;
const double ServerParam::BALL_DECAY = 0.94;
const double ServerParam::BALL_RAND = 0.05;
const double ServerParam::BALL_WEIGHT = 0.2;
const double ServerParam::BALL_T_VEL = 0.001;
const double ServerParam::BALL_SPEED_MAX = 3.0; // [12.0.0] 2.7 -> 3.0;
const double ServerParam::BALL_ACCEL_MAX = 2.7;

const double ServerParam::PLAYER_SIZE = 0.3;
const double ServerParam::PLAYER_WIDGET_SIZE = 1.0;
const double ServerParam::PLAYER_DECAY = 0.4;
const double ServerParam::PLAYER_RAND = 0.1;
const double ServerParam::PLAYER_WEIGHT = 60.0;
const double ServerParam::PLAYER_SPEED_MAX = 1.05; //1.2
// th 6.3.00
const double ServerParam::PLAYER_ACCEL_MAX = 1.0;
//
const double ServerParam::IMPARAM = 5.0;

const double ServerParam::STAMINA_MAX = 8000.0;
const double ServerParam::STAMINA_INC_MAX = 45.0;
const double ServerParam::RECOVERY_DEC_THR = 0.3;
const double ServerParam::RECOVERY_DEC = 0.002;
const double ServerParam::RECOVERY_MIN = 0.5;
const double ServerParam::EFFORT_DEC_THR = 0.3;
const double ServerParam::EFFORT_DEC = 0.005;
const double ServerParam::EFFORT_MIN = 0.6;
const double ServerParam::EFFORT_INC_THR = 0.6;
const double ServerParam::EFFORT_INC = 0.01;

const double ServerParam::KICK_RAND = 0.1; // [12.0.0] 0.0 -> 0.1
const double ServerParam::PRAND_FACTOR_L = 1.0;
const double ServerParam::PRAND_FACTOR_R = 1.0;
const double ServerParam::KICK_RAND_FACTOR_L = 1.0;
const double ServerParam::KICK_RAND_FACTOR_R = 1.0;


const double ServerParam::GOALIE_CATCHABLE_POSSIBILITY = 1.0;
const double ServerParam::GOALIE_CATCHABLE_AREA_LENGTH = 1.2; // [12.0.0] 2.0 -> 1.2
const double ServerParam::GOALIE_CATCHABLE_AREA_WIDTH = 1.0;
const int ServerParam::GOALIE_CATCH_BAN_CYCLE = 5;
const int ServerParam::GOALIE_MAX_MOVES = 2;

const double ServerParam::VISIBLE_ANGLE = 90.0;
const double ServerParam::VISIBLE_DISTANCE = 3.0;
const double ServerParam::AUDIO_CUT_OFF_DIST = 50.0;

const double ServerParam::DASHPOWERRATE = 0.006;
const double ServerParam::KICKPOWERRATE = 0.027;
const double ServerParam::MAXPOWER = 100.0;
const double ServerParam::MINPOWER = -100.0;

const double ServerParam::KICKABLE_MARGIN = 0.7;
const double ServerParam::CONTROL_RADIUS = 2.0;

const double ServerParam::DIST_QSTEP = 0.1;
const double ServerParam::LAND_QSTEP = 0.01;
const double ServerParam::DIR_QSTEP = 0.1;

const double ServerParam::MAXMOMENT = 180;
const double ServerParam::MINMOMENT = -180;

const double ServerParam::MAX_NECK_MOMENT = 180;
const double ServerParam::MIN_NECK_MOMENT = -180;

const double ServerParam::MAX_NECK_ANGLE = 90;
const double ServerParam::MIN_NECK_ANGLE = -90;

const int ServerParam::DEF_SAY_COACH_MSG_SIZE = 128;
const int ServerParam::DEF_SAY_COACH_CNT_MAX = 128;

const double ServerParam::WIND_DIR = 0.0;
const double ServerParam::WIND_FORCE = 0.0;
const double ServerParam::WIND_RAND = 0.0;
const double ServerParam::WIND_WEIGHT = 10000.0;

const double ServerParam::OFFSIDE_ACTIVE_AREA_SIZE = 2.5;
const double ServerParam::OFFSIDE_KICK_MARGIN = 9.15;

const int ServerParam::POINT_TO_BAN = 5;
const int ServerParam::POINT_TO_DURATION= 20;
const int ServerParam::SAY_MSG_SIZE = 10;
const int ServerParam::HEAR_MAX = 1;
const int ServerParam::HEAR_INC = 1;
const int ServerParam::HEAR_DECAY = 1;

const double ServerParam::TACKLE_DIST = 2.0;
const double ServerParam::TACKLE_BACK_DIST = 0.0; // [12.0.0] 0.5 -> 0.0
const double ServerParam::TACKLE_WIDTH = 1.25; // [12.0.0] 1.0 -> 1.25
const double ServerParam::TACKLE_EXPONENT = 6.0;
const int ServerParam::TACKLE_CYCLES = 10;
const double ServerParam::TACKLE_POWER_RATE = 0.027;

const int ServerParam::NR_NORMAL_HALFS = 2;
const int ServerParam::NR_EXTRA_HALFS = 2;
const bool ServerParam::PENALTY_SHOOT_OUTS = true;

const int    ServerParam::PEN_BEFORE_SETUP_WAIT = 10; // [13.2.0] 30 -> 10
const int    ServerParam::PEN_SETUP_WAIT = 70; // [13.2.0] 100 -> 70
const int    ServerParam::PEN_READY_WAIT = 10; // [13.2.0] 50 -> 10
const int    ServerParam::PEN_TAKEN_WAIT = 150; // [13.2.0] 200 -> 150
const int    ServerParam::PEN_NR_KICKS = 5;
const int    ServerParam::PEN_MAX_EXTRA_KICKS = 5; // [13.2.0] 10 -> 5
const double ServerParam::PEN_DIST_X = 42.5;
const bool   ServerParam::PEN_RANDOM_WINNER = false;
const double ServerParam::PEN_MAX_GOALIE_DIST_X = 14;
const bool   ServerParam::PEN_ALLOW_MULT_KICKS = true;
const bool   ServerParam::PEN_COACH_MOVES_PLAYERS = true;


const int ServerParam::FREEFORM_WAIT_PERIOD = 600;
const int ServerParam::FREEFORM_SEND_PERIOD = 20;

const bool ServerParam::FREE_KICK_FAULTS = true;
const bool ServerParam::BACK_PASSES = true;

const bool ServerParam::PROPER_GOAL_KICKS = false;
const double ServerParam::STOPPED_BALL_VEL = 0.01;
const int ServerParam::MAX_GOAL_KICKS = 3;

const int ServerParam::CLANG_DEL_WIN = 1;
const int ServerParam::CLANG_RULE_WIN = 1;

// 11.0.0
const double ServerParam::BALL_STUCK_AREA = 3.0;

// 12.0.0
const double ServerParam::MAX_TACKLE_POWER = 100.0;
const double ServerParam::MAX_BACK_TACKLE_POWER = 0.0; //50.0;
const double ServerParam::PLAYER_SPEED_MAX_MIN = 0.75;
const double ServerParam::EXTRA_STAMINA = 50.0;
const int ServerParam::SYNCH_SEE_OFFSET = 0; // [13.2.0] 30 -> 0;

const double ServerParam::UNUM_FAR_LENGTH = 20.0;
const double ServerParam::UNUM_TOO_FAR_LENGTH = 40.0;
const double ServerParam::TEAM_FAR_LENGTH = 40.0;
const double ServerParam::TEAM_TOO_FAR_LENGTH = 60.0;

// 13.0.0
const double ServerParam::STAMINA_CAPACITY = 130600.0; //13.2.2 : 148600.0; //127000.0;
const double ServerParam::MAX_DASH_ANGLE = +180.0;
const double ServerParam::MIN_DASH_ANGLE = -180.0;
const double ServerParam::DASH_ANGLE_STEP = 45.0; //13.2.2 : 90.0;
const double ServerParam::SIDE_DASH_RATE = 0.4; //13.2.2 : 0.25;
const double ServerParam::BACK_DASH_RATE = 0.6; //13.2.2 : 0.5;
const double ServerParam::MAX_DASH_POWER = +100.0;
const double ServerParam::MIN_DASH_POWER = -100.0;

// 14.0.0
const double ServerParam::TACKLE_RAND_FACTOR = 2.0;
const double ServerParam::FOUL_DETECT_PROBABILITY = 0.5;
const double ServerParam::FOUL_EXPONENT = 10.0;
const int ServerParam::FOUL_CYCLES = 5;

const char ServerParam::CONF_FILE[] = "./conf/server.conf";

ServerParam &ServerParam::instance()
{
    static ServerParam server_param;
    return server_param;
}

ServerParam::ServerParam()
{
    AddParams();
    MaintainConsistency();
}

void ServerParam::AddParams()
{
    AddParam( "goal_width", & M_goal_width, GOAL_WIDTH );
    AddParam( "inertia_moment", & M_inertia_moment, IMPARAM );
    AddParam( "player_size", & M_player_size, PLAYER_SIZE );
    AddParam( "player_decay", & M_player_decay, PLAYER_DECAY );
    AddParam( "player_rand", & M_player_rand, PLAYER_RAND );
    AddParam( "player_weight", & M_player_weight, PLAYER_WEIGHT );
    AddParam( "player_speed_max", & M_player_speed_max, PLAYER_SPEED_MAX );
	// th 6.3.00
    AddParam( "player_accel_max", & M_player_accel_max, PLAYER_ACCEL_MAX );
    //
    AddParam( "stamina_max", & M_stamina_max, STAMINA_MAX );
	AddParam("stamina_capacity" , & M_stamina_capacity ,STAMINA_CAPACITY);
    AddParam( "stamina_inc_max", & M_stamina_inc, STAMINA_INC_MAX );
    AddParam( "recover_init", & M_recover_init, 1.0 );
    AddParam( "recover_dec_thr", &M_recover_dec_thr, RECOVERY_DEC_THR );
    AddParam( "recover_min", & M_recover_min, RECOVERY_MIN );
    AddParam( "recover_dec", & M_recover_dec, RECOVERY_DEC );
    AddParam( "effort_init", & M_effort_init, 1.0 );
    AddParam( "effort_dec_thr", & M_effort_dec_thr, EFFORT_DEC_THR );
    AddParam( "effort_min", & M_effort_min, EFFORT_MIN );
    AddParam( "effort_dec", & M_effort_dec, EFFORT_DEC );
    AddParam( "effort_inc_thr", & M_effort_inc_thr, EFFORT_INC_THR );
    AddParam( "effort_inc", & M_effort_inc, EFFORT_INC );
	// pfr 8/14/00: for RC2000 evaluation
    AddParam( "kick_rand",           & M_kick_rand, KICK_RAND );
    AddParam( "team_actuator_noise", & M_team_actuator_noise, false );
    AddParam( "prand_factor_l",      & M_player_rand_factor_l, PRAND_FACTOR_L );
    AddParam( "prand_factor_r",      & M_player_rand_factor_r, PRAND_FACTOR_L );
    AddParam( "kick_rand_factor_l",  & M_kick_rand_factor_l, KICK_RAND_FACTOR_L );
    AddParam( "kick_rand_factor_r",  & M_kick_rand_factor_r, KICK_RAND_FACTOR_R );

    AddParam( "ball_size", & M_ball_size, BALL_SIZE );
    AddParam( "ball_decay", & M_ball_decay, BALL_DECAY );
    AddParam( "ball_rand", & M_ball_rand, BALL_RAND );
    AddParam( "ball_weight", & M_ball_weight, BALL_WEIGHT );
    AddParam( "ball_speed_max", & M_ball_speed_max, BALL_SPEED_MAX );
	// th 6.3.00
    AddParam( "ball_accel_max", & M_ball_accel_max, BALL_ACCEL_MAX );
    //
    AddParam( "dash_power_rate", & M_dash_power_rate, DASHPOWERRATE );
    AddParam( "kick_power_rate", & M_kick_power_rate, KICKPOWERRATE);
    AddParam( "kickable_margin", & M_kickable_margin, KICKABLE_MARGIN );
    AddParam( "control_radius", & M_control_radius, CONTROL_RADIUS );

    AddParam( "catch_probability", & M_catch_probability, GOALIE_CATCHABLE_POSSIBILITY );
    AddParam( "catchable_area_l", & M_catchable_area_l, GOALIE_CATCHABLE_AREA_LENGTH );
    AddParam( "catchable_area_w", & M_catchable_area_w, GOALIE_CATCHABLE_AREA_WIDTH );
    AddParam( "goalie_max_moves", & M_goalie_max_moves, GOALIE_MAX_MOVES );

    AddParam( "maxpower", & M_max_power, MAXPOWER );
    AddParam( "minpower", & M_min_power, MINPOWER );
    AddParam( "maxmoment", & M_max_moment, MAXMOMENT );
    AddParam( "minmoment", & M_min_moment, MINMOMENT );
    AddParam( "maxneckmoment", & M_max_neck_moment, MAX_NECK_MOMENT );
    AddParam( "minneckmoment", & M_min_neck_moment, MIN_NECK_MOMENT );
    AddParam( "maxneckang", & M_max_neck_angle, MAX_NECK_ANGLE );
    AddParam( "minneckang", & M_min_neck_angle, MIN_NECK_ANGLE );
    AddParam( "visible_angle", & M_visible_angle, VISIBLE_ANGLE );
    AddParam( "visible_distance", & M_visible_distance, VISIBLE_DISTANCE );
    AddParam( "audio_cut_dist", & M_audio_cut_dist, AUDIO_CUT_OFF_DIST );
    AddParam( "quantize_step", & M_quantize_step, DIST_QSTEP );
    AddParam( "quantize_step_l", & M_landmark_quantize_step, LAND_QSTEP );

    AddParam( "ckick_margin", & M_corner_kick_margin, CORNER_KICK_MARGIN );
    AddParam( "wind_dir", & M_wind_dir, WIND_DIR );
    AddParam( "wind_force", & M_wind_force, WIND_FORCE );
    AddParam( "wind_ang", & M_wind_angle, 0.0 );
    AddParam( "wind_rand", & M_wind_rand, WIND_RAND );
    AddParam( "wind_none", & M_wind_none, false );
    AddParam( "wind_random", & M_wind_random, false );
    AddParam( "half_time", & M_half_time, 300 );
    AddParam( "extra_half_time", & M_extra_half_time, 100 );
    AddParam( "drop_ball_time", & M_drop_ball_time, DROP_TIME );
    AddParam( "port", & M_port, DEFAULT_PORT_NUMBER );
    AddParam( "coach_port", & M_coach_port, COACH_PORT_NUMBER );
    AddParam( "olcoach_port", & M_olcoach_port, OLCOACH_PORT_NUMBER );
    AddParam( "host", & M_host, std::string(DEFAULT_HOST));
    AddParam( "say_coach_cnt_max", & M_say_cnt_max, DEF_SAY_COACH_CNT_MAX );
    AddParam( "say_coach_msg_size", & M_say_coach_msg_size, DEF_SAY_COACH_MSG_SIZE );
    AddParam( "simulator_step", & M_simulator_step, SIMULATOR_STEP_INTERVAL_MSEC );
    AddParam( "send_step", & M_send_step, UDP_SEND_STEP_INTERVAL_MSEC );
    AddParam( "recv_step", & M_recv_step, UDP_RECV_STEP_INTERVAL_MSEC );
    AddParam( "sense_body_step", & M_sense_body_step, SENSE_BODY_INTERVAL_MSEC );
    AddParam( "say_msg_size", & M_say_msg_size, SAY_MSG_SIZE );
    AddParam( "clang_win_size",     & M_clang_win_size, 300 );
    AddParam( "clang_define_win",   & M_clang_define_win, 1 );
    AddParam( "clang_meta_win",     & M_clang_meta_win, 1 );
    AddParam( "clang_advice_win",   & M_clang_advice_win, 1 );
    AddParam( "clang_info_win",     & M_clang_info_win, 1 );
    AddParam( "clang_del_win",      & M_clang_del_win, CLANG_DEL_WIN );
    AddParam( "clang_rule_win",     & M_clang_rule_win, CLANG_RULE_WIN );
    AddParam( "clang_mess_delay",   & M_clang_mess_delay, 50 );
    AddParam( "clang_mess_per_cycle", & M_clang_mess_per_cycle, 1 );
    AddParam( "hear_max", & M_hear_max, HEAR_MAX );
    AddParam( "hear_inc", & M_hear_inc, HEAR_INC );
    AddParam( "hear_decay", & M_hear_decay, HEAR_DECAY );
    AddParam( "catch_ban_cycle", & M_catch_ban_cycle, GOALIE_CATCH_BAN_CYCLE );
    AddParam( "send_vi_step", & M_send_vi_step, SEND_VISUALINFO_INTERVAL_MSEC );
    AddParam( "use_offside", & M_use_offside, true );
    AddParam( "offside_active_area_size", & M_offside_active_area_size, OFFSIDE_ACTIVE_AREA_SIZE );
    AddParam( "forbid_kick_off_offside", & M_forbid_kick_off_offside, true );
    AddParam( "verbose", & M_verbose, false );
    AddParam( "offside_kick_margin", & M_offside_kick_margin, OFFSIDE_KICK_MARGIN );
    AddParam( "slow_down_factor", & M_slow_down_factor, 1 );
    AddParam( "synch_mode", & M_synch_mode, false );
    AddParam( "synch_offset", & M_synch_offset, 60 );
    AddParam( "synch_micro_sleep", & M_synch_micro_sleep, 1 ); //pfr:SYNCH

    AddParam( "start_goal_l",      & M_start_goal_l, 0 );
    AddParam( "start_goal_r",      & M_start_goal_r, 0 );

    AddParam( "fullstate_l",       & M_fullstate_l, false );
    AddParam( "fullstate_r",       & M_fullstate_r, false );
    AddParam( "point_to_ban", & M_point_to_ban, POINT_TO_BAN );
    AddParam( "point_to_duration", & M_point_to_duration, POINT_TO_DURATION );

    AddParam( "tackle_dist", & M_tackle_dist, TACKLE_DIST );
    AddParam( "tackle_back_dist", & M_tackle_back_dist, TACKLE_BACK_DIST );
    AddParam( "tackle_width", & M_tackle_width, TACKLE_WIDTH );
    AddParam( "tackle_exponent", & M_tackle_exponent, TACKLE_EXPONENT );
    AddParam( "tackle_cycles", & M_tackle_cycles, TACKLE_CYCLES );
    AddParam( "tackle_power_rate", & M_tackle_power_rate, TACKLE_POWER_RATE );

    AddParam( "freeform_wait_period", & M_freeform_wait_period, FREEFORM_WAIT_PERIOD );
    AddParam( "freeform_send_period", & M_freeform_send_period, FREEFORM_SEND_PERIOD );

    AddParam( "free_kick_faults", & M_free_kick_faults, FREE_KICK_FAULTS );
    AddParam( "back_passes", & M_back_passes, BACK_PASSES );

    AddParam( "proper_goal_kicks", & M_proper_goal_kicks, PROPER_GOAL_KICKS );
    AddParam( "stopped_ball_vel", & M_stopped_ball_vel, STOPPED_BALL_VEL );
    AddParam( "max_goal_kicks", & M_max_goal_kicks, MAX_GOAL_KICKS );

    AddParam( "nr_normal_halfs", & M_nr_normal_halfs, NR_NORMAL_HALFS );
    AddParam( "nr_extra_halfs", & M_nr_extra_halfs, NR_EXTRA_HALFS );
    AddParam( "penalty_shoot_outs", & M_penalty_shoot_outs, PENALTY_SHOOT_OUTS );

    AddParam( "pen_before_setup_wait", & M_pen_before_setup_wait, PEN_BEFORE_SETUP_WAIT );
    AddParam( "pen_setup_wait",        & M_pen_setup_wait, PEN_SETUP_WAIT );
    AddParam( "pen_ready_wait",        & M_pen_ready_wait, PEN_READY_WAIT );
    AddParam( "pen_taken_wait",        & M_pen_taken_wait, PEN_TAKEN_WAIT );
    AddParam( "pen_nr_kicks",          & M_pen_nr_kicks, PEN_NR_KICKS );
    AddParam( "pen_max_extra_kicks",   & M_pen_max_extra_kicks, PEN_MAX_EXTRA_KICKS );
    AddParam( "pen_dist_x"         ,   & M_pen_dist_x         , PEN_DIST_X );
    AddParam( "pen_random_winner",     & M_pen_random_winner, PEN_RANDOM_WINNER );
    AddParam( "pen_max_goalie_dist_x", & M_pen_max_goalie_dist_x, PEN_MAX_GOALIE_DIST_X );
    AddParam( "pen_allow_mult_kicks",  & M_pen_allow_mult_kicks, PEN_ALLOW_MULT_KICKS );
    AddParam( "pen_coach_moves_players", & M_pen_coach_moves_players, PEN_COACH_MOVES_PLAYERS );

    AddParam( "ball_stuck_area", & M_ball_stuck_area, BALL_STUCK_AREA );

    AddParam( "max_tackle_power", & M_max_tackle_power, MAX_TACKLE_POWER );
    AddParam( "max_back_tackle_power", & M_max_back_tackle_power, MAX_BACK_TACKLE_POWER );
    AddParam( "player_speed_max_min", & M_player_speed_max_min, PLAYER_SPEED_MAX_MIN );
    AddParam( "extra_stamina", & M_extra_stamina, EXTRA_STAMINA );

    AddParam( "synch_see_offset", & M_synch_see_offset, SYNCH_SEE_OFFSET );

    AddParam( "max_dash_angle", & M_max_dash_angle, MAX_DASH_ANGLE);
    AddParam( "min_dash_angle", & M_min_dash_angle, MIN_DASH_ANGLE);
    AddParam( "dash_angle_step", & M_dash_angle_step, DASH_ANGLE_STEP);
    AddParam( "side_dash_rate", & M_side_dash_rate, SIDE_DASH_RATE);
    AddParam( "back_dash_rate", & M_back_dash_rate, BACK_DASH_RATE);
    AddParam( "max_dash_power", & M_max_dash_power, MAX_DASH_POWER);
    AddParam( "min_dash_power", & M_min_dash_power, MIN_DASH_POWER);

	// v14
	AddParam( "tackle_rand_factor", & M_tackle_rand_factor, TACKLE_RAND_FACTOR);
	AddParam( "foul_detect_probability", & M_foul_detect_probability, FOUL_DETECT_PROBABILITY);
	AddParam( "foul_exponent", & M_foul_exponent, FOUL_EXPONENT);
	AddParam( "foul_cycles", & M_foul_cycles, FOUL_CYCLES);

	// private
    AddParam( "server_conf", & M_server_conf_file, std::string(CONF_FILE));

    AddParam( "unum_far_length", & M_unum_far_length, UNUM_FAR_LENGTH);
    AddParam( "unum_too_far_length", & M_unum_too_far_length, UNUM_TOO_FAR_LENGTH);
    AddParam( "team_far_length", & M_team_far_length, TEAM_FAR_LENGTH);
    AddParam( "team_too_far_length", & M_team_too_far_length, TEAM_TOO_FAR_LENGTH);
}

void ServerParam::init(int argc, char **argv)
{
    //std::cout << "Parsing Server Param From Cmd Line 1st Time..." << std::endl;
    ParseFromCmdLine(argc, argv);                       //首先分析命令行，因为有可能命令行里面改了配置文件的路径
    //std::cout << "Parsing Server Param From Config File " << M_server_conf_file.c_str() << "..." << std::endl;
    ParseFromConfigFile(M_server_conf_file.c_str());    //分析配置文件
    //std::cout << "Parsing Server Param From Cmd Line 2nd Time..." << std::endl;
    ParseFromCmdLine(argc, argv);                       //再次分析命令行，因为命令行有权更改配置文件里面的默认设置
    MaintainConsistency();
}

bool ServerParam::SaveParam()
{
    std::cout << "Saving Server Param To Config File " << M_server_conf_file.c_str() << "..." << std::endl;
    return SaveToConfigFile(M_server_conf_file.c_str());
}

void ServerParam::MaintainConsistency()
{
	M_kickable_area = M_player_size + M_kickable_margin + M_ball_size;
	M_control_radius_width = M_control_radius - M_player_size;

    M_max_catchable_area    = Vector(M_catchable_area_l, 0.5 * M_catchable_area_w).Mod();
    M_one_minus_ball_decay  = 1.0 - M_ball_decay;
    M_log_ball_decay        = log(M_ball_decay);

    M_ball_decay_array[0]   = 1.0;
    for (int i = 1; i < 100; ++i) {
        M_ball_decay_array[i] = M_ball_decay_array[i - 1] * M_ball_decay;
    }

    // set the field information
    M_left_line     = Line(Vector(-PITCH_LENGTH / 2.0, 0.0), Vector(-PITCH_LENGTH / 2.0, 1.0));
    M_right_line    = Line(Vector(PITCH_LENGTH / 2.0, 0.0), Vector(PITCH_LENGTH / 2.0, 1.0));
    M_top_line      = Line(Vector(0.0, -PITCH_WIDTH / 2.0), Vector(1.0, -PITCH_WIDTH / 2.0));
    M_bottom_line      = Line(Vector(0.0, PITCH_WIDTH / 2.0), Vector(1.0, PITCH_WIDTH / 2.0));

    M_pitch_rectanglar  = Rectangular(-PITCH_LENGTH / 2.0, PITCH_LENGTH / 2.0,
        -PITCH_WIDTH / 2.0, PITCH_WIDTH / 2.0);
    M_active_rectanglar = Rectangular(-PITCH_LENGTH / 2.0 + 1.0, PITCH_LENGTH / 2.0 - 1.0,
        -PITCH_WIDTH / 2.0 + 1.0, PITCH_WIDTH / 2.0 - 1.0);
    M_our_penalty_area  = Rectangular(-PITCH_LENGTH / 2.0, -PITCH_LENGTH / 2.0 + PENALTY_AREA_LENGTH,
        -PENALTY_AREA_WIDTH / 2.0, PENALTY_AREA_WIDTH / 2.0);
    M_opp_penalty_area  = Rectangular(PITCH_LENGTH / 2.0 - PENALTY_AREA_LENGTH, PITCH_LENGTH / 2.0,
        -PENALTY_AREA_WIDTH / 2.0, PENALTY_AREA_WIDTH / 2.0);
    M_our_goal_area     = Rectangular(-PITCH_LENGTH / 2.0, -PITCH_LENGTH / 2.0 + GOAL_AREA_LENGTH,
        -GOAL_AREA_WIDTH / 2.0, GOAL_AREA_WIDTH / 2.0);
    M_opp_goal_area     = Rectangular(PITCH_LENGTH / 2.0 - GOAL_AREA_LENGTH, PITCH_LENGTH / 2.0,
        -GOAL_AREA_WIDTH / 2.0, GOAL_AREA_WIDTH / 2.0);

    M_our_left_goal_kick_point  = M_our_goal_area.TopRightCorner();
    M_our_right_goal_kick_point = M_our_goal_area.BottomRightCorner();
    M_opp_left_goal_kick_point  = M_opp_goal_area.TopLeftCorner();
    M_opp_right_goal_kick_point = M_opp_goal_area.BottomLeftCorner();

    M_our_left_goal_post.SetValue(- PITCH_LENGTH / 2.0, - M_goal_width / 2.0);
    M_our_right_goal_post.SetValue(- PITCH_LENGTH / 2.0, M_goal_width / 2.0);
    M_opp_left_goal_post.SetValue(PITCH_LENGTH / 2.0, - M_goal_width / 2.0);
    M_opp_right_goal_post.SetValue(PITCH_LENGTH / 2.0, M_goal_width / 2.0);

    M_pitch_top_left_point      = M_pitch_rectanglar.TopLeftCorner();
    M_pitch_bottom_left_point   = M_pitch_rectanglar.BottomLeftCorner();
    M_pitch_top_right_point     = M_pitch_rectanglar.TopRightCorner();
    M_pitch_bottom_right_point  = M_pitch_rectanglar.BottomRightCorner();

    M_our_goal = Vector(-PITCH_LENGTH / 2.0, 0.0);
    M_opp_goal = Vector(PITCH_LENGTH / 2.0, 0.0);

    M_ball_run_dist_with_max_speed = M_ball_speed_max / M_one_minus_ball_decay;
    M_max_tackle_dist = Sqrt(M_tackle_dist * M_tackle_dist + M_tackle_width * M_tackle_width);

    M_effort_dec_stamina = M_effort_dec_thr * M_stamina_max;
    M_effort_inc_stamina = M_effort_inc_thr * M_stamina_max;
    M_recover_dec_stamina = M_recover_dec_thr * M_stamina_max;

	M_half_time = M_half_time * 1000 / M_simulator_step;
	M_extra_half_time = M_extra_half_time * 1000 / M_simulator_step;

	M_max_tackle_area = 0.0;
	int exponent = Max(M_foul_exponent, M_tackle_exponent);

	for (double x = 0.0; x <= M_tackle_dist; x += 0.01) {
		for (double y = 0.0; y <= M_tackle_width; y += 0.01) {
			double prob = 1.0 -
					MinMax(
						0.0,
						pow(x / M_tackle_dist, exponent) + pow(y / M_tackle_width, exponent),
						1.0
					);

			if (prob > FLOAT_EPS) {
				M_max_tackle_area = Max(M_max_tackle_area, Sqrt(Sqr(x) + Sqr(y)));
			}
		}
	}
}
