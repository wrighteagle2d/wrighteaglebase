/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2016                                                    *
 * Copyright (c) 1998-2012 WrightEagle 2D Soccer Simulation Team,                   *
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

#include <list>
#include <string>
#include <sstream>
#include "Trainer.h"
#include "DynamicDebug.h"
#include "Thread.h"
#include "UDPSocket.h"
#include "WorldModel.h"
#include "Agent.h"
#include "Logger.h"
#include "PlayerParam.h"
#include "Utilities.h"


#include <iostream>

using namespace std;

Trainer::PlayerState::PlayerState(Unum num, Vector pos, Vector vel, AngleDeg dir, int type)
{
	mUnum = num;
	mPos = pos;
	mVel = vel;
	mBodyDir = dir;
	mPlayerType = type;
}

Trainer::Trainer()
{
	mTrainTime = 0;
	mTrainCount = 0;
	mInitialized = false;
	mConverse = false;
	mPrepared = false;
	mHaveRcg = 0;
	mpEndCondition = new Trainer::Condition(this, ET_Null, 0, 0, 0x0, mConverse);
	mLastStopTime = 0;
	mpObserver->SetSelfUnum( TRAINER_UNUM );
	mInitialTime = 0;
	mPlayerStatesList.clear();
	mBallStateList.clear();
	mNeedOpponentList.clear();
	if(!PlayerParam::instance().ForcePenaltyMode())
	{
		ReadConfigFile();
	}
}

Trainer::~Trainer()
{
	Record();
	delete mpEndCondition;
}

void Trainer::SendOptionToServer()
{
	while (!mpParser->IsEyeOnOk())
	{
		UDPSocket::instance().Send("(eye on)");
		WaitFor(200);
	}

	while (!mpParser->IsEarOnOk())
	{
		cout << "Send (ear on)" << endl;
		UDPSocket::instance().Send("(ear on)");
		WaitFor(200);
	}
}

void Trainer::Run()
{
	//TIMETEST("coach_run");

	mpObserver->Lock();

	/** 下面几个更新顺序不能变 */
	mpAgent->CheckCommands(mpObserver); // 检查上周期发送命令情况
	mpWorldModel->Update(mpObserver);

	mpObserver->UnLock();

	DoDecisionMaking();

	Logger::instance().LogSight();
}

void Trainer::DoDecisionMaking()
{
    switch (mpAgent->World().GetPlayMode())
    {
    	case PM_Our_Penalty:
    	case PM_Opp_Penalty:
        case PM_Penalty_On_Our_Field:
	    case PM_Penalty_On_Opp_Field:
        case PM_Our_Penalty_Setup:
        case PM_Our_Penalty_Ready:
        case PM_Our_Penalty_Taken:
        case PM_Our_Penalty_Score:
        case PM_Our_Penalty_Miss:
        case PM_Our_Penalty_Foul:
        case PM_Our_Penalty_Winner:
        case PM_Opp_Penalty_Setup:
        case PM_Opp_Penalty_Ready:
        case PM_Opp_Penalty_Taken:
        case PM_Opp_Penalty_Score:
        case PM_Opp_Penalty_Miss:
        case PM_Opp_Penalty_Foul:
        case PM_Opp_Penalty_Winner:
        	return;
        default:
        	break;
     }

	if(PlayerParam::instance().ForcePenaltyMode())	//普通比赛，临近结束时强制进行点球
	{
		if(mpAgent->World().GetPlayMode() == PM_Before_Kick_Off && mpAgent->World().GetLastPlayMode() == PM_Play_On)
			mPrepared = false;

		if(mpAgent->World().GetTeammateScore() != mpAgent->World().GetOpponentScore()
			&& !mPrepared)

		mpAgent->World().GetTeammateScore() < mpAgent->World().GetOpponentScore()? Score(true) : Score(false);
	}
	else if(CheckOpponent()){
		if(!mInitialized)
		{
			InitializeStadium();
		}
		if(mpEndCondition->CheckState(mpAgent->World()) || mpAgent->World().GetPlayMode() == PM_Before_Kick_Off)
		{
			mPrepared = false;
		}
		if(!mPrepared && mInitialized)
		{
			PrepareForTrain();
		}
	}
}

void Trainer::Start()
{
	mpAgent->Start();
}

