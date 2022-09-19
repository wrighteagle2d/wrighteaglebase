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

#ifndef AGENT_H_
#define AGENT_H_

#include "ActionEffector.h"
#include "Analyser.h"
#include "Formation.h"
#include "InfoState.h"
#include "PositionInfo.h"
#include "Strategy.h"
#include "WorldState.h"

class WorldModel;
class ActiveBehavior;

/**
 * Identifies an agent.
 */
struct AgentID {
  Unum mAgentUnum; //总是为正
  Time mCurrentTime;
  bool mReverse;

  AgentID(Unum unum = 0, Time time = Time(-3, 0), bool reverse = false)
      : mAgentUnum(unum), mCurrentTime(time), mReverse(reverse) {}
  bool operator==(const AgentID &o) {
    return mCurrentTime == o.mCurrentTime && mAgentUnum == o.mAgentUnum &&
           mReverse == o.mReverse;
  }
  bool operator!=(const AgentID &o) { return !(*this == o); }

  friend std::ostream &operator<<(std::ostream &os, const AgentID &o) {
    if (o.mReverse) {
      return os << '-' << o.mAgentUnum;
    } else {
      return os << '+' << o.mAgentUnum;
    }
  }
};

/**
 * 教练和球员通用
 */
class Agent {
  friend class Client;

  Agent(Agent &);
  Agent(Unum unum, WorldModel *world_model, bool reverse);

public:
  virtual ~Agent();

  /**
   * Interface to create an agent which represents a team mate.
   */
  Agent *CreateTeammateAgent(Unum unum); ///反算队友

  /**
   * Interface to create an agent which represents an opponent.
   */
  Agent *CreateOpponentAgent(Unum unum); ///反算对手

  /**
   * Interfaces to get the agent's world state.
   */
  WorldState &World() { return *mpWorldState; }
  const WorldState &GetWorldState() const { return *mpWorldState; }

  /**
   * Interfaces to get the agent's info state.
   */
  InfoState &Info() { return *mpInfoState; }
  const InfoState &GetInfoState() const { return *mpInfoState; }

  /**
   * 自己相关的接口
   * Interfaces to get information about the agent it self.
   */
  AgentID GetAgentID() const {
    return AgentID(mSelfUnum, GetWorldState().CurrentTime(), mReverse);
  }
  Unum GetSelfUnum() const { return mSelfUnum; }

  const PlayerState &GetSelf() const {
    if (!mSelfUnum || mSelfUnum == TRAINER_UNUM) {
      static PlayerState coach; // dummy player state for coach
      return coach;
    }

    return GetWorldState().GetTeammate(mSelfUnum);
  }

  PlayerState &Self() {
    if (!mSelfUnum || mSelfUnum == TRAINER_UNUM) {
      static PlayerState coach; // dummy player state for coach
      return coach;
    }

    return World().Teammate(mSelfUnum);
  }

  /**
   * Get state information after queued actions are executed.
   */
  Vector GetSelfPosWithQueuedActions() {
    return GetActionEffector().GetSelfPosWithQueuedActions();
  }
  Vector GetSelfVelWithQueuedActions() {
    return GetActionEffector().GetSelfVelWithQueuedActions();
  }
  AngleDeg GetSelfBodyDirWithQueuedActions() {
    return GetActionEffector().GetSelfBodyDirWithQueuedActions();
  }
  Vector GetBallPosWithQueuedActions() {
    return GetActionEffector().GetBallPosWithQueuedActions();
  }
  Vector GetBallVelWithQueuedActions() {
    return GetActionEffector().GetBallVelWithQueuedActions();
  }

