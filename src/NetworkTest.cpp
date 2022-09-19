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

#include "NetworkTest.h"
#include <algorithm>

StatisticUnit::StatisticUnit() {
  //初始化CU
  CU.count = 0;
  CU.average = 0;
  CU.quadrature = 0;
  CU.variance = 0;
  CU.MaxDelay.val = 0;
  CU.MaxDelay.ValTime = Time(-3, 0);
  CU.MinDelay.val = 111111110;
  CU.MinDelay.ValTime = Time(-3, 0);

  BeginName = "";
  EndName = "";
  int threshold = 20;
  MissBegin = 0;
  for (int i = 0; i < 9; i++) {
    Interval[i].threshold = threshold;
    Interval[i].count = 0;
    Interval[i].probility = 0;
    threshold += 10;
  }

  CMDSend.Dashs = 0;
  CMDSend.Turns = 0;
  CMDSend.Kicks = 0;
  CMDSend.Says = 0;
  CMDSend.TurnNecks = 0;
  CMDSend.Catchs = 0;
  CMDSend.Moves = 0;
  CMDSend.ChangeViews = 0;
  CMDSend.Tackles = 0;
  CMDSend.Pointtos = 0;
  CMDSend.Attentiontos = 0;

  CMDExecute.Dashs = 0;
  CMDExecute.Turns = 0;
  CMDExecute.Kicks = 0;
  CMDExecute.Says = 0;
  CMDExecute.TurnNecks = 0;
  CMDExecute.Catchs = 0;
  CMDExecute.Moves = 0;
  CMDExecute.ChangeViews = 0;
  CMDExecute.Tackles = 0;
  CMDExecute.Pointtos = 0;
  CMDExecute.Attentiontos = 0;
}

StatisticUnit::~StatisticUnit() {}

/*
 * Write statistic infomation to file. Can ONLY be called once.
 */
void StatisticUnit::Flush() {
  char fileName[128];
  sprintf(fileName, "Test/%s-%s-%d.txt", BeginName.data(), EndName.data(),
          MyID);
  FILE *fp = fopen(fileName, "w");
  if (fp != 0) {
    PrintStatic(fp);
    OutPutInterVal(fp);
    PrintCommond(fp);
    fclose(fp);
  } else {
    PRINT_ERROR("open file error " << fileName);
  }
}

void StatisticUnit::PrintStatic(FILE *fp) {
  if (fp == 0) {
    return;
  }

  fprintf(fp, "Num: %ld\n", CU.count);
  fprintf(fp, "Ave: %f\n", CU.average);
  fprintf(fp, "Var: %f\n", CU.variance);
  fprintf(fp, "Max: %ld --cycle(%d,%d)\n", CU.MaxDelay.val,
          CU.MaxDelay.ValTime.T(), CU.MaxDelay.ValTime.S());
  fprintf(fp, "Min: %ld --cycle(%d,%d)\n", CU.MinDelay.val,
          CU.MinDelay.ValTime.T(), CU.MinDelay.ValTime.S());
  fprintf(fp, "Miss %s : %d\n", BeginName.data(), MissBegin);
}

void StatisticUnit::OutPutInterVal(FILE *fp) {
  if (fp == 0) {
    return;
  }

  fprintf(fp, "\n\n");
  fprintf(fp, "\t+------------+----------+----------+\n");
  fprintf(fp, "\t|interval(ms)|count     |probility |\n");
  fprintf(fp, "\t+------------+----------+----------+\n");
  long count = 0;
  double probility = 0;
  fprintf(fp, "\t| <=%3ld      |%4d      | %1.6f |\n", Interval[0].threshold,
          Interval[0].count, Interval[0].probility);
  for (int i = 1; i < 9; i++) {
    count = Interval[i].count;
    probility = Interval[i].probility;
    fprintf(fp, "\t| %3ld~%3ld    |%4ld      | %1.6f |\n",
            Interval[i - 1].threshold, Interval[i].threshold, count, probility);
  }
  fprintf(fp, "\t+------------+----------+----------+\n\n\n");
}

