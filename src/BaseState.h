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

#ifndef __BASESTATE_H__
#define __BASESTATE_H__


#include "ServerParam.h"
#include "PlayerParam.h"
#include <cstring>

//最基本的值
template<class T>
class StateValue
{
public:
	StateValue():
		mValue (T()),
		mCycleDelay(9999),
		mConf(0)
	{
	}

	StateValue(const StateValue & o) {
		mValue = o.mValue;
		mCycleDelay = o.mCycleDelay;
		mConf = o.mConf;
	}

	/**内部自更新
	* @param 每周期delay加上的数据
	* @param 每周期conf衰减的数据*/
	void AutoUpdate(int delay_add = 1 , double conf_dec_factor = 1)
	{
		mCycleDelay += delay_add;
		mConf *= conf_dec_factor;
	}

public:
	/**值大小*/
	T mValue;

	/**值距当前时间*/
	int mCycleDelay;

	/**值的可信度*/
	double mConf;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**最基础的BaseState类*/
class BaseState
{
public:
	BaseState()
	{
		mPos.mValue = Vector(10000 , 10000);
		mPosEps = 10000;
	}

	BaseState(const BaseState & o) {
		mPos = o.mPos;
	}

	/**更新位置
	* @param 位置
	* @param 时间
	* @param 置信度*/
	void UpdatePos(const Vector & pos , int delay = 0, double conf = 1);

	/**获得当前或前几次的位置
	* @param 最新值之前的周期数，范围为0到RECORDSIZE-1,否则会循环
	*/
	const Vector & GetPos() const { return mPos.mValue; }

	/**获得当前或前几次的时间
	* @param 最新值之前的周期数，范围为0到RECORDSIZE-1,否则会循环
	*/
	int GetPosDelay() const { return mPos.mCycleDelay; }

	/**获得当前或前几次的置信度
	* @param 最新值之前的周期数，范围为0到RECORDSIZE-1,否则会循环
	*/
	const double & GetPosConf() const { return mPos.mConf; }


	/**
	 *  设置pos的误差范围,主要指误差圆的半径
	 */
	void UpdatePosEps(double eps)  { mPosEps = eps;}

	/**
	 * 获得pos的误差的eps
	 */
	const double & GetPosEps() const { return mPosEps;}

	/**内部自更新
	* @param 每周期delay加上的数据
	* @param 每周期conf衰减的数据*/
	void AutoUpdate(int delay_add = 1 , double conf_dec_factor = 1);
private:
	/**存储数周期的位置*/
	StateValue<Vector> mPos;

	double mPosEps;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
/**静态物体基类*/
/**注：暂时没想到有什么信息*/
class StaticState: public BaseState
{
};

class MobileState: public BaseState
{
public:
	MobileState(double decay, double effective_speed_max):
		mDecay(decay),
		mEffectiveSpeedMax(effective_speed_max),
		mpPredictor(new Predictor(mDecay)),
		mGuessed (0)
	{
		mVelEps = 10000;
		ResetPredictor();
	}

    MobileState(const MobileState &m):
    	BaseState(m),
    	mDecay(m.GetDecay()),
    	mEffectiveSpeedMax(m.GetEffectiveSpeedMax()),
    	mpPredictor(new Predictor(mDecay)),
    	mGuessed (0)
    {
		mVel = m.mVel;
		mVelEps = m.mVelEps;
		UpdatePos(m.GetPos(), m.GetPosDelay(), m.GetPosConf());
	}

	virtual ~MobileState() { delete mpPredictor; }

	void UpdatePos(const Vector & pos , int delay = 0, double conf = 1.0);

	const MobileState & operator=(const MobileState &v)
	{
		mVel = v.mVel;
		mDecay = v.mDecay;

		UpdatePos(v.GetPos(), v.GetPosDelay(), v.GetPosConf()); //use old predictor

		mEffectiveSpeedMax = v.mEffectiveSpeedMax;

		return *this;
	}

	/**更新速度
	* @param 速度
	* @param 时间
	* @param 置信度*/
	void UpdateVel(const Vector & vel , int delay = 0, double conf = 1);

