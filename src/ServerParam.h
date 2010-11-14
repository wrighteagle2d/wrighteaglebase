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

#ifndef SERVERPARAM_H_
#define SERVERPARAM_H_

#include "ParamEngine.h"
#include "Geometry.h"

class ServerParam: public ParamEngine {
    ServerParam(); // private
    ServerParam( const ServerParam & ); // not used
    ServerParam & operator=( const ServerParam & ); // not used

public:
	~ServerParam(){};
	static ServerParam &instance();
	void init(int argc, char **argv);

	//default values
	static const int DEFAULT_PORT_NUMBER;
	static const int COACH_PORT_NUMBER;
	static const int OLCOACH_PORT_NUMBER;
	static const char DEFAULT_HOST[];

	static const int SIMULATOR_STEP_INTERVAL_MSEC; /* milli-sec */
	static const int UDP_RECV_STEP_INTERVAL_MSEC; /* milli-sec */
	static const int UDP_SEND_STEP_INTERVAL_MSEC; /* milli-sec */
	static const int SENSE_BODY_INTERVAL_MSEC; /* milli-sec */
	static const int SEND_VISUALINFO_INTERVAL_MSEC; /* milli-sec */

	static const int HALF_TIME;
	static const int EXTRA_HALF_TIME;
	static const int DROP_TIME;

public:
	static const double PITCH_LENGTH;
	static const double PITCH_WIDTH;
	static const double PITCH_MARGIN;
	static const double CENTER_CIRCLE_R;
	static const double PENALTY_AREA_LENGTH;
	static const double PENALTY_AREA_WIDTH;
	static const double GOAL_AREA_LENGTH;
	static const double GOAL_AREA_WIDTH;
	static const double GOAL_WIDTH;
	static const double GOAL_DEPTH;
	static const double PENALTY_SPOT_DIST;
	static const double CORNER_ARC_R;
	static const double KICK_OFF_CLEAR_DISTANCE;

	static const double CORNER_KICK_MARGIN;

	static const int TEAM_SIZE;

private:
	static const double BALL_SIZE;
	static const double BALL_DECAY;
	static const double BALL_RAND;
	static const double BALL_WEIGHT;
	static const double BALL_T_VEL;
	static const double BALL_SPEED_MAX;
	// th 6.3.00
	static const double BALL_ACCEL_MAX;

	static const double PLAYER_SIZE;
	static const double	PLAYER_WIDGET_SIZE;
	static const double PLAYER_DECAY;
	static const double PLAYER_RAND;
	static const double PLAYER_WEIGHT;
	static const double PLAYER_SPEED_MAX;
	static const double PLAYER_SPEED_MAX_MIN; // [12.0.0]
	                                              // th 6.3.00
	static const double PLAYER_ACCEL_MAX;
	//
	static const double IMPARAM; /* Inertia-Moment Parameter */

	static const double	STAMINA_MAX;
	static const double EXTRA_STAMINA;
	static const double	STAMINA_INC_MAX;
	static const double RECOVERY_DEC_THR;
	static const double RECOVERY_DEC;
	static const double RECOVERY_MIN;
	static const double EFFORT_DEC_THR;
	static const double EFFORT_DEC;
	static const double EFFORT_MIN;
	static const double EFFORT_INC_THR;
	static const double EFFORT_INC;

	static const double KICK_RAND;
	static const double PRAND_FACTOR_L;
	static const double PRAND_FACTOR_R;
	static const double KICK_RAND_FACTOR_L;
	static const double KICK_RAND_FACTOR_R;

	static const double GOALIE_CATCHABLE_POSSIBILITY;
	static const double GOALIE_CATCHABLE_AREA_LENGTH;
	static const double GOALIE_CATCHABLE_AREA_WIDTH;
	static const int GOALIE_CATCH_BAN_CYCLE;
	static const int GOALIE_MAX_MOVES;

	static const double VISIBLE_ANGLE;
	static const double VISIBLE_DISTANCE;
	static const double AUDIO_CUT_OFF_DIST;

	static const double DASHPOWERRATE;
	static const double KICKPOWERRATE;
	static const double MAXPOWER;
	static const double MINPOWER;

	static const double KICKABLE_MARGIN;
	static const double CONTROL_RADIUS;

	static const double DIST_QSTEP;
	static const double LAND_QSTEP;
	static const double DIR_QSTEP;

	static const double MAXMOMENT;
	static const double MINMOMENT;