void StatisticUnit::PrintCommond(FILE *fp) {
  if (fp == 0) {
    return;
  }

  int miss = 0;
  double missrate = 0.0;

  fprintf(fp, "Command        Send      Execute   Lost      Miss rate\n");
  miss = CMDSend.Dashs - CMDExecute.Dashs;
  missrate = 0.0;
  if (CMDSend.Dashs != 0) {
    missrate = (double)miss / CMDSend.Dashs;
  }
  fprintf(fp, "dash           %-10d%-10d%-10d%1.9f\n", CMDSend.Dashs,
          CMDExecute.Dashs, miss, missrate);

  miss = CMDSend.Turns - CMDExecute.Turns;
  missrate = 0.0;
  if (CMDSend.Turns != 0) {
    missrate = (double)miss / CMDSend.Turns;
  }
  fprintf(fp, "turn           %-10d%-10d%-10d%1.9f\n", CMDSend.Turns,
          CMDExecute.Turns, miss, missrate);

  miss = CMDSend.Kicks - CMDExecute.Kicks;
  missrate = 0.0;
  if (CMDSend.Kicks != 0) {
    missrate = (double)miss / CMDSend.Kicks;
  }
  fprintf(fp, "kick           %-10d%-10d%-10d%1.9f\n", CMDSend.Kicks,
          CMDExecute.Kicks, miss, missrate);

  miss = CMDSend.Says - CMDExecute.Says;
  missrate = 0.0;
  if (CMDSend.Says != 0) {
    missrate = (double)miss / CMDSend.Says;
  }
  fprintf(fp, "say            %-10d%-10d%-10d%1.9f\n", CMDSend.Says,
          CMDExecute.Says, miss, missrate);

  miss = CMDSend.TurnNecks - CMDExecute.TurnNecks;
  missrate = 0.0;
  if (CMDSend.TurnNecks != 0) {
    missrate = (double)miss / CMDSend.TurnNecks;
  }
  fprintf(fp, "turn_neck      %-10d%-10d%-10d%1.9f\n", CMDSend.TurnNecks,
          CMDExecute.TurnNecks, miss, missrate);

  miss = CMDSend.Catchs - CMDExecute.Catchs;
  missrate = 0.0;
  if (CMDSend.Catchs != 0) {
    missrate = (double)miss / CMDSend.Catchs;
  }
  fprintf(fp, "catch          %-10d%-10d%-10d%1.9f\n", CMDSend.Catchs,
          CMDExecute.Catchs, miss, missrate);

  miss = CMDSend.Moves - CMDExecute.Moves;
  missrate = 0.0;
  if (CMDSend.Moves != 0) {
    missrate = (double)miss / CMDSend.Moves;
  }
  fprintf(fp, "move           %-10d%-10d%-10d%1.9f\n", CMDSend.Moves,
          CMDExecute.Moves, miss, missrate);

  miss = CMDSend.ChangeViews - CMDExecute.ChangeViews;
  missrate = 0.0;
  if (CMDSend.ChangeViews != 0) {
    missrate = (double)miss / CMDSend.ChangeViews;
  }
  fprintf(fp, "change_view    %-10d%-10d%-10d%1.9f\n", CMDSend.ChangeViews,
          CMDExecute.ChangeViews, miss, missrate);

  miss = CMDSend.Tackles - CMDExecute.Tackles;
  missrate = 0.0;
  if (CMDSend.Tackles != 0) {
    missrate = (double)miss / CMDSend.Tackles;
  }
  fprintf(fp, "tackle         %-10d%-10d%-10d%1.9f\n", CMDSend.Tackles,
          CMDExecute.Tackles, miss, missrate);

  miss = CMDSend.Pointtos - CMDExecute.Pointtos;
  missrate = 0.0;
  if (CMDSend.Pointtos != 0) {
    missrate = (double)miss / CMDSend.Pointtos;
  }
  fprintf(fp, "pointto        %-10d%-10d%-10d%1.9f\n", CMDSend.Pointtos,
          CMDExecute.Pointtos, miss, missrate);

  miss = CMDSend.Attentiontos - CMDExecute.Attentiontos;
  missrate = 0.0;
  if (CMDSend.Attentiontos != 0) {
    missrate = (double)miss / CMDSend.Attentiontos;
  }
  fprintf(fp, "attentionto    %-10d%-10d%-10d%1.9f\n", CMDSend.Attentiontos,
          CMDExecute.Attentiontos, miss, missrate);
}

