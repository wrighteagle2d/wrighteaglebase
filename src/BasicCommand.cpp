/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D) * BASE SOURCE CODE RELEASE 2016 *
 * Copyright (c) 1998-2016 WrightEagle 2D Soccer Simulation Team, * Multi-Agent
 *Systems Lab.,                                * School of Computer Science and
 *Technology,               * University of Science and Technology of China *
 * All rights reserved. *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without *
 * modification, are permitted provided that the following conditions are met: *
 *     * Redistributions of source code must retain the above copyright *
 *       notice, this list of conditions and the following disclaimer. *
 *     * Redistributions in binary form must reproduce the above copyright *
 *       notice, this list of conditions and the following disclaimer in the *
 *       documentation and/or other materials provided with the distribution. *
 *     * Neither the name of the WrightEagle 2D Soccer Simulation Team nor the *
 *       names of its contributors may be used to endorse or promote products *
 *       derived from this software without specific prior written permission. *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED    * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *PURPOSE ARE           * DISCLAIMED. IN NO EVENT SHALL WrightEagle 2D Soccer
 *Simulation Team BE LIABLE    * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *EXEMPLARY, OR CONSEQUENTIAL       * DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *PROCUREMENT OF SUBSTITUTE GOODS OR       * SERVICES; LOSS OF USE, DATA, OR
 *PROFITS; OR BUSINESS INTERRUPTION) HOWEVER       * CAUSED AND ON ANY THEORY OF
 *LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,    * OR TORT (INCLUDING
 *NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF * THIS SOFTWARE,
 *EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                *
 ************************************************************************************/

#include "BasicCommand.h"
#include "Agent.h"
#include "WorldState.h"
#include <sstream>

bool BasicCommand::Execute(std::list<CommandInfo> &command_queue) {
  if (mCommandInfo.mTime != mAgent.GetWorldState().CurrentTime()) {
    return false;
  }

  ActionEffector::CMD_QUEUE_MUTEX.Lock();
  command_queue.push_back(mCommandInfo);
  ActionEffector::CMD_QUEUE_MUTEX.UnLock();

  return true;
}

Turn::Turn(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Turn;
  mCommandInfo.mMutex = true;
}

void Turn::Plan(double moment) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  mCommandInfo.mAngle = moment;
  std::ostringstream cmd_str;
  cmd_str << "(turn " << mCommandInfo.mAngle << ")";
  mCommandInfo.mString = cmd_str.str();
}

Dash::Dash(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Dash;
  mCommandInfo.mMutex = true;
}

void Dash::Plan(double power, AngleDeg dir) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  mCommandInfo.mPower = GetNormalizeDashPower(power);
  mCommandInfo.mAngle = GetNormalizeDashAngle(dir);
  std::ostringstream cmd_str;
  cmd_str << "(dash " << mCommandInfo.mPower << " " << mCommandInfo.mAngle
          << ")";
  mCommandInfo.mString = cmd_str.str();
}

TurnNeck::TurnNeck(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_TurnNeck;
  mCommandInfo.mMutex = false;
}

void TurnNeck::Plan(double moment) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  mCommandInfo.mAngle = GetNormalizeNeckMoment(moment);
  std::ostringstream cmd_str;
  cmd_str << "(turn_neck " << mCommandInfo.mAngle << ")";
  mCommandInfo.mString = cmd_str.str();
}

Say::Say(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Say;
  mCommandInfo.mMutex = false;
}

void Say::Plan(std::string msg) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  if (PlayerParam::instance().isCoach()) {
    cmd_str << "(say " << msg << ")";
  } else {
    cmd_str << "(say \"" << msg << "\")";
  }
  mCommandInfo.mString = cmd_str.str();
}

Attentionto::Attentionto(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Attentionto;
  mCommandInfo.mMutex = false;
}

void Attentionto::Plan(bool on, Unum num) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  if (on == false) {
    cmd_str << "(attentionto off)";
  } else {
    if (num < 0) {
      cmd_str << "(attentionto opp " << -num << ")";
    } else {
      cmd_str << "(attentionto our " << num << ")";
    }
  }
  mCommandInfo.mString = cmd_str.str();
}

Kick::Kick(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Kick;
  mCommandInfo.mMutex = true;
}