	static const double MAX_NECK_MOMENT;
	static const double MIN_NECK_MOMENT;

	static const double MAX_NECK_ANGLE;
	static const double MIN_NECK_ANGLE;

	static const int DEF_SAY_COACH_MSG_SIZE;
	static const int DEF_SAY_COACH_CNT_MAX;

	static const double	WIND_DIR;
	static const double	WIND_FORCE;
	static const double	WIND_RAND;
	static const double WIND_WEIGHT;

	static const double OFFSIDE_ACTIVE_AREA_SIZE;
	static const double OFFSIDE_KICK_MARGIN;

	static const char CONF_FILE[];

	static const int POINT_TO_BAN;
	static const int POINT_TO_DURATION;

	static const int SAY_MSG_SIZE;
	static const int HEAR_MAX;
	static const int HEAR_INC;
	static const int HEAR_DECAY;

	static const double TACKLE_DIST;
	static const double TACKLE_BACK_DIST;
	static const double TACKLE_WIDTH;
	static const double TACKLE_EXPONENT;
	static const int TACKLE_CYCLES;
	static const double TACKLE_POWER_RATE;
	static const double MAX_TACKLE_POWER;
	static const double MAX_BACK_TACKLE_POWER;
	static const double TACKLE_RAND_FACTOR;//14.0.0

	static const int NR_NORMAL_HALFS;
	static const int NR_EXTRA_HALFS;
	static const bool PENALTY_SHOOT_OUTS;

	static const int    PEN_BEFORE_SETUP_WAIT;
	static const int    PEN_SETUP_WAIT;
	static const int    PEN_READY_WAIT;
	static const int    PEN_TAKEN_WAIT;
	static const int    PEN_NR_KICKS;
	static const int    PEN_MAX_EXTRA_KICKS;
	static const bool   PEN_RANDOM_WINNER;
	static const double PEN_DIST_X;
	static const double PEN_MAX_GOALIE_DIST_X;
	static const bool   PEN_ALLOW_MULT_KICKS;
	static const bool   PEN_COACH_MOVES_PLAYERS;

	static const double FOUL_DETECT_PROBABILITY;//14.0.0
	static const double FOUL_EXPONENT;//14.0.0
	static const int FOUL_CYCLES;//14.0.0

	static const int FREEFORM_WAIT_PERIOD;
	static const int FREEFORM_SEND_PERIOD;

	static const bool FREE_KICK_FAULTS;
	static const bool BACK_PASSES;

	static const bool PROPER_GOAL_KICKS;
	static const double STOPPED_BALL_VEL;
	static const int MAX_GOAL_KICKS;

	static const int CLANG_DEL_WIN;
	static const int CLANG_RULE_WIN;

	static const double BALL_STUCK_AREA;

    static const int SYNCH_SEE_OFFSET;

    static const double STAMINA_CAPACITY;
    static const double MAX_DASH_ANGLE;
    static const double MIN_DASH_ANGLE;
    static const double DASH_ANGLE_STEP;
    static const double SIDE_DASH_RATE;
    static const double BACK_DASH_RATE;
    static const double MAX_DASH_POWER;
    static const double MIN_DASH_POWER;

	//视觉距离
	static const double UNUM_FAR_LENGTH;
	static const double UNUM_TOO_FAR_LENGTH;
	static const double TEAM_FAR_LENGTH;
	static const double TEAM_TOO_FAR_LENGTH;

	double M_goal_width; /* goal width */
	double M_inertia_moment; /* intertia moment for turn */
	double M_player_size; /* player size */
	double M_player_decay; /* player decay */
	double M_player_rand; /* player rand */
	double M_player_weight; /* player weight */
	double M_player_speed_max; /* player speed max */
	// th 6.3.00
	double M_player_accel_max; /* player acceleration max */
	//
	double M_stamina_max; /* player stamina max */
	double M_stamina_inc; /* player stamina inc */
	double M_recover_init; /* player recovery init */
	double M_recover_dec_thr; /* player recovery decriment threshold */
	double M_recover_min; /* player recovery min */
	double M_recover_dec; /* player recovery decriment */
	double M_effort_init; /* player dash effort init */
	double M_effort_dec_thr; /* player dash effort decriment threshold */
	double M_effort_min; /* player dash effrot min */
	double M_effort_dec; /* player dash effort decriment */
	double M_effort_inc_thr; /* player dash effort incriment threshold */
	double M_effort_inc; /* player dash effort incriment */
	// pfr 8/14/00: for RC2000 evaluation
	double M_kick_rand; /* noise added directly to kicks */
	bool M_team_actuator_noise; /* flag whether to use team specific actuator noise */
	double M_player_rand_factor_l; /* factor to multiple prand for left team */
	double M_player_rand_factor_r; /* factor to multiple prand for right team */
	double M_kick_rand_factor_l; /* factor to multiple kick_rand for left team */
	double M_kick_rand_factor_r; /* factor to multiple kick_rand for right team */

