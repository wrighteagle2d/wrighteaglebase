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

#ifndef __Parser_H__
#define __Parser_H__

#include "Types.h"
#include "Thread.h"


enum ObjectType
{
	OBJ_Line,
	OBJ_Ball,
	OBJ_Marker,
	OBJ_Marker_Behind,  /* Not seen */
	OBJ_Player,
	OBJ_None
};

enum SideLineType
{
	SL_Left = 0,
	SL_Right,
	SL_Top,
	SL_Bottom,

	SL_MAX,
	SL_NONE
};

enum MarkerType
{
	Goal_L = 0,
	Goal_R,

	Flag_C,
	Flag_CT,
	Flag_CB,
	Flag_LT,
	Flag_LB,
	Flag_RT,
	Flag_RB,

	Flag_PLT,
	Flag_PLC,
	Flag_PLB,
	Flag_PRT,
	Flag_PRC,
	Flag_PRB,

	Flag_GLT,
	Flag_GLB,
	Flag_GRT,
	Flag_GRB,

	Flag_TL50,
	Flag_TL40,
	Flag_TL30,
	Flag_TL20,
	Flag_TL10,
	Flag_T0,
	Flag_TR10,
	Flag_TR20,
	Flag_TR30,
	Flag_TR40,
	Flag_TR50,

	Flag_BL50,
	Flag_BL40,
	Flag_BL30,
	Flag_BL20,
	Flag_BL10,
	Flag_B0,
	Flag_BR10,
	Flag_BR20,
	Flag_BR30,
	Flag_BR40,
	Flag_BR50,

	Flag_LT30,
	Flag_LT20,
	Flag_LT10,
	Flag_L0,
	Flag_LB10,
	Flag_LB20,
	Flag_LB30,

	Flag_RT30,
	Flag_RT20,
	Flag_RT10,
	Flag_R0,
	Flag_RB10,
	Flag_RB20,
	Flag_RB30,

	FLAG_MAX,
	FLAG_NONE
};

enum ServerMsgType
{
	None_Msg,
	Sight_Msg,
	CoachSight_Msg,
	Hear_Msg,
	Sense_Msg,
	Fullstate_Msg,
	ServerParam_Msg,
	PlayerParam_Msg,
	ChangePlayerType_Msg,
    Clang_Msg,
	PlayerType_Msg,
	Error_Msg,
    Disp_Msg,
	Ok_Msg,
	Score_Msg,
	Initialize_Msg
};

class Observer;

class Parser: public Thread
{
	Observer *mpObserver;

public:
    /**
     * 构造函数和析构函数
     */
    Parser(Observer *p_observer);
    virtual ~Parser();

    /**
     * 主循环函数
     */
   void StartRoutine();

    /**
     * 动态调试需要用到的接口
     */
	void Parse(char *msg);
	bool ParseInitializeMsg(const char *msg);

private:
	void ConnectToServer();
	void SendInitialLizeMsg();
	void ParseServerParam(char *msg);
	void ParsePlayerParam(char *msg);
	void ParsePlayerType(char *msg);

	void ParseTime(char *msg, char **end_ptr, bool is_new_cycle = false);
	void ParseRefereeMsg(char *msg);
	void ParsePlayMode(char *msg);
	void ParseSense(char *msg);
	void ParseSight(char *msg);
	void ParseSight_Coach(char *msg);
	void ParseSound(char *msg);
	void ParseChangePlayerType(char *msg);
	void ParseCard(char *msg);
	void ParseOkMsg(char *msg);

	ServerPlayMode mLastServerPlayMode;

	struct ObjType {
		ObjType(): type(OBJ_None), marker(FLAG_NONE), line(SL_NONE), side('?'), num(0)
		{

		}

		ObjectType type;
		MarkerType marker;
		SideLineType line;
		char side;
		Unum num;
	};

	struct ObjProperty {
		ObjProperty(): dist(INVALID_VALUE), dir(INVALID_VALUE), dist_chg(INVALID_VALUE),
			dir_chg(INVALID_VALUE), body_dir(INVALID_VALUE), head_dir(INVALID_VALUE),
			pointing(false), point_dir(INVALID_VALUE), tackling(false), kicked(false), 
			lying(false), card_type(CR_None)
		{

		}

		double dist;
		double dir;
		double dist_chg;
		double dir_chg;
		AngleDeg body_dir;
		AngleDeg head_dir;
		bool pointing;
		AngleDeg point_dir;
		bool tackling;
		bool kicked;
		bool lying;
		CardType card_type;
	};

	struct ObjProperty_Coach {
		ObjProperty_Coach(): x(INVALID_VALUE), y(INVALID_VALUE), vx(INVALID_VALUE), vy(INVALID_VALUE),
				body_dir(INVALID_VALUE), head_dir(INVALID_VALUE), pointing(false), point_dir(INVALID_VALUE), tackling(false), lying(false), card_type(CR_None)
		{

		}

		double x;
		double y;
		double vx;
		double vy;
		AngleDeg body_dir;
		AngleDeg head_dir;
		bool pointing;
		AngleDeg point_dir;
		bool tackling;
		bool lying;
		CardType card_type;
	};

	ObjType ParseObjType(char *msg);
	ObjType ParseGoal(char *msg);
	ObjType ParseMarker(char *msg);
	ObjType ParseLine(char *msg);
	ObjType ParsePlayer(char *msg);
	ObjType ParseBall(char *msg);

	ObjProperty ParseObjProperty(char* msg);
	ObjProperty_Coach ParseObjProperty_Coach(char* msg);

	ThreadMutex mOkMutex; //更新ok信息是要与决策线程互斥
	int mHalfTime; // 记录是第几个half
    bool mConnectServerOk;
	bool mClangOk;
	bool mSynchOk;
	bool mEyeOnOk; //only used when is coach
	PlayerArray<bool, true> mChangePlayerTypeOk;

	static char mBuf[MAX_MESSAGE];

public:
    bool IsConnectServerOk() { bool ret; mOkMutex.Lock(); ret = mConnectServerOk; mOkMutex.UnLock(); return ret; }
	bool IsClangOk() { bool ret; mOkMutex.Lock(); ret = mClangOk; mOkMutex.UnLock(); return ret; }
	bool IsSyncOk() { bool ret; mOkMutex.Lock(); ret = mSynchOk; mOkMutex.UnLock(); return ret; }
	bool IsEyeOnOk() { bool ret; mOkMutex.Lock(); ret = mEyeOnOk; mOkMutex.UnLock(); return ret; }
	bool IsChangePlayerTypeOk(Unum i) { bool ret; mOkMutex.Lock(); ret = mChangePlayerTypeOk[i]; mOkMutex.UnLock(); return ret; }

	static bool IsPlayerTypesReady() { return mIsPlayerTypesReady; }

private:
	static bool mIsPlayerTypesReady;
	static const double INVALID_VALUE;

	static bool InvalidValue(const double & x) {
		return x >= INVALID_VALUE - FLOAT_EPS;
	}
};


#endif
