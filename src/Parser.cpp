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

#include <limits>
#include "Parser.h"
#include "DynamicDebug.h"
#include "TimeTest.h"
#include "Observer.h"
#include "UDPSocket.h"
#include "ServerParam.h"
#include "PlayerParam.h"
#include "Logger.h"
#include "Thread.h"
#include "NetworkTest.h"

char Parser::mBuf[MAX_MESSAGE];
bool Parser::mIsPlayerTypesReady = false;
const double Parser::INVALID_VALUE = std::numeric_limits<double>::max();

#ifdef _Debug
#define PARSE_ERROR(x) PRINT_ERROR((x))
#else
#define PARSE_ERROR(x)
#endif


Parser::Parser(Observer *p_observer)
{
	mpObserver = p_observer;

	mConnectServerOk = false;
	mHalfTime = 0;
	mClangOk = false;
	mSynchOk = false;
	mEyeOnOk = false;
	mEarOnOk = false;

	mLastServerPlayMode = SPM_Null;

	ServerPlayModeMap::instance();

	if (PlayerParam::instance().isCoach()){
		UDPSocket::instance().Initial(
				ServerParam::instance().serverHost().c_str(),
				ServerParam::instance().onlineCoachPort()
		);
	}
	else if (PlayerParam::instance().isTrainer()) {
		UDPSocket::instance().Initial(
				ServerParam::instance().serverHost().c_str(),
				ServerParam::instance().offlineCoachPort()
		);
	}
	else {
		UDPSocket::instance().Initial(
				ServerParam::instance().serverHost().c_str(),
				ServerParam::instance().playerPort()
		);
	}
}

Parser::~Parser()
{
}


void Parser::StartRoutine()
{
	ConnectToServer();

	while( true )
	{
		if (UDPSocket::instance().Receive(mBuf) > 0)
		{
			NetworkTest::instance().AddParserBegin();

			DynamicDebug::instance().AddMessage(mBuf, MT_Parse); // 动态调试记录Parser信息

			mpObserver->Lock(); //parse时禁止WorldState更新
			Parse(mBuf);
			mpObserver->UnLock();

			NetworkTest::instance().AddParserEnd(mpObserver->CurrentTime());
		}
	}
}

void Parser::ConnectToServer()
{
	mOkMutex.Lock();
	mConnectServerOk = false;
	mOkMutex.UnLock();

	SendInitialLizeMsg();
	do {
		UDPSocket::instance().Receive(mBuf);
	} while (!ParseInitializeMsg(mBuf));

	mOkMutex.Lock();
	mConnectServerOk = true;
	mOkMutex.UnLock();

	DynamicDebug::instance().Initial(mpObserver); // 动态调试的初始化，知道自己是哪边了才能初始化，位置不能动
	DynamicDebug::instance().AddMessage(mBuf, MT_Parse); // 动态调试记录Parse信息

	//Logger::instance().GetTextLogger("msg-text") << mBuf << std::endl;
}

void Parser::SendInitialLizeMsg(){
	//TODO: how about reconnect
	std::ostringstream init_string;
	if ( PlayerParam::instance().isCoach()){
		init_string << "(init " << PlayerParam::instance().teamName() <<
				" (version " << PlayerParam::instance().coachVersion() << "))";
	}
	else if ( PlayerParam::instance().isGoalie()){
		init_string << "(init " << PlayerParam::instance().teamName() <<
				" (version " << PlayerParam::instance().playerVersion() << ") (goalie))";
	}
	else if ( PlayerParam::instance().isTrainer()) {
		init_string << "(init " <<
				" (version " << PlayerParam::instance().playerVersion() << "))";
	}
	else {
		init_string << "(init " << PlayerParam::instance().teamName() <<
				" (version " << PlayerParam::instance().playerVersion() << "))";
	}

	if (UDPSocket::instance().Send(init_string.str().c_str()) < 0){
		PRINT_ERROR("send_initialize_message failed");
	}
}

bool Parser::ParseInitializeMsg(const char *msg)
{
	char play_mode[128];
	char my_side = '?';
	Unum my_unum = 0;
	//TODO: how about reconnect

	if ( PlayerParam::instance().isCoach()){
		if ( !(strncmp(msg,"(init", 4))) { //initialize message
			sscanf(msg,"(init %c %[^)]",&my_side, play_mode);
		}
		else {
			return false;
		}
	}
	else if ( PlayerParam::instance().isTrainer()) {
		if ( !(strcmp(msg,"(init ok)"))) { //initialize message
			//初始信息没有我们的信息，Trainer是全局的
		}
		else {
			return false;
		}
		my_side = 'l';			//设定trainer是我方的
		my_unum = TRAINER_UNUM ;//设定trainer的号码为12号
		play_mode[0] = 'b';
	}
	else {
		if ( !(strncmp(msg,"(init", 4))) { //initialize message
			sscanf(msg,"(init %c %d %[^)]",&my_side, &my_unum, play_mode);
		}
		else {
			return false;
		}
	}

	mpObserver->SetOurInitSide(my_side);
	mpObserver->SetOurSide(my_side);
	mpObserver->SetSelfUnum(my_unum);
	++ mHalfTime;
	if ( play_mode[0] == 'b' || PlayerParam::instance().isCoach()){ /* Before_kick_off */
		mpObserver->SetPlayMode(PM_Before_Kick_Off);
		mpObserver->SetServerPlayMode(SPM_BeforeKickOff);
		if ( mpObserver->OurInitSide() == 'l' )
			mpObserver->SetKickOffMode(KO_Ours);
		else
			mpObserver->SetKickOffMode(KO_Opps);
	}
	else{
		mpObserver->SetPlayMode(PM_Play_On);
		mpObserver->SetServerPlayMode(SPM_PlayOn);
	}

	mpObserver->Initialize(); //必须知道自己是哪边的才能初始化

	TimeTest::instance().SetUnum(my_unum); // TimeTest的记录文件名会用到
	NetworkTest::instance().SetUnum(my_unum);

	return true;
}