void StatisticUnit::SetMinVal(TIMEDATA td) {
  if (CU.MinDelay.val > td.val) {
    CU.MinDelay = td;
  }
  return;
}

void StatisticUnit::InterValStacistic(long val) {
  for (int i = 0; i < 9; i++) {
    if (Interval[i].threshold * 1000 >= val) {
      Interval[i].count++;
      break;
    }
  }
  for (int i = 0; i < 9; i++) {
    Interval[i].probility = ((double)Interval[i].count) / (double)CU.count;
  }
}

void StatisticUnit::SetMaxVal(TIMEDATA td) {
  if (CU.MaxDelay.val < td.val) {
    CU.MaxDelay = td;
  }
  return;
}
//计算平均值
void StatisticUnit::SetAverage(long val) {
  CU.average = (CU.average * CU.count + val) / ((double)(CU.count + 1));
}

//计算二阶原点矩
void StatisticUnit::Setquadrature(long val) {
  CU.quadrature =
      (CU.quadrature * CU.count + val * val) / ((double)(CU.count + 1));
}

//计算方差
void StatisticUnit::SetVariance() {
  CU.variance = CU.quadrature - CU.average * CU.average;
}

void StatisticUnit::SetStatistic(long val, Time current) {
  TIMEDATA td;
  td.val = val;
  td.ValTime = current;
  SetMaxVal(td);
  SetMinVal(td);
  SetAverage(val);
  Setquadrature(val);
  SetVariance();
  CU.count++;
  InterValStacistic(val);
}

NetworkTest::NetworkTest() {
  BeginTimelist.clear();
  EndTimelist.clear();
  StatUnit.clear();
  mUnum = 0;

  CMDSend.Dashs = 0;
  CMDSend.Turns = 0;
  CMDSend.Kicks = 0;
  CMDSend.Says = 0;
  CMDSend.TurnNecks = 0;
  CMDSend.Catchs = 0;
  CMDSend.Moves = 0;
  CMDSend.ChangeViews = 0;
  CMDSend.Tackles = 0;
  CMDSend.Pointtos = 0;
  CMDSend.Attentiontos = 0;

  CMDExecute.Dashs = 0;
  CMDExecute.Turns = 0;
  CMDExecute.Kicks = 0;
  CMDExecute.Says = 0;
  CMDExecute.TurnNecks = 0;
  CMDExecute.Catchs = 0;
  CMDExecute.Moves = 0;
  CMDExecute.ChangeViews = 0;
  CMDExecute.Tackles = 0;
  CMDExecute.Pointtos = 0;
  CMDExecute.Attentiontos = 0;
}

NetworkTest::~NetworkTest() {
  if (StatUnit.size() > 0) {
    StatUnit[0].setCMDData(CMDSend, CMDExecute);
  }
  // BeginTimelist.clear();
  // EndTimelist.clear();
  // StatUnit.clear();

  WriteRealTimeRecord(); // record RealTime information
  std::for_each(StatUnit.begin(), StatUnit.end(),
                std::mem_fun_ref(&StatisticUnit::Flush));
}

NetworkTest &NetworkTest::instance() {
  static NetworkTest NWT;
  return NWT;
}

void NetworkTest::Begin(const std::string BeginName) {
  if (PlayerParam::instance().NetworkTest()) {
    int i = 0, size = BeginTimelist.size();
    for (; i < size; i++) {
      if (!BeginTimelist[i].Name.compare(BeginName)) {
        break;
      }
    }
    if (i == size) {
      TIMEUNIT Bg;
      Bg.RT = GetRealTime();
      Bg.Name = BeginName;
      Bg.IsRecord = false;
      BeginTimelist.push_back(Bg);
    } else {
      BeginTimelist[i].RT = GetRealTime();
      BeginTimelist[i].IsRecord = true;
    }
  }
}