	double M_ball_size; /* ball size */
	double M_ball_decay; /* ball decay */
	double M_ball_rand; /* ball rand */
	double M_ball_weight; /* ball weight */
	double M_ball_speed_max; /* ball speed max */
	// th 6.3.00
	double M_ball_accel_max; /* ball acceleration max */
	//
	double M_dash_power_rate; /* dash power rate */
	double M_kick_power_rate; /* kick power rate */
	double M_kickable_margin; /* kickable margin */
	double M_control_radius; /* control radius */
	double M_control_radius_width; /* (control radius) - (plyaer size) */

	double M_max_power; /* max power */
	double M_min_power; /* min power */
	double M_max_moment; /* max moment */
	double M_min_moment; /* min moment */
	double M_max_neck_moment; /* max neck moment */
	double M_min_neck_moment; /* min neck moment */
	double M_max_neck_angle; /* max neck angle */
	double M_min_neck_angle; /* min neck angle */
	double M_visible_angle; /* visible angle */
	double M_visible_distance; /* visible distance */

	double M_wind_dir; /* wind direction */
	double M_wind_force; /* wind force */
	double M_wind_angle; /* wind angle for rand */
	double M_wind_rand; /* wind force for force */
	bool M_wind_none; /* wind factor is none */
	bool M_wind_random; /* wind factor is random */

	double M_kickable_area; /* kickable_area */

	double M_catchable_area_l; /* goalie catchable area length */
	double M_catchable_area_w; /* goalie catchable area width */
	double M_catch_probability; /* goalie catchable possibility */
	int M_goalie_max_moves; /* goalie max moves after a catch */
	double M_corner_kick_margin; /* corner kick margin */
	double M_offside_active_area_size; /* offside active area size */
	int M_port; /* port number */
	std::string M_host;
	int M_coach_port; /* coach port number */
	int M_olcoach_port; /* online coach port number */
	int M_say_cnt_max; /* max count of coach SAY (freeform) */
	int M_say_coach_msg_size; /* max length of coach SAY (freeform) */
	int M_clang_win_size; /* coach language: time window size */
	int M_clang_define_win; /* coach language: define messages per window */
	int M_clang_meta_win; /* coach language: meta messages per window */
	int M_clang_advice_win; /* coach language: advice messages per window */
	int M_clang_info_win; /* coach language: info messages per window */
	int M_clang_mess_delay; /* coach language: delay between receive and send */
	int M_clang_mess_per_cycle; /* coach language: number of messages to flush per cycle */

	int M_raw_half_time; /* half time */
	int M_half_time; /* half time */

	int M_drop_ball_time; /* cycles for dropping
	                             the ball after a free kick,
	                             corner kick message and
	                             noone kicking the ball.
	                             0 means don't drop
	                             automatically  */
	int M_nr_normal_halfs; /* nr of normal halfs: default 2)      */
	int M_nr_extra_halfs; /* nr of extra halfs: -1 is infinite)  */
	bool M_penalty_shoot_outs;/* penalty shoot outs after extra halfs*/

	int M_pen_before_setup_wait;/* cycles waited before penalty setup*/
	int M_pen_setup_wait; /* cycles waited to setup penalty    */
	int M_pen_ready_wait; /* cycles waited to take penalty     */
	int M_pen_taken_wait; /* cycles waited to finish penalty   */
	int M_pen_nr_kicks; /* number of penalty kicks           */
	int M_pen_max_extra_kicks; /* max. nr of extra penalty kicks    */
	double M_pen_dist_x; /* distance from goal to place ball  */
	bool M_pen_random_winner; /* random winner when draw?          */
	bool M_pen_allow_mult_kicks; /* can attacker kick mult. times */
	double M_pen_max_goalie_dist_x; /*max distance from goal for goalie*/
	bool M_pen_coach_moves_players; /*coach moves players when positioned wrongly */

