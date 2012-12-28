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

#ifndef __BasicCommand_H__
#define __BasicCommand_H__


#include <list>
#include <cstring>
#include <string>
#include "Types.h"
#include "Utilities.h"
#include "Geometry.h"

enum EarMode
{
	EM_Null,
	EM_Partial,
	EM_Complete,
    EM_All
};

enum CommandType
{
	CT_None,
	CT_Turn,
	CT_Dash,
	CT_TurnNeck,
	CT_Say,
	CT_Attentionto,
	CT_Kick,
	CT_Tackle,
	CT_Pointto,
	CT_Catch,
	CT_Move,
	CT_ChangeView,
	CT_Compression,
	CT_SenseBody,
	CT_Score,
	CT_Bye,
	CT_Done,
	CT_Clang,
	CT_Ear,
	CT_SynchSee,
	CT_ChangePlayerType,

	/////for trainer/////
	CT_ChangePlayerTypeForTrainer,
	CT_Start,
	CT_ChangePlayMode,
	CT_MovePlayer,
	CT_MoveBall,
	CT_Look,
	CT_TeamNames,
	CT_Recover,
	CT_CheckBall,

	CT_Max

};

class Agent;

struct CommandInfo
{
	CommandType mType;
	int mLevel;
	int mMinVer;
	int mMaxVer;
	bool mMutex;
	Time mTime;
	double mDist;
	double mPower;
	double mAngle;
	Vector mMovePos;
	ViewWidth mViewWidth;
    std::string mString;

	CommandInfo()
	{
		mType = CT_None;
	}

	const double & GetPower() const { return mPower; }
	const double & GetAngle() const { return mAngle; }
	const Vector & GetMovePos() const { return mMovePos; }
};


class BasicCommand
{
public:
	BasicCommand(const Agent & agent): mAgent(agent) {}
	virtual ~BasicCommand() {}

	bool Execute(std::list<CommandInfo> &command_queue);

	const double & GetPower() const { return mCommandInfo.GetPower(); }
	const double & GetAngle() const { return mCommandInfo.GetAngle(); }
	const Vector & GetMovePos() const { return mCommandInfo.GetMovePos(); }

protected:
	const Agent     & mAgent;
	CommandInfo mCommandInfo; // 在Plan()中赋值，在Execute()中压入队列
};


/**
* 下面是server可以接收的原子命令的类，定义的顺序与SoccerServer - Player::parseCommand()中的顺序一致
*/

class Turn : public BasicCommand
{
public:
	Turn(const Agent & agent);
	~Turn() {}

	void Plan(double moment);
};


class Dash : public BasicCommand
{
public:
	Dash(const Agent & agent);
	~Dash() {}

	void Plan(double power, AngleDeg dir);
};


class TurnNeck : public BasicCommand
{
public:
	TurnNeck(const Agent & agent);
	~TurnNeck() {}

	void Plan(double moment);
};


class Say : public BasicCommand
{
public:
	Say(const Agent & agent);
	~Say() {}

	void Plan(std::string msg);
};


class Attentionto : public BasicCommand
{
public:
	Attentionto(const Agent & agent);
	~Attentionto() {}

	void Plan(bool on, Unum num = 0);
};


class Kick : public BasicCommand
{
public:
	Kick(const Agent & agent);
	~Kick() {}

	void Plan(double power, double dir);
};


class Tackle : public BasicCommand
{
public:
	Tackle(const Agent & agent);
	~Tackle() {}

	void Plan(double dir, const bool foul);
};


class Pointto : public BasicCommand
{
public:
	Pointto(const Agent & agent);
	~Pointto() {}

	void Plan(bool on, double dist = 0.0, double dir = 0.0);
};


class Catch : public BasicCommand
{
public:
	Catch(const Agent & agent);
	~Catch() {}

	void Plan(double dir);
};


class Move : public BasicCommand
{
public:
	Move(const Agent & agent);
	~Move() {}

	void Plan(Vector pos);
};

class ChangeView : public BasicCommand
{
public:
	ChangeView(const Agent & agent);
	~ChangeView() {}

	void Plan(ViewWidth view_width);
};


class Compression : public BasicCommand
{
public:
	Compression(const Agent & agent);
	~Compression() {}

	void Plan(int level);
};


class SenseBody : public BasicCommand
{
public:
	SenseBody(const Agent & agent);
	~SenseBody() {}

	void Plan();
};


class Score : public BasicCommand
{
public:
	Score(const Agent & agent);
	~Score() {}

	void Plan();
};


class Bye : public BasicCommand
{
public:
	Bye(const Agent & agent);
	~Bye() {}

	void Plan();
};


class Done : public BasicCommand
{
public:
	Done(const Agent & agent);
	~Done() {}

	void Plan();
};


class Clang : public BasicCommand
{
public:
	Clang(const Agent & agent);
	~Clang() {}

	void Plan(int min_ver, int max_ver);
};


class Ear : public BasicCommand
{
public:
	Ear(const Agent & agent);
	~Ear() {}

	void Plan(bool on, bool our_side, EarMode ear_mode = EM_All);
};


class SynchSee : public BasicCommand
{
public:
	SynchSee(const Agent & agent);
	~SynchSee() {}

	void Plan();
};

class ChangePlayerType : public BasicCommand
{
public:
	ChangePlayerType(const Agent & agent);
	~ChangePlayerType() {}

	void Plan(Unum num, int player_type);

	void Plan(std::string teamname, Unum num, int player_type);
};


//以下为Trainer特殊的原子命令类，顺序与SoccerServer - Coach::parse_command相同

class Start : public BasicCommand
{
public:
	Start(const Agent & agent);
	~Start() {}

	void Plan();
};

class ChangePlayMode : public BasicCommand
{
public:
	ChangePlayMode(const Agent & agent);
	~ChangePlayMode() {}

	void Plan(ServerPlayMode spm); //这里采用ServerPlayMode的原因是，没有Observer，我不知道自己是左还是右。
};

class MovePlayer : public BasicCommand
{
public:
	MovePlayer(const Agent & agent);
	~MovePlayer() {}

	void Plan(std::string team_name, Unum num, Vector pos, Vector vel, AngleDeg dir);
};

class MoveBall : public BasicCommand
{
public:
	MoveBall(const Agent & agent);
	~MoveBall() {}

	void Plan(Vector pos, Vector vel);
};

class Look : public BasicCommand
{
public:
	Look(const Agent & agent);
	~Look() {}

	void Plan();
};

class TeamNames : public BasicCommand
{
public:
	TeamNames(const Agent & agent);
	~TeamNames() {}

	void Plan();
};

class Recover : public BasicCommand
{
public:
	Recover(const Agent & agent);
	~Recover() {}

	void Plan();
};

class CheckBall : public BasicCommand
{
public:
	CheckBall(const Agent & agent);
	~CheckBall() {}

	void Plan();
};


#endif