  /**
   * Action interfaces
   */
  bool Turn(AngleDeg angle) { return GetActionEffector().SetTurnAction(angle); }
  bool Dash(double power, AngleDeg dir) {
    return GetActionEffector().SetDashAction(power, dir);
  }
  bool TurnNeck(AngleDeg angle) {
    return GetActionEffector().SetTurnNeckAction(angle);
  }
  bool Say(std::string msg) { return GetActionEffector().SetSayAction(msg); }
  bool Attentionto(Unum num) {
    return GetActionEffector().SetAttentiontoAction(num);
  }
  bool AttentiontoOff() {
    return GetActionEffector().SetAttentiontoOffAction();
  }
  bool Kick(double power, AngleDeg angle) {
    return GetActionEffector().SetKickAction(power, angle);
  }
  bool Tackle(AngleDeg angle, const bool foul) {
    return GetActionEffector().SetTackleAction(angle, foul);
  }
  bool Pointto(double dist, AngleDeg angle) {
    return GetActionEffector().SetPointtoAction(dist, angle);
  }
  bool PointtoOff() { return GetActionEffector().SetPointtoOffAction(); }
  bool Catch(AngleDeg angle) {
    return GetActionEffector().SetCatchAction(angle);
  }
  bool Move(Vector pos) { return GetActionEffector().SetMoveAction(pos); }
  bool ChangeView(ViewWidth view_width) {
    return GetActionEffector().SetChangeViewAction(view_width);
  }
  bool Compression(int level) {
    return GetActionEffector().SetCompressionAction(level);
  }
  bool SenseBody() { return GetActionEffector().SetSenseBodyAction(); }
  bool Score() { return GetActionEffector().SetScoreAction(); }
  bool Bye() { return GetActionEffector().SetByeAction(); }
  bool Done() { return GetActionEffector().SetDoneAction(); }
  bool Clang(int min_ver, int max_ver) {
    return GetActionEffector().SetClangAction(min_ver, max_ver);
  }
  bool EarOn(bool our_side, EarMode ear_mode = EM_All) {
    return GetActionEffector().SetEarOnAction(our_side, ear_mode);
  }
  bool EarOff(bool our_side, EarMode ear_mode = EM_All) {
    return GetActionEffector().SetEarOffAction(our_side, ear_mode);
  }
  bool SynchSee() { return GetActionEffector().SetSynchSeeAction(); }
  bool ChangePlayerType(Unum num, int player_type) {
    return GetActionEffector().SetChangePlayerTypeAction(num, player_type);
  }

  bool ChangePlayerType(std::string teamname, Unum num, int player_type) {
    return GetActionEffector().SetChangePlayerTypeAction(teamname, num,
                                                         player_type);
  }
  bool Start() { return GetActionEffector().SetStartAction(); }
  bool ChangePlayMode(ServerPlayMode spm) {
    return GetActionEffector().SetChangePlayModeAction(spm);
  }
  bool MoveBall(Vector pos, Vector vel) {
    return GetActionEffector().SetMoveBallAction(pos, vel);
  }
  bool MovePlayer(std::string teamname, Unum num, Vector pos, Vector vel,
                  AngleDeg dir) {
    return GetActionEffector().SetMovePlayerAction(teamname, num, pos, vel,
                                                   dir);
  }
  bool Look() { return GetActionEffector().SetLookAction(); }
  bool TeamNames() { return GetActionEffector().SetTeamNamesAction(); }
  bool Recover() { return GetActionEffector().SetRecoverAction(); }
  bool CheckBall() { return GetActionEffector().SetCheckBallAction(); }

  /**
   * Get ActionEffector of this agent.
   */
  ActionEffector &GetActionEffector() {
    if (mpActionEffector == 0) {
      mpActionEffector = new ActionEffector(*this);
    }
    return *mpActionEffector;
  }

