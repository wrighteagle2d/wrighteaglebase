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

#include "Formation.h"
#include "Agent.h"
#include "CommunicateSystem.h"
#include "PlayerParam.h"
#include "Logger.h"
#include <fstream>
#include <sstream>
using namespace std;

PlayerRole RoleType::ToPlayerRole() const
{
	switch (mLineType){
	case LT_Goalie:
		return PR_Goalie;
	case LT_Defender:
		switch(mPositionType){
		case PT_Left:
		case PT_Right: return PR_BackSide;
		default: return PR_BackCenter;
		}
		break;
	case LT_Midfielder:
		switch(mPositionType){
		case PT_Left:
		case PT_Right: return PR_MidSide;
		default: return PR_MidCenter;
		}
		break;
	case LT_Forward:
		switch(mPositionType){
		case PT_Left:
		case PT_Right: return PR_ForwardSide;
		default: return PR_ForwardCenter;
		}
		break;
	default: return PR_Max;
	}
	return PR_Max;
}

FormationBase::FormationBase(FormationType type):
	mFormationType(type),
	mHBallFactor(0),
	mVBallFactor(0)
{
	for (int i(0); i <= TEAMSIZE; ++i)
	{
		mUnum2Index[i] = i;
		mIndex2Unum[i] = i;
	}
}

/**
 * Get unum from formation position.
 * \param index_i line, goalie is 0 and attackers are FORMATION_LINE_NUM.
 * \param index_j list, left most is 1.
 * \return unum, 0 for non-existence.
 */
Unum FormationBase::GetUnumFromPos(int index_i, int index_j)
{
    return mIndex2Unum[mPos2Index[index_i][index_j-1]];
}

/**
 * Set unum for formation position. This method will change mIndex2Unum
 * and mIndex2Unum to let player1 come to the given position and the original
 * player come to player1's position.
 * \param player1
 * \param index_i line, goalie is 0 and attackers are FORMATION_LINE_NUM.
 * \param index_j list, left most is 1.
 */
bool FormationBase::SetUnumForPos(Unum player1, int index_i, int index_j)
{
	int pos1 = mUnum2Index[player1], pos2 = mPos2Index[index_i][index_j-1];
	Unum player2 = mIndex2Unum[pos2];
	if (player1 != player2)
	{
		swap(mIndex2Unum[pos1], mIndex2Unum[pos2]);
		swap(mUnum2Index[player1], mUnum2Index[player2]);
		return true;
	}
	return false;
}

/**
 * Get the center of the formation.
 * Formation center 就是己方所有队员的位置的加权平均。
 * \param world_state
 * \param is_teammate
 * \param min_conf by default is 0.6.
 * \return center of the formation.
 */
const Vector & FormationBase::GetFormationCenter(const WorldState & world_state, bool is_teammate, double min_conf)
{
    double conf = 0.0;
	double total_conf = 0.0;
	Vector center = Vector(0.0, 0.0);

	for (int i = 1; i <= TEAMSIZE; ++i)
    {
        if (is_teammate)
        {
		    if (mIndex2Unum[i] == world_state.GetTeammateGoalieUnum())
            {
                continue;
		    }
		    conf = world_state.GetTeammate(i).GetPosConf();
			if (conf > min_conf)
			{
				center += (world_state.GetTeammate(i).GetPos() + mOffsetMatrix[mUnum2Index[i]][0]) * conf;
				total_conf += conf;
			}
		}
		else
		{
			if (mIndex2Unum[i] == world_state.GetOpponentGoalieUnum())
			{
				continue;
			}
			conf = world_state.GetOpponent(i).GetPosConf();
			if (conf > min_conf)
			{
				center += (world_state.GetOpponent(i).GetPos() + mOffsetMatrix[mUnum2Index[i]][0]) * conf;
				total_conf += conf;
			}
		}
	}

	mFormationCT = center / total_conf;
	return mFormationCT;
}

/**
 * Set the center of the formation by someone's expected formation point.
 * \param unum player's unum, always be positive.
 * \param position expected formation point.
 */
void FormationBase::SetFormationCenter(Unum num, Vector position)
{
	position = mActiveField[mUnum2Index[num]].AdjustToWithin(position);//调整到有效范围
	mFormationCT = position + mOffsetMatrix[mUnum2Index[num]][0];
}

/**
 * Get player's formation point.
 * \param world_state
 * \param is_teammate
 * \param unum player's unum, always be positive.
 * \param min_conf by default is 0.6.
 * \return player's formation point.
 */
Vector FormationBase::GetFormationPoint(const WorldState & world_state,	bool is_teammate, Unum unum, double min_conf)
{
	if (is_teammate)
	{
		return GetFormationCenter(world_state, is_teammate, min_conf) + mOffsetMatrix[0][mUnum2Index[unum]];
	}
	else
	{
		return GetFormationCenter(world_state, is_teammate, min_conf) - mOffsetMatrix[0][mUnum2Index[unum]];
	}
}

/**
 * Expected goalie position.
 * \param ball_pos
 * \param run
 * \param goalie_pos
 * \param cycle_delay
 * \return Expected goalie position.
 */