	int M_simulator_step; /* simulator step interval msec */
	int M_sense_body_step; /* sense_body interval step msec */
	int M_send_vi_step; /* online coach's look interval step */
	int M_send_step; /* udp send step interval msec for player's default view mode*/
	int M_recv_step; /* udp recv step interval msec */
	int M_catch_ban_cycle; /* goalie catch ban cycle */
	int M_slow_down_factor; /* factor to slow down simulator and send intervals */
	bool M_use_offside; /* flag for using off side rule */
	bool M_forbid_kick_off_offside; /* flag for permit kick off offside */
	double M_offside_kick_margin; /* offside kick margin */
	double M_audio_cut_dist; /* audio cut off distance */
	double M_quantize_step; /* quantize step of distance */
	double M_landmark_quantize_step; /* quantize step of distance for landmark */
	bool M_verbose; /* flag for verbose mode */

	bool M_synch_mode; /* pfr:SYNCH whether to run in synchronized mode */
	int M_synch_offset; /* pfr:SYNCH the offset from the start of the cycle to tell players to run */
	int M_synch_micro_sleep; /* pfr:SYNCH the # of microseconds to sleep while waiting for players */

	int M_start_goal_l; /* The starting score of the left team */
	int M_start_goal_r; /* The starting score of the right team */

	bool M_fullstate_l; /* flag to send fullstate messages to left team; supresses visual info */
	bool M_fullstate_r; /* flag to send fullstate messages to right team; supresses visual info */

	int M_point_to_ban;
	int M_point_to_duration;

	int M_say_msg_size;	/* string size of say message */
	int M_hear_max; /* player hear_capacity_max */
	int M_hear_inc; /* player hear_capacity_inc */
	int M_hear_decay; /* player hear_capacity_decay */

	double M_tackle_dist;
	double M_max_tackle_area;
	double M_tackle_back_dist;
	double M_tackle_width;
	double M_tackle_exponent;
	int M_tackle_cycles;
	double M_tackle_power_rate;

	int M_freeform_wait_period;
	int M_freeform_send_period;

	bool M_free_kick_faults;
	bool M_back_passes;

	bool M_proper_goal_kicks;
	double M_stopped_ball_vel;
	int M_max_goal_kicks;

	int M_clang_del_win;
	int M_clang_rule_win;

	// 11.0.0
	double M_ball_stuck_area; /* threshold distance checked by BallStuckRef */

	// 12.0.0
	double M_max_tackle_power;
	double M_max_back_tackle_power;
	double M_player_speed_max_min; // minumum value of player speed max
	double M_extra_stamina;
	int M_synch_see_offset; //!< synch see offset

    // 12.1.3
    int M_raw_extra_half_time;
    int M_extra_half_time;

    // 13.0.0
    double M_stamina_capacity;
    double M_max_dash_angle;
    double M_min_dash_angle;
    double M_dash_angle_step;
    double M_side_dash_rate;
    double M_back_dash_rate;
    double M_max_dash_power;
    double M_min_dash_power;

	// 14.0.0
	double M_tackle_rand_factor;
	double M_foul_detect_probability;
	double M_foul_exponent;
	int M_foul_cycles;

    std::string M_server_conf_file;

    double M_unum_far_length;
    double M_unum_too_far_length;
    double M_team_far_length;
    double M_team_too_far_length;

    double M_effort_dec_stamina;
    double M_recover_dec_stamina;
    double M_effort_inc_stamina;

private:
	void AddParams();
public:
    bool SaveParam();

    // access methods

    const double & effortDecStamina() const { return M_effort_dec_stamina; }
    const double & recoverDecStamina() const { return M_recover_dec_stamina; }
    const double & effortIncStamina() const { return M_effort_inc_stamina; }