void NetworkTest::End(const std::string BeginName, const std::string EndName) {
  if (PlayerParam::instance().NetworkTest()) {
    int i = 0, size = EndTimelist.size();
    for (; i < size; i++) {
      if (EndTimelist[i].Name.compare(EndName) == 0) {
        break;
      }
    }

    if (i == size) {
      TIMEUNIT End;
      End.RT = GetRealTime();
      End.Name = EndName;
      End.IsRecord = false;
      EndTimelist.push_back(End);
      EndtoBegin[EndName] = BeginName;
    } else {
      EndTimelist[i].RT = GetRealTime();
      EndTimelist[i].IsRecord = true;
    }
  }
}

long NetworkTest::getInterval(const std::string BeginName,
                              const std::string EndName) {
  if (PlayerParam::instance().NetworkTest()) {
    int i, Size = EndTimelist.size();
    for (i = 0; i < Size; i++) {
      if (!EndTimelist[i].Name.compare(EndName)) {
        int j, BSize = BeginTimelist.size();
        for (j = 0; j < BSize; j++) {
          if (!BeginTimelist[i].Name.compare(BeginName)) {
            long UseTime =
                (EndTimelist[i].RT.GetSec() - BeginTimelist[j].RT.GetSec()) *
                    1000000 +
                EndTimelist[i].RT.GetUsec() - BeginTimelist[j].RT.GetUsec();
            return UseTime;
          }
        }
      }
    }
  }
  return 0;
}

void NetworkTest::Update(const Time &time) {
  if (PlayerParam::instance().NetworkTest()) {
    if (time != CurrentTime) {
      CurrentTime = time; //更新时间
      int i, size = EndTimelist.size();
      for (i = 0; i < size; i++) {
        std::string BeginName = EndtoBegin[EndTimelist[i].Name];
        int k, SUSize = StatUnit.size();
        for (k = 0; k < SUSize; k++) {
          if (StatUnit[k].GetBegin().compare(BeginName) == 0
              /*&&!EndTimelist[i].Name*/) {
            if (!BeginTimelist[k].IsRecord) {
              StatUnit[k].MissBeginIncrement();
              EndTimelist[i].IsRecord = false;
            }
            break;
          }
        }

        if (k == SUSize) {
          StatisticUnit SU;
          SU.setBegin(BeginName);
          SU.setEndName(EndTimelist[i].Name);
          SU.setMyID(mUnum);
          StatUnit.push_back(SU);
        } else if (EndTimelist[i].IsRecord) {
          long Interval = getInterval(BeginName, EndTimelist[i].Name);
          StatUnit[k].SetStatistic(Interval, time);
          EndTimelist[i].IsRecord = false;
          BeginTimelist[k].IsRecord = false;
        }
      }
    }
  }
}
void NetworkTest::SetCommandSendCount(const CommandInfo &cmd) {
  if (PlayerParam::instance().NetworkTest()) {
    if (cmd.mType != CT_None) {
      switch (cmd.mType) {
      case CT_Kick:
        ++CMDSend.Kicks;
        break;
      case CT_Turn:
        ++CMDSend.Turns;
        break;
      case CT_Dash:
        ++CMDSend.Dashs;
        break;
      case CT_Say:
        ++CMDSend.Says;
        break;
      case CT_TurnNeck:
        ++CMDSend.TurnNecks;
        break;
      case CT_Catch:
        ++CMDSend.Catchs;
        break;
      case CT_Move:
        ++CMDSend.Moves;
        break;
      case CT_ChangeView:
        ++CMDSend.ChangeViews;
        break;
      case CT_Pointto:
        ++CMDSend.Pointtos;
        break;
      case CT_Attentionto:
        ++CMDSend.Attentiontos;
        break;
      case CT_Tackle:
        ++CMDSend.Tackles;
        break;
      default:
        break;
      }
    }
  }
}

void NetworkTest::SetCommandExecuteCount(int d, int k, int tu, int s, int tn,
                                         int c, int m, int cv, int pt, int tk,
                                         int fc) {
  if (PlayerParam::instance().NetworkTest()) {
    CMDExecute.Dashs = d;
    CMDExecute.Kicks = k;
    CMDExecute.Turns = tu;
    CMDExecute.Says = s;
    CMDExecute.TurnNecks = tn;
    CMDExecute.Catchs = c;
    CMDExecute.Moves = m;
    CMDExecute.ChangeViews = cv;
    CMDExecute.Pointtos = pt;
    CMDExecute.Tackles = tk;
    CMDExecute.Attentiontos = fc;
  }
}

