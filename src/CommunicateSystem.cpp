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

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include "CommunicateSystem.h"
#include "WorldState.h"
#include "Agent.h"
#include "ServerParam.h"
#include "PlayerParam.h"
#include "Logger.h"
#include "Formation.h"
using namespace std;

const unsigned char *CommunicateSystem::CODE = (const unsigned char *)"uMKJNPpA1Yh0)f6_x3WU<>SgQ4wbDizV5dc9t2XZ?(/7*s.FEHvLG8yRTkej-OlB+armnoqCI";

int CommunicateSystem::CODE_TO_INT[128];
const int CommunicateSystem::CODE_SIZE = 73;
const int CommunicateSystem::MAX_MSG_SIZE = 10;
int CommunicateSystem::MAX_BITS_USED = 61;

CommunicateSystem::CommunicateSystem() {
	memset(CODE_TO_INT, 0, sizeof(CODE_TO_INT));
	for (int i = 0; i < CODE_SIZE; ++i){
		CODE_TO_INT[CODE[i]] = i;
	}

	memset(mCodecBitCount, 0, sizeof(mCodecBitCount));
	memset(mCodecMask, 0, sizeof(mCodecMask));

	mCommuFlagBitCount = static_cast<int>(ceil(log(static_cast<double>(COMMU_MAX)) / log(2.0)));
	mCommuFlagMask = (1 << mCommuFlagBitCount) - 1;

	mFreeFormFlagBitCount = static_cast<int>(ceil(log(static_cast<double>(FREE_FORM_MAX)) / log(2.0)));
	mFreeFormFlagMask = (1 << mFreeFormFlagBitCount) - 1;

	mUnumBitCount = static_cast<int>(ceil(log(static_cast<double>(TEAMSIZE)) / log(2.0)));
	mUnumMask = (1 << mUnumBitCount) - 1;

	mTacticsBitCount = 5;
	mTacticsFlagMask = (1 << mTacticsBitCount) - 1;

	mCodecBitCount[FREE_FORM][POS_X] = BitCountOfEps(PlayerParam::instance().sayPosXEps(), POS_X);
	mCodecMask[FREE_FORM][POS_X] = (1 << mCodecBitCount[FREE_FORM][POS_X]) - 1;
	mCodecBitCount[FREE_FORM][POS_Y] = BitCountOfEps(PlayerParam::instance().sayPosYEps(), POS_Y);
	mCodecMask[FREE_FORM][POS_Y] = (1 << mCodecBitCount[FREE_FORM][POS_Y]) - 1;
	mCodecBitCount[FREE_FORM][BALL_SPEED] = BitCountOfEps(PlayerParam::instance().sayBallSpeedEps(), BALL_SPEED);
	mCodecMask[FREE_FORM][BALL_SPEED] = (1 << mCodecBitCount[FREE_FORM][BALL_SPEED]) - 1;
	mCodecBitCount[FREE_FORM][PLAYER_SPEED] = BitCountOfEps(PlayerParam::instance().sayPlayerSpeedEps(), BALL_SPEED);
	mCodecMask[FREE_FORM][PLAYER_SPEED] = (1 << mCodecBitCount[FREE_FORM][PLAYER_SPEED]) - 1;
	mCodecBitCount[FREE_FORM][DIR] = BitCountOfEps(PlayerParam::instance().sayDirEps(), DIR);
	mCodecMask[FREE_FORM][DIR] = (1 << mCodecBitCount[FREE_FORM][DIR]) - 1;

	mFreeFormCodecBitCount[BALL_WITH_SPEED] = mCodecBitCount[FREE_FORM][POS_X] + mCodecBitCount[FREE_FORM][POS_Y] + mCodecBitCount[FREE_FORM][BALL_SPEED] + mCodecBitCount[FREE_FORM][DIR] + mFreeFormFlagBitCount;
	mFreeFormCodecBitCount[BALL_WITH_ZERO_SPEED] = mCodecBitCount[FREE_FORM][POS_X] + mCodecBitCount[FREE_FORM][POS_Y] + mFreeFormFlagBitCount;
	mFreeFormCodecBitCount[BALL_ONLY_POS] = mCodecBitCount[FREE_FORM][POS_X] + mCodecBitCount[FREE_FORM][POS_Y] + mFreeFormFlagBitCount;
	mFreeFormCodecBitCount[TEAMMATE_ONLY_POS] = mCodecBitCount[FREE_FORM][POS_X] + mCodecBitCount[FREE_FORM][POS_Y] + mFreeFormFlagBitCount;
	mFreeFormCodecBitCount[OPPONENT_ONLY_POS] = mCodecBitCount[FREE_FORM][POS_X] + mCodecBitCount[FREE_FORM][POS_Y] + mFreeFormFlagBitCount;

	MAX_BITS_USED -= mCommuFlagBitCount; //这个是一定要用的位
}