    const double & unumFarLength() const { return M_unum_far_length; }
    const double & unumTooFarLength() const { return M_unum_too_far_length; }
    const double & teamFarLength() const { return M_unum_far_length; }
    const double & teamTooFarLength() const { return M_unum_too_far_length; }
    const double & goalWidth() const { return M_goal_width; }
    const double & inertiaMoment() const { return M_inertia_moment; }
    const double & playerSize() const { return M_player_size; }
    const double & playerDecay() const { return M_player_decay; }
    const double & playerRand() const { return M_player_rand; }
    const double & playerWeight() const { return M_player_weight; }
    const double & playerSpeedMax() const { return M_player_speed_max; }
    const double & playerAccelMax() const { return M_player_accel_max; }
    const double & staminaMax() const { return M_stamina_max; }
    const double & staminaInc() const { return M_stamina_inc; }
    const double & recoverInit() const { return M_recover_init; }
    const double & recoverDecThr() const { return M_recover_dec_thr; }
    const double & recoverMin() const { return M_recover_min; }
    const double & recoverDec() const { return M_recover_dec; }
    const double & effortInit() const { return M_effort_init; }
    const double & effortDecThr() const { return  M_effort_dec_thr; }
    const double & effortMin() const { return M_effort_min; }
    const double & effortDec() const { return M_effort_dec; }
    const double & effortIncThr() const { return  M_effort_inc_thr; }
    const double & effortInc() const { return M_effort_inc; }
    const double & kickRand() const { return M_kick_rand; }
    bool teamActuatorNoise() const { return M_team_actuator_noise; }
    const double & playerRandFactorLeft() const { return M_player_rand_factor_l; }
    const double & playerRandFactorRight() const { return M_player_rand_factor_r; }
    const double & kickRandFactorLeft() const { return M_kick_rand_factor_l; }
    const double & kickRandFactorRight() const { return M_kick_rand_factor_r; }

    const double & ballSize() const { return M_ball_size; }
    const double & ballDecay() const { return M_ball_decay; }
    const double & ballRand() const { return M_ball_rand; }
    const double & ballWeight() const { return M_ball_weight; }
    const double & ballSpeedMax() const { return M_ball_speed_max; }
    const double & ballAccelMax() const { return M_ball_accel_max; }
    const double & dashPowerRate() const { return M_dash_power_rate; }
    const double & kickPowerRate() const { return M_kick_power_rate; }
    const double & kickableMargin() const { return M_kickable_margin; }
    const double & controlRadius() const { return M_control_radius; }
    const double & controlRadiusWidth() const { return M_control_radius_width; }

    const double & maxPower() const { return M_max_power; }
    const double & minPower() const { return M_min_power; }
    const double & maxMoment() const { return M_max_moment; }
    const double & minMoment() const { return M_min_moment; }
    const double & maxNeckMoment() const { return M_max_neck_moment; }
    const double & minNeckMoment() const { return M_min_neck_moment; }
    const double & maxNeckAngle() const { return M_max_neck_angle; }
    const double & minNeckAngle() const { return M_min_neck_angle; }
    const double & visibleAngleDegree() const { return M_visible_angle; }
    const double & visibleDistance() const{ return M_visible_distance; }

    const double & windDir() const { return M_wind_dir; }
    const double & windForce() const { return M_wind_force; }
    const double & windAngle() const { return M_wind_angle; }
    const double & windRand() const { return M_wind_rand; }
    const double & windWeight() const { return WIND_WEIGHT; }
    bool windNone() const { return M_wind_none; }
    bool windRandom() const { return M_wind_random; }

    const double & kickableArea() const { return M_kickable_area; }

    const double & catchAreaLength() const { return M_catchable_area_l; }
    const double & catchAreaWidth() const { return M_catchable_area_w; }
    const double & catchProb() const { return M_catch_probability; }
    int goalieMaxMoves() const { return M_goalie_max_moves; }

    const double & cornerKickMargin() const { return M_corner_kick_margin; }
    const double & offsideActiveArea() const { return M_offside_active_area_size; }

    std::string host() const { return M_host; }
    int playerPort() const { return M_port; }
    const std::string & serverHost() const { return M_host; }
    int offlineCoachPort() const { return M_coach_port; }
    int onlineCoachPort() const { return M_olcoach_port; }

    unsigned int freeformCountMax() const { return (unsigned int)M_say_cnt_max; }
    unsigned int freeformMsgSize() const { return (unsigned int)M_say_coach_msg_size; }

    int clangWinSize() const { return M_clang_win_size; }
    int clangDefineWin() const { return M_clang_define_win; }
    int clangMetaWin() const { return M_clang_meta_win; }
    int clangAdviceWin() const { return M_clang_advice_win; }
    int clangInfoWin() const { return M_clang_info_win; }
    int clangMessDelay() const { return M_clang_mess_delay; }
    int clangMessPerCycle() const{ return M_clang_mess_per_cycle; }

    int halfTime() const { return M_half_time; }
    int extraHalfTime() const { return M_extra_half_time; }
    int dropTime() const { return M_drop_ball_time; }
    int nrNormalHalfs() const { return M_nr_normal_halfs; }
    int nrExtraHalfs() const { return M_nr_extra_halfs; }
    bool penaltyShootOuts() const { return M_penalty_shoot_outs; }

