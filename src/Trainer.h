/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D) * BASE SOURCE CODE RELEASE 2016 *
 * Copyright (c) 1998-2012 WrightEagle 2D Soccer Simulation Team, * Multi-Agent
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

#ifndef TRAINER_H_
#define TRAINER_H_

#include "Client.h"
#include "Geometry.h"
#include "Types.h"
#include "Utilities.h"
#include "WorldState.h"
#include <vector>

using namespace std;

class Trainer : public Client {

  struct PlayerState //简化过后的PlayerState，供存储数据用
  {
    Unum mUnum;
    Vector mPos;
    Vector mVel;
    AngleDeg mBodyDir;
    int mPlayerType;
    PlayerState(Unum num, Vector pos, Vector vel, double dir, int type);
  };

  enum ConditionType //终止一次训练的条件
  {
    ET_Null = 0,
    ET_Time,          //过x个周期自然终止
    ET_BallXbt,       //球的x坐标大于某值停止
    ET_BallXlt,       //球的x坐标小于某值停止
    ET_BallYbt,       //球的y坐标。。
    ET_BallYlt,       //。。
    ET_Ball2Playerbt, //球到某人的距离大于某值
    ET_Ball2Playerlt, //球到某人的距离小于某值
    ET_NonPlayOn,
    ET_ANDComplex, //复合与条件，全部满足才行
    ET_ORComplex   //复合或条件，满足一个即可
  };

  struct Condition {
    Condition(Trainer *trainer, Trainer::ConditionType type, double arg1,
              double arg2, Trainer::Condition *pSuper, bool converse) {
      mpTrainer = trainer;
      mType = type;
      mArg1 = arg1;
      mArg2 = arg2;
      mpSuperCondition = pSuper;
      mMaxInterval = 0;
      mMinInterval = 6000;
      mMinTime = 0;
      mMaxTime = 0;
      mCount = 0;
      mConverse = converse;
    }

    ~Condition() {}

    void SetConditionType(Trainer::ConditionType type) { mType = type; }
    Trainer::ConditionType GetConditionType() { return mType; }
    void SetSuperCondition(Trainer::Condition *pSuper) {
      mpSuperCondition = pSuper;
    }
    Trainer::Condition *GetSuperCondition() { return mpSuperCondition; }
    std::vector<Trainer::Condition> &GetSubConditions() {
      return mSubCondition;
    }
    Trainer::Condition *AddSubCondition(Trainer::Condition sub) {
      sub.SetSuperCondition(this);
      mSubCondition.push_back(sub);
      return &(mSubCondition.back());
    }
    void OptimizeConditionTree(); //对之前产生的条件树进行优化，删掉所有的空条件
    bool CheckState(const WorldState &state);
    void Report(ofstream &os) const;

  private:
    Trainer::ConditionType mType; //终止条件类型
    double mArg1;                 //终止参数一
    double mArg2;                 //终止参数二
    std::vector<Trainer::Condition> mSubCondition;
    Trainer::Condition *mpSuperCondition;
    Trainer *mpTrainer;
    int mMaxInterval; //满足该终止条件最长时间
    int mMinInterval; //满足该终止条件最短时间
    int mMinTime; //满足该终止条件最短时间开始一次训练时的周期
    int mMaxTime; //满足该终止条件最长时间开始一次训练时的周期
    int mCount; //该条件出现次数
    bool mConverse;
  };

public:
  /**
   * 构造函数和析构函数
   */
  Trainer();
  virtual ~Trainer();

  void Run();
  void SendOptionToServer();
  int GetLastStopTime() { return mLastStopTime; }

private:
  void DoDecisionMaking();
  void DoSampleAction();
  void CheckState();
  void ReadConfigFile();
  void Parse(const std::string &section,
             const std::vector<std::string> &content);
  void ParseCondition(const std::string &content);
  void Record();
  void ReadConverseConf(
      const std::string &str); //场景对调，这个可以试试看能不能找到一些新方法
  void AddCondition(Trainer::Condition *superCondition, const char *name,
                    const char *buffer);
  void InitializeStadium();
  void PrepareForTrain();
  bool ReadRcgFile();
  bool CheckOpponent() const;

  void Start();
  void ChangePlayMode(PlayMode pm);
  void MovePlayer(Unum num, Vector pos, Vector vel, AngleDeg dir = 0);
  void MovePlayer(Trainer::PlayerState ps);
  void MoveBall(Vector pos, Vector vel);
  void Look();
  void TeamNames();
  void Recover();
  void CheckBall();
  void EndTrain();
  void Score(bool isLeft);            //参数为让左边得分
  Vector FindValidPoint(bool isLeft); //不让人能碰到的点

  // say

  // ear

  // eye

  void ChangePlayerType(Unum num, int player_type);

private:
  bool mInitialized;
  bool mPrepared;
  bool mHaveRcg;
  bool mConverse;
  int mTm2Opp[12]; //解决对手和我们相同号码不同角色的问题
  int mOpp2Tm[12]; //和上面那个刚好是倒过来的，一个用于自己变对手，一个用于
  PlayMode mTrainPM;
  unsigned int mTrainTime;  //训练场景所在rcg的时间
  unsigned int mTrainCount; //总共训练次数
  char mTrainRcg[128];
  vector<vector<Trainer::PlayerState>> mPlayerStatesList;
  unsigned int mInitialTime;
  vector<Unum> mNeedOpponentList;
  Trainer::Condition *mpEndCondition;
  vector<pair<Vector, Vector>> mBallStateList;
  vector<ServerPlayMode> mServerPlayModeList;
  ServerPlayMode mCurrentServerPlayMode;
  unsigned int mLastStopTime;
};

#endif /* TRAINER_H_ */
