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

#include <cstdio>
#include <sstream>
#include "Logger.h"
#include "Observer.h"
#include "WorldState.h"
#include "PositionInfo.h"
#include "InterceptInfo.h"


/**
 * SightLogger's constructor
 */
SightLogger::SightLogger(Observer *observer, WorldState *world_state)
{
	Assert(observer && world_state);

	mpObserver = observer;
	mpWorldState = world_state;

	mHeader = "ULG4\n";
	mHeaderReady = false;
	mHeaderLogged = false;
	mLoggedPlayerTypeCount = 0;

	mServerPlayMode = SPM_Null;
	mServerPlayMode_dirty = true;
	mLeftScore = 0;
	mRightScore = 0;
	mLeftPenTaken = 0;
	mRightPenTaken = 0;
	mTeamState_dirty = true;

	char file_name[256];
	sprintf( file_name, "%s/%s-%d-sight.log", PlayerParam::instance().logDir().c_str(), PlayerParam::instance().teamName().c_str(), mpObserver->SelfUnum());
	os.open(file_name);
	if (!os.good()){
		PRINT_ERROR("open sight log file error");
	}
}

/**
 * SightLogger's destructor
 */
SightLogger::~SightLogger()
{
	os.close();
}

/**
 * Set sever param message of the sight log
 */
void SightLogger::SetServerParamMsg(const char *msg)
{
	mServerParamMsg = msg;
	mServerParamMsg += "\n";
}

/**
 * Set player param message of the sight log
 */
void SightLogger::SetPlayerParamMsg(const char *msg)
{
	mPlayerParamMsg = msg;
	mPlayerParamMsg += "\n";
}

/**
 * Set hetero player type message of the sight log
 */
void SightLogger::SetPlayerTypeMsg(const char *msg)
{
	mPlayerTypeMsg += msg;
	mPlayerTypeMsg += "\n";
	mLoggedPlayerTypeCount ++;

	if (mLoggedPlayerTypeCount >= PlayerParam::instance().playerTypes()){
		mHeaderReady = true;
	}
}

/**
 * Flush sight log to file.
 */
