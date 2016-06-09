/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2016                                                    *
 * Copyright (c) 1998-2016 WrightEagle 2D Soccer Simulation Team,                   *
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

#ifndef __Logger_H__
#define __Logger_H__

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "Types.h"
#include "Geometry.h"
#include "Thread.h"
#include "WorldState.h"

class Observer;
class BallState;
class PlayerState;
class InfoState;

/**
 * Sight log class
 */
class SightLogger
{
public:
	enum Color {
		Red,
		Blue,
		Green,
		Navy,
		Orange,
		Cyan,
		Purple,
		White,
		Black,
		Yellow,
		Olive
	};

	/** Constructor & destructor */
	SightLogger(Observer *observer, WorldState *world_state);
	~SightLogger();

	/** Header tools */
	void SetServerParamMsg(const char *msg);
	void SetPlayerParamMsg(const char *msg);
	void SetPlayerTypeMsg(const char *msg);

	/** Log sight */
	void LogSight();

	/** flush */
	void Flush();

	/** Basic drawing tools */
	void DecLock()   { mDecMutex.Lock(); }
	void DecUnLock() { mDecMutex.UnLock(); }
	void LogDec() { mTime = mpWorldState->CurrentTime(); }

	void AddPoint(const Vector & point, const char* comment = 0, Color color = Red) { points.push_back(PointShape(point, color, comment)); }
	void AddLine(const Vector & origin, const Vector & target, Color color = Yellow) { lines.push_back(LineShape(origin, target, color)); }
	void AddCircle(const Vector & origin, const double & radius, Color color = White) { circles.push_back(CircleShape(origin, radius, color)); }

	/** Integrated drawing tools */
    void LogPlayerInfo(const PlayerState & player);
    void LogBallInfo(const BallState & ball);

private:
	std::ofstream os;
	ThreadMutex mSightMutex;
	ThreadMutex mDecMutex;

	Observer   *mpObserver;
	WorldState *mpWorldState;

	std::string mHeader;
	std::string mServerParamMsg;
	std::string mPlayerParamMsg;
	std::string mPlayerTypeMsg;
	int mLoggedPlayerTypeCount;
	bool mHeaderReady; //是否可以记录视觉信息（要先记录好server_param等）
	bool mHeaderLogged;
	ServerPlayMode mServerPlayMode;
	bool mServerPlayMode_dirty;
	Time mTime;
	int mLeftScore; //自己总是左边的
	int mRightScore;
	int mLeftPenTaken;
	int mRightPenTaken;
	std::string mLeftName;
	std::string mRightName;
	bool mTeamState_dirty;

	BallState *mpBall;
	PlayerArray<PlayerState*> mpLeftTeam;
	PlayerArray<PlayerState*> mpRightTeam;

	struct ItemShape {
		Color line_color;

		const char *color() const {
			switch (line_color){
			case Red: return "red";
			case Blue: return "blue";
			case Green: return "green";
			case Navy: return "navy";
			case Orange: return "orange";
			case Cyan: return "cyan";
			case Purple: return "purple";
			case White: return "white";
			case Black: return "black";
			case Yellow: return "yellow";
			case Olive: return "olive";
			}
			return "black";
		}

		ItemShape (Color color){
			line_color = color;
		}
	};

	struct PointShape: public ItemShape {
		double x, y;
		std::string comment;

		PointShape (const Vector & point, Color color, const char* cmt = 0): ItemShape(color){
			x = point.X();
			y = point.Y();
			if (cmt)
			{
				comment.assign(cmt);
			}
		};

		friend std::ostream& operator<<(std::ostream &os, PointShape &point) {
			return os << "(point " << point.x << ' ' << point.y << ' ' << "\"" << point.color() << "\" "<<point.comment<<")";
		}
	};

	struct LineShape: public ItemShape {
		double x1, y1;
		double x2, y2;
		int width;

		LineShape (const Vector & from, const Vector & to, Color color): ItemShape(color){
			x1 = from.X();
			y1 = from.Y();
			x2 = to.X();
			y2 = to.Y();
		};

