#ifndef __Types_H__
#define __Types_H__

#include <algorithm>
#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

#ifndef M_E
#define M_E         2.71828182845904523536
#endif

#define SQRT_2       1.41421356237309504880 // 根号2
#define FLOAT_EPS    0.000006
#define MAX_MESSAGE  8192
#define HUGE_VALUE   1.0e6
#define Unum_Unknown 0

#ifdef WIN32
typedef unsigned int uint;
#endif

/**
 * 强制造成段错误，以便被捕获，能记录下log和coredump
 * Create a segmentation fault for recording log and coredump file.
 */
extern int abort_with_SIGSEGV(const char *expr = 0, const char *file = 0, int line = 0, const char *function = 0);

/**
 * 因为release版本里面是没有定义Assert的，所以Assert里面不要放有效的表达式，否则release版本中将丢失。
 * Because there is no release version of the definition of Assert, so do not put inside Assert effective
 * expression, or release version will be lost.
 */
#ifdef _Debug
#ifndef WIN32
#define Assert(expr) ((expr)? (0): abort_with_SIGSEGV (__STRING(expr), __FILE__, __LINE__, __PRETTY_FUNCTION__))
#else
#include <cassert>
#define Assert(expr) assert(expr)
#endif
#else
#define Assert(expr)
#endif

#define PRINT_ERROR(error) \
	do { \
		std::cerr << __FILE__ << ":" << __LINE__ << " : " << error << std::endl; \
	} while(0)

/**
 * 球员的个数
 * Number of players.
 */
#define TEAMSIZE 11

/**
 * Number of hetero player types.
 */
#define MAX_PLAYER_TYPES 18

typedef double AngleRad;
typedef double AngleDeg;
typedef int    Unum;
typedef int    ObjectIndex;
typedef std::pair<double, double> SinCosT;

enum ServerPlayMode
{
	SPM_Null,
	SPM_BeforeKickOff,
	SPM_TimeOver,
	SPM_PlayOn,
	SPM_KickOff_Left,
	SPM_KickOff_Right,
	SPM_KickIn_Left,
	SPM_KickIn_Right,
	SPM_FreeKick_Left,
	SPM_FreeKick_Right,
	SPM_CornerKick_Left,
	SPM_CornerKick_Right,
	SPM_GoalKick_Left,
	SPM_GoalKick_Right,
	SPM_AfterGoal_Left,
	SPM_AfterGoal_Right,
	SPM_Drop_Ball,
	SPM_OffSide_Left,
	SPM_OffSide_Right,
	// [I.Noda:00/05/13] added for 3D viewer/commentator/small league
	SPM_PK_Left,
	SPM_PK_Right,
	SPM_FirstHalfOver,
	SPM_Pause,
	SPM_Human,
	SPM_Foul_Charge_Left,
	SPM_Foul_Charge_Right,
	SPM_Foul_Push_Left,
	SPM_Foul_Push_Right,
	SPM_Foul_MultipleAttacker_Left,
	SPM_Foul_MultipleAttacker_Right,
	SPM_Foul_BallOut_Left,
	SPM_Foul_BallOut_Right,
	SPM_Back_Pass_Left,
	SPM_Back_Pass_Right,
	SPM_Free_Kick_Fault_Left,
	SPM_Free_Kick_Fault_Right,
	SPM_CatchFault_Left,
	SPM_CatchFault_Right,
	SPM_IndFreeKick_Left,
	SPM_IndFreeKick_Right,
	SPM_PenaltySetup_Left,
	SPM_PenaltySetup_Right,
	SPM_PenaltyReady_Left,
	SPM_PenaltyReady_Right,
	SPM_PenaltyTaken_Left,
	SPM_PenaltyTaken_Right,
	SPM_PenaltyMiss_Left,
	SPM_PenaltyMiss_Right,
	SPM_PenaltyScore_Left,
	SPM_PenaltyScore_Right,

	//not standard
	SPM_GoalieCatchBall_Left,
	SPM_GoalieCatchBall_Right,
	SPM_Foul_Left,
	SPM_Foul_Right,
	SPM_PenaltyOnfield_Left,
	SPM_PenaltyOnfield_Right,
	SPM_PenaltyFoul_Left,
	SPM_PenaltyFoul_Right,
	SPM_PenaltyWinner_Left,
	SPM_PenaltyWinner_Right,
	SPM_HalfTime,
	SPM_TimeUp,
	SPM_TimeExtended,

	SPM_MAX
};

/**
 * 从ServerPlayMode中解析出来，高层直接用这个方便
 * Generated from ServerPlayMode, it's better to use this one.
 */
enum PlayMode
{
	PM_No_Mode,

	PM_Play_On,

	PM_Our_Kick_Off,
	PM_Our_Kick_In,
	PM_Our_Corner_Kick,
	PM_Our_Goal_Kick,
	PM_Our_Free_Kick,
	PM_Our_Indirect_Free_Kick,
	PM_Our_Goalie_Free_Kick,    /* not a real play mode */
	PM_Our_Offside_Kick,
	PM_Our_Back_Pass_Kick,
	PM_Our_Free_Kick_Fault_Kick,
	PM_Our_CatchFault_Kick,
	PM_Our_Foul_Charge_Kick,

	PM_Our_Penalty_Setup,
	PM_Our_Penalty_Ready,
	PM_Our_Penalty_Taken,
	PM_Our_Penalty_Score,
	PM_Our_Penalty_Miss,
    PM_Our_Penalty_Foul,
    PM_Our_Penalty_Winner,

	PM_Our_Mode,

	PM_Before_Kick_Off,
	PM_Penalty_On_Our_Field,
	PM_Our_Penalty,
	PM_Opp_Penalty,
	PM_Penalty_On_Opp_Field,
	PM_Drop_Ball,
	PM_Half_Time,
	PM_Time_Over,
    PM_Time_Up,
	PM_Extended_Time,
	PM_Goal_Ours,
	PM_Goal_Opps,
    PM_Our_Foul,
    PM_Opp_Foul,

	PM_Opp_Mode,

	PM_Opp_Kick_Off,
	PM_Opp_Kick_In,
	PM_Opp_Corner_Kick,
	PM_Opp_Goal_Kick,
	PM_Opp_Free_Kick,
	PM_Opp_Indirect_Free_Kick,
	PM_Opp_Goalie_Free_Kick,  /* not a real play mode */
	PM_Opp_Offside_Kick,
	PM_Opp_Free_Kick_Fault_Kick,
	PM_Opp_Back_Pass_Kick,
	PM_Opp_CatchFault_Kick,
	PM_Opp_Foul_Charge_Kick,

	PM_Opp_Penalty_Setup,
	PM_Opp_Penalty_Ready,
	PM_Opp_Penalty_Taken,
	PM_Opp_Penalty_Score,
	PM_Opp_Penalty_Miss,
    PM_Opp_Penalty_Foul,
    PM_Opp_Penalty_Winner,
};

enum CardType
{
	CR_None = 0,
	CR_Yellow,
	CR_Red
};

enum KickOffMode
{
	KO_Ours,
	KO_Opps,
};

enum ViewWidth
{
	VW_None,
	VW_Narrow,
	VW_Normal,
	VW_Wide
};

enum Situation
{
	ST_Forward_Attack,
	ST_Penalty_Attack,
	ST_Defense
};

#endif
