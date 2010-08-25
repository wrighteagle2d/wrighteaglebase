/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2010                                                    *
 * Copyright (C) 1998-2010 WrightEagle 2D Soccer Simulation Team,                   *
 *                         Multi-Agent Systems Lab.,                                *
 *                         School of Computer Science and Technology,               *
 *                         University of Science and Technology of China, China.    *
 *                                                                                  *
 * This program is free software; you can redistribute it and/or                    *
 * modify it under the terms of the GNU General Public License                      *
 * as published by the Free Software Foundation; either version 2                   *
 * of the License, or (at your option) any later version.                           *
 *                                                                                  *
 * This program is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 * GNU General Public License for more details.                                     *
 *                                                                                  *
 * You should have received a copy of the GNU General Public License                *
 * along with this program; if not, write to the Free Software                      *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.  *
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