CommunicateSystem::~CommunicateSystem() {
}

CommunicateSystem & CommunicateSystem::instance()
{
    static CommunicateSystem communicate_system;
    return communicate_system;
}

void CommunicateSystem::Initial(Observer *observer , Agent* agent)
{
    mpObserver = observer;
	mpAgent  = agent;
}

int CommunicateSystem::BitCountOfEps(double eps, CodecType type)
{
	double min = 0.0, max = 1.0;
	GetCodecRange(type, min, max);

	double range = (max - min) / eps;
	return static_cast<int>(ceil(log(range) / log(2.0)));
}


void CommunicateSystem::GetCodecRange(CodecType type, double &min, double &max)
{
	switch (type){
	case POS_X: min = -ServerParam::instance().PITCH_LENGTH * 0.5; max = ServerParam::instance().PITCH_LENGTH * 0.5; break;
	case POS_Y: min = -ServerParam::instance().PITCH_WIDTH * 0.5; max = ServerParam::instance().PITCH_WIDTH * 0.5; break;
	case BALL_SPEED: min = 0.0; max = ServerParam::instance().ballSpeedMax(); break;
	case PLAYER_SPEED: min = 0.0; max = ServerParam::instance().playerSpeedMax(); break;
	case DIR: min = -180.0; max = 180.0; break;
	default: PRINT_ERROR("unknown codec type"); break;
	}
}

CommunicateSystem::DWORD64 CommunicateSystem::DoubleToBit(double value, CommunicateSystem::CodecType type)
{
	int range = static_cast<int>((*mpCodecMask)[type]); // [0, range]

	double min = 0.0, max = 1.0;
	GetCodecRange(type, min, max);

	if(value > max)
	{
		value = max - 0.001 * max;
	}
	else if (value < min)
	{
		value = min + 0.001 * min;
	}

	return static_cast<DWORD64>(((value - min) / (max - min) * static_cast<double>(range)));
}

double CommunicateSystem::BitToDouble(DWORD64 bits, CommunicateSystem::CodecType type)
{
	int range = static_cast<int>((*mpCodecMask)[type]); // [0, range]
	DWORD64 mask = (*mpCodecMask)[type];

	double min = 0.0, max = 1.0;
	GetCodecRange(type, min, max);

	return static_cast<double>(bits & mask) / static_cast<double>(range) * (max - min) + min;
}

void CommunicateSystem::Encode(DWORD64 bits, unsigned char *msg , bool is_coach)
{
	int i = 0, rem;

	while (bits != 0 || (is_coach && i < MAX_MSG_SIZE) ){
		rem = bits % CODE_SIZE;
		bits = (bits - rem) / CODE_SIZE;
		if (i >= MAX_MSG_SIZE){
			PRINT_ERROR("codec msg size greater than " << MAX_MSG_SIZE);
			break;
		}
		msg[i++] = CODE[rem];
	}
	msg[i] = '\0';
}

void CommunicateSystem::Decode(const unsigned char *msg, DWORD64& bits , bool is_coach)
{
	int n = is_coach ? MAX_MSG_SIZE : strlen((const char *)msg);
	bits = 0;
	DWORD64 plus = 1;
	for (int i = 0; i < n; ++i){
		bits += CODE_TO_INT[msg[i]] * plus;
		plus *= CODE_SIZE;
	}
}