void Trainer::ChangePlayMode(PlayMode pm)
{
	bool isOurLeft = ( mpObserver->OurInitSide() == 'l' );//我们是否在左侧
	ServerPlayMode spm = SPM_Null;
	switch(pm){
	case PM_Play_On:
		spm = SPM_PlayOn;
		break;
	case PM_Our_Kick_Off:
		spm = isOurLeft ? SPM_KickOff_Left : SPM_KickOff_Right;
		break;
	case PM_Our_Kick_In:
		spm = isOurLeft ? SPM_KickIn_Left : SPM_KickIn_Right;
		break;
	case PM_Our_Corner_Kick:
		spm = isOurLeft ? SPM_CornerKick_Left : SPM_CornerKick_Right;
		break;
	case PM_Our_Goal_Kick:
		spm = isOurLeft ? SPM_GoalKick_Left : SPM_GoalKick_Right;
		break;
	case PM_Our_Free_Kick:
		spm = isOurLeft ? SPM_FreeKick_Left : SPM_FreeKick_Right;
		break;
	case PM_Our_Indirect_Free_Kick:
		spm = isOurLeft ? SPM_IndFreeKick_Left : SPM_IndFreeKick_Right;
		break;
		//case PM_Our_Goalie_Free_Kick,    /* not a real play mode */
	case PM_Our_Offside_Kick:
		spm = isOurLeft ? SPM_OffSide_Right : SPM_OffSide_Left;//对方越位，我们才能踢球
		break;
	case PM_Our_Back_Pass_Kick:
		spm = isOurLeft ? SPM_Back_Pass_Right : SPM_Back_Pass_Left;
		break;
	case PM_Our_Free_Kick_Fault_Kick:
		spm = isOurLeft ? SPM_Free_Kick_Fault_Right : SPM_Free_Kick_Fault_Left;
		break;
	case PM_Our_CatchFault_Kick:
		spm = isOurLeft ? SPM_CatchFault_Right : SPM_CatchFault_Left;
		break;
	case PM_Our_Foul_Charge_Kick:
		spm = isOurLeft ? SPM_Foul_Charge_Right : SPM_Foul_Charge_Left;
		break;
		//    	case PM_Our_Penalty_Setup:
		//    	case PM_Our_Penalty_Ready:
		//    	case PM_Our_Penalty_Taken:
		//    	case PM_Our_Penalty_Score:
		//    	case PM_Our_Penalty_Miss:
		//    	case PM_Our_Penalty_Foul:
		//    	case PM_Our_Penalty_Winner:

		//PM_Our_Mode,

	case PM_Before_Kick_Off:
		spm = SPM_BeforeKickOff;
		break;
		//    	case PM_Penalty_On_Our_Field:
		//    	case PM_Our_Penalty:
		//    	case PM_Opp_Penalty:
		//    	case PM_Penalty_On_Opp_Field:
	case PM_Drop_Ball:
		spm = SPM_Drop_Ball;
		break;
	case PM_Half_Time:
		spm = SPM_FirstHalfOver;
		break;
	case PM_Time_Over:
		spm = SPM_TimeOver;
		break;
	case PM_Time_Up:
		spm = SPM_TimeUp;
		break;
	case PM_Extended_Time:
		spm = SPM_TimeExtended;
		break;
		//    	case PM_Goal_Ours:
		//    	case PM_Goal_Opps:
	case PM_Our_Foul:
		spm = isOurLeft ? SPM_Foul_Left : SPM_Foul_Right;
		break;
	case PM_Opp_Foul:
		spm = isOurLeft ? SPM_Foul_Right : SPM_Foul_Left;
		break;

		//PM_Opp_Mode,

	case PM_Opp_Kick_Off:
		spm = isOurLeft ? SPM_KickOff_Right : SPM_KickOff_Left;
		break;
	case PM_Opp_Kick_In:
		spm = isOurLeft ? SPM_KickIn_Right : SPM_KickIn_Left;
		break;
	case PM_Opp_Corner_Kick:
		spm = isOurLeft ? SPM_CornerKick_Right : SPM_CornerKick_Left;
		break;
	case PM_Opp_Goal_Kick:
		spm = isOurLeft ? SPM_GoalKick_Right : SPM_GoalKick_Left;
		break;
	case PM_Opp_Free_Kick:
		spm = isOurLeft ? SPM_FreeKick_Right : SPM_FreeKick_Left;
		break;
	case PM_Opp_Indirect_Free_Kick:
		spm = isOurLeft ? SPM_IndFreeKick_Right : SPM_IndFreeKick_Left;
		break;
		//	case PM_Opp_Goalie_Free_Kick,  /* not a real play mode */
	case PM_Opp_Offside_Kick:
		spm = isOurLeft ? SPM_OffSide_Left : SPM_OffSide_Right;//我方越位，对方才能踢球
		break;
	case PM_Opp_Free_Kick_Fault_Kick:
		spm = isOurLeft ? SPM_Free_Kick_Fault_Left : SPM_Free_Kick_Fault_Right;
		break;
	case PM_Opp_Back_Pass_Kick:
		spm = isOurLeft ? SPM_Back_Pass_Left : SPM_Back_Pass_Right;
		break;
	case PM_Opp_CatchFault_Kick:
		spm = isOurLeft ? SPM_CatchFault_Left : SPM_CatchFault_Right;
		break;
	case PM_Opp_Foul_Charge_Kick:
		spm = isOurLeft ? SPM_Foul_Charge_Left : SPM_Foul_Charge_Right;
		break;
		//    	case PM_Opp_Penalty_Setup:
		//    	case PM_Opp_Penalty_Ready:
		//    	case PM_Opp_Penalty_Taken:
		//    	case PM_Opp_Penalty_Score:
		//    	case PM_Opp_Penalty_Miss:
		//    	case PM_Opp_Penalty_Foul:
		//    	case PM_Opp_Penalty_Winner:
	default:
		PRINT_ERROR("Cannot change to this play mode");
		return;
	}

	mpAgent->ChangePlayMode(spm);
}