void SightLogger::Flush()
{
	static const double prec = 0.0001;

	if (mHeaderReady && PlayerParam::instance().SaveSightLog())
	{
		if (!mHeaderLogged)
		{
			mHeaderLogged = true;
			os << mHeader << mServerParamMsg << mPlayerParamMsg << mPlayerTypeMsg;

			for (int i = 0; i < mTime.S(); ++i) {
				os << "(show " << 0
				<< " ((b)"
				<< ' ' << Quantize( mpBall->GetPos().X(), prec )
				<< ' ' << Quantize( mpBall->GetPos().Y(), prec )
				<< ' ' << Quantize( mpBall->GetVel().X(), prec )
				<< ' ' << Quantize( mpBall->GetVel().Y(), prec )
				<< ')';

				for (char side = 'l'; side <= 'r'; side += 'r'-'l'){
					for (Unum i = 1; i <= TEAMSIZE; ++i){
						os << " ((" << side << ' ' << i << ')'
						<< ' ' << 0
						<< ' ' << "0x0"
						<< ' ' << i * 4.0 * (side == 'l'? -1: 1)
						<< ' ' << -37.0
						<< ' ' << 0
						<< ' ' << 0
						<< ' ' << 0
						<< ' ' << 0
						<< " (v h " <<  60 << ')'
						<< " (s " << 0 << ' ' << 0 << ' ' << '1' << ')'
						<< " (c 0 0 0 0 0 0 0 0 0 0 0))";
						// end of player
					}
				}
				os << ")\n";
			}
		}

		mSightMutex.Lock();

		if (mServerPlayMode_dirty)
		{
			mServerPlayMode_dirty = false;
			os << "(playmode " << mTime.T() << ' ' << ServerPlayModeMap::instance().GetPlayModeString(mServerPlayMode) << ")\n";
		}

		if ( mTeamState_dirty )
		{
			mTeamState_dirty = false;
			os << "(team " << mTime.T()
			   << ' ' << ( mLeftName.empty() ? "null" : mLeftName.c_str() )
			   << ' ' << ( mRightName.empty() ? "null" : mRightName.c_str() )
			   << ' ' << mLeftScore
			   << ' ' << mRightScore
			   << ")\n";
		}

		os << "(show " << mTime.T()
		   << " ((b)"
		   << ' ' << Quantize( mpBall->GetPos().X(), prec )
		   << ' ' << Quantize( mpBall->GetPos().Y(), prec )
		   << ' ' << Quantize( mpBall->GetVel().X(), prec )
		   << ' ' << Quantize( mpBall->GetVel().Y(), prec )
		   << ')';

		LogBallInfo(*mpBall);

		const PlayerState *p;
		for (char side = 'l'; side <= 'r'; side += 'r'-'l'){
			for (Unum i = 1; i <= TEAMSIZE; ++i){
				p = (side == 'l') ? mpLeftTeam[i] : mpRightTeam[i];

				if (p->IsAlive()){
					LogPlayerInfo(*p);
				}

				os << " ((" << side << ' ' << i << ')'
				   << ' ' << p->GetPlayerType()
				   << ' ' << (p->IsAlive()? (p->IsGoalie()? "0x9": "0x1"): "0x0")
				   << ' ' << (p->IsAlive()? Quantize( p->GetPos().X(), prec ): i * 4.0 * (side == 'l'? -1: 1))
				   << ' ' << (p->IsAlive()? Quantize( p->GetPos().Y(), prec ): -37.0)
				   << ' ' << Quantize( p->GetVel().X(), prec )
				   << ' ' << Quantize( p->GetVel().Y(), prec )
				   << ' ' << Quantize( p->GetBodyDir(), prec )
				   << ' ' << Quantize( p->GetNeckDir(), prec )
				   << " (v h " <<  sight::ViewAngle(p->GetViewWidth()) << ')'
				   << " (s " << p->GetStamina() << ' ' << p->GetEffort() << ' ' << '1' << ')'
				   << " (c 0 0 0 0 0 0 0 0 0 0 0))";
				// end of player
			}
		}

		mSightMutex.UnLock();

		os << ")\n";
		os.flush();
	}

	if (PlayerParam::instance().SaveDecLog())
	{
		mDecMutex.Lock();
		int time = mTime.S()<<16 | mTime.T();
		if (!points.empty()) {
			for (std::vector<PointShape>::iterator it = points.begin(); it != points.end(); it++)
			{
				os << "(draw " << time << ' '<< *it <<")\n";
			}
			points.clear();
		}
		if (!lines.empty()) {
			for (std::vector<LineShape>::iterator it = lines.begin(); it != lines.end(); it++)
			{
				os << "(draw " << time << ' '<< *it <<")\n";
			}
			lines.clear();
		}
		if (!circles.empty()) {
			for (std::vector<CircleShape>::iterator it = circles.begin(); it != circles.end(); it++)
			{
				os << "(draw " << time << ' '<< *it <<")\n";
			}
			circles.clear();
		}
		mDecMutex.UnLock();
		os.flush();
	}
}

/**
 * Log the player's sight state.
 */