void Kick::Plan(double power, double dir) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  mCommandInfo.mPower = GetNormalizeKickPower(power);
  mCommandInfo.mAngle = GetNormalizeMoment(dir);
  std::ostringstream cmd_str;
  cmd_str << "(kick " << mCommandInfo.mPower << " " << mCommandInfo.mAngle
          << ")";
  mCommandInfo.mString = cmd_str.str();
}

Tackle::Tackle(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Tackle;
  mCommandInfo.mMutex = true;
}

void Tackle::Plan(double dir, const bool foul) {
  std::string foul_signal;
  foul_signal += (foul) ? " true" : " false";
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  mCommandInfo.mAngle = GetNormalizeMoment(dir);
  std::ostringstream cmd_str;
  if (PlayerParam::instance().playerVersion() > 14)
    cmd_str << "(tackle " << mCommandInfo.mAngle << foul_signal << ")";
  else
    cmd_str << "(tackle " << mCommandInfo.mAngle << ")";
  mCommandInfo.mString = cmd_str.str();
}

Pointto::Pointto(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Pointto;
  mCommandInfo.mMutex = false;
}

void Pointto::Plan(bool on, double dist, double dir) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;

  if (on) {
    mCommandInfo.mDist = dist;
    mCommandInfo.mAngle = dir;
    cmd_str << "(pointto " << mCommandInfo.mDist << " " << mCommandInfo.mAngle
            << ")";
  } else {
    cmd_str << "(pointto off)";
  }
  mCommandInfo.mString = cmd_str.str();
}

Catch::Catch(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Catch;
  mCommandInfo.mMutex = true;
}

void Catch::Plan(double dir) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  mCommandInfo.mAngle = dir;
  std::ostringstream cmd_str;
  cmd_str << "(catch " << mCommandInfo.mAngle << ")";
  mCommandInfo.mString = cmd_str.str();
}

Move::Move(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Move;
  mCommandInfo.mMutex = true;
}

void Move::Plan(Vector pos) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  mCommandInfo.mMovePos = pos;
  std::ostringstream cmd_str;
  cmd_str << "(move " << mCommandInfo.mMovePos.X() << " "
          << mCommandInfo.mMovePos.Y() << ")";
  mCommandInfo.mString = cmd_str.str();
}

ChangeView::ChangeView(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_ChangeView;
  mCommandInfo.mMutex = true;
}

void ChangeView::Plan(ViewWidth view_width) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  mCommandInfo.mViewWidth = view_width;
  std::ostringstream cmd_str;

  switch (mCommandInfo.mViewWidth) {
  case VW_Narrow:
    cmd_str << "(change_view narrow)";
    break;
  case VW_Normal:
    cmd_str << "(change_view normal)";
    break;
  case VW_Wide:
    cmd_str << "(change_view wide)";
    break;
  default:
    break;
  }
  mCommandInfo.mString = cmd_str.str();
}

Compression::Compression(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Compression;
  mCommandInfo.mMutex = true;
}

void Compression::Plan(int level) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  mCommandInfo.mLevel = level;
  std::ostringstream cmd_str;
  cmd_str << "(compression " << mCommandInfo.mLevel << ")";
  mCommandInfo.mString = cmd_str.str();
}

SenseBody::SenseBody(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_SenseBody;
  mCommandInfo.mMutex = false;
}

void SenseBody::Plan() {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(sense_body)";
  mCommandInfo.mString = cmd_str.str();
}

Score::Score(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Score;
  mCommandInfo.mMutex = false;
}

void Score::Plan() {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(score)";
  mCommandInfo.mString = cmd_str.str();
}

Bye::Bye(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Bye;
  mCommandInfo.mMutex = false;
}

void Bye::Plan() {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(bye)";
  mCommandInfo.mString = cmd_str.str();
}

Done::Done(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Done;
  mCommandInfo.mMutex = false;
}

void Done::Plan() {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(done)";
  mCommandInfo.mString = cmd_str.str();
}

Clang::Clang(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Clang;
  mCommandInfo.mMutex = false;
}

void Clang::Plan(int min_ver, int max_ver) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  mCommandInfo.mMinVer = min_ver;
  mCommandInfo.mMaxVer = max_ver;
  std::ostringstream cmd_str;
  cmd_str << "(clang (ver " << mCommandInfo.mMinVer << " "
          << mCommandInfo.mMaxVer << "))";
  mCommandInfo.mString = cmd_str.str();
}