void Trainer::MovePlayer(PlayerState ps)
{
	MovePlayer(ps.mUnum, ps.mPos, ps.mVel, ps.mBodyDir);
}

void Trainer::MovePlayer(Unum num, Vector pos, Vector vel, AngleDeg dir)
{
	if(num > 0) {
		mpAgent->MovePlayer(PlayerParam::instance().teamName(), num, pos, vel, dir);
	}
	else {
		mpAgent->MovePlayer(PlayerParam::instance().opponentTeamName(), -num, pos, vel, dir);
	}
}

void Trainer::MoveBall(Vector pos, Vector vel)
{
	mpAgent->MoveBall(pos,vel);
}

void Trainer::Look()
{
	mpAgent->Look();
}

void Trainer::TeamNames()
{
	mpAgent->TeamNames();
}

void Trainer::Recover() //TODO：需要发条信息给教练
{
	mpAgent->Recover();
}

void Trainer::CheckBall()
{
	mpAgent->CheckBall();
}

void Trainer::ChangePlayerType(Unum num, int player_type)
{
	if(num > 0) {
		mpAgent->ChangePlayerType(PlayerParam::instance().teamName(), num, player_type);
		mpObserver->SetTeammateType(num, player_type);
	}
	else {
		mpAgent->ChangePlayerType(PlayerParam::instance().opponentTeamName(), -num, player_type);
		mpObserver->SetOpponentType(-num, player_type);
	}
}