  /**
   * Get Formation of this agent.
   */
  Formation &GetFormation() {
    if (mpFormation == 0) {
      mpFormation = new Formation(*this);
    }
    return *mpFormation;
  }

private:
  /**
   * Interface template for getting certain type of decision data.
   */
  template <class DecisionDataDerived>
  DecisionDataDerived &GetDecisionData(DecisionDataDerived **xptr) {
    if (*xptr == 0) {
      *xptr = new DecisionDataDerived(*this);
    }
    (*xptr)->Update();
    return **xptr;
  }

public:
  /**
   * 每次调用的时候，检查是否需要更新，大量使用时，最好先赋值给变量，这样比较快
   * Each the following methods will check if an update should be performed
   * first when it is called. It's better to store the returned value in a
   * variable when multiple uses are needed.
   */
  Strategy &GetStrategy() { return GetDecisionData(&mpStrategy); }
  Analyser &GetAnalyser() { return GetDecisionData(&mpAnalyser); }

public:
  /**
   * Check commands sent to server, based on ActionEffector::CheckCommands. Will
   * update IsNewSight and BallSeenTime.
   */
  void CheckCommands(Observer *observer) {
    mIsNewSight = observer->IsNewSight();
    mBallSeenTime = observer->Ball().GetDist().time();
    GetActionEffector().CheckCommands(observer);
  }

  /**
   * Interface to ActionEffector::SendCommands.
   */
  void SendCommands(char *msg) { GetActionEffector().SendCommands(msg); }

  /**
   * If there's a new sight arrived in current cycle.
   */
  bool IsNewSight() const { return mIsNewSight; }

  /**
   * Get last time the agent saw the ball.
   */
  Time GetBallSeenTime() const { return mBallSeenTime; }

public:
  /**
   * If this agent is created for reverse calculation.
   */
  bool IsReverse() const { return mReverse; }

private:
  const Unum mSelfUnum; // Agent的号码总是为正
  const bool mReverse;  //标记这个Agent是否反算对手
  WorldModel *const mpWorldModel;
  WorldState *const mpWorldState;

  /** 以上变量在 Agent
   * 的生存周期内是不会变的，各种形式的反算（包括反算队友和对手）都要 new 一个
   * Agent */

  InfoState *mpInfoState;

  bool mIsNewSight;
  Time mBallSeenTime;

  /** 以下变量在第一次使用时生成指向实例，在每次调用时检查是否需要更新 */
  Strategy *mpStrategy;
  Analyser *mpAnalyser;

  ActionEffector *mpActionEffector;
  Formation *mpFormation;

  /**
   * 关于last behavior的接口
   * 注意: 设置的接口都是在外面调用的，behavior*里面不用管
   **/
public:
  /**
   * 得到上周期保存的activebehavior，没有则为空
   * @param type
   * @return
   */
  ActiveBehavior *GetLastActiveBehavior(BehaviorType type) const;

  /**
   * 得到上周期实际执行的activebehavior
   * @return
   */
  ActiveBehavior *GetLastActiveBehaviorInAct() const {
    return mLastActiveBehavior[0];
  }

  /**
   * 返回本周期执行的行为 -- 这个要在DecisionTree::Decision返回后才有意义
   * @return
   */
  ActiveBehavior *GetActiveBehaviorInAct() const { return mActiveBehavior[0]; }

  /**
   * 返回上周期是否执行了type类型的activebehavior
   * @param type
   * @return
   */
  bool IsLastActiveBehaviorInActOf(BehaviorType type) const {
    return GetLastActiveBehaviorInAct() &&
           GetLastActiveBehaviorInAct()->GetType() == type;
  }

  void SetHistoryActiveBehaviors();

private:
  /**
   * 保存behavior*决策出的最优activebehavior -- plan结束时保存
   * @param type
   */
  void SaveActiveBehavior(const ActiveBehavior &beh);

  friend class DecisionTree;

  void SaveActiveBehaviorList(const std::list<ActiveBehavior> &behavior_list);

  /**
   * 设置本周期实际执行的activebehavior -- excute时设置
   * @param type
   */
  void SetActiveBehaviorInAct(BehaviorType type);

private:
  Array<ActiveBehavior *, BT_Max, true> mActiveBehavior;
  Array<ActiveBehavior *, BT_Max, true> mLastActiveBehavior;
};

#endif /* AGENT_H_ */