void CommunicateSystem::PrintBits(DWORD64 bits)
{
	UDWORD64 mask = 1;
	mask <<= 63;

	while (mask != 0){
		if (bits & mask) cerr << '1' ;
		else cerr << '0';
		mask >>= 1;
	}
	cerr << endl;
}

void CommunicateSystem::Update()
{
	//some reset work
	mCommuBits = 0;
	mBitsUsed = 0;
	mBallSended = false;

	mTeammateSended.bzero();
	mOpponentSended.bzero();

	mCommuType = FREE_FORM;
	SetCommunicateType(FREE_FORM);

	// our coach say
    if (mpObserver->Audio().IsOurCoachSayValid())
    {
    	;
	}

    // teammate say
    if (mpObserver->Audio().IsTeammateSayValid())
    {
        ParseReceivedTeammateMsg((unsigned char *)(mpObserver->Audio().GetTeammateSayContent().c_str()));
    }
}

bool CommunicateSystem::AddDataToCommuBits(double value, CodecType type)
{
	if (mBitsUsed + (*mpCodecBitCount)[type] > MAX_BITS_USED){
		PRINT_ERROR("bits used greater then " << MAX_BITS_USED);
		return false;
	}
	else {
		mCommuBits <<= (*mpCodecBitCount)[type];
		mCommuBits += DoubleToBit(value, type);
		mBitsUsed += (*mpCodecBitCount)[type];

		return true;
	}
}

bool CommunicateSystem::AddFreeFormFlagToCommuBits(FreeFormType type)
{
	if (mBitsUsed + mFreeFormFlagBitCount > MAX_BITS_USED){
		PRINT_ERROR("bits used greater then " << MAX_BITS_USED);
		return false;
	}
	else {
		mCommuBits <<= mFreeFormFlagBitCount;
		mCommuBits += static_cast<DWORD64>(type);
		mBitsUsed += mFreeFormFlagBitCount;

		return true;
	}
}

bool CommunicateSystem::AddCommuFlagToCommuBits(CommuType type)
{
	mCommuBits <<= mCommuFlagBitCount;
	mCommuBits += static_cast<DWORD64>(type);
	mBitsUsed += mCommuFlagBitCount;

	return true;
}

bool CommunicateSystem::AddUnumToCommuBits(Unum num)
{
	if (mBitsUsed + mUnumBitCount > MAX_BITS_USED){
		PRINT_ERROR("bits used greater then " << MAX_BITS_USED);
		return false;
	}
	else {
		mCommuBits <<= mUnumBitCount;
		mCommuBits += static_cast<DWORD64>(num);
		mBitsUsed += mUnumBitCount;

		return true;
	}
}