    int penBeforeSetupWait() const { return M_pen_before_setup_wait; }
    int penSetupWait() const { return M_pen_setup_wait; }
    int penReadyWait() const { return M_pen_ready_wait; }
    int penTakenWait() const { return M_pen_taken_wait; }
    int penNrKicks() const { return M_pen_nr_kicks; }
    int penMaxExtraKicks() const { return M_pen_max_extra_kicks; }
    const double & penDistX() const { return M_pen_dist_x; }
    bool penRandomWinner() const { return M_pen_random_winner; }
    bool penAllowMultiKicks() const { return M_pen_allow_mult_kicks; }
    const double & penMaxGoalieDistX() const { return M_pen_max_goalie_dist_x; }
    bool penCoachMovesPlayers() const { return M_pen_coach_moves_players; }

    int simStep() const { return M_simulator_step; }
    int senseBodyStep() const { return M_sense_body_step; }
    int coachVisualStep() const { return M_send_vi_step; }
    int sendStep() const { return M_send_step; }
    int recvStep() const { return M_recv_step; }

    int catchBanCycle() const { return M_catch_ban_cycle; }
    int slowDownFactor() const { return M_slow_down_factor; }
    bool useOffside() const { return M_use_offside; }
    bool kickOffOffside() const { return M_forbid_kick_off_offside; }
    const double & offsideKickMargin() const { return M_offside_kick_margin; }

    const double & audioCutDist() const { return M_audio_cut_dist; }

    const double & quantizeStep() const { return M_quantize_step; }
    const double & landmarkQuantizeStep() const { return M_landmark_quantize_step; }

    bool verboseMode() const { return M_verbose; }

    bool synchMode() const { return M_synch_mode; }
    int synchOffset() const { return M_synch_offset; }
    unsigned int synchMicroSleep() const { return static_cast< unsigned int >( M_synch_micro_sleep ); }

    int startGoalLeft() const { return M_start_goal_l; }
    int startGoalRight() const { return M_start_goal_r; }

    bool fullstateLeft() const { return M_fullstate_l; }
    bool fullstateRight() const { return M_fullstate_r; }

    unsigned int pointToBan() const { return static_cast< unsigned int >( M_point_to_ban ); }
    unsigned int pointToDuration() const { return static_cast< unsigned int >( M_point_to_duration ); }

    unsigned int sayMsgSize() const { return static_cast< unsigned int >( M_say_msg_size ); }
    unsigned int hearMax() const { return static_cast< unsigned int >( M_hear_max ); }
    unsigned int hearInc() const { return static_cast< unsigned int >( M_hear_inc ); }
    unsigned int hearDecay() const { return static_cast< unsigned int >( M_hear_decay ); }

    const double & tackleDist() const { return M_tackle_dist; }
    const double & maxTackleArea() const { return M_max_tackle_area; } //最大的可铲区域半径
    const double & tackleBackDist() const { return M_tackle_back_dist; }
    const double & tackleWidth() const { return M_tackle_width; }
    const double & tackleExponent() const { return M_tackle_exponent; }
    int tackleCycles() const { return M_tackle_cycles; }
    const double & tacklePowerRate() const { return M_tackle_power_rate; }

    unsigned int freeformWaitPeriod() const { return static_cast< unsigned int >( M_freeform_wait_period ); }
    unsigned int freeformSendPeriod() const { return static_cast< unsigned int >( M_freeform_send_period ); }

    bool freeKickFaults() const { return M_free_kick_faults; }
    bool backPasses() const { return M_back_passes; }
    bool properGoalKicks() const { return M_proper_goal_kicks; }
    const double & stoppedBallVel() const { return M_stopped_ball_vel; }
    int maxGoalKicks() const { return M_max_goal_kicks; }

    int clangDelWin() const { return M_clang_del_win; }
    int clangRuleWin() const { return M_clang_rule_win; }

    double goalPostRadius() const { return 0.06; }

    // v.11
    const double & ballStuckArea() const { return M_ball_stuck_area; }

    // v.12
    const double & maxTacklePower() const { return M_max_tackle_power; }
    const double & maxBackTacklePower() const { return M_max_back_tackle_power; }
    const double & playerSpeedMaxMin() const { return M_player_speed_max_min; }
    const double & extraStamina() const { return M_extra_stamina; }
    int synchSeeOffset() const { return M_synch_see_offset; }