Ear::Ear(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Ear;
  mCommandInfo.mMutex = false;
}

void Ear::Plan(bool on, bool our_side, EarMode ear_mode) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();

  const char *on_string;
  const char *side_string;
  const char *ear_mode_string;

  if (on == true) {
    on_string = "on";
  } else {
    on_string = "off";
  }

  if (our_side) {
    side_string = " our";
  } else {
    side_string = " opp";
  }

  if (ear_mode == EM_Partial) {
    ear_mode_string = " partial";
  } else if (ear_mode == EM_Complete) {
    ear_mode_string = " complete";
  } else {
    ear_mode_string = "";
  }

  std::ostringstream cmd_str;
  cmd_str << "(ear (" << on_string << side_string << ear_mode_string << "))";
  mCommandInfo.mString = cmd_str.str();
}

SynchSee::SynchSee(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_SynchSee;
  mCommandInfo.mMutex = false;
}

void SynchSee::Plan() {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(synch_see)";
  mCommandInfo.mString = cmd_str.str();
  mCommandInfo.mString = cmd_str.str();
}

ChangePlayerType::ChangePlayerType(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_ChangePlayerType;
  mCommandInfo.mMutex = false;
}

void ChangePlayerType::Plan(Unum num, int player_type) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(change_player_type " << num << " " << player_type << ")";
  mCommandInfo.mString = cmd_str.str();
}

//////////////////////////////////for
///trainer////////////////////////////////////
void ChangePlayerType::Plan(std::string teamname, Unum num, int player_type) {
  mCommandInfo.mType = CT_ChangePlayerTypeForTrainer;
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(change_player_type " << teamname << " " << num << " "
          << player_type << ")";
  mCommandInfo.mString = cmd_str.str();
}

Start::Start(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Start;
  mCommandInfo.mMutex = false;
}

void Start::Plan() {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(start)";
  mCommandInfo.mString = cmd_str.str();
}

ChangePlayMode::ChangePlayMode(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_ChangePlayMode;
  mCommandInfo.mMutex = false;
}

void ChangePlayMode::Plan(ServerPlayMode spm) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(change_mode "
          << ServerPlayModeMap::instance().GetPlayModeString(spm) << ")";
  mCommandInfo.mString = cmd_str.str();
}

MovePlayer::MovePlayer(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_MovePlayer;
  mCommandInfo.mMutex = false;
}

void MovePlayer::Plan(std::string team_name, Unum num, Vector pos, Vector vel,
                      AngleDeg dir) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(move (player " << team_name << " " << num << ") " << pos.X()
          << " " << pos.Y() << " " << dir << " " << vel.X() << " " << vel.Y()
          << ")";
  mCommandInfo.mString = cmd_str.str();
}

MoveBall::MoveBall(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_MoveBall;
  mCommandInfo.mMutex = false;
}

void MoveBall::Plan(Vector pos, Vector vel) {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(move (ball) " << pos.X() << " " << pos.Y() << " "
          << " 0 " << vel.X() << " " << vel.Y() << ")";
  mCommandInfo.mString = cmd_str.str();
}

Look::Look(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Look;
  mCommandInfo.mMutex = false;
}

void Look::Plan() {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(look)";
  mCommandInfo.mString = cmd_str.str();
}

TeamNames::TeamNames(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_TeamNames;
  mCommandInfo.mMutex = false;
}

void TeamNames::Plan() {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(team_names)";
  mCommandInfo.mString = cmd_str.str();
}

Recover::Recover(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_Recover;
  mCommandInfo.mMutex = false;
}

void Recover::Plan() {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(recover)";
  mCommandInfo.mString = cmd_str.str();
}

CheckBall::CheckBall(const Agent &agent) : BasicCommand(agent) {
  mCommandInfo.mType = CT_CheckBall;
  mCommandInfo.mMutex = false;
}

void CheckBall::Plan() {
  mCommandInfo.mTime = mAgent.GetWorldState().CurrentTime();
  std::ostringstream cmd_str;
  cmd_str << "(check_ball)";
  mCommandInfo.mString = cmd_str.str();
}