bool Trainer::Condition::CheckState(const WorldState & state)
{
	//检测当前状态的条件
	bool result = false;
	switch (mType){
	case ET_ANDComplex:
		if(!mSubCondition.empty())
		{
			for(std::vector<Trainer::Condition>::iterator it = mSubCondition.begin();
					it != mSubCondition.end(); it++)
			{
				if(!it->CheckState(state))
				{
					return false;//有一个不满足就不满足
				}
			}
		}
		break;
	case ET_ORComplex:
		if(!mSubCondition.empty())
		{
			for(std::vector<Trainer::Condition>::iterator it = mSubCondition.begin();
					it != mSubCondition.end(); it++)
			{
				if(it->CheckState(state))
				{
					return true;//有一个满足就满足
				}
			}
		}
		break;
	case ET_Time://过x个周期自然终止
		if(state.CurrentTime().T() - mpTrainer->GetLastStopTime() >= mArg1)
		{
			std::cout << "Trigger time longer than " << mArg1 << std::endl;
			result = true;
		}
		break;
	case ET_BallXbt://球的x坐标大于某值停止
		if((state.GetBall().GetPos().X() > mArg1 && !mConverse) || (!state.GetBall().GetPos().X() > mArg1 && mConverse))
		{
			std::cout << "Trigger Ball's X larger than " << mArg1 << std::endl;
			result = true;
		}
		break;
	case ET_BallXlt://球的x坐标小于某值停止
		if((state.GetBall().GetPos().X() < mArg1 && !mConverse) || (!state.GetBall().GetPos().X() < mArg1 && mConverse))
		{
			std::cout << "Trigger Ball's X less than " << mArg1 << std::endl;
			result = true;
		}
		break;
	case ET_BallYbt://球的y坐标。。
		if((state.GetBall().GetPos().Y() > mArg1 && !mConverse) || (!state.GetBall().GetPos().Y() > mArg1 && mConverse))
		{
			std::cout << "Trigger Ball's Y larger than " << mArg1 << std::endl;
			result = true;
		}
		break;
	case ET_BallYlt://。。
		if((state.GetBall().GetPos().Y() < mArg1 && !mConverse) || (!state.GetBall().GetPos().Y() < mArg1 && mConverse))
		{
			std::cout << "Trigger Ball's Y less than " << mArg1 << std::endl;
			result = true;
		}
		break;
	case ET_Ball2Playerbt://球到某人的距离大于某值
		if(!mArg2)
		{
			for(int i = 1; i <= TEAMSIZE; i++){
				if(state.GetOpponent(i).IsAlive())
					if(state.GetOpponent(i).GetPos().Dist(state.GetBall().GetPos()) > mArg1)
					{
						std::cout << "Trigger Ball to #" << i << "Player's distance longer than " << mArg1 << std::endl;
						result = true;
						break;
					}
			}
			break;
		}
		if(state.GetBall().GetPos().Dist(state.GetPlayer(mArg2).GetPos()) > mArg1)
		{
			std::cout << "Trigger Ball to #" << mArg2 << "Player's distance longer than " << mArg1 << std::endl;
			result = true;
		}
		break;
	case ET_Ball2Playerlt://球到某人的距离小于某值
		if(!mArg2)
		{
			for(int i = 1; i <= TEAMSIZE; i++){
				if(state.GetOpponent(i).IsAlive())
					if(state.GetOpponent(i).GetPos().Dist(state.GetBall().GetPos()) < mArg1)
					{
						std::cout << "Trigger Ball to #" << i << "Player's distance less than " << mArg1 << std::endl;
						result = true;
						break;
					}
			}
			break;
		}
		if(state.GetBall().GetPos().Dist(state.GetPlayer(mArg2).GetPos()) < mArg1)
		{
			std::cout << "Trigger Ball to #" << mArg2 << "Player's distance less than " << mArg1 << std::endl;
			result = true;
		}
		break;
	case ET_NonPlayOn:
		if(state.GetPlayMode() != PM_Play_On)
		{
			result = true;
			std::cout << "Trigger NonPlayOn" << std::endl;
		}
		break;
	case ET_Null: break;
	}

	if(result) //满足终止条件了
	{
		int intervalCycle = state.CurrentTime().T() - mpTrainer->GetLastStopTime();
		if(intervalCycle > mMaxInterval)
		{
			mMaxInterval = intervalCycle;
			mMaxTime = state.CurrentTime().T();
		}
		if(intervalCycle < mMinInterval)
		{
			mMinInterval = intervalCycle;
			mMinTime = state.CurrentTime().T();
		}
		mCount ++;
	}

	return result;
}

void Trainer::ReadConfigFile()
{
	//读取配置文件
	char train_file[128];
	sprintf( train_file, "./%s", PlayerParam::instance().trainDataFile().c_str());

	ifstream fin( train_file );

	if ( ! fin.is_open() )
	{
		std::cerr << __FILE__ << ':' << __LINE__ << ':'
				<< " ERROR: failed to open train data file "
				<< std::endl;
	}
	string conf, section;
	vector<string> content;
	while (getline(fin, conf))
	{
		if (conf[0] == '[')
		{
			if(!content.empty())
				Parse(section, content);

			section = conf.substr(1, conf.rfind(']') - 1); //new section
			content.clear();
		}
		else if (conf[0] != '#' && (!conf.empty()))
		{
			content.push_back(conf);
		}
	}

	Parse(section, content);
}