    // v13
    const double & staminaCapacity() const { return M_stamina_capacity; }
    const double & maxDashAngle() const { return M_max_dash_angle; }
    const double & minDashAngle() const { return M_min_dash_angle; }
    const double & dashAngleStep() const { return M_dash_angle_step; }
    const double & sideDashRate() const { return M_side_dash_rate; }
    const double & backDashRate() const { return M_back_dash_rate; }
    const double & maxDashPower() const { return M_max_dash_power; }
    const double & minDashPower() const { return M_min_dash_power; }

	// v14
	const double & tackleRandFactor() const { return M_tackle_rand_factor; }
	const double & foulDetectProbability() const { return M_foul_detect_probability; }
	const double & foulExponent() const { return M_foul_exponent; }
	const int & foulCycles() const { return M_foul_cycles; }

    /**
     *  一些有依赖关系的变量的初始化和赋值
     */
    void MaintainConsistency();

//==============================================================================
public:
    const double & maxCatchableArea() const         { return M_max_catchable_area; }
    const double & oneMinusBallDecay() const        { return M_one_minus_ball_decay; }
    const double & logBallDecay() const             { return M_log_ball_decay; }

    const Line & leftLine() const                   { return M_left_line; }
    const Line & rightLine() const                  { return M_right_line; }
    const Line & topLine() const                    { return M_top_line; }
    const Line & bottomLine() const                 { return M_bottom_line; }

    const Rectangular & pitchRectanglar() const     { return M_pitch_rectanglar; }
    const Rectangular & activeRectanglar() const    { return M_active_rectanglar; }
    const Rectangular & ourPenaltyArea() const      { return M_our_penalty_area; }
    const Rectangular & oppPenaltyArea() const      { return M_opp_penalty_area; }
    const Rectangular & ourGoalArea() const         { return M_our_goal_area; }
    const Rectangular & oppGoalArea() const         { return M_opp_goal_area; }

    /** 以下左右定义是基于自己在左边上场时的方向 */
    const Vector & ourLeftGoalKickPoint() const     { return M_our_left_goal_kick_point; }
    const Vector & ourRightGoalKickPoint() const    { return M_our_right_goal_kick_point; }
    const Vector & oppLeftGoalKickPoint() const     { return M_opp_left_goal_kick_point; }
    const Vector & oppRightGoalKickPoint() const    { return M_opp_right_goal_kick_point; }

    const Vector & ourLeftGoalPost() const          { return M_our_left_goal_post; }
    const Vector & ourRightGoalPost() const         { return M_our_right_goal_post; }
    const Vector & oppLeftGoalPost() const          { return M_opp_left_goal_post; }
    const Vector & oppRightGoalPost() const         { return M_opp_right_goal_post; }

    const Vector & pitchTopLeftPoint() const        { return M_pitch_top_left_point; }
    const Vector & pitchBottomLeftPoint() const     { return M_pitch_bottom_left_point; }
    const Vector & pitchTopRightPoint() const       { return M_pitch_top_right_point; }
    const Vector & pitchBottomRightPoint() const    { return M_pitch_bottom_right_point; }

    const Vector & ourGoal() const                  { return M_our_goal; }
    const Vector & oppGoal() const                  { return M_opp_goal; }

    const double & ballRunDistWithMaxSpeed() const { return M_ball_run_dist_with_max_speed; };
    const double & maxTackleDist() const { return M_max_tackle_dist; }

private:
	double      M_ball_run_dist_with_max_speed;
    double      M_max_tackle_dist;

    double      M_max_catchable_area;
    double      M_one_minus_ball_decay;
    double      M_log_ball_decay;
    Array<double, 100> M_ball_decay_array; // just use in this class

    Line        M_left_line;
    Line        M_right_line;
    Line        M_top_line;
    Line        M_bottom_line;

    Rectangular M_pitch_rectanglar;
    Rectangular M_active_rectanglar;
    Rectangular M_our_penalty_area;
    Rectangular M_opp_penalty_area;
    Rectangular M_our_goal_area;
    Rectangular M_opp_goal_area;

    Vector      M_our_goal; //球门中心
    Vector      M_opp_goal;

    Vector      M_our_left_goal_kick_point;
    Vector      M_our_right_goal_kick_point;
    Vector      M_opp_left_goal_kick_point;
    Vector      M_opp_right_goal_kick_point;