	/**获得当前或前几次的速度
	* @param 最新值之前的周期数，范围为0到RECORDSIZE-1,否则会循环
	*/
	const Vector & GetVel() const { return mVel.mValue; }

	/**获得当前或前几次的速度的时间
	* @param 最新值之前的周期数，范围为0到RECORDSIZE-1,否则会循环
	*/
	int GetVelDelay() const { return mVel.mCycleDelay; }

	/**获得当前或前几次的速度的置信度
	* @param 最新值之前的周期数，范围为0到RECORDSIZE-1,否则会循环
	*/
	const double & GetVelConf() const { return mVel.mConf; }

	/**
	 * 计算速度误差
	 */
	void UpdateVelEps(double eps) { mVelEps = eps;}

	/**
	 * 获得速度误差
	 */
	double GetVelEps() const { return mVelEps;};

	/**
	* 得到运动 decay
	* @return
	*/
	const double & GetDecay() const { return mDecay; }

	void SetDecay(const double & decay) { mDecay = decay; }

	/**
	* 实际最大速度
	*/
	const double & GetEffectiveSpeedMax() const { return mEffectiveSpeedMax; }
	void SetEffectiveSpeedMax(double effective_speed_max) { mEffectiveSpeedMax = effective_speed_max; }

	/**内部自更新
	* @param 每周期delay加上的数据
	* @param 每周期conf衰减的数据*/
	void AutoUpdate(int delay_add = 1 , double conf_dec_factor = 1);

public:
	class Predictor {
	public:
		Predictor(const double & decay): mDecay(decay) {
		}

		enum {
			MAX_STEP = 50
		};

		const Vector & GetPredictedPos(int step = 1){
			step = Min(step, int(MAX_STEP));

			if (step <= mPredictedStep){
				return mPredictedPos[step];
			}
			else {
				PredictTo(step);

				return mPredictedPos[step];
			}
		}	

		const Vector & GetPredictedVel(int step = 1){
			step = Min(step, int(MAX_STEP));

			if (step <= mPredictedStep){
				return mPredictedVel[step];
			}
			else {
				PredictTo(step);

				return mPredictedVel[step];
			}
		}

		void UpdatePosAndVel(const Vector & pos, const Vector & vel) {
			mPredictedPos[0] = pos;
			mPredictedVel[0] = vel;
			mPredictedStep = 0;
		}

	private:
		void PredictTo(const int & step) {
			for (int i = mPredictedStep + 1; i <= step; ++i){
				mPredictedPos[i] = mPredictedPos[i-1] + mPredictedVel[i-1];
				mPredictedVel[i] = mPredictedVel[i-1] * mDecay;
			}
			mPredictedStep = step;
		}

		const double mDecay;

		Array<Vector, MAX_STEP + 1> mPredictedPos;
		Array<Vector, MAX_STEP + 1> mPredictedVel;
		int mPredictedStep;
	};

	void ResetPredictor() {
		mpPredictor->UpdatePosAndVel(GetPos(), GetVel());
	}

public:
	const Vector & GetPredictedPos(int step = 1) const {
		return mpPredictor->GetPredictedPos(step);
	}

	const Vector & GetPredictedVel(int step = 1) const {
		return mpPredictor->GetPredictedVel(step);
	}

	Vector GetFinalPos() const { return GetPos() + GetVel() / (1.0 - GetDecay()); }

public:
	//guessed time is now only for WorldStateUpdater , other should not use it;
	/** set guessed times*/
	void UpdateGuessedTimes(int guessed)
	{
		Assert(guessed >= 0);
		mGuessed = guessed;
	}

	/** get guessed times*/
	int GetGuessedTimes() const
	{
		return mGuessed;
	}

private:
	/**存储数个周期的速度*/
	StateValue<Vector> mVel;

	double mVelEps;

	double mDecay;
	double mEffectiveSpeedMax;

private:
	Predictor *mpPredictor;

	int mGuessed; //indicate that the times of this object ot be guessed -- only used from WorldStateUpdater::Run
};
#endif

