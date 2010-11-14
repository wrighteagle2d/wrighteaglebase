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

#ifndef COMMUNICATESYSTEM_H_
#define COMMUNICATESYSTEM_H_

#ifdef WIN32
#include <Winsock2.h>
#include <windows.h>
#else
#include <stdint.h>
#endif

#include "Types.h"
#include "Geometry.h"
#include "BehaviorBase.h"

class WorldState;
class BallState;
class Observer;
class Agent;
class OpponentFormation;
class HearMessage;

static const int MAX_DENGER_OPP = 4;

class CommunicateSystem {
public:
	static const unsigned char *CODE;
	static int CODE_TO_INT[128];
	static const int CODE_SIZE;
	static const int MAX_MSG_SIZE;
	static int MAX_BITS_USED;

private:
	Observer *mpObserver;
	Agent* mpAgent;

#ifndef WIN32
	typedef int64_t DWORD64; //注意这里是有符号的，右移是左边补1
	typedef uint64_t UDWORD64;
#else
    typedef __int64 DWORD64;
	typedef unsigned __int64 UDWORD64;
#endif

public:
	enum CodecType {
		POS_X,
		POS_Y,
		BALL_SPEED,
		PLAYER_SPEED,
		DIR,
		CODEC_MAX
	};
	//CommuType排列顺序代表优先级序列	不可超过四个字节 即16种
	enum CommuType {
		FREE_FORM, //自由发送若干球和球员的组合信息
		//...
		COMMU_MAX
	};

	enum FreeFormType {
		BALL_WITH_SPEED,
		BALL_WITH_ZERO_SPEED,
		BALL_ONLY_POS,
		TEAMMATE_ONLY_POS,
		OPPONENT_ONLY_POS,
		FREE_FORM_DUMMY,
		FREE_FORM_MAX
	};

	/**
	 * set codec range to min, max of codec type type
	 */
	void GetCodecRange(CodecType type, double &min, double &max);

	/**
	 * convert double value to bits
	 */
	DWORD64 DoubleToBit(double value, CodecType type);

	/**
	 * convert bits to double value
	 */
	double BitToDouble(DWORD64 bits, CodecType type);

	/**
	 * convert bits to message
	 */
	void Encode(DWORD64 bits, unsigned char *msg , bool is_coach = false);

	/**
	 * convert message to bits
	 */
	void Decode(const unsigned char *msg, DWORD64& bits , bool is_coach = false);

	/**
	 * print binary bits
	 */
	void PrintBits(DWORD64 bits);

	/**
	 * 决定使用多少二进制位表示数据
	 * @param 期望的精度
	 */
	int BitCountOfEps(double eps, CodecType type);

	DWORD64 mCommuBits;
	int mBitsUsed; ///最多能用61位（10位73进制 ==> 61位2进制）
	bool mBallSended;
	PlayerArray<bool> mTeammateSended;
	PlayerArray<bool> mOpponentSended;
	CommuType mCommuType;

	int  		mCommuFlagBitCount;
	DWORD64 	mCommuFlagMask;
	int  		mFreeFormFlagBitCount;
	DWORD64  	mFreeFormFlagMask;
	int			mTacticsBitCount;
	DWORD64     mTacticsFlagMask;
	int  		mUnumBitCount;
	DWORD64  	mUnumMask;

	int  		mCodecBitCount[COMMU_MAX][CODEC_MAX];
	DWORD64  	mCodecMask[COMMU_MAX][CODEC_MAX];

	int  		(*mpCodecBitCount)[CODEC_MAX];
	DWORD64  	(*mpCodecMask)[CODEC_MAX];

	int  		mFreeFormCodecBitCount[FREE_FORM_MAX];


	/**
	 * add some data to communicate bits
	 * @param value data value
	 * @param type codec type
	 * @return success: true && failure: false
	 */
	bool AddDataToCommuBits(double value, CodecType type);

	/**
	 * extract data from communicate bits
	 * @param type
	 * @param bit_left
	 */
	double ExtractDataFromBits(DWORD64 &bits, CodecType type, int &bit_left);

	/**
	 * add flag to communicate bits
	 * @param type flag type
	 * @return
	 */
	bool AddFreeFormFlagToCommuBits(FreeFormType type);
	bool AddCommuFlagToCommuBits(CommuType type);

	/**
	 * add player unum to communicate bits
	 * @param num unum
	 * @return
	 */
	bool AddUnumToCommuBits(Unum num);
	int ExtractUnumFromBits(DWORD64 &bits, int &bit_left);

	/**
	 * 处理 free_form 信息
	 * @param bits
	 */
	void RecvFreeForm(DWORD64 bits);

	void SetCommunicateType(CommuType type) {
		mpCodecBitCount = & mCodecBitCount[type];
		mpCodecMask = & mCodecMask[type];
	}

private:
	CommunicateSystem();

	/**
	 * 默认的通信内容
	 */
	void DoCommunication();

public:
	virtual ~CommunicateSystem();

    static CommunicateSystem & instance();
    void Initial(Observer *observer , Agent* agent);

	/**
	 * reset some values at the beginning of every cycle and deal with hear message from server
	 */
	void Update();

	/**
	 * send say message to server
	 */
	void Decision();

	/**
	 * send ball status if ball cycle delay is smaller then cd
	 * @param cd max cycle delay of ball
	 */
	bool SendBallStatus(const BallState &ball_state, int cd = 0);

	/**
	 * send player status if ball cycle delay is smaller then cd
	 * @param cd max cycle delay of ball
	 */
	bool SendTeammateStatus(const WorldState *pWorldState, Unum num, int cd = 0);
	bool SendOpponentStatus(const WorldState *pWorldState, Unum num, int cd = 0);

	/**
	 * parse hear message from server
	 * @param msg
	 */
	void ParseReceivedTeammateMsg(unsigned char *msg);
};

#endif /* COMMUNICATESYSTEM_H_ */