bool Trainer::ReadRcgFile()
{
	//TODO 读取之前的PlayMode
	string line;
	const char * buf = 0;
	ifstream fin( mTrainRcg );
	int time = 0, n_read = 0;

	const int begin_time = mTrainTime - mInitialTime;
	const int end_time = mTrainTime;

	ServerPlayMode server_playmode = SPM_PlayOn;

	while (1) {
		vector<PlayerState> ps;
		pair<Vector, Vector> bs;

		bool read = false;

		while (getline(fin, line))
		{
			read = true;
			if(!line.compare(0, 5, "(show"))
			{
				if(sscanf( line.c_str(), "(show %d %n",&time, &n_read ) != 1)
				{
					PRINT_ERROR("error time info:" << line);
					return false;
				}

				if(time >= begin_time) {
					buf = line.c_str();
					buf += n_read;
					break;
				}
			}
			else if ( line.compare( 0, 10, "(playmode " ) == 0 )
			{
				int time = 0;
				char pm_string[32];

				if ( std::sscanf( line.c_str(),
						" (playmode %d %31[^)] ) ",
						&time, pm_string ) != 2 )
				{
					std::cerr << "error: "
							<< "Illegal playmode line. \"" << line << "\"" << std::endl;
					return false;
				}

				server_playmode = ServerPlayModeMap::instance().GetServerPlayMode(pm_string);
			}
		}

		if (!read) {
			break;
		}

		mServerPlayModeList.push_back(server_playmode);

		float bx,by,bvx,bvy;

		if( buf == 0 ) //没有找到对应周期
		{
			return false;
		}

		{
			if ( sscanf( buf, " ((b) %f %f %f %f) %n",
					&bx, &by, &bvx, &bvy,
					&n_read ) != 4 )
			{
				PRINT_ERROR( "error ball info:" << line);
				return false;
			}
			buf += n_read;
			bs.first= Vector (bx, by);
			bs.second = Vector (bvx, bvy);
			bs.second /= 0.94;
			bs.first -= bs.second; //这是因为Server接到后会模拟一周期
		}

		// players
		char side;
		short unum;
		short type;
		int state;
		float x, y, vx, vy, body, neck;
		for ( int i = 0; i < ServerParam::TEAM_SIZE * 2; ++i )
		{
			if ( *buf == '\0' || *buf == ')' ) break;

			if ( sscanf( buf,
					" ((%c %hd) %hd %x %f %f %f %f %f %f %n",
					&side, &unum,
					&type, &state,
					&x, &y, &vx, &vy, &body, &neck,
					&n_read ) != 10 )
			{
				PRINT_ERROR( "error player info:" << buf);
				return false;
			}
			buf += n_read;

			int idx = unum - 1;
			if ( side == 'r' ) idx += ServerParam::TEAM_SIZE;
			if ( idx < 0 || ServerParam::TEAM_SIZE * 2 <= idx )
			{
				PRINT_ERROR( "error player info:" << buf);
				return false;
			}

			if(side == 'r')
				unum = -unum;

			//对球员的微调安排在开场前，而不是建立前，因为牵涉到Server发来的异构
			ps.push_back(Trainer::PlayerState(unum, Vector(x,y) , Vector(vx, vy), body, type));

			while(strncmp(buf, "((", 2))
			{
				buf++;
				if ( *buf == '\0' ) break;
			}
		}

		if (ps.size() == 22) {
			mPlayerStatesList.push_back(ps);
			mBallStateList.push_back(bs);
		}

		if (time >= end_time) {
			break;
		}
	}

	Assert(mPlayerStatesList.size() == mBallStateList.size());
	Assert(mPlayerStatesList.size() == mServerPlayModeList.size());

	return !mPlayerStatesList.empty();
}

void Trainer::Condition::OptimizeConditionTree()
{
	for(std::vector<Trainer::Condition>::iterator it = mSubCondition.begin(); it != mSubCondition.end();)
	{
		if(it->mType == ET_Null && it->mSubCondition.empty()) //删除其空子条件
		{
			it = mSubCondition.erase(it);
		}
		else
		{
			it->OptimizeConditionTree();
			it ++ ;
		}
	}
	if(mType == ET_Null){
		if(mSubCondition.size() == 1) //子条件仅有1个时，提升
		{
			Trainer::Condition & sc = mSubCondition[0];
			mType = sc.mType;
			mArg1 = sc.mArg1;
			mArg2 = sc.mArg2;
			mSubCondition = sc.mSubCondition;
		}
		else
		{
			PRINT_ERROR("error condition relationship");
		}
	}
}