bool CommunicateSystem::SendBallStatus(const BallState &ball_state, int cd)
{
	if (mCommuType != FREE_FORM){
		return false;
	}

	SetCommunicateType(FREE_FORM);

	if (mBallSended == true){
		return false;
	}

	int pos_cd = ball_state.GetPosDelay();
	int vel_cd = ball_state.GetVelDelay();
	Vector pos = ball_state.GetPos();
	Vector vel = ball_state.GetVel();
	FreeFormType send_type = FREE_FORM_MAX;

	if (pos_cd <= cd){
		if (mFreeFormCodecBitCount[BALL_ONLY_POS] + mBitsUsed <= MAX_BITS_USED){
			if (vel_cd <= cd){
				if (vel.Mod2() > FLOAT_EPS){ //BALL_WITH_SPEED
					if (mFreeFormCodecBitCount[BALL_WITH_SPEED] + mBitsUsed <= MAX_BITS_USED){
						send_type = BALL_WITH_SPEED;
					}
					else { //BALL_ONLY_POS
						send_type = BALL_ONLY_POS;
					}
				}
				else { //BALL_WITH_ZERO_SPEED
					if (mFreeFormCodecBitCount[BALL_WITH_ZERO_SPEED] + mBitsUsed <= MAX_BITS_USED){
						send_type = BALL_WITH_ZERO_SPEED;
					}
					else { //BALL_ONLY_POS
						send_type = BALL_ONLY_POS;
					}
				}
			}
			else { //BALL_ONLY_POS
				send_type = BALL_ONLY_POS;
			}
		}
	}
	if (send_type != FREE_FORM_MAX){
		switch(send_type){
		case BALL_WITH_SPEED:
			AddDataToCommuBits(pos.X(), POS_X);
			AddDataToCommuBits(pos.Y(), POS_Y);
			AddDataToCommuBits(vel.Mod(), BALL_SPEED);
			AddDataToCommuBits(vel.Dir(), DIR);
			AddFreeFormFlagToCommuBits(BALL_WITH_SPEED);
			mBallSended = true;
			Logger::instance().GetTextLogger("freeform") << mpAgent->GetWorldState().CurrentTime() << " send ball: " << pos << " " << vel << endl;
			return true;
		case BALL_WITH_ZERO_SPEED:
			AddDataToCommuBits(pos.X(), POS_X);
			AddDataToCommuBits(pos.Y(), POS_Y);
			AddFreeFormFlagToCommuBits(BALL_WITH_ZERO_SPEED);
			mBallSended = true;
			Logger::instance().GetTextLogger("freeform") << mpAgent->GetWorldState().CurrentTime() << " send ball: " << pos << " " << Vector(0, 0) << endl;
			return true;
		case BALL_ONLY_POS:
			AddDataToCommuBits(pos.X(), POS_X);
			AddDataToCommuBits(pos.Y(), POS_Y);
			AddFreeFormFlagToCommuBits(BALL_ONLY_POS);
			Logger::instance().GetTextLogger("freeform") << mpAgent->GetWorldState().CurrentTime() << " send ball: " << pos << endl;
			mBallSended = true;
			return true;
		default: PRINT_ERROR("send ball status"); break;
		}
	}
	return false;
}

bool CommunicateSystem::SendTeammateStatus(const WorldState *pWorldState, Unum num, int cd)
{
	Assert(num > 0);

	if (mCommuType != FREE_FORM){
		return false;
	}

	SetCommunicateType(FREE_FORM);

	if (mTeammateSended[num] == true){
		return false;
	}
	if (pWorldState->GetTeammate(num).IsAlive() == false){
		return false;
	}
	if (mpAgent->GetInfoState().GetPositionInfo().GetPlayerDistToPlayer(mpAgent->GetSelfUnum(), num) > ServerParam::instance().unumFarLength()) {
		return false;
	}

	int pos_cd = pWorldState->GetTeammate(num).GetPosDelay();
	if (pos_cd <= cd){
		if (mFreeFormCodecBitCount[TEAMMATE_ONLY_POS] + mBitsUsed <= MAX_BITS_USED){
			Vector pos = pWorldState->GetTeammate(num).GetPos();
			AddUnumToCommuBits(num);
			AddDataToCommuBits(pos.X(), POS_X);
			AddDataToCommuBits(pos.Y(), POS_Y);
			AddFreeFormFlagToCommuBits(TEAMMATE_ONLY_POS);
			Logger::instance().GetTextLogger("freeform") << mpAgent->GetWorldState().CurrentTime() << " send tm: " << num << " " << pos << endl;
			mTeammateSended[num] = true;
			return true;
		}
	}
	return false;
}