void Parser::Parse(char *msg)
{
	ServerMsgType msg_type = None_Msg;

	switch ( msg[1] ) {
	case 'i': msg_type = Initialize_Msg; break; /* ( i nit */
	case 'c':
		switch (msg[2]){
		case 'h': msg_type = ChangePlayerType_Msg; break;	/* ( c h ange */
		case 'l': msg_type = Clang_Msg; break;   /* ( c l ang */
		}
		break;
	case 'd': msg_type = Disp_Msg; break; /* (d isp */
	case 'e': msg_type = Error_Msg; break;					/* ( e rror */
	case 'f': msg_type = Fullstate_Msg; break;			/* ( f ullstate */
	case 'h': msg_type = Hear_Msg; break;					/* ( h ear  */
	case 'o': msg_type = Ok_Msg; break;							/* ( o k */
	case 'p':
		switch( msg[8] ) {
		case 'p': msg_type = PlayerParam_Msg; break;		/* (p layer_ p ram */
		case 't': msg_type = PlayerType_Msg; break;  	/* ( p layer_ t ype */
		default: msg_type = None_Msg; break;
		}
		break;
	case 's':
		switch( msg[3]) {
		case 'o': msg_type = Score_Msg; break;				/* ( s c o re */
		case 'e':
			switch( msg[4]){
			case ' ': msg_type = Sight_Msg; break;		 		/* ( s e e   */
			case '_': msg_type = CoachSight_Msg; break; 		/* ( s e e _global   */
			default: msg_type = None_Msg; break;
			}
			break;
		case 'n': msg_type = Sense_Msg; break;		 		/* ( s e n se_body */
		case 'r': msg_type = ServerParam_Msg; break;		/* ( s e r ver_param */
		default: msg_type = None_Msg; break;
		}
		break;
	default: msg_type = None_Msg; break;
	}

	char *time_end = 0;
	switch (msg_type){
	case Sight_Msg: ParseTime(msg, &time_end, false); ParseSight(time_end); mpObserver->SetNewSight(); break;
	case CoachSight_Msg: ParseTime(msg, &time_end, true); ParseSight_Coach(time_end); mpObserver->SetNewSight(); break;
	case Sense_Msg: ParseTime(msg, &time_end, true); ParseSense(time_end); mpObserver->SetNewSense(); break;
	case Hear_Msg: if(!ParseForTrainer(msg)){ ParseTime(msg, &time_end, true); ParseSound(time_end); }break;
	case PlayerParam_Msg: ParsePlayerParam(msg); break;
	case ServerParam_Msg: ParseServerParam(msg); break;
	case Disp_Msg: /*TODO: process disp message */ break;
	case Fullstate_Msg: /*TODO: disable null processing */ break;
	case ChangePlayerType_Msg: ParseChangePlayerType(msg); break;
	case Clang_Msg: /*TODO: process clang message*/ break;
	case PlayerType_Msg: ParsePlayerType(msg); break;
	case Score_Msg: /*TODO: process score message*/ break;
	case Ok_Msg: ParseOkMsg(msg); break;
	case Initialize_Msg: ParseInitializeMsg(msg); break;
	case Error_Msg: std::cout << __LINE__ << " Cycle: " << mpObserver->CurrentTime() << "; msg : " << msg << std::endl; break;
	case None_Msg: PRINT_ERROR(mpObserver->CurrentTime() << msg); break;
	}
}

void Parser::ParseTime(char *msg, char **end_ptr, bool is_new_cycle)
{
	while (!isspace(*msg)) msg++;
	*end_ptr = msg;
	int time = parser::get_int(end_ptr);

	RealTime real_time = GetRealTimeParser();

	if (mpObserver->IsPlanned()) { // -- 决策完了，才收到信息
		std::cerr << "# " << mpObserver->SelfUnum() << " @ " << mpObserver->CurrentTime() << " got a deprecated message" << std::endl;
	}

	if (is_new_cycle == true)
	{
		/** player: sense msg & hear msg */
		/** coach:  sight msg & hear msg */
		if (time > mpObserver->CurrentTime().T()) // new cycle
		{
			mpObserver->SetCurrentTime(Time(time, 0));
			mpObserver->SetLastCycleBeginRealTime(real_time);
		}
		else
		{
			if (real_time - mpObserver->GetLastCycleBeginRealTime() >
			ServerParam::instance().simStep() * 0.7) // stoppage time, add some buffer
			{
				mpObserver->SetCurrentTime(Time(time, mpObserver->CurrentTime().S() + 1));
				mpObserver->SetLastCycleBeginRealTime(real_time);
			}
			else
			{
				// the current time has been set by another msg just now
			}
		}
	}
	else
	{
		/** player: sight msg */
		if (time != mpObserver->CurrentTime().T()) // new cycle -- miss a sense
		{
			mpObserver->SetCurrentTime(Time(time, 0));
			mpObserver->SetLastCycleBeginRealTime(real_time - ServerParam::instance().synchOffset());

			PRINT_ERROR("Player " << mpObserver->SelfUnum() << " miss a sense at " << mpObserver->CurrentTime());
		}
	}
}

void Parser::ParsePlayerParam(char *msg)
{
	Logger::instance().InitSightLogger(PlayerParam_Msg, msg);

	msg += 13; // 去掉"(player_param"
	PlayerParam::instance().ParseFromServerMsg(msg);
	PlayerParam::instance().MaintainConsistency();
}

void Parser::ParseServerParam(char *msg)
{
	Logger::instance().InitSightLogger(ServerParam_Msg, msg);

	msg += 13; // 去掉"(server_param"
	ServerParam::instance().ParseFromServerMsg(msg);
	ServerParam::instance().MaintainConsistency();
}

void Parser::ParsePlayerType(char *msg)
{
	Logger::instance().InitSightLogger(PlayerType_Msg, msg);

	char *p = msg;
	while (!isspace(*p)) p++;
	while (isspace(*p)) p++;
	char *line = p;
	while (!isspace(*p)) p++;
	int type = parser::get_int(p);
	PlayerParam::instance().AddPlayerType(type, line);

	if (type >= PlayerParam::instance().playerTypes() - 1) {
		mIsPlayerTypesReady = true;
	}
}

void Parser::ParseChangePlayerType(char *msg)
{
	int player = parser::get_int(&msg);
	if (*msg != ')'){
		int type = parser::get_int(msg);
		mpObserver->SetTeammateType(player, type);
	}
	else {
		mpObserver->AddOpponentTypeChangedCount(player);
	}
}