void Trainer::Parse(const string & section, const vector<string> & content)
{
	Unum num;
	double x, y, vx, vy, dir;
	int playerType;
	if (section == "Scene")
	{
		sscanf( content.at(0).c_str(), "%d %s", &mTrainTime, mTrainRcg);
		mHaveRcg = ReadRcgFile();
	}
	else if ( section == "NeedOpponent")
	{
		Unum t;
		stringstream sin;
		sin.str(*content.begin());
		while(sin >> t)
		{
			mNeedOpponentList.push_back(t);
		}
	}
	else if( section == "Converse")
	{
		ReadConverseConf(content.at(0));
	}
	else if( section == "InitialTime")
	{
		sscanf( content.at(0).c_str(), "%d", &mInitialTime);
	}
	else if (!mHaveRcg) {
		mInitialTime = 0;
		if (section == "BallData")
		{
			sscanf( content.at(0).c_str(), "%lf %lf %lf %lf", &x, &y, &vx, &vy);
			mBallStateList.push_back(pair<Vector, Vector>(Vector(x, y), Vector(vx, vy)));
		}
		else if (section == "TeammateData")
		{
			vector<PlayerState> ps;
			for(vector<string>::const_iterator it = content.begin(); it != content.end(); it++)
			{
				sscanf( it->c_str(), "%d %d %lf %lf %lf %lf %lf", &num, &playerType, &x, &y, &vx, &vy, &dir);
				ps.push_back(PlayerState(num, Vector(x, y), Vector(vx, vy), dir, playerType));
			}
			mPlayerStatesList.push_back(ps);
		}
		else if (section == "OpponentData")
		{
			vector<PlayerState> ps;
			for(vector<string>::const_iterator it = content.begin(); it != content.end(); it++)
			{
				sscanf( it->c_str(), "%d %d %lf %lf %lf %lf %lf", &num, &playerType, &x, &y, &vx, &vy, &dir);
				ps.push_back(PlayerState(-num, Vector(x, y), Vector(vx, vy), dir, playerType));
			}
			mPlayerStatesList.push_back(ps);
		}
	}
	else if (section == "EndCondition")
	{
		std::string tmp; //把几行变成一行，便于控制结构

		for(vector<string>::const_iterator it = content.begin(); it != content.end(); it++)
		{
			tmp.append(it->c_str());
		}
		tmp.append("\0");
		ParseCondition(tmp);
	}
}

void Trainer::ParseCondition(const std::string & content)
{
	Trainer::Condition * nowCondition = mpEndCondition; //当前条件层
	bool beforeEquator = true;
	int n = 0;
	char name[128];
	char buffer[128];

	for(std::string::const_iterator it = content.begin(); it != content.end(); it++)
	{
		switch(*it)
		{
		case '(':
			nowCondition = nowCondition->AddSubCondition(Condition(this, ET_Null, 0, 0, nowCondition, mConverse));
			break;
		case ')':
			buffer[n] = 0;
			if(n!=0)
				AddCondition(nowCondition, name, buffer);
			nowCondition = nowCondition->GetSuperCondition();
			beforeEquator = true;
			n = 0;
			break;
		case '&':
			buffer[n] = 0;
			if(nowCondition->GetConditionType() == ET_ORComplex)
			{
				Trainer::Condition * tmp;
				nowCondition->SetSuperCondition(new Trainer::Condition(this, ET_ANDComplex,0,0,0, mConverse));
				tmp = nowCondition->GetSuperCondition();
				if(nowCondition == mpEndCondition)
				{
					mpEndCondition = tmp;
				}
				nowCondition = tmp;
				tmp = NULL;
			}
			else
			{
				nowCondition->SetConditionType(ET_ANDComplex);
			}
			if(n!=0)
				AddCondition(nowCondition, name, buffer);
			beforeEquator = true;
			n = 0;
			break;
		case '|':
			buffer[n] = 0;
			if(nowCondition->GetConditionType() == ET_ANDComplex) //如果发生冲突，降一级
			{
				Trainer::Condition * tmp;
				nowCondition->SetSuperCondition(new Trainer::Condition(this, ET_ANDComplex,0,0,0, mConverse));
				tmp = nowCondition->GetSuperCondition();
				if(nowCondition == mpEndCondition)
				{
					mpEndCondition = tmp;
				}
				nowCondition = tmp;
				tmp = NULL;
			}
			else
			{
				nowCondition->SetConditionType(ET_ORComplex);
			}
			if(n!=0)
				AddCondition(nowCondition, name, buffer);
			beforeEquator = true;
			n = 0;
			break;
		case '=':
			name[n] = 0;
			beforeEquator = !beforeEquator;
			n = 0;
			break;
		case ' ': //跳过空格
			break;
		default:
			if(beforeEquator)
			{
				name[n++] = *it;
			}
			else
			{
				buffer[n++] = *it;
			}
			break;
		}
	}
	if(n != 0)
	{
		buffer[n] = 0;
		AddCondition(nowCondition, name, buffer);
	}
	mpEndCondition->OptimizeConditionTree();
}