bool CommunicateSystem::SendOpponentStatus(const WorldState *pWorldState, Unum num, int cd)
{
	Assert(num > 0);

	if (mCommuType != FREE_FORM){
		return false;
	}

	SetCommunicateType(FREE_FORM);

	if (mOpponentSended[num] == true){
		return false;
	}
	if (pWorldState->GetOpponent(num).IsAlive() == false){
		return false;
	}
	if (mpAgent->GetInfoState().GetPositionInfo().GetPlayerDistToPlayer(mpAgent->GetSelfUnum(), -num) > ServerParam::instance().unumFarLength()) {
		return false;
	}

	int pos_cd = pWorldState->GetOpponent(num).GetPosDelay();
	if (pos_cd <= cd){
		if (mFreeFormCodecBitCount[OPPONENT_ONLY_POS] + mBitsUsed <= MAX_BITS_USED){
			Vector pos = pWorldState->GetOpponent(num).GetPos();
			AddUnumToCommuBits(num);
			AddDataToCommuBits(pos.X(), POS_X);
			AddDataToCommuBits(pos.Y(), POS_Y);
			AddFreeFormFlagToCommuBits(OPPONENT_ONLY_POS);
			mOpponentSended[num] = true;
			Logger::instance().GetTextLogger("freeform") << mpAgent->GetWorldState().CurrentTime() << " send opp: " << num << " " << pos << endl;
			return true;
		}
	}
	return false;
}

void CommunicateSystem::Decision()
{
	DoCommunication();

	if (mBitsUsed != 0){
		unsigned char msg[11]; //MAX_MSG_SIZE == 10

		AddCommuFlagToCommuBits(mCommuType);

		DWORD64 mask = 1;
		mask <<= mBitsUsed;
		mask -= 1;
		mCommuBits += ~mask; //左边补1
		mask = 1;
		mask <<= (MAX_BITS_USED + mCommuFlagBitCount);
		mask -= 1;
		mCommuBits &= mask; //去掉高三位

		Encode(mCommuBits, msg);

		if (strlen((const char *)msg) > 0){
			mpAgent->Say(string((const char *)msg));
		}
		else {
			PRINT_ERROR("bug here?");
		}
	}
}

double CommunicateSystem::ExtractDataFromBits(DWORD64 &bits, CodecType type, int &bit_left)
{
	double res;

	res = BitToDouble(bits, type);
	bits >>= (*mpCodecBitCount)[type];
	bit_left -= (*mpCodecBitCount)[type];

	return res;
}

int CommunicateSystem::ExtractUnumFromBits(DWORD64 &bits, int &bit_left)
{
	int num;

	num = static_cast<int>(bits & mUnumMask);
	bits >>= mUnumBitCount;
	bit_left -= mUnumBitCount;

	return num;
}

