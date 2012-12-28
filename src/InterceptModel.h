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

#ifndef INTERCEPTMODEL_H_
#define INTERCEPTMODEL_H_

#include "Geometry.h"
#include "ServerParam.h"

class PlayerState;
class WorldState;

class InterceptModel {
	InterceptModel();

public:
	virtual ~InterceptModel();
	static InterceptModel &instance();

	static const double IMPOSSIBLE_BALL_SPEED;

public:
	struct InterceptSolution
	{
		InterceptSolution ():
			tangc (0),
			interc (1)
		{

		}

		int tangc;     	//切点数目 {0, 1, 2}
		int interc;		//交点数目 {1, 3}

		Array<double, 2> tangp;		//外切点(0)和内切点(1)
		Array<double, 2> tangv;		//外切点速度(0)和内切点速度v
		Array<double, 3> interp;	//最多三个交点
		Array<double, 3> intert;	//交点处对应的周期数
	};

public:
	/**
	 * 下面的函数是用来求解理想截球模型
	 */
	void CalcInterception(const Vector & ball_pos, const Vector & ball_vel, const double buffer, const PlayerState * player, InterceptSolution * sol);
	int CalcTangPoint(double x0, double y0, double vp, double ka, double cd, InterceptSolution * sol);
	double CalcInterPoint(double x_init, double x0, double y0, double vb, double vp, double ka, double cd);

	/**
	 * 下面的函数跟peakpt和穿越速度相关
	 */
	///下面的 fix 是球员的跑动延迟，包括加速延迟和反应延迟，这里默认去1.5个周期
	double CalcPeakPoint(const Vector & relpos, const double & vp, const double & ka, const double fix = 1.5);
    double CalcGoingThroughSpeed(const PlayerState & player, const Ray & ballcourse, const double & distance, const double fix = 1.5);

private:
	/**
	 * 画出理想截球曲线
	 * @param x0
	 * @param y0
	 * @param v0
	 * @param vp
	 * @param ka
	 * @param cd
	 */
	void PlotInterceptCurve(double x0, double y0, double v0, double vp, double ka, double cd, double max_x);
};

#endif /* INTERCEPTMODEL_H_ */