		friend std::ostream& operator<<(std::ostream &os, const LineShape &line) {
			return os << "(line " << line.x1 << ' ' << line.y1 << ' ' << line.x2 << ' ' << line.y2 << " \"" << line.color() << "\")";
		}
	};

	struct CircleShape: public ItemShape {
		double x, y;
		double radius;

		CircleShape (const Vector & center, const double & r, Color color): ItemShape(color){
			x = center.X();
			y = center.Y();
			radius = r;
		};

		friend std::ostream& operator<<(std::ostream &os, CircleShape &circle) {
			return os << "(circle " << circle.x << ' ' << circle.y << ' ' << circle.radius << " \"" << circle.color() << "\")";
		}
	};

	std::vector<PointShape> points;
	std::vector<LineShape> lines;
	std::vector<CircleShape> circles;

};

/**
 * Text log class
 */
class TextLogger
{
	std::ofstream     os;
	std::stringstream mBuffer;

public:
	TextLogger(Observer* observer, const char* log_name);
	TextLogger();
	~TextLogger();
	void Flush();

	/**
	 * Operator <<
	 * Usage is the same as ostream.
	 */
	template<typename T>
	TextLogger& operator<<(const T& value)
	{
		if (PlayerParam::instance().SaveTextLog())
		{
			mBuffer << value;
		}
		return *this;
	}

	/**
	 * Copied from Mersad-5.9.5-RC2005. I dont know what these two specials
	 * are for. Will cause "undefined reference ..." if removed.
	 */
	TextLogger& operator<<(std::ostream& (*man)(std::ostream&))
	{
		if (PlayerParam::instance().SaveTextLog())
		{
			mBuffer << man;
		}
		return *this;
	}
	TextLogger& operator<<(std::ios_base& (*man)(std::ios_base&))
	{
		if (PlayerParam::instance().SaveTextLog())
		{
			mBuffer << man;
		}
		return *this;
	}

};


/**
 * Logger
 */
class Logger: public Thread
{
	Observer   *mpObserver;
	WorldState *mpWorldState;

	SightLogger *mpSightLogger;
	std::map<std::string, TextLogger*> mTextLoggers;
	static TextLogger mTextLoggerNull;

	ThreadCondition mCondFlush;

	Logger(): mpObserver(0), mpWorldState(0), mpSightLogger(0) {}

public:
	static Logger& instance();

    /**
     * init函数和析构函数
     * Initialize.
     */
    void Initial(Observer *observer, WorldState *world_state );
    virtual ~Logger();

    /**
     * 主循环函数
     * Mainloop.
     */
    void StartRoutine();

    /**
     * flush logs
     */
    void Flush();

    /**
     * set mCondFlush to let LoggerLoop flush logs.
     */
    void SetFlushCond();

    const Time & CurrentTime();

    /**
     * get subloggers
     */
    SightLogger* GetSightLogger();
    TextLogger& GetTextLogger(const char* logger_name);

    /**
     * utilitis - sight log & dec log
     */
    void InitSightLogger(ServerMsgType msg_type, const char* msg);
    void LogSight();
    void LogPoint(const Vector & target, SightLogger::Color color = SightLogger::Red, const char* comment = 0);
    void LogLine(const Vector & begin, const Vector & end, SightLogger::Color color = SightLogger::Yellow, const char* comment = 0);
    void LogCircle(const Vector & o, const double & r, SightLogger::Color color = SightLogger::Yellow);
    void LogRectangular(const Rectangular & rect, SightLogger::Color color = SightLogger::Yellow);
    void LogGoToPoint(const Vector & start, const Vector & target, const char* comment = 0);
    void LogShoot(const Vector & start, const Vector & target, const char* comment = 0);
    void LogIntercept(const Vector & interpt, const char* comment =0);
    void LogDribble(const Vector & start, const Vector & target, const char * comment = 0, bool is_execute = false);
    void LogPass(const bool reverse, const Vector & start, const Vector & target, const char * comment = 0, bool is_execute = false);
};

#endif