void Parser::ParseSight(char *msg)
{
	NetworkTest::instance().End("Sense", "Sight");

	mpObserver->SetLastSightRealTime(GetRealTimeParser()); // set the last sight time
	mpObserver->SetLatestSightTime(mpObserver->CurrentTime());

	msg = strstr(msg,"((");
	while ( msg != 0 ){ // 直到没有object为止
		msg += 2; // 跳过 ((
		ObjType obj = ParseObjType(msg); // 获得object的类型
		msg = strchr(msg,')');
		ObjProperty prop = ParseObjProperty(msg + 1);  // 获得object的属性

		switch ( obj.type ) {
		case OBJ_Marker:
		case OBJ_Marker_Behind:
			if ( obj.marker == FLAG_NONE ){
				if ( obj.type != OBJ_Marker_Behind ) {
					PARSE_ERROR("Should know the marker");
				}
				break;
			}
			if (InvalidValue(prop.dir_chg)) {
				mpObserver->SeeMarker(obj.marker, prop.dist, prop.dir);
			}
			else {                                          /* know direction*/
				mpObserver->SeeMarker(obj.marker, prop.dist, prop.dir, prop.dist_chg, prop.dir_chg);
			}
			break;
		case OBJ_Line:
			mpObserver->SeeLine(obj.line, prop.dist, prop.dir);
			break;
		case OBJ_Ball:
			if (InvalidValue(prop.dir_chg)) {                  /* high quality  */
				mpObserver->SeeBall(prop.dist, prop.dir);
			}
			else {                                          /* know direction*/
				mpObserver->SeeBall(prop.dist, prop.dir, prop.dist_chg, prop.dir_chg);
			}
			break;
		case OBJ_Player:
			if ( obj.side == '?'){                      /* Too far for team or num */
				if (InvalidValue(prop.dir_chg)) {         /* high quality  */
					mpObserver->SeePlayer(prop.dist, prop.dir);
				}
				else { /* know direction*/
					PARSE_ERROR("Shouldn't know dirChng when the player's far");
				}
			}
			else{ // 知道是哪边的
				if ( obj.num == 0 ){                  /* Too far for number     */
					if (InvalidValue(prop.dir_chg)) {   /* high quality  */
						mpObserver->SeePlayer(obj.side, prop.dist, prop.dir, prop.tackling, prop.kicked, prop.lying, prop.card_type);
					}
					else  {         /* know direction*/
						PARSE_ERROR("Shouldn't know dirChng when the team Member's far");
					}
				}
				else{                                   /* Know side AND number   */
					if (InvalidValue(prop.dir_chg)) {                  /* high quality  */
						mpObserver->SeePlayer(obj.side, obj.num, prop.dist, prop.dir, prop.pointing, prop.point_dir, prop.tackling, prop.kicked, prop.lying, prop.card_type);
					} else {                                          /* know direction*/
						mpObserver->SeePlayer(obj.side, obj.num, prop.dist, prop.dir, prop.dist_chg, prop.dir_chg,
								prop.body_dir, prop.head_dir, prop.pointing, prop.point_dir, prop.tackling, prop.kicked, prop.lying, prop.card_type);
					}
				}
			}
			break;
		default:
			break;
		}
		msg = strstr(msg,"(("); // 下一个object
	}
}

Parser::ObjType Parser::ParseObjType(char *msg)
{
	switch( *msg ) {
	case 'g':
	case 'G':
		return ParseGoal(msg);
	case 'f':
	case 'F':
		return ParseMarker(msg);
	case 'l':
		return ParseLine(msg);
	case 'p':
	case 'P':
		return ParsePlayer(msg);
	case 'b':
	case 'B':
		return ParseBall(msg);
	default:
		PARSE_ERROR("unknown object"); break;
	}

	ObjType result;
	result.type = OBJ_None;
	return result;
}

Parser::ObjType Parser::ParseGoal(char *msg)
{
	ObjType result;
	if ( *msg == 'g' ) { // goal
		result.type = OBJ_Marker;
		msg += 2; // 'g '
		if ( *msg == 'r' ) { result.marker = Goal_R; } else
			if ( *msg == 'l' ) { result.marker = Goal_L; }
			else { PARSE_ERROR("goal ?"); }
	}
	else if ( *msg =='G' ){ // goal behind
		result.type = OBJ_Marker_Behind;
		//result.marker = _pMem->ClosestGoal(); TODO: how to process?
	}
	return result;
}

Parser::ObjType Parser::ParseMarker(char *msg)
{
	ObjType result;
	if ( *msg == 'f' ){	// flag
		result.type = OBJ_Marker;
		msg += 2;     // 'f '
		if ( *msg == 'r' ){
			msg += 2; // 'r'
			if ( *msg == '0' ) { result.marker = Flag_R0; }
			else if ( *msg == 'b' ){
				msg += 1;
				if ( *msg== ')' ) { result.marker = Flag_RB; }
				else { // flag r b
					msg += 1; // skip space
					if ( *msg == '1' ) { result.marker = Flag_RB10; } else
						if ( *msg == '2' ) { result.marker = Flag_RB20; } else
							if ( *msg == '3' ) { result.marker = Flag_RB30; }
							else { PARSE_ERROR("flag r b ?"); }
				}
			} else
				if ( *msg == 't' ) {
					msg += 1;
					if ( *msg == ')' ) { result.marker = Flag_RT;}
					else {   // flag r t
						msg += 1;
						if ( *msg == '1' ) { result.marker = Flag_RT10; } else
							if ( *msg == '2' ) { result.marker = Flag_RT20; } else
								if ( *msg == '3' ) { result.marker = Flag_RT30; }
								else { PARSE_ERROR("flag r t ?"); }
					}
				} else { PARSE_ERROR("flag r ?"); }
		} else
			if ( *msg == 'l' ) { // flag l
				msg += 2; // 'l '
				if ( *msg == '0' ) { result.marker = Flag_L0; }  else
					if ( *msg == 'b' ) { // flag l b
						msg += 1;
						if ( *msg == ')' ) { result.marker = Flag_LB; }
						else {
							msg += 1;
							if ( *msg == '1' ) { result.marker = Flag_LB10; } else
								if ( *msg == '2' ) { result.marker = Flag_LB20; } else
									if ( *msg == '3' ) { result.marker = Flag_LB30; }
									else { PARSE_ERROR("flag l b ?"); }
						}
					}else
						if ( *msg == 't' ){	 // flag l t
							msg += 1;
							if ( *msg == ')' ) { result.marker = Flag_LT; }
							else{
								msg += 1;
								if ( *msg == '1' ) { result.marker = Flag_LT10; } else
									if ( *msg == '2' ) { result.marker = Flag_LT20; } else
										if ( *msg == '3' ) { result.marker = Flag_LT30; }
										else { PARSE_ERROR("flag l t ?"); }
							}
						} else { PARSE_ERROR("flag l ?"); }
			} else
				if ( *msg == 't' ){ // flag t
					msg += 2;
					if ( *msg == '0' ) { result.marker = Flag_T0; }  else
						if ( *msg == 'l' ){
							msg += 2;
							if ( *msg == '1' ) { result.marker = Flag_TL10; } else
								if ( *msg == '2' ) { result.marker = Flag_TL20; } else
									if ( *msg == '3' ) { result.marker = Flag_TL30; } else
										if ( *msg == '4' ) { result.marker = Flag_TL40; } else
											if ( *msg == '5' ) { result.marker = Flag_TL50; }
											else { PARSE_ERROR("flag t l ?"); }
						} else
							if ( *msg=='r' ){
								msg += 2;
								if ( *msg == '1' ) { result.marker = Flag_TR10; } else
									if ( *msg == '2' ) { result.marker = Flag_TR20; } else
										if ( *msg == '3' ) { result.marker = Flag_TR30; } else
											if ( *msg == '4' ) { result.marker = Flag_TR40; } else
												if ( *msg == '5' ) { result.marker = Flag_TR50; }
												else { PARSE_ERROR("flag t r ?"); }
							} else { PARSE_ERROR("flag t ?"); }
				} else
					if ( *msg == 'b' ){
						msg += 2;
						if ( *msg == '0' ) { result.marker = Flag_B0; } else
							if ( *msg == 'l' ) {
								msg += 2;
								if ( *msg == '1' ) { result.marker = Flag_BL10; } else
									if ( *msg == '2' ) { result.marker = Flag_BL20; } else
										if ( *msg == '3' ) { result.marker = Flag_BL30; } else
											if ( *msg == '4' ) { result.marker = Flag_BL40; } else
												if ( *msg == '5' ) { result.marker = Flag_BL50; }
												else { PARSE_ERROR("flag b l ?"); }
							} else
								if ( *msg == 'r' ) {
									msg += 2;
									if ( *msg == '1' ) { result.marker = Flag_BR10; } else
										if ( *msg == '2' ) { result.marker = Flag_BR20; } else
											if ( *msg == '3' ) { result.marker = Flag_BR30; } else
												if ( *msg == '4' ) { result.marker = Flag_BR40; } else
													if ( *msg == '5' ) { result.marker = Flag_BR50; }
													else { PARSE_ERROR("flag b r ?"); }
								} else { PARSE_ERROR("flag b ?"); }
					} else
						if ( *msg == 'c' ){
							msg+=1;
							if ( *msg == ')' ) { result.marker = Flag_C; }
							else {
								msg += 1;
								if ( *msg == 'b' ) { result.marker = Flag_CB; } else
									if ( *msg == 't' ) { result.marker = Flag_CT; }
									else { PARSE_ERROR("flag c ?"); }
							}
						} else
							if ( *msg == 'p' ){
								msg += 2;
								if ( *msg == 'r' ) {
									msg += 2;
									if ( *msg == 't') { result.marker = Flag_PRT; } else
										if ( *msg == 'c') { result.marker = Flag_PRC; } else
											if ( *msg == 'b') { result.marker = Flag_PRB; }
											else { PARSE_ERROR("flag p r ?"); }
								} else
									if ( *msg == 'l' ) {
										msg += 2;
										if ( *msg == 't') { result.marker = Flag_PLT; } else
											if ( *msg == 'c') { result.marker = Flag_PLC; } else
												if ( *msg == 'b') { result.marker = Flag_PLB; }
												else { PARSE_ERROR("flag p l ?"); }
									} else { PARSE_ERROR("flag p ?"); }
							} else
								if ( *msg == 'g' ){
									msg+=2;
									if ( *msg == 'l' ) {
										msg += 2;
										if ( *msg == 't' ) { result.marker = Flag_GLT; } else
											if ( *msg == 'b' ) { result.marker = Flag_GLB; }
											else { PARSE_ERROR("flag g l ?"); }
									} else
										if ( *msg == 'r' ){
											msg += 2;
											if ( *msg == 't' ) { result.marker = Flag_GRT; } else
												if ( *msg == 'b' ) { result.marker = Flag_GRB; }
												else { PARSE_ERROR("flag g r ?"); }
										} else { PARSE_ERROR("flag g ?"); }
								} else { PARSE_ERROR("flag ?"); }
	} else
		if ( *msg == 'F' ){ // Flag behind
			result.type = OBJ_Marker_Behind;
			//result.marker = _pMem->ClosestFlagTo(); /*TODO:  could be No_Marker */
		}
	return result;
}