Vector FormationBase::GetExpectedGoaliePos(const Vector & ball_pos, double run, const Vector & goalie_pos, int cycle_delay)
{
	Vector p[2];
	p[0] = Vector(ServerParam::instance().PITCH_LENGTH / 2.0, -ServerParam::instance().goalWidth() / 2.0 + 0.1); // top
	p[1] = Vector(ServerParam::instance().PITCH_LENGTH / 2.0,  ServerParam::instance().goalWidth() / 2.0 - 0.1); // down

	double tmp, distg;
	distg = ball_pos.Dist(goalie_pos);
	AngleDeg mid;
	mid = GetNormalizeAngleDeg(((p[1] - ball_pos).Dir()	+ (p[0] - ball_pos).Dir()) / 2);//取角平分线
	tmp	= ball_pos.Dist(Vector(ServerParam::instance().PITCH_LENGTH / 2.0, 0.0)) - 2.0;
	Vector tgPos, jgPos;
	tgPos = ball_pos + Polar2Vector(distg, mid); // 估计守门员可能的位置,守门员向角平分线运动
	if (tgPos.Dist(ball_pos) > tmp)
	{
		tgPos = ball_pos + (tgPos - ball_pos).SetLength(tmp); // 调整到球场内
	}
	if (goalie_pos.Dist(tgPos) < run + cycle_delay * 0.8)
	{
		jgPos = tgPos; // 守门员向tgpos尽可能运动
	}
	else
	{
		jgPos = goalie_pos + Polar2Vector(run + cycle_delay * 0.8, (tgPos - goalie_pos).Dir());
	}
	return jgPos;
}