//==============================================================================
void NetworkTest::AddParserBegin() {
  if (PlayerParam::instance().NetworkTest()) {
    mParserRecord.mBeginTime = GetRealTime();
  }
}

//==============================================================================
void NetworkTest::AddParserEnd(Time current_time) {
  if (PlayerParam::instance().NetworkTest()) {
    mParserRecord.mEndTime = GetRealTime();
    mParserRecord.mCostTime = mParserRecord.mEndTime - mParserRecord.mBeginTime;
    mParserRecord.mTime = current_time;
    mParserList.push_back(mParserRecord);
  }
}

//==============================================================================
void NetworkTest::AddDecisionBegin() {
  if (PlayerParam::instance().NetworkTest()) {
    mDecisionRecord.mBeginTime = GetRealTime();
  }
}

//==============================================================================
void NetworkTest::AddDecisionEnd(Time current_time) {
  if (PlayerParam::instance().NetworkTest()) {
    mDecisionRecord.mEndTime = GetRealTime();
    mDecisionRecord.mCostTime =
        mDecisionRecord.mEndTime - mDecisionRecord.mBeginTime;
    mDecisionRecord.mTime = current_time;
    mDecisionList.push_back(mDecisionRecord);
  }
}

//==============================================================================
void NetworkTest::AddCommandSendBegin() {
  if (PlayerParam::instance().NetworkTest()) {
    mCommandSendRecord.mBeginTime = GetRealTime();
  }
}

//==============================================================================
void NetworkTest::AddCommandSendEnd(Time current_time) {
  if (PlayerParam::instance().NetworkTest()) {
    mCommandSendRecord.mEndTime = GetRealTime();
    mCommandSendRecord.mCostTime =
        mCommandSendRecord.mEndTime - mCommandSendRecord.mBeginTime;
    mCommandSendRecord.mTime = current_time;
    mCommandSendList.push_back(mCommandSendRecord);
  }
}

//==============================================================================
void NetworkTest::WriteRealTimeRecord() {
  if (PlayerParam::instance().NetworkTest()) {
    if (mParserList.size() > 0) {
      char parser_file[128];
      sprintf(parser_file, "Test/RealTime-Parser-%d.txt", mUnum);
      std::ofstream out_parser(parser_file);
      if (out_parser.good() == true) {
        for (unsigned i = 0; i < mParserList.size(); ++i) {
          out_parser << mParserList[i].mCostTime << " ms"
                     << "\t" << mParserList[i].mBeginTime << "\t"
                     << mParserList[i].mEndTime << "\t" << mParserList[i].mTime
                     << std::endl;
        }
      }
      out_parser.close();
    }

    if (mDecisionList.size() > 0) {
      char decision_file[128];
      sprintf(decision_file, "Test/RealTime-Decision-%d.txt", mUnum);
      std::ofstream out_decision(decision_file);
      if (out_decision.good() == true) {
        for (unsigned i = 0; i < mDecisionList.size(); ++i) {
          out_decision << mDecisionList[i].mCostTime << " ms"
                       << "\t" << mDecisionList[i].mBeginTime << "\t"
                       << mDecisionList[i].mEndTime << "\t"
                       << mDecisionList[i].mTime << std::endl;
        }
      }
      out_decision.close();
    }

    if (mCommandSendList.size() > 0) {
      char commandsend_file[128];
      sprintf(commandsend_file, "Test/RealTime-CommandSend-%d.txt", mUnum);
      std::ofstream out_commandsend(commandsend_file);
      if (out_commandsend.good() == true) {
        for (unsigned i = 0; i < mCommandSendList.size(); ++i) {
          out_commandsend << mCommandSendList[i].mCostTime << " ms"
                          << "\t" << mCommandSendList[i].mBeginTime << "\t"
                          << mCommandSendList[i].mEndTime << "\t"
                          << mCommandSendList[i].mTime << std::endl;
        }
      }
      out_commandsend.close();
    }
  }
}

// end of NetworkTest.cpp