Parser::ObjType Parser::ParseLine(char *msg)
{
	ObjType result;
	result.type = OBJ_Line;
	msg += 2;     // 'l'
	if ( *msg == 'r' ) { result.line = SL_Right;  } else
		if ( *msg == 'l' ) { result.line = SL_Left;   } else
			if ( *msg == 't' ) { result.line = SL_Top;    } else
				if ( *msg == 'b' ) { result.line = SL_Bottom; }
				else { PARSE_ERROR("line ?"); }
	return result;
}

Parser::ObjType Parser::ParsePlayer(char* msg)
{
	ObjType result;
	const char *team_name = PlayerParam::instance().teamName().c_str();
	int team_name_len = PlayerParam::instance().teamNameLen();

	result.type = OBJ_Player;
	msg += 1;     // 'p'
	if ( *msg == ' ' ){              /* there's a team */
		msg += 2; // skip space and "
		if (msg[team_name_len] == '"' && !strncmp(msg, team_name, team_name_len)) {
			/* 一定要比较到双引号("),防止出现一个队名包含另一个的问题 */
			result.side = mpObserver->OurSide();
		}
		else {
			if ( PlayerParam::instance().opponentTeamName().empty()){
				int a = 0;
				char tmp[256];
				while ( *msg != '"' ) {
					tmp[a++] = *msg++;
				}
				tmp[a] = '\0';
				PlayerParam::instance().setOpponentTeamName(tmp);
			}
			result.side = mpObserver->OppSide();
		}
		while ( *msg != ' ' && *msg != ')' ) {
			msg++;
		} /* advance past team name */
		if ( *msg == ' ' ) { /* there's a number */
			result.num = parser::get_int(&msg);
		}

		if ( *msg == ' ' ){ //will be goalie
			msg += 1;
			if ( !strncmp(msg,"goalie", 6) ) {  // there's goalie
				if (result.side == mpObserver->OurInitSide()){
					// our goalie unum is no need to set
				}
				else {
					mpObserver->SeeOppGoalie(result.num);
				}
			}
			while ( *msg != ' ' && *msg != ')' ) {
				msg++;
			}/* advance past goalie */
		}
	}
	return result;
}

Parser::ObjType Parser::ParseBall(char* msg)
{
	(void) msg;
	ObjType result;
	result.type = OBJ_Ball;
	return result;
}

Parser::ObjProperty Parser::ParseObjProperty(char* msg)
{
	ObjProperty result;

	/* 'high' quality only */
	result.dist = parser::get_double(&msg);/* 'high' quality     */
	result.dir = parser::get_double(&msg);

	if ( *msg != ')' ){
		if( *msg == ' ' ){
			++msg;
		}
		if( *msg == 't' ){
			result.tackling = true;
			++msg;
		}
		else if ( *msg == 'k' ){
			result.kicked = true;
			++msg;
		}
		else if ( *msg == 'f' ){
			result.lying = true;
			++msg;
		}
	}
	if ( *msg != ')' ){
		result.dist_chg = parser::get_double(&msg);	// 也可能是pointdir
		if ( *msg == ' ' ) { // 后面还有
			msg++; // 跳过空格
			if ( *msg == 't' || *msg == 'k' || *msg == 'f') {
				if (*msg == 't'){
					result.tackling = true;
				}
				else if (*msg == 'k'){
					result.kicked = true;
				}
				else result.lying = true;
				msg++;
				result.point_dir = result.dist_chg;
				result.pointing = true;
				result.dist_chg = INVALID_VALUE;
			}
			else {
				result.dir_chg = parser::get_double(&msg);
			}
		}
		else if (*msg == ')'){
			result.pointing = true;
			result.point_dir = result.dist_chg;
			result.dist_chg = INVALID_VALUE;
		}
	}

	if ( *msg != ')' ) {
		result.body_dir = parser::get_double(&msg);
		result.head_dir = parser::get_double(&msg);
		if ( *msg == ' ' ) { // 后面还有 pointdir and/or tackling/kicked flag
			msg ++;
			if( *msg == 't' || *msg == 'k' || *msg == 'f' ) { // 只有tackling/kicked flag
				if (*msg == 't'){
					result.tackling = true;
				}
				else if (*msg == 'k'){
					result.kicked = true;
				}
				else result.lying = true;
				msg++;
			}
			else { // 是数字
				result.pointing = true;
				result.point_dir = parser::get_double(&msg);
				if( *msg == ' ' ) {
					while ( *msg == ' ' ) ++msg;
					if( *msg == 't' || *msg == 'k' || *msg == 'f' ) {
						if (*msg == 't'){
							result.tackling = true;
							++msg;
						}
						else if (*msg == 'k'){
							result.kicked = true;
							++msg;
						}
						else {
							result.lying = true;
							++msg;
						}
						while (*msg == ' ') ++msg;
					}
					if ( *msg == 'y' )
					{
						result.card_type = CR_Yellow;
						++msg;
					}
					else if ( *msg == 'r' )
					{
						result.card_type = CR_Red;
						++msg;
					}
					else if ( *msg != ')' ) 
					{
						PARSE_ERROR("why come to this place");
					}
				}
			}
		}
	}

	if ( *msg != ')' ) {
		PARSE_ERROR("Should be done with object info here");
	}

	Assert(result.card_type == CR_None); //TODO: server 不会发card_type和foul_charged
	Assert(result.lying == false);

	return result;
}