#define formation_tactic_map_register(tactic) do { \
		formation_tactic_map[#tactic] = FTT_##tactic; \
		mTactics[FTT_##tactic] = new FormationTactic##tactic(); \
	} while (0)

TeammateFormation::TeammateFormation(FormationType type, const char * config_file) :
	FormationBase(type)
{
	mTactics.bzero();
	map<string, FormationTacticType> formation_tactic_map;


	formation_tactic_map_register(KickOffPosition);

	ifstream fin(config_file);
	string conf, section;
	vector<string> content;
	while (getline(fin, conf))
	{
		if (conf[0] == '[')
		{
			if (section == "Formation")
			{
				InitialFormation(content);
			}
			else if (formation_tactic_map.find(section) != formation_tactic_map.end())
			{
				mTactics[formation_tactic_map[section]]->Initial(content, mIndex2Unum.instance(), mUnum2Index.instance());
			}
			section = conf.substr(1, conf.rfind(']') - 1);
			content.clear();
		}
		else if (conf[0] != '#' && conf.size() > 0)
		{
			content.push_back(conf);
		}
	}
	if (section == "Formation")
	{
		InitialFormation(content);
	}
	else if (formation_tactic_map.find(section) != formation_tactic_map.end())
	{
		mTactics[formation_tactic_map[section]]->Initial(content, mIndex2Unum.instance(), mUnum2Index.instance());
	}
}

TeammateFormation::~TeammateFormation()
{
	for (int i(0); i < FTT_MAX; ++i)
	{
		delete mTactics[i];
	}
}

void TeammateFormation::InitialFormation(std::vector<std::string> & config)
{
	vector<string>::iterator conf_iter(config.begin());
	stringstream sin;
	for (int i(FORMATION_LINE_NUM); i >= 0 && conf_iter != config.end(); --i, ++conf_iter)
	{
		sin.str(*conf_iter); sin.clear();
		int j(0);
		for (; sin >> mPos2Index[i][j]; ++j)
		{
			mPlayerRole[mPos2Index[i][j]].mIndexX = i;
			mPlayerRole[mPos2Index[i][j]].mIndexY = j+1;
		}
		mLineArrange[i] = j;
	}
	if (conf_iter != config.end()) { sin.str(*conf_iter); sin.clear(); }
	for (int i(0); i < FORMATION_LINE_NUM - 1 && sin >> mHInterval[i]; ++i);
	if (conf_iter != config.end()) { sin.str(*(++conf_iter)); sin.clear(); }
	for (int i(1); i <= FORMATION_LINE_NUM && sin >> mVInterval[i]; ++i);
	
	if (conf_iter != config.end()) { sin.str(*(++conf_iter)); sin.clear(); }
	sin >> m_FORMATION_MAX_X;
	if (conf_iter != config.end()) { sin.str(*(++conf_iter)); sin.clear(); }
	sin >> m_FORMATION_MAX_Y;
	
	SetTeammateRole();
	SetHInterval(mHInterval[0], mHInterval[1]);
	SetVInterval(mVInterval[1], mVInterval[2], mVInterval[3]);

	if (conf_iter != config.end()) { sin.str(*(++conf_iter)); sin.clear(); }
	for (int i(1), j; i <= TEAMSIZE && sin>>j; ++i)
	{
		mIndex2Unum[i] = j;
		mUnum2Index[j] = i;
	}
}

void TeammateFormation::SetTeammateRole()
{
	for (int i = 1; i <= TEAMSIZE; ++i)
	{
		mPlayerRole[i].mLineType = LineType(mPlayerRole[i].mIndexX);

		/** 确定mPositionType */
		double midoffset = mPlayerRole[i].mIndexY - (mLineArrange[mPlayerRole[i].mIndexX]+1) * 0.5;
		if (fabs(midoffset) < FLOAT_EPS)
		{
			mPlayerRole[i].mPositionType = PT_Middle;
		}
		else if (fabs(midoffset - 0.5) < FLOAT_EPS)
		{
			mPlayerRole[i].mPositionType = PT_RightMiddle;
		}
		else if (fabs(midoffset + 0.5) < FLOAT_EPS)
		{
			mPlayerRole[i].mPositionType = PT_LeftMiddle;
		}
		else if (fabs(midoffset - 1.25) < 0.25 + FLOAT_EPS)
		{ //1.5 or 1.0
			mPlayerRole[i].mPositionType = PT_Right;
		}
		else if (fabs(midoffset + 1.25) < 0.25 + FLOAT_EPS)
		{ //1.5 or 1.0
			mPlayerRole[i].mPositionType = PT_Left;
		}
		else if (midoffset > 2.0 - FLOAT_EPS)
		{
			mPlayerRole[i].mPositionType = PT_RightRight;
		}
		else
		{
			mPlayerRole[i].mPositionType = PT_LeftLeft;
		}
	}
}

void TeammateFormation::SetHInterval(double back_middle, double middle_front)
{
	mHBallFactor = (m_FORMATION_MAX_X - (back_middle + middle_front) / 2.0) / ServerParam::instance().PITCH_LENGTH * 2.0;
	mHBallFactor = Max(0.0, mHBallFactor);

	RoleType role_i, role_j;
	for (int i = 1; i <= TEAMSIZE; ++i)
	{
		role_i = mPlayerRole[i];
		if (role_i.mIndexX == 1) /** 后卫 */
		{
			mActiveField[i].SetLeft(-m_FORMATION_MAX_X);
			mActiveField[i].SetRight(m_FORMATION_MAX_X - mHInterval[0] - mHInterval[1]);
			mOffsetMatrix[0][i].SetX(-mHInterval[0]);
			mOffsetMatrix[i][0].SetX(mHInterval[0]);
		}
		else if (role_i.mIndexX == 2) /** 中场 */
		{
			mActiveField[i].SetLeft(-m_FORMATION_MAX_X + mHInterval[0]);
			mActiveField[i].SetRight(m_FORMATION_MAX_X - mHInterval[1]);
			mOffsetMatrix[0][i].SetX(0.0);
			mOffsetMatrix[i][0].SetX(0.0);
		}
		else if (role_i.mIndexX == 3) /** 前锋 */
		{
			mActiveField[i].SetLeft(-m_FORMATION_MAX_X + mHInterval[0] + mHInterval[1]);
			mActiveField[i].SetRight(m_FORMATION_MAX_X);
			mOffsetMatrix[0][i].SetX(mHInterval[1]);
			mOffsetMatrix[i][0].SetX(-mHInterval[1]);
		}

		for (int j = 1; j <= TEAMSIZE; ++j)
		{
			role_j = mPlayerRole[j];

			if (role_i.mIndexX == 1)
			{
				if (role_j.mIndexX == 1)
				{
					mOffsetMatrix[i][j].SetX(0.0);
				}
				else if (role_j.mIndexX == 2)
				{
					mOffsetMatrix[i][j].SetX(mHInterval[0]);
				}
				else if (role_j.mIndexX == 3)
				{
					mOffsetMatrix[i][j].SetX(mHInterval[0] + mHInterval[1]);
				}
			}
			else if (role_i.mIndexX == 2)
			{
				if (role_j.mIndexX == 1)
				{
					mOffsetMatrix[i][j].SetX(-mHInterval[0]);
				}
				else if (role_j.mIndexX == 2)
				{
					mOffsetMatrix[i][j].SetX(0.0);
				}
				else if (role_j.mIndexX == 3)
				{
					mOffsetMatrix[i][j].SetX(mHInterval[1]);
				}
			}
			else if (role_i.mIndexX == 3)
			{
				if (role_j.mIndexX == 1)
				{
					mOffsetMatrix[i][j].SetX(-mHInterval[0] - mHInterval[1]);
				}
				else if (role_j.mIndexX == 2)
				{
					mOffsetMatrix[i][j].SetX(-mHInterval[1]);
				}
				else if (role_j.mIndexX == 3)
				{
					mOffsetMatrix[i][j].SetX(0.0);
				}
			}
		}
	}
}

void TeammateFormation::SetVInterval(double back, double middle, double front)
{
	double max_wide = Max(back * (mLineArrange[1] - 1), middle * (mLineArrange[2] - 1));
	max_wide = Max(max_wide, front * (mLineArrange[3] - 1));
	mVBallFactor = (m_FORMATION_MAX_Y - max_wide / 2.0) / ServerParam::instance().PITCH_WIDTH * 2.0;
	mVBallFactor = Max(0.0, mVBallFactor);

	RoleType role_i, role_j;
	for (int i = 1; i <= TEAMSIZE; ++i)
	{
		role_i = mPlayerRole[i];
		mActiveField[i].SetTop(-m_FORMATION_MAX_Y + (role_i.mIndexY - 1) * mVInterval[role_i.mIndexX]);
		mActiveField[i].SetBottom(m_FORMATION_MAX_Y - (mLineArrange[role_i.mIndexX] - role_i.mIndexY) * mVInterval[role_i.mIndexX]);
		double iy = (role_i.mIndexY - (mLineArrange[role_i.mIndexX] + 1) / 2.0) * mVInterval[role_i.mIndexX];
		mOffsetMatrix[0][i].SetY(iy);
		mOffsetMatrix[i][0].SetY(-iy);

		for (int j = 1; j <= TEAMSIZE; ++j)
		{
			role_j = mPlayerRole[j];
			double jy = (role_j.mIndexY - (mLineArrange[role_j.mIndexX] + 1) / 2.0)	* mVInterval[role_j.mIndexX];
			mOffsetMatrix[i][j].SetY(jy - iy);
		}
	}
}

OpponentFormation::OpponentFormation(Unum goalie_unum, FormationType type) :
	FormationBase(type),
	mGoalieUnum(goalie_unum)
{
	mTactics.bzero();
	map<string, FormationTacticType> formation_tactic_map;

	mUsedTimes = 0;
	mHIntervalTimes[0] = 0;
	mHIntervalTimes[1] = 0;
}

OpponentFormation::~OpponentFormation()
{
	for (int i(0); i < FTT_MAX; ++i)
	{
		delete mTactics[i];
	}
}

double OpponentFormation::mForwardMaxX = -100000;
double OpponentFormation::mDefenderMinX = 100000;

void OpponentFormation::SetFormationRole()
{
	mLineArrange[0] = 1;
	mLineArrange[1] = mLineMember[0].size();
	mLineArrange[2] = mLineMember[1].size();
	mLineArrange[3] = mLineMember[2].size();
	//建立位置索引
	//第一个是门将，索引(0,1)
	//其他队员
	int xidx;
	int yidx;
	int n;
	for (n = 1; n <= 11; n++)
		mPlayerRole[n].mLineType = LT_Null;
	for (xidx = 1; xidx <= 3; xidx++)
	{
		for (yidx = 1; yidx <= mLineArrange[xidx]; yidx++)
		{
			n = mLineMember[xidx - 1][yidx - 1]; // 得到号码
			if (n == mGoalieUnum)
			{
				mPos2Index[0][1] = n;
				mPlayerRole[mGoalieUnum].mIndexX = 0;
				mPlayerRole[mGoalieUnum].mIndexY = 1;
				mPlayerRole[mGoalieUnum].mLineType = LT_Goalie;
			}
			else
			{
				mPos2Index[xidx][yidx] = n;
				mPlayerRole[n].mIndexX = xidx;
				mPlayerRole[n].mIndexY = yidx;
				mPlayerRole[n].mLineType = LineType(xidx + 1);
				double midoffset = yidx - (mLineArrange[xidx] + 1) * 0.5;
				if (fabs(midoffset) < FLOAT_EPS)
				{
					mPlayerRole[n].mPositionType = PT_Middle;
				}
				else if (fabs(midoffset - 0.5) < FLOAT_EPS)
				{
					mPlayerRole[n].mPositionType = PT_RightMiddle;
				}
				else if (fabs(midoffset + 0.5) < FLOAT_EPS)
				{
					mPlayerRole[n].mPositionType = PT_LeftMiddle;
				}
				else if (fabs(midoffset - 1.25) < 0.25 + FLOAT_EPS)
				{ //1.5 or 1.0
					mPlayerRole[n].mPositionType = PT_Right;
				}
				else if (fabs(midoffset + 1.25) < 0.25 + FLOAT_EPS)
				{ //1.5 or 1.0
					mPlayerRole[n].mPositionType = PT_Left;
				}
				else if (midoffset > 2.0 - FLOAT_EPS)
				{
					mPlayerRole[n].mPositionType = PT_RightRight;
				}
				else
				{
					mPlayerRole[n].mPositionType = PT_LeftLeft;
				}
			}
		}
	}
}

void OpponentFormation::SetOffsetMatrix()
{
	if (mHIntervalTimes[0] == 0	|| mHIntervalTimes[1] == 0)
		return;
	RoleType iRole, jRole;
	const double MAXX = 50.0;
	const double MINX = -50.0;
	const double MAXY = 23.0;
	const double MINY = -23.0;

	mHBallFactor = (MAXX - (mHInterval[0] + mHInterval[1]) * 0.5) / ServerParam::instance().PITCH_LENGTH * 2;
	mHBallFactor = Max(0.0, mHBallFactor);

	double maxwide = Max(mVInterval[1] * ( mLineArrange[1] - 1 ) , mVInterval[2] * ( mLineArrange[2] - 1 ));
	maxwide	= Max(maxwide, mVInterval[3] * (mLineArrange[3] - 1) );
	mVBallFactor = (MAXY - maxwide * 0.5) / ServerParam::instance().PITCH_WIDTH * 2;
	mVBallFactor = Max(0.0, mVBallFactor);

	for (int i = 1; i <= TEAMSIZE; i++)
	{
		if (i == mGoalieUnum)
			continue;
		iRole = mPlayerRole[i];
		if (iRole.mLineType == LT_Null)
			continue;
		if (iRole.mIndexX == 1)
		{
			mActiveField[i].SetLeft(MINX);
			mActiveField[i].SetRight(MAXX - mHInterval[1] - mHInterval[0]);
		}
		else if (iRole.mIndexX == 2)
		{
			mActiveField[i].SetLeft(MINX + mHInterval[0]);
			mActiveField[i].SetRight(MAXX - mHInterval[1]);
		}
		else if (iRole.mIndexX == 3)
		{
			mActiveField[i].SetLeft(MINX + mHInterval[0] + mHInterval[1]);
			mActiveField[i].SetRight(MAXX);
		}
		mActiveField[i].SetBottom(MAXY - (mLineArrange[iRole.mIndexX] - iRole.mIndexY) * mVInterval[iRole.mIndexX]);
		mActiveField[i].SetTop(MINY + iRole.mIndexY	* mVInterval[iRole.mIndexX]);
		//与中心的相互偏移
		if (iRole.mIndexX == 1)
		{
			mOffsetMatrix[0][i].SetX(-mHInterval[0]);
			mOffsetMatrix[i][0].SetX(mHInterval[0]);
		}
		else if (iRole.mIndexX == 2)
		{
			mOffsetMatrix[0][i].SetX(0);
			mOffsetMatrix[i][0].SetX(0);
		}
		else if (iRole.mIndexX == 3)
		{
			mOffsetMatrix[0][i].SetX(mHInterval[1]);
			mOffsetMatrix[i][0].SetX(-mHInterval[1]);
		}

		double iy = (iRole.mIndexY - mLineArrange[iRole.mIndexX] * 0.5) * mVInterval[iRole.mIndexX];
		mOffsetMatrix[0][i].SetY(iy);
		mOffsetMatrix[i][0].SetY(-iy);

		for (int j = 1; j < 11; j++)
		{
			if (j == mGoalieUnum || i == j)
				continue;
			jRole = mPlayerRole[j];
			if (jRole.mLineType == LT_Null)
				continue;
			//与其他队员的相互偏移
			if (iRole.mIndexX == 1)
			{
				if (jRole.mIndexX == 1)
				{
					mOffsetMatrix[i][j].SetX(0.0);
				}
				else if (jRole.mIndexX == 2)
				{
					mOffsetMatrix[i][j].SetX(mHInterval[0]);
				}
				else if (jRole.mIndexX == 3)
				{
					mOffsetMatrix[i][j].SetX(mHInterval[0] + mHInterval[1]);
				}
			}
			else if (iRole.mIndexX == 2)
			{
				if (jRole.mIndexX == 1)
				{
					mOffsetMatrix[i][j].SetX(-mHInterval[0]);
				}
				else if (jRole.mIndexX == 2)
				{
					mOffsetMatrix[i][j].SetX(0);
				}
				else if (jRole.mIndexX == 3)
				{
					mOffsetMatrix[i][j].SetX(mHInterval[1]);
				}
			}
			else if (iRole.mIndexX == 3)
			{
				if (jRole.mIndexX == 1)
				{
					mOffsetMatrix[i][j].SetX(-mHInterval[0] - mHInterval[1]);
				}
				else if (jRole.mIndexX == 2)
				{
					mOffsetMatrix[i][j].SetX(-mHInterval[1]);
				}
				else if (jRole.mIndexX == 3)
				{
					mOffsetMatrix[i][j].SetX(0.0);
				}
			}
			double jy = (jRole.mIndexY - (mLineArrange[jRole.mIndexX] + 1) * 0.5) * mVInterval[jRole.mIndexX];
			mOffsetMatrix[i][j].SetY(jy - iy);
		}
	}
}


Formation::Formation(Agent & agent) :
	mAgent(agent)
{
	if (mAgent.IsReverse())
	{
		mpTeammateFormation = &instance.GetOpponentFormation(FT_Attack_Forward);
		mpOpponentFormation = &instance.GetTeammateFormation(FT_Attack_Forward);
	}
	else
	{
		mpTeammateFormation = &instance.GetTeammateFormation(FT_Attack_Forward);
		mpOpponentFormation = &instance.GetOpponentFormation(FT_Attack_Forward);
	}
}

const RoleType & Formation::GetMyRole() const
{
	return GetTeammateRoleType(mAgent.GetSelfUnum());
}

const Vector & Formation::GetTeammateFormationCenter(double min_conf)
{
	return mpTeammateFormation->GetFormationCenter(mAgent.GetWorldState(), true, min_conf);
}

void Formation::SetTeammateFormationCenter(Unum num, Vector position)
{
	mpTeammateFormation->SetFormationCenter(num, position);
}

Vector Formation::GetTeammateFormationPoint(Unum unum, double min_conf)
{
	return mpTeammateFormation->GetFormationPoint(mAgent.GetWorldState(), true,	unum, min_conf);
}

Vector Formation::GetTeammateExpectedGoaliePos(Vector bp, double run)
{
	const WorldState & world_state = mAgent.GetWorldState();

	Vector gPos;
	int Uncyc;
	if (world_state.GetTeammateGoalieUnum() == 0 || world_state.GetTeammate(world_state.GetTeammateGoalieUnum()).GetPosConf() < FLOAT_EPS)
	{ //取守门员的位置以及没有见到的周期
		gPos = Vector(ServerParam::instance().PITCH_LENGTH / 2.0, 0.0); // 估值而已

		Vector p[2];
		double dist[2];
		p[0] = Vector(ServerParam::instance().PITCH_LENGTH / 2.0,
				-ServerParam::instance().goalWidth() / 2.0 + 0.1); // top
		p[1] = Vector(ServerParam::instance().PITCH_LENGTH / 2.0,
				ServerParam::instance().goalWidth() / 2.0 - 0.1); // down
		dist[0] = bp.Dist(p[0]);
		dist[1] = bp.Dist(p[1]);
		Uncyc = (int) (Min(dist[0], dist[1]) / 5.0);
	}
	else
	{
		gPos = world_state.GetTeammate(world_state.GetTeammateGoalieUnum()).GetPos();
		Uncyc = world_state.GetTeammate(world_state.GetTeammateGoalieUnum()).GetPosDelay();
	}

	return mpTeammateFormation->GetExpectedGoaliePos(bp, run, gPos, Uncyc);
}

Vector Formation::GetTeammateFormationPoint(Unum unum, const Vector & ball_pos)
{
	Assert(unum >= 1 && unum <= TEAMSIZE);

	Vector point;
	if (unum == mAgent.GetWorldState().GetTeammateGoalieUnum())
	{
		return Vector(-47.5, 0);
	}

	point.SetX(	ball_pos.X() * mpTeammateFormation->GetHBallFactor() + mpTeammateFormation->GetOffside(0, unum).X() );
	point.SetY(	ball_pos.Y() * mpTeammateFormation->GetVBallFactor() + mpTeammateFormation->GetOffside(0, unum).Y() );
	return point;
}

Vector Formation::GetTeammateFormationPoint(Unum unum, Unum focusTm, Vector focusPt)
{
	Assert(unum >= 1 && unum <= TEAMSIZE);
	if (unum == mAgent.GetWorldState().GetTeammateGoalieUnum())
	{
		return Vector(-47.5, 0);
	}

	if (focusTm == mAgent.GetWorldState().GetTeammateGoalieUnum())
	{
		int back_center = mpTeammateFormation->GetLineArrange(1) / 2 + 1; // 中后卫
		focusTm = mpTeammateFormation->GetUnumFromPos(1, back_center);
		focusPt.SetX(focusPt.X() + 9.0);
	}
	else if (focusTm <= Unum_Unknown || mAgent.GetWorldState().CurrentTime().T() < 1)
	{
		return GetTeammateFormationPoint(unum);
	}

	focusPt	= mpTeammateFormation->GetActiveField(focusTm).AdjustToWithin(focusPt);//调整到有效范围
	if (focusTm == unum)
	{
		return focusPt;
	}
	return focusPt + mpTeammateFormation->GetOffside(focusTm, unum);
}

const Vector & Formation::GetOpponentFormationCenter(double min_conf)
{
	return mpOpponentFormation->GetFormationCenter(mAgent.GetWorldState(), false, min_conf);
}

void Formation::SetOpponentFormationCenter(Unum num, Vector position)
{
	mpOpponentFormation->SetFormationCenter(num, position);
}

Vector Formation::GetOpponentFormationPoint(Unum unum, double min_conf)
{
	return mpOpponentFormation->GetFormationPoint(mAgent.GetWorldState(),false, unum, min_conf);
}

Vector Formation::GetOpponentExpectedGoaliePos(Vector bp, double run)
{
	const WorldState & world_state = mAgent.GetWorldState();

	Vector gPos;
	int Uncyc;
	if (world_state.GetOpponentGoalieUnum() == 0 || world_state.GetOpponent(world_state.GetOpponentGoalieUnum()).GetPosConf() < FLOAT_EPS)
	{ //取守门员的位置以及没有见到的周期
		gPos = Vector(ServerParam::instance().PITCH_LENGTH / 2.0, 0.0); // 估值而已

		Vector p[2];
		double dist[2];
		p[0] = Vector(ServerParam::instance().PITCH_LENGTH / 2.0, -ServerParam::instance().goalWidth() / 2.0 + 0.1); // top
		p[1] = Vector(ServerParam::instance().PITCH_LENGTH / 2.0,  ServerParam::instance().goalWidth() / 2.0 - 0.1); // down
		dist[0] = bp.Dist(p[0]);
		dist[1] = bp.Dist(p[1]);
		Uncyc = (int) (Min(dist[0], dist[1]) / 5.0);
	}
	else
	{
		gPos = world_state.GetOpponent(world_state.GetOpponentGoalieUnum()).GetPos();
		Uncyc = world_state.GetOpponent(world_state.GetOpponentGoalieUnum()).GetPosDelay();
	}

	return mpOpponentFormation->GetExpectedGoaliePos(bp, run, gPos, Uncyc);
}

Vector Formation::GetOpponentFormationPoint(Unum unum, const Vector & ball_pos)
{
	double x = ball_pos.X() * mpOpponentFormation->GetHBallFactor()	- mpOpponentFormation->GetOffside(0, unum).X();
	if (mAgent.GetWorldState().GetPlayMode() == PM_Our_Kick_Off && x > 0)
	{
		x = 0.0;
	}
	double y = ball_pos.Y() * mpOpponentFormation->GetVBallFactor()	- mpOpponentFormation->GetOffside(0, unum).Y();
	return Vector(x, y);
}

bool Formation::IsOpponentRoleValid()
{
    return mAgent.IsReverse() ? true : (static_cast<OpponentFormation *>(mpOpponentFormation)->IsRoleValid());
}

void Formation::SetTeammateFormationType(FormationType type)
{
	if (mAgent.IsReverse())
	{
		mpTeammateFormation = &instance.GetOpponentFormation(type);
	}
	else
	{
		mpTeammateFormation = &instance.GetTeammateFormation(type);
	}
}

void Formation::SetOpponentFormationType(FormationType type)
{
	if (mAgent.IsReverse())
	{
		mpOpponentFormation = &instance.GetTeammateFormation(type);
	}
	else
	{
		mpOpponentFormation = &instance.GetOpponentFormation(type);
	}
}

/**
 * This is only the preparation part of the update procedure. The substantive work will be done by
 * Strategy and Analyzer.
 */
void Formation::Update(UpdatePolicy policy, std::string update_name)
{
	switch (policy)
	{
	case Offensive:
		if (mAgent.GetStrategy().GetSituation() != ST_Defense)
		{
			SetTeammateFormationType(FT_Attack_Forward);
		}
		else
		{
			SetTeammateFormationType(FT_Defend_Back);
		}

		// TODO: 对手的阵型分析，这里WE2008用的比较混乱，可能会有bug
		if (mAgent.GetStrategy().GetSituation() != ST_Defense)
		{
			if (ServerParam::instance().ourPenaltyArea().IsWithin(mAgent.GetStrategy().GetBallInterPos()) && mAgent.GetStrategy().IsBallFree())
			{
				SetOpponentFormationType(FT_Attack_Forward); // 认为对手还处于进攻状态
			}
			else
			{
				SetOpponentFormationType(FT_Defend_Back);
			}
		}
		else
		{
			if (mAgent.GetStrategy().GetBallInterPos().X() < -26.0)
			{
				SetOpponentFormationType(FT_Attack_Forward);
			}
			else
			{
				SetOpponentFormationType(FT_Attack_Midfield);
			}
		}
		break;

	case Defensive:
	    if (mAgent.GetStrategy().GetBallInterPos().X() < -30.0)
	    {
	        SetOpponentFormationType(FT_Attack_Forward);
	    }
	    else
	    {
	        SetOpponentFormationType(FT_Attack_Midfield);
	    }
	    break;
	}

	mFormationTypeStack.push(make_pair(mpTeammateFormation->GetFormationType(), mpOpponentFormation->GetFormationType()));

	Logger::instance().GetTextLogger("formation_update")<<mAgent.GetWorldState().CurrentTime() << ", "<< mAgent.GetAgentID() << ", +, " <<  mFormationTypeStack.size() << ": " << update_name <<endl;
}

/**
 * Rollback.
 */
void Formation::Rollback(std::string update_name)
{
	if (!mFormationTypeStack.empty())
	{
		mFormationTypeStack.pop();
		if (!mFormationTypeStack.empty())
		{
			SetTeammateFormationType(mFormationTypeStack.top().first);
			SetOpponentFormationType(mFormationTypeStack.top().second);
		}
	}
	else
	{
		Assert(!"mFormationTypeStack.empty()");
	}

	Logger::instance().GetTextLogger("formation_update")<<mAgent.GetWorldState().CurrentTime() << ", " <<mAgent.GetAgentID() << ", -, " <<  mFormationTypeStack.size() << ": " << update_name <<endl;
}

Formation::Instance Formation::instance;

Formation::Instance::Instance() :
	mpAgent(0)
{
	mpTeammateFormationsImp[0] = new TeammateFormation(FT_Attack_Forward, "formations/433_Attack");
	mpTeammateFormationsImp[1] = new TeammateFormation(FT_Defend_Back, "formations/433_Defend");
	mpTeammateFormationsImp[2] = new TeammateFormation(FT_Attack_Forward, "formations/442_Attack");
	mpTeammateFormationsImp[3] = new TeammateFormation(FT_Defend_Back, "formations/442_Defend");
	SetTeammateFormations();

	mpOpponentFormationsImp[0] = new OpponentFormation(0, FT_Attack_Forward);
	mpOpponentFormationsImp[1] = new OpponentFormation(0, FT_Attack_Midfield);
	mpOpponentFormationsImp[2] = new OpponentFormation(0, FT_Defend_Midfield);
	mpOpponentFormationsImp[3] = new OpponentFormation(0, FT_Defend_Back);
	for (int i = 0; i < FT_Max; ++i)
		SetOpponentFormation(FormationType(i), mpOpponentFormationsImp[i]);
}

Formation::Instance::~Instance()
{
	for (int i=0; i<4; ++i)
		delete mpOpponentFormationsImp[i];
	for (int i=0; i<4; ++i)
		delete mpTeammateFormationsImp[i];
}

void Formation::Instance::AssignWith(Agent * agent)
{
	mpAgent = agent;
}

void Formation::Instance::SetTeammateFormations()
{
	mpTeammateFormations[FT_Attack_Forward] = mpTeammateFormationsImp[0];
	mpTeammateFormations[FT_Attack_Midfield] = mpTeammateFormationsImp[0];
	mpTeammateFormations[FT_Defend_Midfield] = mpTeammateFormationsImp[1];
	mpTeammateFormations[FT_Defend_Back] = mpTeammateFormationsImp[1];
}

/**
 * Get the Formation of the agent which represents this program.
 */
Formation & Formation::Instance::operator ()()
{
	Assert(mpAgent);
	return mpAgent->GetFormation();
}

void Formation::Instance::UpdateOpponentRole()
{
	if (!mpAgent) {
		Assert(0);
		return;
	}

	const WorldState & world_state = mpAgent->GetWorldState();
	const InfoState & info_state = mpAgent->GetInfoState();
	const std::list<KeyPlayerInfo> & opp_list =	info_state.GetPositionInfo().GetXSortOpponent();
	const FormationBase & teammate_formation = mpAgent->GetFormation().GetTeammateFormation();
	if (opp_list.size() < TEAMSIZE || world_state.GetPlayMode() != PM_Play_On || world_state.CurrentTime() % 10 != 0)
	{
		return; // 按照WE2008，不满足这些条件就返回
	}

	int i;
	for (i = 0; i < FT_Max; ++i)
	{
		if (mpOpponentFormations[i]->mUsedTimes	== 0)
		{
			break;
		}
	}
	if (i == FT_Max)
	{
		return; // 如果所有阵型信息均被教练的消息设置过，则不再考虑
	}

	const int & forward_num = teammate_formation.GetLineArrange(3);
	const int & midfielder_num = teammate_formation.GetLineArrange(2);
	const int & defender_num = teammate_formation.GetLineArrange(1);

	int count = 0;
	KeyPlayerInfo kp;
	std::vector<KeyPlayerInfo> forward, midfielder, defender; // 守门员不考虑
	for (std::list<KeyPlayerInfo>::const_iterator it = opp_list.begin(); it
			!= opp_list.end(); ++it)
	{
		kp.mUnum = it->mUnum;
		kp.mValue = -world_state.GetOpponent(kp.mUnum).GetPos().Y();
		if (count < forward_num)
		{
			forward.push_back(kp);
		}
		else if (count < forward_num + midfielder_num)
		{
			midfielder.push_back(kp);
		}
		else if (count < forward_num + midfielder_num + defender_num)
		{
			defender.push_back(kp);
		}
		++count;
	}
	std::sort(forward.begin(), forward.end());
	std::sort(midfielder.begin(), midfielder.end());
	std::sort(defender.begin(), defender.end());

	for (Unum i = 1; i <= TEAMSIZE; ++i)
	{
		if (teammate_formation.GetPlayerRoleType(i).mLineType == LT_Goalie)
		{
			continue;
		}

		const RoleType & role = teammate_formation.GetPlayerRoleType(i);
		switch (role.mLineType)
		{
		case LT_Forward:
			Assert(role.mIndexY - 1 >= 0 && role.mIndexY - 1 < (int)forward.size());
			for (int j = 0; j < FT_Max; ++j)
			{
				if (mpOpponentFormations[j]->mUsedTimes == 0)
				{
					mpOpponentFormations[j]->mPlayerRole[forward[role.mIndexY - 1].mUnum] = role;
				}
			}
			break;
		case LT_Midfielder:
			Assert(role.mIndexY - 1 >= 0 && role.mIndexY - 1 < (int)midfielder.size());
			for (int j = 0; j < FT_Max; ++j)
			{
				if (mpOpponentFormations[j]->mUsedTimes == 0)
				{
					mpOpponentFormations[j]->mPlayerRole[midfielder[role.mIndexY - 1].mUnum] = role;
				}
			}
			break;
		case LT_Defender:
			Assert(role.mIndexY - 1 >= 0 && role.mIndexY - 1 < (int)defender.size());
			for (int j = 0; j < FT_Max; ++j)
			{
				if (mpOpponentFormations[j]->mUsedTimes == 0)
				{
					mpOpponentFormations[j]->mPlayerRole[defender[role.mIndexY - 1].mUnum] = role;
				}
			}
			break;
		default:
			PRINT_ERROR("line type error");
			break;
		}
	}
}

void Formation::Instance::SetOpponentGoalieUnum(Unum goalie_unum)
{
	for (int i = FT_Attack_Forward; i < FT_Max; ++i)
		mpOpponentFormations[i]->SetGoalieUnum(goalie_unum);
}
