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

#ifndef VISUALSYSTEM_H_
#define VISUALSYSTEM_H_

#include <cstring>
#include <list>
#include <fstream>

#include "Types.h"
#include "Utilities.h"
#include "Geometry.h"
#include "PlayerState.h"
#include "ServerParam.h"
#include <set>

class MobileState;
class PlayerState;
class BallState;
class Observer;
class Agent;
class WorldState;
class InfoState;

/**
* 各个根结点的行为可以向视觉系统提出视觉请求，由视觉系统最终做出视觉决策
*/
class VisualSystem {
	VisualSystem();

	class VisualRequest
	{
	public:
		VisualRequest(): mPrePos(Vector(HUGE_VALUE, HUGE_VALUE)), mCycleDelay(0), mConf(0.0), mpObject(0)
		{
			Clear();
		}

		void Clear() {
			mValid = false;
			mFreq = 100.0;
			mScore = 0.0;
		}

		Vector mPrePos;

		double PreDistance() {
			return mPrePos.Mod();
		}

		bool mValid;
		int  mCycleDelay;

		double mFreq;    //视觉请求要求的视觉频度
		double mConf;    //频度周期内的置信度
		double mScore;   //满足请求的信息增益

		const MobileState *mpObject;

		int mUnum; //+, -, 0

		bool operator > (const VisualRequest &o){
			return mConf == o.mConf?
					mCycleDelay == o.mCycleDelay?
							mFreq < o.mFreq
							: mCycleDelay > o.mCycleDelay
							  : mConf < o.mConf;
		}

		class PointerGreater {
		public:
			bool operator()(VisualRequest *p, VisualRequest *q){ //定义：p < q if *p > *q;
				return *p > *q;
			}
		};

		void UpdateEvaluation() {
		    mFreq = Max(1.0, mFreq);
			mConf = Max(1.0 - (mCycleDelay / mFreq), 0.0);
			mScore = 1.0 - mConf;
			mScore = Max(mScore, 0.001);
		}

		double Multi() {
			return 1.0 + mScore;
		}
	};

	struct VisualAction
	{
		AngleDeg mDir;
		double   mScore;

		VisualAction(AngleDeg dir = 0.0, double score = -1.0): mDir(dir), mScore(score) {}
	};

	/**
	* 视觉周围一周的评价分布 -- 精确到一度
	*/
	class VisualRing {
	public:
		VisualRing() {
			Clear();
		}

		double & Score(const AngleDeg & ang) {
			return mScore[static_cast<int>(GetNormalizeAngleDeg(ang, 0.0))];
		}

		VisualAction GetBestVisualAction(const AngleDeg left_most, const AngleDeg right_most, const AngleDeg interval_length);

		void Dump(std::ostream & os, AngleDeg left_most, AngleDeg right_most) {
			double sum = 0.0;
			for (AngleDeg a = left_most; a < right_most; ++a) {
				double score = Score(a);
				os << a << ' ' << score << std::endl;
				sum += score;
			}
			std::cerr << "sum score: " << sum << std::endl;
		}

		void Show(AngleDeg left_most, AngleDeg right_most) {
			std::ofstream file("pp");
			Dump(file, left_most, right_most);
			file.close();

			Plotter::instance().GnuplotExecute("set xrange [%g:%g]", left_most, right_most);
			Plotter::instance().GnuplotExecute("plot \"pp\" with steps");
		}

		void Clear() {
			mScore.bzero();
		}

	private:
		Array<double, 361> mScore;
	};

public:
	static VisualSystem & instance();
	virtual ~VisualSystem();

	void ResetVisualRequest();
	void Decision();

	void RaiseBall(Agent & agent, double eva = 0.0) { if (mpAgent == & agent) { RaiseBall(eva); } }
	void RaisePlayer(Agent & agent, Unum num, double eva = 0.0) { if (mpAgent == & agent) { RaisePlayer(num, eva); } }