Parser::ObjProperty_Coach Parser::ParseObjProperty_Coach(char *msg)
{
	ObjProperty_Coach result;

	// goal or ball or player
	result.x = parser::get_double(&msg);
	result.y = parser::get_double(&msg);

	if (*msg != ')'){ //ball or player
		result.vx = parser::get_double(&msg);
		result.vy = parser::get_double(&msg);

		if (*msg != ')'){ //player
			result.body_dir = parser::get_double(&msg);
			result.head_dir = parser::get_double(&msg);

			while (*msg == ' ') ++msg;
			if (*msg != ')'){
				switch( *msg )
				{
				case 't':
					result.tackling = true;
					++msg;
					break;
				case 'f':
					result.lying = true;
					++msg;
					break;
				case 'y':
					result.card_type = CR_Yellow;
					++msg;
					break;
				case 'r':
					result.card_type = CR_Red;
					++msg;
					break;
				case 'k':
					//kicking
					++msg;
					break;
				default:
					result.pointing = true;
					result.point_dir = parser::get_double(&msg);
					break;
				}
				while ( *msg == ' ' ) ++msg;
				if (*msg != ')'){
					if (*msg == 't'){
						result.tackling = true;
					}
					else if (*msg == 'f'){
						result.lying = true;
					}
					else if (*msg == 'k'){
					}
					else if (*msg == 'y'){
						result.card_type = CR_Yellow;
					}
					else if (*msg == 'r'){
						result.card_type = CR_Red;
					}
					while ( *msg == ' ' ) ++msg;
					if ( *msg != ')' )
					{
						++msg;
						if ( *msg == 'y' )
						{
							result.card_type = CR_Yellow;
						}
						else if ( *msg == 'r' )
						{
							result.card_type = CR_Red;
						}
						//						else {
						//							PARSE_ERROR("why come to this place");
						//						}
					}
					/*else
					{
						PARSE_ERROR("why come to this place");
					}*/
				}
			}
		}
	}
	return result;
}

void Parser::ParseSense(char *msg)
{
	if (!PlayerParam::instance().isCoach() || !PlayerParam::instance().isTrainer())
	{
		TimeTest::instance().Update(mpObserver->CurrentTime()); // player每周期都有sense
	}

	NetworkTest::instance().Update(mpObserver->CurrentTime());
	NetworkTest::instance().Begin("Sense");

	parser::get_word(&msg);
	parser::get_next_word(&msg); //skip 'view'
	parser::get_next_word(&msg); //skip 'mode'
	parser::get_next_word(&msg); //skip not used view quality

	ViewWidth view_width = VW_None;
	switch ( msg[1] ) {
	case 'o': view_width = VW_Normal; break;  /* normal */
	case 'a': view_width = VW_Narrow; break;  /* narrow */
	case 'i': view_width = VW_Wide;   break;  /* wide   */
	default:  PARSE_ERROR("Unknown view quality"); break;
	}

	double stamina = parser::get_double(&msg);
	double effort  = parser::get_double(&msg);
	double capacity = parser::get_double(&msg);

	double speed   = parser::get_double(&msg);
	double speed_dir   = parser::get_double(&msg);
	double neck_dir = parser::get_double(&msg);

	int kicks  =   parser::get_int(&msg);
	int dashes =   parser::get_int(&msg);
	int turns  =   parser::get_int(&msg);
	int says   =   parser::get_int(&msg);
	int turn_necks   =   parser::get_int(&msg);
	int catchs = parser::get_int(&msg);
	int moves  = parser::get_int(&msg);
	int change_views = parser::get_int(&msg);

	int arm_movable_ban = parser::get_int(&msg); // 直到下次手臂能动的剩余周期数
	int arm_expires = parser::get_int(&msg); // 直到手臂动作失效剩余的周期数
	double arm_target_dist = parser::get_double(&msg); // 指向的目标的距离
	AngleDeg arm_target_dir = parser::get_double(&msg); // 指向的目标的方向
	int points = parser::get_int(&msg); // point count
	parser::get_next_word(&msg); // focus
	parser::get_next_word(&msg); // target
	parser::get_next_word(&msg); // side
	char focus_side = msg[0];

	Unum focus_unum = 0;
	if( focus_side != 'l' && focus_side != 'r' ){ // 没有特别注意的队员
		focus_side = '?';
	} else { // 特别注意的队员号码
		focus_unum = parser::get_int(&msg);
	}
	int focuses = parser::get_int(&msg);
	int tackle_expires = parser::get_int(&msg);
	int tackles = parser::get_int(&msg);

	//以下是碰撞和犯规信息
	mpObserver->ClearCollisionState();

	parser::get_word(&msg); // (collision {none|[(ball)][(player)][(post)]})
	parser::get_next_word(&msg); //skip 'collision'
	if (msg[0] != 'n') {
		do {
			switch (msg[1]) {
			case 'a': mpObserver->SetCollideWithBall(); break;
			case 'l': mpObserver->SetCollideWithPlayer(); break;
			case 'o': mpObserver->SetCollideWithPost(); break;
			}
			parser::get_next_word(&msg);
		} while (msg[0] != 'f' && msg[0]);
	}

	int foul_charged_cycle = 0;
	CardType card_type = CR_None;

	if (msg[0]) { //v14
		foul_charged_cycle = parser::get_int(&msg); // (foul (charged CYCLE) (card {none|yellow|red})))
		parser::get_word(&msg);
		parser::get_next_word(&msg); //skip 'card'

		switch (msg[0]) {
		case 'n': break;
		case 'y': card_type = CR_Yellow; break;
		case 'r': card_type = CR_Red; break;
		}
	}

	mpObserver->SetSenseBody(
			view_width,
			stamina,
			effort,
			capacity,

			speed,
			speed_dir,
			neck_dir,

			kicks,
			dashes,
			turns,
			says,
			turn_necks,
			catchs,
			moves,
			change_views,

			arm_movable_ban,
			arm_expires,
			arm_target_dist,
			arm_target_dir,
			points,

			focus_side,
			focus_unum,
			focuses,

			tackle_expires,
			tackles,

			foul_charged_cycle,
			card_type,
			mpObserver->CurrentTime()
	);

	NetworkTest::instance().SetCommandExecuteCount(
			dashes, kicks, turns, says, turn_necks, catchs, moves, change_views, points, tackles, focuses);
}