void Trainer::AddCondition(Trainer::Condition * superCondition, const char* name, const char* buffer)
{
	if(!strcmp(buffer,"false") || !strcmp(buffer,"off"))
		return;

	Trainer::ConditionType type = ET_Null;
	double arg; //初始化而已
	static int keyPlayer=0;
	if(!strcmp(name,"Time"))
	{
		sscanf(buffer,"%lf",&arg);
		type = ET_Time;
	}
	else if(!strcmp(name, "BallX>"))
	{
		sscanf(buffer,"%lf",&arg);
		type = ET_BallXbt;
	}
	else if(!strcmp(name, "BallX<"))
	{
		sscanf(buffer,"%lf",&arg);
		type = ET_BallXlt;
	}
	else if(!strcmp(name, "BallY>"))
	{
		sscanf(buffer,"%lf",&arg);
		type = ET_BallYbt;
	}
	else if(!strcmp(name, "BallY<"))
	{
		sscanf(buffer,"%lf",&arg);
		type = ET_BallYlt;
	}
	else if(!strcmp(name, "KeyPlayer"))
	{
		sscanf(buffer,"%d",&keyPlayer);
	}
	else if(!strcmp(name, "Ball2Player>"))
	{
		sscanf(buffer,"%lf",&arg);
		type = ET_Ball2Playerbt;
	}
	else if(!strcmp(name, "Ball2Player<"))
	{
		sscanf(buffer,"%lf",&arg);
		type = ET_Ball2Playerlt;
	}
	else if(!strcmp(name, "NonPlayOn"))
	{
		type = ET_NonPlayOn;
	}

	if(type != ET_Null)
	{
		if(type == ET_Ball2Playerbt || type == ET_Ball2Playerlt)
			superCondition->AddSubCondition(Trainer::Condition(this, type, arg, keyPlayer, superCondition, mConverse));
		else
			superCondition->AddSubCondition(Trainer::Condition(this, type, arg, 0, superCondition, mConverse));
	}
	else if(strcmp(name, "KeyPlayer"))
	{
		PRINT_ERROR("error end condition: " << name);
	}
}

void Trainer::Condition::Report(ofstream  & os) const
{
	char name[32];
	switch (mType){
	case ET_ANDComplex:
	case ET_ORComplex://复合条件仅记录子条件
		for(vector<Trainer::Condition>::const_iterator it = mSubCondition.begin(); it != mSubCondition.end(); it++)
		{
			it->Report(os);
		}
		break;
	case ET_Null: return;
	case ET_Time://过x个周期自然终止
		sprintf(name,"After %4.0f Cycle Stop", mArg1);
		break;
	case ET_BallXbt://球的x坐标大于某值停止
		sprintf(name,"Ball.X > %5.2f", mArg1);
		break;
	case ET_BallXlt://球的x坐标小于某值停止
		sprintf(name,"Ball.X < %5.2f", mArg1);
		break;
	case ET_BallYbt://球的y坐标。。
		sprintf(name,"Ball.Y > %5.2f", mArg1);
		break;
	case ET_BallYlt://。。
		sprintf(name,"Ball.Y < %5.2f", mArg1);
		break;
	case ET_Ball2Playerbt://球到某人的距离大于某值
		sprintf(name,"Ball to %2.0f 's Distance > %5.2f", mArg2, mArg1);
		break;
	case ET_Ball2Playerlt://球到某人的距离小于某值
		sprintf(name,"Ball to %2.0f 's Distance < %5.2f", mArg2, mArg1);
		break;
	case ET_NonPlayOn:
		sprintf(name,"NonPlayOn");
		break;
	}
	os << name << endl;
	os << "Count:" << mCount << endl;
	os << "MaxIntervalCycle:" << mMaxInterval << "\t Time:" << mMaxTime << endl;
	os << "MinIntervalCycle:" << mMinInterval << "\t Time:" << mMinTime << endl;
}

void Trainer::Record()
{
	//记录统计信息
	ofstream os;
	os.open("./train/train.report");
	if(mConverse)
		os << "Converse Train!!" << endl;
	os << "Total Train Count:" << mTrainCount << endl;
	mpEndCondition->Report(os);
	os.close();
}

bool Trainer::CheckOpponent() const
{
	WorldState & world = mpAgent->World();
	if(world.CurrentTime().T() > 1)
	{
		return true;  //已经开场之后，如果有人不齐的话，估计就是死了……
	}
	for(std::vector<Unum>::const_iterator it = mNeedOpponentList.begin(); it != mNeedOpponentList.end(); it++)
	{
		if(!world.GetOpponent(*it).IsAlive())
			return false;
	}
	return true;
}