    Vector      M_our_left_goal_post;
    Vector      M_our_right_goal_post;
    Vector      M_opp_left_goal_post;
    Vector      M_opp_right_goal_post;

    Vector      M_pitch_top_left_point;
    Vector      M_pitch_bottom_left_point;
    Vector      M_pitch_top_right_point;
    Vector      M_pitch_bottom_right_point;

public:
    double GetBallDistance(const double & speed, const uint & cycle)
    {
        return ( cycle <= 99 ) ?
            speed * (1.0 -  M_ball_decay_array[cycle]) / M_one_minus_ball_decay :
            speed / M_one_minus_ball_decay;
    }

    double GetBallSpeed(const uint & cycle, const double & dist)
    {
        return ( cycle <= 99 ) ?
            dist * M_one_minus_ball_decay / (1.0 - M_ball_decay_array[cycle]) :
            dist * M_one_minus_ball_decay;
    }

    double GetBallCycle(const double & speed, const double & dist)
    {
        double tmp = 1.0 - dist * M_one_minus_ball_decay / speed;
        return (tmp > 0.0) ? (log(tmp) / M_log_ball_decay) : 1000.0;
    }

    double GetBallDecay(const uint & cycle)
    {
    	return ( cycle <= 99 )? M_ball_decay_array[cycle]: 0.0;
    }
};


//from SoccerServer - Player.cpp
inline double GetNormalizeDashPower( const double & p )
{
    return MinMax( ServerParam::instance().minDashPower(), p, ServerParam::instance().maxDashPower() );
}

inline double GetNormalizeDashAngle( const double & d )
{
    return MinMax( ServerParam::instance().minDashAngle(), d, ServerParam::instance().maxDashAngle() );
}

inline double GetNormalizeKickPower( const double & p )
{
    return MinMax( ServerParam::instance().minPower(), p, ServerParam::instance().maxPower() );
}

inline double GetNormalizeTacklePower( const double & p )
{
    return MinMax( - ServerParam::instance().maxBackTacklePower(), p, ServerParam::instance().maxTacklePower() );
}

inline double GetNormalizeMoment( const double & p )
{
    return MinMax( ServerParam::instance().minMoment(), p, ServerParam::instance().maxMoment() );
}

inline double GetNormalizeNeckMoment( const double & p )
{
    return MinMax( ServerParam::instance().minNeckMoment(), p, ServerParam::instance().maxNeckMoment() );
}

inline double GetNormalizeNeckAngle( const double & p )
{
    return MinMax( ServerParam::instance().minNeckAngle(), p, ServerParam::instance().maxNeckAngle() );
}


// from 08: CP_at_point_buffer = 0.2; same to XToInBoundsX(..);
inline double YToInBoundsY( const double & y, const double & buffer )
{
    return ( y > 0 ? Min( y, ServerParam::instance().PITCH_WIDTH / 2 - buffer ) : Max( y, -ServerParam::instance().PITCH_WIDTH / 2 + buffer ) );
}

inline double YToInBoundsY( const double & y )
{
    return YToInBoundsY( y, 0.2 );
}

inline double XToInBoundsX( const double & x, const double & buffer )
{
    return ( x > 0 ? Min( x, ServerParam::instance().PITCH_LENGTH / 2 - buffer ) : Max( x, -ServerParam::instance().PITCH_LENGTH / 2 + buffer ) );
}

inline double XToInBoundsX( const double & x )
{
    return XToInBoundsX( x, 0.2 );
}

inline Vector PositionToInBoundsPosition( Vector pos, const double & bufferx, const double & buffery )
{
    pos.SetX(XToInBoundsX( pos.X(), bufferx ));
    pos.SetY(YToInBoundsY( pos.Y(), buffery ));
    return pos;
}

inline Vector PositionToInBoundsPosition( const Vector & pos )
{
    return PositionToInBoundsPosition( pos, 0.4, 0.4 );
}

inline bool IsPointInBounds(const Vector & point, const double buffer = 0.0)
{
    return (point.X() > -ServerParam::instance().PITCH_LENGTH / 2 + buffer
        && point.X() < ServerParam::instance().PITCH_LENGTH / 2 - buffer
        && point.Y() > -ServerParam::instance().PITCH_WIDTH / 2 + buffer
        && point.Y() < ServerParam::instance().PITCH_WIDTH / 2 - buffer ) ? true : false;
}

#endif /* SERVERPARAM_H_ */