void Parser::ParseRefereeMsg(char *msg)
{
	if ( msg[0] == 'y' || msg[0] == 'r' ) ParseCard(msg);
	else ParsePlayMode(msg);
}

void Parser::ParseSound(char *msg)
{
	char *end;
	int n;
	static char buffer[MAX_MESSAGE];

	msg++;
	if (msg[0] == 'r') // referee
	{
		parser::get_next_word(&msg);
		end = msg;
		while (*end != ')') end++;
		n = end - msg;
		strncpy(buffer, msg, n * sizeof(char));
		buffer[n] = '\0';

		ParseRefereeMsg(buffer);
	}
	else if (msg[0] == 's') // self
	{
		// self say
	}
	else if (msg[0] == 'o') // coach say
	{
		msg += 13; /** online_coach_ */
		if (msg[0] == mpObserver->OurInitSide())
		{
			while (*msg != ' ') ++msg;
			++msg;

			end = msg;
			//  while (*end != ')' || *(end + 1) != ')') end++; // 对手阵型信息中有可能包含右括号
			n = strlen(msg);
			strncpy(buffer, msg, n * sizeof(char));
			buffer[n] = '\0';

			mpObserver->HearOurCoachSay(std::string(buffer));
		}
		else
		{
			// opponent coach say
		}
	}
	else // player say
	{ //dir our|opp unum
		AngleDeg dir = parser::get_double(&msg);
		if (msg[2] == 'u'){ //our
			Unum unum = parser::get_int(&msg);
			while (*msg != '\"') msg++;
			msg++;
			end = msg;
			while (*end != '\"') end++;
			n = end - msg;
			strncpy(buffer, msg, n * sizeof(char));
			buffer[n] = '\0';

			mpObserver->HearTeammateSay(dir, unum, std::string(buffer));
		}
		else
		{
			// opponent player say
		}
	}
}

void Parser::ParseOkMsg(char *msg)
{
	bool is_ok_say = false;

	msg++;
	parser::get_next_word(&msg);
	switch(msg[0])
	{
	case 'c':
		switch(msg[1])
		{
		case 'l': mOkMutex.Lock(); mClangOk = true; mOkMutex.UnLock(); break;
		case 'h':
			/*
			mOkMutex.Lock(); mChangePlayerTypeOk[parser::get_int(msg)] = true; mOkMutex.UnLock(); break;
			*/
				  mOkMutex.Lock();
				  if((!(mpObserver->SelfUnum() == TRAINER_UNUM)) || std::strncmp(parser::get_next_word(&msg), "WrightEagleBASE", 15) == 0) //Trainer接收到的信息包括己方和敌方，现在只处理己方
					  mChangePlayerTypeOk[parser::get_int(msg)] = true;
				  mOkMutex.UnLock();
				  break;
		default: PRINT_ERROR("unknow ok message " << msg); break;
		}
		break;
		case 's':
			switch(msg[1])
			{
			case 'y': mOkMutex.Lock(); mSynchOk = true; mOkMutex.UnLock(); break;
			case 'a': is_ok_say = true; break; /** (ok say) */
			default: PRINT_ERROR("unknow ok message " << msg); break;
			}
			break;
//			case 'e': mOkMutex.Lock(); mEyeOnOk = true; mOkMutex.UnLock(); break;
		case 'e':
			switch(msg[1])
			{
			case 'y': mOkMutex.Lock(); mEyeOnOk = true; mOkMutex.UnLock(); break;
			case 'a': mOkMutex.Lock(); mEarOnOk = true; mOkMutex.UnLock(); break;
			default: PRINT_ERROR("unknow ok message " << msg); break;
			}
			break;
		case 'm':
		case 'r': /*std::cout << "#" << PlayerParam::instance().teamName() << " Trainer: (" << msg << std::endl;*/ break;
		default: PRINT_ERROR("unknow ok message " << msg); break;
	}

	if (is_ok_say && (PlayerParam::instance().isCoach() || PlayerParam::instance().isTrainer()))
	{
		mpObserver->SetCoachSayCount(mpObserver->GetCoachSayCount() + 1);
	}
	else
	{
		if (mpObserver->SelfUnum() == 0)
		{
			std::cout << "#" << PlayerParam::instance().teamName() << " Coach: (" << msg << std::endl;
		}
		else if (mpObserver->SelfUnum() == TRAINER_UNUM )
		{
			/*std::cout << "#" << PlayerParam::instance().teamName() << " Trainer: (" << msg << std::endl;*/
		}
		else
		{
			std::cout << "#" << PlayerParam::instance().teamName() << " " << mpObserver->SelfUnum() << ": (" << msg << std::endl;
		}
	}
}