void SightLogger::LogSight()
{
	if (mHeaderReady)
	{
		mSightMutex.Lock();

		if (mServerPlayMode != mpObserver->GetServerPlayMode())
		{
			mServerPlayMode = mpObserver->GetServerPlayMode();
			mServerPlayMode_dirty = true;
		}

		mTime = mpWorldState->CurrentTime();

		if ( mLeftScore != mpWorldState->GetTeammateScore()
				|| mRightScore != mpWorldState->GetOpponentScore()
				|| mLeftName != PlayerParam::instance().teamName()
				|| mRightName != PlayerParam::instance().opponentTeamName()
		)
		{
			mLeftName = PlayerParam::instance().teamName();
			mRightName = PlayerParam::instance().opponentTeamName();
			mLeftScore = mpWorldState->GetTeammateScore();
			mRightScore = mpWorldState->GetOpponentScore();
			mTeamState_dirty = true;
		}

		mpBall = & (mpWorldState->Ball());

		for (Unum i = 1; i <= TEAMSIZE; ++i)
		{
			mpLeftTeam[i] = & (mpWorldState->Teammate(i));
			mpRightTeam[i] = & (mpWorldState->Opponent(i));
		}

		mSightMutex.UnLock();
	}
}

void SightLogger::LogPlayerInfo(const PlayerState & player)
{
	if (PlayerParam::instance().SaveDecLog()){
		DecLock();
		LogDec();

		std::stringstream ss;
		ss << "pos [" << player.GetPosDelay() << ", " << player.GetPosConf() << "]" << '#'
		   << "vel [" << player.GetVelDelay() << ", " << player.GetVelConf() << "]" << '#'
		   << "dir [" << player.GetBodyDirDelay() << ", " << player.GetBodyDirConf() << "]" << '#'
		   << "neck [" << player.GetNeckDirDelay() << ", " << player.GetNeckDirConf() << "]";
		AddPoint(player.GetPos(), ss.str().c_str(), White);

		DecUnLock();
	}
}

void SightLogger::LogBallInfo(const BallState & ball)
{
	if (PlayerParam::instance().SaveDecLog()){
		DecLock();
		LogDec();

		std::stringstream ss;
		ss << "pos [" << ball.GetPosDelay() << ", " << ball.GetPosConf() << "]" << '#'
		<< "vel [" << ball.GetVelDelay() << ", " << ball.GetVelConf() << "]" << '#';
		AddPoint(ball.GetPos(), ss.str().c_str(), White);

		DecUnLock();
	}
}

/**
 * TextLogger's constructor
 * The name of the log file will be "team_name-unum-log_name.log".
 * \param observer for getting player's unum
 * \param log_name
 */
TextLogger::TextLogger(Observer* observer, const char* log_name)
{
	char file_name[256];

	Assert(std::string(log_name) != std::string("msg"));
	Assert(std::string(log_name) != std::string("sight"));

	sprintf( file_name, "Logfiles/%s-%d-%s.log", PlayerParam::instance().teamName().c_str(), observer->SelfUnum(), log_name);
	os.open(file_name);
	if (!os.good()){
		PRINT_ERROR("open log file error");
	}
}

/**
 * TextLogger's constructor
 * Construct a null logger.
 */
TextLogger::TextLogger()
{
}

/**
 * TextLogger's destructor
 */
TextLogger::~TextLogger()
{
	os.flush();
	os.close();
}

/**
 * Flush log to file
 */
void TextLogger::Flush()
{
	if (PlayerParam::instance().SaveTextLog())
	{
		os << mBuffer.str();
		os.flush();
	}
	mBuffer.str("");
}

/**
 * Operator <<
 * Usage is the same as ostream.
 */
//template<typename T>
//TextLogger& operator<<(TextLogger& logger, const T &value)
//{
//	if (PlayerParam::instance().SaveTextLog())
//	{
//		logger.mBuffer << value;
//	}
//	return logger;
//}

/**
 * Return an static instance of Logger
 * Usage of Logger should be Logger::instance().blablabla
 */
Logger& Logger::instance()
{
	static Logger logger;
	return logger;
}

/**
 * Logger's real constructor
 * \param observer
 * \param world_state
 */
void Logger::Initial(Observer *observer, WorldState *world_state)
{
	mpObserver = observer;
	mpWorldState = world_state;
}

/**
 * Logger's destructor
 */