void Trainer::InitializeStadium()
{
	for(std::vector<Trainer::PlayerState>::const_iterator it = mPlayerStatesList[0].begin(); it != mPlayerStatesList[0].end(); it++)
	{
//		ChangePlayerType(it->mUnum, it->mPlayerType);让教练自己选择异构吧
	}

	for(vector<vector<Trainer::PlayerState> >::iterator it = mPlayerStatesList.begin(); it != mPlayerStatesList.end(); it++)
	{
		for(std::vector<Trainer::PlayerState>::iterator it2 = it -> begin(); it2 != it -> end(); it2++)
		{
			it2->mVel /= PlayerParam::instance().HeteroPlayer(it2->mPlayerType).playerDecay();
			it2->mPos -= it2->mVel;
		}
	}
	mInitialized = true;
}

void Trainer::PrepareForTrain()
{
	static uint i = 0;

	if (i == 0) {
		if (mpAgent->World().CurrentTime().T()) {
			std::cerr << "#" << PlayerParam::instance().teamName() << " Trainer: episode " << mTrainCount - 1 << " last " << mpAgent->World().CurrentTime().T() - mLastStopTime << " cycles"<< std::endl;
		}

		std::cerr << "#" << PlayerParam::instance().teamName() << " Trainer: episode " << mTrainCount << " prepair @ " << mpAgent->World().CurrentTime() << std::endl;
	}

	mLastStopTime = mpAgent->World().CurrentTime().T();

	if(!mConverse)
		MoveBall(mBallStateList[i].first, mBallStateList[i].second);
	else
		MoveBall(-mBallStateList[i].first, -mBallStateList[i].second);

	for(std::vector<Trainer::PlayerState>::const_iterator it = mPlayerStatesList[i].begin(); it != mPlayerStatesList[i].end(); it++) {
		if(!mConverse)
		{
			MovePlayer(it->mUnum, it->mPos, it->mVel, it->mBodyDir);
		}
		else
		{
			if(it->mUnum > 0)
			{
				MovePlayer(-mTm2Opp[it->mUnum], -it->mPos, -it->mVel, GetNormalizeAngleDeg(-180 + it->mBodyDir));
			}
			else
			{
				MovePlayer(mOpp2Tm[-it->mUnum], -it->mPos, -it->mVel, GetNormalizeAngleDeg(-180 + it->mBodyDir));
			}
		}
	}
	mpAgent->ChangePlayMode(mServerPlayModeList[i]);
	Recover();
	i++;

	if(i >= mPlayerStatesList.size()) {
		i = 0;
		mPrepared = true;
		std::cerr << "#" << PlayerParam::instance().teamName() << " Trainer: episode " << mTrainCount << " start @ " << mpAgent->World().CurrentTime() << std::endl;

		mTrainCount ++;
	}
}

void Trainer::ReadConverseConf(const std::string & str)
{
	stringstream sin;
	sin.str(str);
	int i = 1;
	for (; i<=TEAMSIZE && sin >> mTm2Opp[i]; mOpp2Tm[mTm2Opp[i]] = i, ++i);

	if(i == TEAMSIZE + 1)
	{
		mConverse = true;
		cout << "Conversely train!!" << endl;
	}


}

Vector Trainer::FindValidPoint(bool isLeft)
{
	Vector tmp;
	tmp.SetX(isLeft ? ServerParam::instance().PITCH_LENGTH / 2 - 0.1 : -ServerParam::instance().PITCH_LENGTH / 2 + 0.1);
	do
	{
		for(double y = -6.5; y < 6.5; y += 0.1)
		{
			tmp.SetY(y);
			if(mpAgent->GetWorldState().GetPlayer(mpAgent->Info().GetPositionInfo().GetClosePlayerToPoint(tmp).front()).GetPos().Dist(tmp) > 1.0)
			{
				return tmp;
			}
		}
		tmp.SetX(tmp.X() + 0.5);
	}
	while(1);
	Assert(0);
	return Vector(0.0);
}

void Trainer::Score(bool isLeft)
{
	if(mpAgent->World().GetPlayMode() != PM_Play_On)
	{
		ChangePlayMode(PM_Play_On);
	}
	Vector tmp = FindValidPoint(isLeft) - mpAgent->World().Ball().GetVel();
	if(isLeft)
	{
		tmp.SetX(Min(tmp.X(), ServerParam::instance().PITCH_LENGTH / 2 - 3.1));
	}
	else
	{
		tmp.SetX(Max(tmp.X(), -ServerParam::instance().PITCH_LENGTH / 2 + 3.1));
	}
	MoveBall(tmp, Vector(isLeft?ServerParam::instance().ballSpeedMax() : -ServerParam::instance().ballSpeedMax(), 0));
	mPrepared = true;
}

void Trainer::EndTrain()
{
	if(mpAgent->World().CurrentTime().T() == 5999)
	{
		ChangePlayMode(PM_Time_Over);
	}
}