void Parser::ParsePlayMode(char *msg)
{
	ServerPlayMode spm = ServerPlayModeMap::instance().GetServerPlayMode(msg);

	if( spm == SPM_Null ){
		PRINT_ERROR("unknown server play mode: " << msg << " @ " << mpObserver->CurrentTime());
		return;
	}

	PlayMode pm = PM_No_Mode;
	mpObserver->SetServerPlayMode(spm);
	switch( spm ){
	case SPM_PlayOn:
		pm = PM_Play_On;
		break;           /* play_on */
	case SPM_KickIn_Left:                                /* kick_in */
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Kick_In : PM_Opp_Kick_In;
		break;
	case SPM_KickIn_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Kick_In : PM_Opp_Kick_In;
		break;
	case SPM_KickOff_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Kick_Off : PM_Opp_Kick_Off;
		break;
	case SPM_KickOff_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Kick_Off : PM_Opp_Kick_Off;
		break;
	case SPM_GoalKick_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Goal_Kick : PM_Opp_Goal_Kick;
		break;
	case SPM_GoalKick_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Goal_Kick : PM_Opp_Goal_Kick;
		break;
	case SPM_AfterGoal_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Goal_Ours : PM_Goal_Opps;
		break;
	case SPM_AfterGoal_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Goal_Ours : PM_Goal_Opps;
		break;
	case SPM_CornerKick_Left:                                               /* corner_kick */
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Corner_Kick : PM_Opp_Corner_Kick;
		break;
	case SPM_CornerKick_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Corner_Kick : PM_Opp_Corner_Kick;
		break;
	case SPM_Drop_Ball:
		pm = PM_Drop_Ball;
		break;        /* drop_ball */
	case SPM_OffSide_Left:                                               /* offside */
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Opp_Offside_Kick : PM_Our_Offside_Kick;
		break;
	case SPM_OffSide_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Opp_Offside_Kick : PM_Our_Offside_Kick;
		break;
	case SPM_CatchFault_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Opp_CatchFault_Kick : PM_Our_CatchFault_Kick;
		break;
	case SPM_CatchFault_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Opp_CatchFault_Kick : PM_Our_CatchFault_Kick;
		break;
	case SPM_FreeKick_Left:
		if(mLastServerPlayMode == SPM_GoalieCatchBall_Left)
			pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Goalie_Free_Kick : PM_Opp_Goalie_Free_Kick;
		else
			pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Free_Kick : PM_Opp_Free_Kick;
		break;
	case SPM_FreeKick_Right:
		if(mLastServerPlayMode == SPM_GoalieCatchBall_Right)
			pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Goalie_Free_Kick : PM_Opp_Goalie_Free_Kick;
		else
			pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Free_Kick : PM_Opp_Free_Kick;
		break;
	case SPM_Free_Kick_Fault_Left:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Free_Kick_Fault_Kick : PM_Opp_Free_Kick_Fault_Kick;
		break;
	case SPM_Free_Kick_Fault_Right:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Free_Kick_Fault_Kick : PM_Opp_Free_Kick_Fault_Kick;
		break;
	case SPM_FirstHalfOver:                                               /* half_time */
		pm = PM_Half_Time;  /* play_mode to before_kick_off        */
		break;
	case SPM_BeforeKickOff:
		pm = PM_Before_Kick_Off; break;       /* before_kick_off */
		break;
	case SPM_Back_Pass_Left:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Back_Pass_Kick : PM_Opp_Back_Pass_Kick;
		break;
	case SPM_Back_Pass_Right:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Back_Pass_Kick : PM_Opp_Back_Pass_Kick;
		break;
	case SPM_TimeOver:
		pm = PM_Time_Over;
		break;
	case SPM_TimeUp:
		pm = PM_Time_Up;
		break;
	case SPM_IndFreeKick_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Indirect_Free_Kick : PM_Opp_Indirect_Free_Kick;
		break;
	case SPM_IndFreeKick_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Indirect_Free_Kick : PM_Opp_Indirect_Free_Kick;
		break;
	case SPM_PenaltySetup_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Penalty_Setup : PM_Opp_Penalty_Setup;
		break;
	case SPM_PenaltySetup_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Penalty_Setup : PM_Opp_Penalty_Setup;
		break;
	case SPM_PenaltyReady_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Penalty_Ready : PM_Opp_Penalty_Ready;
		break;
	case SPM_PenaltyReady_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Penalty_Ready : PM_Opp_Penalty_Ready;
		break;
	case SPM_PenaltyTaken_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Penalty_Taken : PM_Opp_Penalty_Taken;
		break;
	case SPM_PenaltyTaken_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Penalty_Taken : PM_Opp_Penalty_Taken;
		break;
	case SPM_PenaltyScore_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Penalty_Score : PM_Opp_Penalty_Score;
		break;
	case SPM_PenaltyScore_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Penalty_Score : PM_Opp_Penalty_Score;
		break;
	case SPM_PenaltyMiss_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Penalty_Miss : PM_Opp_Penalty_Miss;
		break;
	case SPM_PenaltyMiss_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Penalty_Miss : PM_Opp_Penalty_Miss;
		break;
	case SPM_GoalieCatchBall_Left:
		pm = mpObserver->GetPlayMode();
		break;
	case SPM_GoalieCatchBall_Right:
		pm = mpObserver->GetPlayMode();
		break;
	case SPM_HalfTime:
		pm = PM_Half_Time;
		break;
	case SPM_PenaltyOnfield_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Penalty_On_Our_Field : PM_Penalty_On_Opp_Field;
		break;
	case SPM_PenaltyOnfield_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Penalty_On_Our_Field : PM_Penalty_On_Opp_Field;
		break;
	case SPM_PenaltyFoul_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Penalty_Foul : PM_Opp_Penalty_Foul;
		break;
	case SPM_PenaltyFoul_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Penalty_Foul : PM_Opp_Penalty_Foul;
		break;
	case SPM_PenaltyWinner_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Penalty_Winner : PM_Opp_Penalty_Winner;
		break;
	case SPM_PenaltyWinner_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Penalty_Winner : PM_Opp_Penalty_Winner;
		break;
	case SPM_Foul_Left:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Foul : PM_Opp_Foul;
		break;
	case SPM_Foul_Right:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Foul : PM_Opp_Foul;
		break;
	case SPM_TimeExtended:
		pm = PM_Extended_Time;
		break;
	case SPM_Foul_Charge_Left:
		pm = ( mpObserver->OurInitSide() == 'r' ) ? PM_Our_Foul_Charge_Kick : PM_Opp_Foul_Charge_Kick;
		break;
	case SPM_Foul_Charge_Right:
		pm = ( mpObserver->OurInitSide() == 'l' ) ? PM_Our_Foul_Charge_Kick : PM_Opp_Foul_Charge_Kick;
		break;
	default:
		PRINT_ERROR("unknown server play mode: " << msg << " @ " << mpObserver->CurrentTime());
		break;
	}
	mLastServerPlayMode = spm;

	if (pm == PM_Penalty_On_Our_Field)
	{
		if (mpObserver->SelfUnum() != PlayerParam::instance().ourGoalieUnum())
		{
			mpObserver->SetOurSide((mpObserver->OurInitSide() == 'l') ? 'r' : 'l');
			mpObserver->Initialize();
		}
	}
	else if (pm == PM_Penalty_On_Opp_Field)
	{
		if (mpObserver->SelfUnum() == PlayerParam::instance().ourGoalieUnum())
		{
			mpObserver->SetOurSide((mpObserver->OurInitSide() == 'l') ? 'r' : 'l');
			mpObserver->Initialize();
		}
	}


	if (pm != PM_No_Mode){
		switch( pm ){
		case PM_Goal_Ours:
			mpObserver->OurScoreInc();
			mpObserver->SetKickOffMode(KO_Opps);
			pm = PM_Before_Kick_Off;
			break;
		case PM_Goal_Opps:
			mpObserver->OppScoreInc();
			mpObserver->SetKickOffMode(KO_Ours);
			pm = PM_Before_Kick_Off;
			break;
		case PM_Our_Penalty_Score:
			mpObserver->OurScoreInc();
			break;
		case PM_Opp_Penalty_Score:
			mpObserver->OppScoreInc();
			break;
		case PM_Half_Time:
		case PM_Extended_Time:
			++ mHalfTime;
			if (mHalfTime % 2 == 1) {
				mpObserver->SetKickOffMode((mpObserver->OurInitSide() == 'l') ? KO_Ours : KO_Opps);
			}
			else {
				mpObserver->SetKickOffMode((mpObserver->OurInitSide() == 'r') ? KO_Ours : KO_Opps);
			}

			break;
		case PM_Drop_Ball:
			mpObserver->DropBall();
			break;
		default:
			break;
		}
		mpObserver->SetPlayMode(pm);
	}
	else {
		PRINT_ERROR("Unkonow playmode : " << msg << " @ " << mpObserver->CurrentTime());
	}
}