Logger::~Logger()
{
	delete mpSightLogger;
	for (std::map<std::string, TextLogger*>::iterator i = mTextLoggers.begin(); i != mTextLoggers.end(); ++i)
	{
		delete i->second;
	}
}

TextLogger Logger::mTextLoggerNull;

/**
 * Logger's main loop
 */
void Logger::StartRoutine()
{
	while (!mCondFlush.Wait(PlayerParam::instance().WaitTimeOut() * 1000))
	{
		Flush();
	}
	Flush();
}

/**
 * Get the sight logger. If it does not exsit, construct one.
 * This method MUST be called AFTER the player's unum has been known.
 * \return a pointer to sight logger.
 */
SightLogger* Logger::GetSightLogger()
{
	if (!mpSightLogger)
	{
		mpSightLogger = new SightLogger(mpObserver, mpWorldState);
	}
	return mpSightLogger;
}

/**
 * Get a text logger named log_name. If it does not exsit, create one.
 * You may use this method as: Logger::instance().GetTextLogger("log_name")<<"log_text";
 * \param log_name name of the text logger, which the log file will be named after.
 * \return the logger.
 */
TextLogger& Logger::GetTextLogger(const char* log_name)
{
	if (PlayerParam::instance().SaveTextLog())
	{
		std::map<std::string, TextLogger*>::iterator logger=mTextLoggers.find(log_name);
		if (logger == mTextLoggers.end())
		{
			logger = mTextLoggers.insert(std::make_pair(log_name, new TextLogger(mpObserver, log_name))).first;
		}
		return *(logger->second);
	}
	else
	{
		return mTextLoggerNull;
	}
}

/**
 * flush logs
 */
void Logger::Flush()
{
	if (mpSightLogger)
	{
		mpSightLogger->Flush();
	}

	for (std::map<std::string, TextLogger*>::iterator i = mTextLoggers.begin(); i != mTextLoggers.end(); ++i)
	{
		i->second->Flush();
	}
}

/**
 * set mCondFlush to let LoggerLoop flush logs.
 */
void Logger::SetFlushCond()
{
	if (PlayerParam::instance().SaveSightLog() || PlayerParam::instance().SaveDecLog() || PlayerParam::instance().SaveTextLog())
		mCondFlush.Set();
}

/**
 * Initalize the sight logger
 * Should be called several times by parser.
 * \param msg_type type of the msg
 * \param msg
 */
void Logger::InitSightLogger(ServerMsgType msg_type, const char* msg)
{
	if (PlayerParam::instance().SaveSightLog())
	{
		switch (msg_type)
		{
			case ServerParam_Msg : GetSightLogger()->SetServerParamMsg(msg); break;
			case PlayerParam_Msg : GetSightLogger()->SetPlayerParamMsg(msg); break;
			case PlayerType_Msg  : GetSightLogger()->SetPlayerTypeMsg(msg);  break;
			default: break;
		}
	}
}

/**
 * Log the player's sight state
 */
void Logger::LogSight()
{
	if (PlayerParam::instance().SaveSightLog())
	{
		GetSightLogger()->LogSight();
	}
}

void Logger::LogPoint(const Vector & target, SightLogger::Color color, const char* comment)
{
	if (PlayerParam::instance().SaveDecLog()){
		SightLogger *pSightLogger = GetSightLogger();
		pSightLogger->DecLock();
		pSightLogger->LogDec();
		pSightLogger->AddPoint(target, comment, color);
		pSightLogger->DecUnLock();
	}
}

void Logger::LogGoToPoint(const Vector & start, const Vector & target, const char* comment)
{
	if (PlayerParam::instance().SaveDecLog()){
		SightLogger *pSightLogger = GetSightLogger();
		pSightLogger->DecLock();
		pSightLogger->LogDec();
		pSightLogger->AddPoint(target, comment, SightLogger::Red);
		pSightLogger->AddCircle(target, 0.2, SightLogger::Red);
		pSightLogger->AddLine(start, target, SightLogger::Red);
		pSightLogger->DecUnLock();
	}
}