	void SetForceSeeBall(const Agent & agent) { if (mpAgent == & agent) { SetForceSeeBall(); } }
	void SetForceSeePlayer(const Agent & agent, Unum i) { if (mpAgent == & agent) { SetForceSeePlayer(i); } }

	void SetCritical(const Agent & agent, bool critical) { if (mpAgent == & agent) { SetCritical(critical); } }
	void ForbidDecision(const Agent & agent) { if (mpAgent == & agent) { mForbidden = true;} }
	void ChangeViewWidth(const Agent & agent, ViewWidth view_width) { if (mpAgent == & agent) { ChangeViewWidth(view_width); } }
	void SetCanTurn(const Agent & agent, bool can_turn) { if (mpAgent == & agent) { SetCanTurn(can_turn); } }

public:
	void Initial(Agent * agent);

private:
	void UpdatePredictInfo();
	void RaiseBall(double eva = 0.0);
	void RaisePlayer(ObjectIndex num, double eva = 0.0);
	void RaiseForgotObject(ObjectIndex unum);

	void ViewModeDecision();
	void DoInfoGather();
	void DealVisualRequest();
	void DealWithSpecialObjects();
	void SetVisualRing();
	void GetBestVisualAction();
	VisualAction GetBestVisualActionWithViewWidth(ViewWidth view_width, bool force = false);
	bool ForceSearchBall();
	void DoVisualExecute();
	void DoDecision();

	void SetForceSeeBall();
	void SetForceSeePlayer(Unum i);

	void SetCritical(bool critical) { mIsCritical = critical; }
	void ChangeViewWidth(ViewWidth view_width) { mViewWidth = view_width; }
	void SetCanTurn(bool can_turn) { mCanTurn = can_turn; }

	/**
	* 评价视觉请求，计算score
	*/
	void EvaluateVisualRequest();

	void DoInfoGatherForBallFree();
	void DoInfoGatherForFastForward();
	void DoInfoGatherForPenaltyAttack();
	void DoInfoGatherForDefense();
	void DoInfoGatherForGoalie();

	bool DealWithSetPlayMode();

	int NewSightComeCycle(ViewWidth view_width) { //新视觉到达的周期
		return Max(sight::SightDelay(view_width) - mpSelfState->GetPosDelay(), 1);
	}

	int NewSightWaitCycle(ViewWidth view_width) { //视觉到达时等待的周期数
		return NewSightComeCycle(view_width) + mpSelfState->GetPosDelay();
	}

	VisualRequest * GetVisualRequest(Unum i) {
		Assert(i != 0);
		return & mVisualRequest[i];
	}

private:
	Agent       * mpAgent;
	WorldState  * mpWorldState;
	BallState   * mpBallState;
	PlayerState * mpSelfState;
	InfoState   * mpInfoState;

	ObjectArray<VisualRequest> mVisualRequest;
	ObjectArray<bool> mForceToSeeObject;
	std::set<VisualRequest *> mHighPriorityPlayerSet;

	bool mIsCritical;
	bool mCanForceChangeViewWidth;
	bool mIsSearching;
	bool mForbidden;

	bool mCanTurn;
	ViewWidth mViewWidth;			//当前视觉宽度
	AngleDeg  mPreBodyDir;		//当前周期动作执行完后的身体角度（比如有turn动作）
	Vector    mPreBallPos;				//当前周期动作执行完后的球的位置（比如有kick动作）
	Vector    mPreSelfPos;          //当前周期动作执行完后的球的位置（比如有dash动作）

	VisualAction mBestVisualAction;
	VisualRing mVisualRing;
	int mSenseBallCycle; //几个周期后球在3m范围内

private:
	int GetSenseBallCycle();

private:
	class VirtualSelf: public PlayerState {
	public:
		VirtualSelf(const PlayerState & player): PlayerState(player) {}
		const double & GetKickableArea() const { return ServerParam::instance().visibleDistance(); }
	};
};

#endif /* VISUALSYSTEM_H_ */