void CommunicateSystem::RecvFreeForm(DWORD64 bits)
{
	SetCommunicateType(FREE_FORM);

	FreeFormType type;
	int bit_left = MAX_BITS_USED;
	while (bit_left > 0){
		 type = static_cast<FreeFormType>(bits & mFreeFormFlagMask);
		 if (type >= FREE_FORM_DUMMY){
			 break; //mCommuBits 的左端全是1，可以用来作为结束标志，因为FREE_FORM_DUMMY是不用的
		 }
		 if (bit_left < mFreeFormCodecBitCount[type]){
			 break;
		 }

		 bits >>= mFreeFormFlagBitCount;
		 bit_left -= mFreeFormFlagBitCount;

		 if (type == BALL_WITH_SPEED){
			 AngleDeg dir = ExtractDataFromBits(bits, DIR, bit_left);
			 double speed = ExtractDataFromBits(bits, BALL_SPEED, bit_left);
			 double y = ExtractDataFromBits(bits, POS_Y, bit_left);
			 double x = ExtractDataFromBits(bits, POS_X, bit_left);
			 mpObserver->HearBall(Vector(x, y), Polar2Vector(speed, dir));
			 Logger::instance().GetTextLogger("freeform") << mpObserver->CurrentTime() << " hear ball: " << Vector(x, y) << " " << Polar2Vector(speed, dir) << endl;
		 }
		 else if (type == BALL_WITH_ZERO_SPEED){
			 double y = ExtractDataFromBits(bits, POS_Y, bit_left);
			 double x = ExtractDataFromBits(bits, POS_X, bit_left);
			 mpObserver->HearBall(Vector(x, y), Vector(0.0, 0.0));
			 Logger::instance().GetTextLogger("freeform") << mpObserver->CurrentTime() << " hear ball: " << Vector(x, y) << " " << Polar2Vector(0, 0) << endl;
		 }
		 else if (type == BALL_ONLY_POS){
			 double y = ExtractDataFromBits(bits, POS_Y, bit_left);
			 double x = ExtractDataFromBits(bits, POS_X, bit_left);
			 mpObserver->HearBall(Vector(x, y));
			 Logger::instance().GetTextLogger("freeform") << mpObserver->CurrentTime() << " hear ball: " << Vector(x, y) << endl;
		 }
		 else if (type == TEAMMATE_ONLY_POS){
			 double y = ExtractDataFromBits(bits, POS_Y, bit_left);
			 double x = ExtractDataFromBits(bits, POS_X, bit_left);
			 Unum num = ExtractUnumFromBits(bits, bit_left);
			 mpObserver->HearTeammate(num, Vector(x, y));
			 Logger::instance().GetTextLogger("freeform") << mpObserver->CurrentTime() << " hear tm: " << num << " " << Vector(x, y) << endl;
		 }
		 else if (type == OPPONENT_ONLY_POS){
			 double y = ExtractDataFromBits(bits, POS_Y, bit_left);
			 double x = ExtractDataFromBits(bits, POS_X, bit_left);
			 Unum num = ExtractUnumFromBits(bits, bit_left);
			 mpObserver->HearOpponent(num, Vector(x, y));
			 Logger::instance().GetTextLogger("freeform") << mpObserver->CurrentTime() << " hear opp: " << num << " " << Vector(x, y) << endl;
		 }
		 else {
			 PRINT_ERROR("recv free form error");
			 return;
		 }
	}
}

void CommunicateSystem::ParseReceivedTeammateMsg(unsigned char *msg)
{
	DWORD64 bits;
	//cerr << mpObserver->CurrentTime() << " (h) " << msg << endl;

	Decode(msg, bits);

	DWORD64 mask = 1;
	mask <<= (MAX_BITS_USED + mCommuFlagBitCount);
	mask -= 1;
	bits += ~mask; //高三位补1

	CommuType type = static_cast<CommuType>(bits & mCommuFlagMask);
	bits >>= mCommuFlagBitCount;

	if (type == FREE_FORM){
		Logger::instance().GetTextLogger("receive") << "freeform" << endl;
		RecvFreeForm(bits);
	}
	else {
		Logger::instance().GetTextLogger("receive") << "???" << endl;
		PRINT_ERROR(msg);
		//TODO: 其他信息
	}
}


void CommunicateSystem::DoCommunication()
{
	if (!mpAgent) return;

	PositionInfo & position_info = mpAgent->GetInfoState().GetPositionInfo();
	const Unum closest_tm = position_info.GetClosestTeammateToBall();

	if (closest_tm == mpAgent->GetSelfUnum()) {
		mpAgent->AttentiontoOff();
	}
	else if (closest_tm) {
		if (closest_tm != mpAgent->GetSelf().GetFocusOnUnum()) {
			mpAgent->Attentionto(closest_tm);
		}
	}

	SendBallStatus(mpAgent->GetWorldState().GetBall());
	SendTeammateStatus(& mpAgent->GetWorldState(), mpAgent->GetSelfUnum());

	const vector<Unum> & o2b = mpAgent->GetInfoState().GetPositionInfo().GetCloseOpponentToTeammate(mpAgent->GetSelfUnum());
	const vector<Unum> & t2b = mpAgent->GetInfoState().GetPositionInfo().GetCloseTeammateToTeammate(mpAgent->GetSelfUnum());

	for (uint i = 0; i < TEAMSIZE; ++i) {
		if (i < o2b.size()) {
			SendOpponentStatus(& mpAgent->GetWorldState(), o2b[i]);
		}
		if (i < t2b.size()) {
			SendTeammateStatus(& mpAgent->GetWorldState(), t2b[i]);
		}
	}
}