void Parser::ParseCard(char *msg)
{
	CardType card_type;
	switch ( msg[0] )
	{
	case 'y':
		card_type = CR_Yellow;
		break;
	case 'r':
		card_type = CR_Red;
		break;
	default:
		card_type = CR_None;
		break;
	}
	Unum player;

	if ( msg[7] == 'd' )
	{
		if ( msg[9] == mpObserver->OurInitSide() )
		{
			player = parser::get_int( &msg );
			mpObserver->SetTeammateCardType( player, card_type );
		}
		else
		{
			player = parser::get_int( &msg );
			mpObserver->SetOpponentCardType( player, card_type );
		}
	}
	else
	{
		if ( msg[12] == mpObserver->OurInitSide() )
		{
			player = parser::get_int( &msg );
			mpObserver->SetTeammateCardType( player, card_type );
		}
		else
		{
			player = parser::get_int( &msg );
			mpObserver->SetOpponentCardType( player, card_type );
		}
	}
}



void Parser::ParseSight_Coach(char *msg)
{
	if (PlayerParam::instance().isCoach() || PlayerParam::instance().isTrainer())
	{
		TimeTest::instance().Update(mpObserver->CurrentTime()); // coach每周期都有sight
	}

	for (int i = 1; i <= TEAMSIZE; ++i){
		mpObserver->Teammate_Coach(i).SetIsAlive(false);
		mpObserver->Opponent_Coach(i).SetIsAlive(false);
	}

	msg = strstr(msg,"((");

	while ( msg != 0 ){ // 直到没有object为止
		msg += 2; // 跳过 ((
		ObjType obj = ParseObjType(msg); // 获得object的类型
		msg = strchr(msg,')');
		ObjProperty_Coach prop = ParseObjProperty_Coach(msg + 1);  // 获得object的属性

		switch ( obj.type ) {
		case OBJ_Ball:
			mpObserver->Ball_Coach().UpdatePos(Vector(prop.x, prop.y), 0, 1.0);
			mpObserver->Ball_Coach().UpdateVel(Vector(prop.vx, prop.vy), 0, 1.0);
			break;
		case OBJ_Player:
			if (obj.side == mpObserver->OurSide()){
				mpObserver->Teammate_Coach(obj.num).SetIsAlive(true);
				mpObserver->Teammate_Coach(obj.num).UpdatePos(Vector(prop.x, prop.y), 0, 1.0);
				mpObserver->Teammate_Coach(obj.num).UpdateVel(Vector(prop.vx, prop.vy), 0, 1.0);
				mpObserver->Teammate_Coach(obj.num).UpdateBodyDir(prop.body_dir, 0, 1.0);
				mpObserver->Teammate_Coach(obj.num).UpdateNeckDir(prop.head_dir, 0, 1.0);
				mpObserver->Teammate_Coach(obj.num).UpdateCardType(prop.card_type);
				if (prop.pointing){
					mpObserver->Teammate_Coach(obj.num).UpdateArmPoint(prop.point_dir, 0, 1.0, 0.0, 0, 0);
					//TODO: 这个接口不好，因为是看不到dist，ban等信息的，这些要自己算然后维护
				}
				if (prop.tackling){
					if (mpObserver->Teammate_Coach(obj.num).GetTackleBan() == 0) {
						mpObserver->Teammate_Coach(obj.num).UpdateTackleBan(ServerParam::instance().tackleCycles() - 1);
					}
				}
				else
				{
					mpObserver->Teammate_Coach(obj.num).UpdateTackleBan(0);
				}
				if (prop.lying){
					if (mpObserver->Teammate_Coach(obj.num).GetFoulChargedCycle() == 0) {
						mpObserver->Teammate_Coach(obj.num).UpdateFoulChargedCycle(ServerParam::instance().foulCycles() - 1);
					}
				}
				else
				{
					mpObserver->Teammate_Coach(obj.num).UpdateFoulChargedCycle(0);
				}
			}
			else {
				mpObserver->Opponent_Coach(obj.num).SetIsAlive(true);
				mpObserver->Opponent_Coach(obj.num).UpdatePos(Vector(prop.x, prop.y), 0, 1.0);
				mpObserver->Opponent_Coach(obj.num).UpdateVel(Vector(prop.vx, prop.vy), 0, 1.0);
				mpObserver->Opponent_Coach(obj.num).UpdateBodyDir(prop.body_dir, 0, 1.0);
				mpObserver->Opponent_Coach(obj.num).UpdateNeckDir(prop.head_dir, 0, 1.0);
				mpObserver->Opponent_Coach(obj.num).UpdateCardType(prop.card_type);
				if (prop.pointing){
					mpObserver->Opponent_Coach(obj.num).UpdateArmPoint(prop.point_dir, 0, 1.0, 0.0, 0, 0);
					//TODO: 这个接口不好，因为是看不到dist，ban等信息的，这些要自己算然后维护
				}
				if (prop.tackling){
					if (mpObserver->Opponent_Coach(obj.num).GetTackleBan() == 0) {
						mpObserver->Opponent_Coach(obj.num).UpdateTackleBan(ServerParam::instance().tackleCycles() - 1);
					}
				}
				else
				{
					mpObserver->Opponent_Coach(obj.num).UpdateTackleBan(0);
				}
				if (prop.lying){
					if (mpObserver->Opponent_Coach(obj.num).GetFoulChargedCycle() == 0) {
						mpObserver->Opponent_Coach(obj.num).UpdateFoulChargedCycle(ServerParam::instance().foulCycles() - 1);
					}
				}
				else
				{
					mpObserver->Opponent_Coach(obj.num).UpdateFoulChargedCycle(0);
				}
			}
			break;
		default:
			break;
		}
		msg = strstr(msg,"(("); // 下一个object
	}
}

bool Parser::ParseForTrainer(char *msg)
{
	char *end;
	int n;
	static char buffer[MAX_MESSAGE];

	if (msg[6] == 'r') // 跳过括号和hear，referee
	{
		ParseTime(msg, &msg, true);
		parser::get_word(&msg);
		end = msg;
		while (*end != ')') end++;
		n = end - msg;
		strncpy(buffer, msg, n * sizeof(char));
		buffer[n] = '\0';

		ParseRefereeMsg(buffer);
		return true;
	}
	else
		return false;
}