void Logger::LogShoot(const Vector & start, const Vector & target, const char* comment)
{
    if (PlayerParam::instance().SaveDecLog())
    {
        SightLogger *pSightLogger = GetSightLogger();
        pSightLogger->DecLock();
        pSightLogger->LogDec();
        pSightLogger->AddPoint(target, comment, SightLogger::Purple);
        pSightLogger->AddLine(start, target, SightLogger::Purple);
        pSightLogger->DecUnLock();
    }
}

void Logger::LogIntercept(const Vector & interpt, const char* comment)
{
	if (PlayerParam::instance().SaveDecLog())
	{
		SightLogger *pSightLogger = GetSightLogger();
		pSightLogger->DecLock();
		pSightLogger->LogDec();
		pSightLogger->AddPoint(interpt, comment, SightLogger::Cyan);
		pSightLogger->AddCircle(interpt, 0.1, SightLogger::Cyan);
		pSightLogger->DecUnLock();
	}
}

void Logger::LogLine(const Vector & begin, const Vector & end, SightLogger::Color color, const char* comment)
{
	if (PlayerParam::instance().SaveDecLog()){
		SightLogger *pSightLogger = GetSightLogger();
		pSightLogger->DecLock();
		pSightLogger->LogDec();
		pSightLogger->AddPoint(end, comment, color);
		pSightLogger->AddLine(begin, end, color);
		pSightLogger->DecUnLock();
	}
}

void Logger::LogCircle(const Vector & o, const double & r, SightLogger::Color color)
{
	if (PlayerParam::instance().SaveDecLog()){
		SightLogger *pSightLogger = GetSightLogger();
		pSightLogger->DecLock();
		pSightLogger->LogDec();
		pSightLogger->AddCircle(o, r, color);
		pSightLogger->DecUnLock();
	}
}

void Logger::LogRectangular(const Rectangular & rect, SightLogger::Color color)
{
	if (PlayerParam::instance().SaveDecLog()){
		SightLogger *pSightLogger = GetSightLogger();
		pSightLogger->DecLock();
		pSightLogger->LogDec();

		pSightLogger->AddLine(rect.TopLeftCorner(), rect.TopRightCorner(), color);
		pSightLogger->AddLine(rect.TopLeftCorner(), rect.BottomLeftCorner(), color);
		pSightLogger->AddLine(rect.BottomLeftCorner(), rect.BottomRightCorner(), color);
		pSightLogger->AddLine(rect.BottomRightCorner(), rect.TopRightCorner(), color);

		pSightLogger->DecUnLock();
	}
}


void Logger::LogDribble(const Vector & start, const Vector & target, const char * comment, bool is_execute)
{
	if (PlayerParam::instance().SaveDecLog())
    {
		SightLogger *pSightLogger = GetSightLogger();
		pSightLogger->DecLock();
		pSightLogger->LogDec();

        pSightLogger->AddPoint(target, comment, SightLogger::Blue);
        if (is_execute)
        {
            pSightLogger->AddLine(start, target, SightLogger::Blue);
        }

		pSightLogger->DecUnLock();
	}
}


void Logger::LogPass(const bool reverse, const Vector & start, const Vector & target, const char * comment, bool is_execute)
{
    if (PlayerParam::instance().SaveDecLog())
    {
        SightLogger *pSightLogger = GetSightLogger();
        pSightLogger->DecLock();
        pSightLogger->LogDec();

        pSightLogger->AddPoint(reverse? target.Rotate(180.0): target, comment, SightLogger::Red);
        if (is_execute)
        {
            pSightLogger->AddLine(reverse? start.Rotate(180.0): start, reverse? target.Rotate(180.0): target, SightLogger::Red);
        }

        pSightLogger->DecUnLock();
    }
}

const Time & Logger::CurrentTime()
{
	return mpWorldState->CurrentTime();
}
