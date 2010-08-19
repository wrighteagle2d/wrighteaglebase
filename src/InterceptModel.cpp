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

#include <cmath>
#include "InterceptModel.h"
#include "ServerParam.h"
#include "PlayerParam.h"
#include "PlayerState.h"
#include "InterceptInfo.h"
#include "Utilities.h"
#include "Dasher.h"
#include "Plotter.h"
#include "Logger.h"

const double InterceptModel::IMPOSSIBLE_BALL_SPEED = 8.0;

InterceptModel::InterceptModel()
{

}

InterceptModel::~InterceptModel()
{

}

InterceptModel &InterceptModel::instance()
{
	static InterceptModel intercept_model;
	return intercept_model;
}

void InterceptModel::CalcInterception(const Vector & ball_pos, const Vector & ball_vel, const double buffer, const PlayerState *player, InterceptSolution *sol)
{
	const double & alpha = ServerParam::instance().ballDecay();
	const double & ln_alpha = ServerParam::instance().logBallDecay();

	Vector start_pt = (player->GetPos() - ball_pos).Rotate(-ball_vel.Dir());

	//取得模型输入
	const double x0 = start_pt.X();
	const double y0 = start_pt.Y();
	const double ball_spd = ball_vel.Mod();

    const double & player_spd = player->GetEffectiveSpeedMax();
    const double & kick_area = buffer;
	const double cycle_delay = double(player->GetPosDelay());

	const double max_x = ball_spd / (1.0 - alpha);

//	if (Logger::instance().CurrentTime() == Time(205, 0) && player->GetUnum() == 4){
//		PlotInterceptCurve(x0, y0, ball_spd, player_spd, kick_area, cycle_delay, max_x);
//	}

	//特殊情况处理
	const double s = Sqrt(x0 * x0 + y0 * y0);
	const double self_fix = kick_area + cycle_delay * player_spd;

	if (s < self_fix){ //最好的情况已经可踢
		sol->tangc = 0;
		sol->interc = 1;
		sol->interp[0] = 0;
		sol->intert[0] = 0;
		return;
	}

	if (ball_spd < 0.1){
		sol->tangc = 0;
		sol->interc = 1;
		sol->interp[0] = 0;

		double p = (s - kick_area)/player_spd - cycle_delay;
		if (p < 0.0) p = 0.0;
		sol->intert[0] = p;
		return;
	}

	//先判断切点个数 -- 根据切点个数得到解的个数，并依次选择迭代的初值
	int n = CalcTangPoint(x0, y0, player_spd, kick_area, cycle_delay, sol);

	if (n < 1){ //没有切点
		sol->interc = 1;

		sol->interp[0] = CalcInterPoint(max_x - 1.0, x0, y0, ball_spd, player_spd, kick_area, cycle_delay);
		sol->intert[0] = log(1.0 - sol->interp[0] * (1.0 - alpha) / ball_spd) / ln_alpha;
	}
	else if (n == 1){
		/**
		* n = 1的情况对应只有一个切点，即外切的时候同时内切
		**/
		sol->interc = 1;

		if (x0 < 0.0) {
			sol->interp[0] = CalcInterPoint(max_x - 1.0, x0, y0, ball_spd, player_spd, kick_area, cycle_delay);
		}
		else {
			sol->interp[0] = CalcInterPoint(x0, x0, y0, ball_spd, player_spd, kick_area, cycle_delay);
		}

		sol->intert[0] = log(1.0 - sol->interp[0] * (1.0 - alpha) / ball_spd) / ln_alpha;
	}
	else {
		if (ball_spd < sol->tangv[1]){ //没有最佳截球区间，早期就可截
			sol->interc = 1;

			sol->interp[0] = CalcInterPoint(x0, x0, y0, ball_spd, player_spd, kick_area, cycle_delay);
			sol->intert[0] = log(1.0 - sol->interp[0] * (1.0 - alpha) / ball_spd) / ln_alpha;
		}
		else if (ball_spd < sol->tangv[0]){ //有最佳截球区间
			sol->interc = 3;

			sol->interp[0] = CalcInterPoint(x0, x0, y0, ball_spd, player_spd, kick_area, cycle_delay);
			sol->intert[0] = log(1.0 - sol->interp[0] * (1.0 - alpha) / ball_spd) / ln_alpha;

			sol->interp[1] = CalcInterPoint((sol->tangp[0] + sol->tangp[1]) * 0.5, x0, y0, ball_spd, player_spd, kick_area, cycle_delay);
			sol->intert[1] = log(1.0 - sol->interp[1] * (1.0 - alpha) / ball_spd) / ln_alpha;

			sol->interp[2] = CalcInterPoint((sol->tangp[1] + max_x) * 0.5, x0, y0, ball_spd, player_spd, kick_area, cycle_delay);
			sol->intert[2] = log(1.0 - sol->interp[2] * (1.0 - alpha) / ball_spd) / ln_alpha;
		}
		else { //没有最佳截球区间，只有后期才可截
			sol->interc = 1;

			sol->interp[0] = CalcInterPoint(max_x - 1.0, x0, y0, ball_spd, player_spd, kick_area, cycle_delay);
			sol->intert[0] = log(1.0 - sol->interp[0] * (1.0 - alpha) / ball_spd) / ln_alpha;
		}
	}
}

/**
 * 计算切点的个数和位置
 *
 * 球运动x距离需要的时间：bt(x) = ln(1 - x(1-α)/v0)/ln(α) -- α为球的decay
 * 人运动到此处所需最短时间：pt(x) = (s(x) - ka)/vp - cd -- s(x) = sqrt((x-x0)**2 + y0**2)，**为幂
 * 切点满足：pt(x) = bt(x) 且 pt'(x) = bt'(x)
 * 消去v0，得到方程：
 * f(x) = 1 - α**pt(x) * (1 - x(x-x0)ln(α)/(s(x)vp))
 * f'(x) = ln(α)/vp * {(f(x)-1)(x-x0)/s(x) + α**pt(x) * {(2x-x0)/s(x) - x(x-x0)**2/s(x)**3}}
 * 取x0为初值，用牛顿迭代法求解，得到外切点，再取一个大一点的x作为初值，求解得到内切点
 *
 * @param x0
 * @param y0
 * @param vp 球员最大速度
 * @param ka 球员可踢范围半径
 * @param cd 球员的cycle_celay
 * @param sol
 * @return 切点个数
 */
int InterceptModel::CalcTangPoint(double x0, double y0, double vp, double ka, double cd, InterceptSolution *sol)
{
	static const double MINERROR = 0.01;

	const double & alpha = ServerParam::instance().ballDecay();
	const double & ln_alpha = ServerParam::instance().logBallDecay();

	double s, p, alpha_p, f, dfdx, last_f = 1000.0, x;
	int iteration_cycle = 0;

    if (fabs(y0) < FLOAT_EPS) {
        sol->tangc = 0;
        return 0; //没有切点
    }

	x = x0;
	do {
		iteration_cycle += 1;
		if (iteration_cycle > 10){
			break;
		}
		s = Sqrt((x - x0) * (x - x0) + y0 * y0);
		p = (s - ka)/vp - cd;
		if (p < 0.0) p = 0.0;
		alpha_p = pow(alpha, (p));
		f = 1.0 - alpha_p * (1 - x * (x - x0) * ln_alpha / (s * vp));

		if(fabs(f) > fabs(last_f)){
			sol->tangc = 0;
			return 0; //没有切点
		}
		else{
			last_f = f;
		}

		dfdx = ln_alpha / vp * ((x - x0) * (f - 1.0) / s + alpha_p * ((x + x - x0) / s - x * (x - x0) * (x - x0) / (s * s * s)));
        dfdx = fabs(dfdx) < FLOAT_EPS? (Sign(dfdx) * FLOAT_EPS): dfdx;
		x = x - f/dfdx;
	} while (fabs(f) > MINERROR);

	sol->tangp[0] = x;

	s = Sqrt((x - x0) * (x - x0) + y0 * y0);
	p = (s - ka)/vp - cd;
	if (p < 0.0) p = 0.0;
	alpha_p = pow(alpha, (p));
	if (1.0 - alpha_p < FLOAT_EPS){ //表示自己到那里几乎不花时间
		sol->tangv[0] = 1000.0;
		sol->tangc = 1;
		return 1;
	}
	else {
		sol->tangv[0] = x * (1.0 - alpha) / (1.0 - alpha_p);
	}

	x += 0.5; //检测是否只有一个切点
	s = Sqrt((x - x0) * (x - x0) + y0 * y0);
	p = (s - ka)/vp - cd;
	if (p < 0.0) p = 0.0;
	alpha_p = pow(alpha, p);
	f = 1.0 - alpha_p * (1 - x * (x - x0) * ln_alpha / (s * vp));
	if (f > 0.0){
		sol->tangc = 1;
		return 1; //只有一个切点
	}
	else {
		do {
			x += 15.0;
			s = Sqrt((x - x0) * (x - x0) + y0 * y0);
			p = (s - ka)/vp - cd;
			if (p < 0.0) p = 0.0;
			alpha_p = pow(alpha, p);
			f = 1.0 - alpha_p * (1 - x * (x - x0) * ln_alpha / (s * vp));
		} while (f < 0.0);
		dfdx = ln_alpha / vp * ((x - x0) * (f - 1.0) / s + alpha_p * ((x + x - x0) / s - x * (x - x0) * (x - x0) / (s * s * s)));
        dfdx = fabs(dfdx) < FLOAT_EPS? (Sign(dfdx) * FLOAT_EPS): dfdx;
		x = x - f/dfdx;

		iteration_cycle = 0;
		while (fabs(f) > MINERROR){
			iteration_cycle += 1;
			if (iteration_cycle > 10){
				break;
			}
			s = Sqrt((x - x0) * (x - x0) + y0 * y0);
			p = (s - ka)/vp - cd;
			if (p < 0.0) p = 0.0;
			alpha_p = pow(alpha, p);
			f = 1.0 - alpha_p * (1 - x * (x - x0) * ln_alpha / (s * vp));
			dfdx = ln_alpha / vp * ((x - x0) * (f - 1.0) / s + alpha_p * ((x + x - x0) / s - x * (x - x0) * (x - x0) / (s * s * s)));
            dfdx = fabs(dfdx) < FLOAT_EPS? (Sign(dfdx) * FLOAT_EPS): dfdx;
			x = x - f/dfdx;
		}

		sol->tangp[1] = x;
		s = Sqrt((x - x0) * (x - x0) + y0 * y0);
		p = (s - ka)/vp - cd;
		if (p < 0.0) p = 0.0;
		alpha_p = pow(alpha, p);
		if (1.0 - alpha_p < FLOAT_EPS){
			sol->tangv[1] = 1000.0;
		}
		else {
			sol->tangv[1] = x * (1.0 - alpha) / (1.0 - alpha_p);
		}
		sol->tangc = 2;
		return 2; //有两个切点
	}
}

/**
 * 求解交点
 *
 * 球运动x距离需要的时间：bt(x) = ln(1 - x(1-α)/v0)/ln(α) -- α为球的decay
 * 人运动到此处所需最短时间：pt(x) = (s(x) - ka)/vp - cd -- s(x) = sqrt((x-x0)**2 + y0**2)，**为幂
 * 截球点满足：pt(x) = bt(x)
 * 得到方程
 * f(x) = pt(x) - bt(x)
 * f'(x) = (x - x0)/(s(x)*vp) + (1/lnα)/(v0/(1-α)-x)
 *
 * @param x_init 迭代的初始值
 * @param x0
 * @param y0
 * @param vb 球的当前速度
 * @param vp 球员最大速度
 * @param ka 球员可踢范围半径
 * @param cd 球员的cycle_celay
 * @param sol
 */
double InterceptModel::CalcInterPoint(double x_init, double x0, double y0, double vb, double vp, double ka, double cd)
{
	static const double MINERROR = 0.01;

	const double & alpha = ServerParam::instance().ballDecay();
	const double & ln_alpha = ServerParam::instance().logBallDecay();

	const double max_x = vb / (1.0 - alpha) - 0.1;

	double s, p, f, dfdx, x;
	int iteration_cycle = 0;

	x = x_init;
	do {
		iteration_cycle += 1;
		if (iteration_cycle > 10){
			break;
		}
		x = Min(x, max_x);
		s = Sqrt((x - x0) * (x - x0) + y0 * y0);
		p = (s - ka)/vp - cd;
		if (p < 0.0) p = 0.0;
		f = p - log(1.0 - x * (1.0 - alpha) / vb) / ln_alpha;
		dfdx = (x - x0) / (s * vp) + (1.0 / ln_alpha) / (vb / (1.0 - alpha) - x);
        dfdx = fabs(dfdx) < FLOAT_EPS? (Sign(dfdx) * FLOAT_EPS): dfdx;
		x = x - f/dfdx;
	}while(fabs(f) > MINERROR);

	return MinMax(0.0, x, max_x);
}

void InterceptModel::PlotInterceptCurve(double x0, double y0, double v0, double vp, double ka, double cd, double max_x)
{
	Plotter::instance().GnuplotExecute("alpha = 0.94");
	Plotter::instance().GnuplotExecute("ln(x) = log(x)");
	Plotter::instance().GnuplotExecute("bt(x) = ln(1 - x * (1 - alpha) / v0) / ln(alpha)");
	Plotter::instance().GnuplotExecute("s(x) = sqrt((x - x0)**2 + y0**2)");
	Plotter::instance().GnuplotExecute("pt(x) = (s(x) - ka) / vp - cd");

	Plotter::instance().GnuplotExecute("x0 = %g", x0);
	Plotter::instance().GnuplotExecute("y0 = %g", y0);
	Plotter::instance().GnuplotExecute("v0 = %g", v0);
	Plotter::instance().GnuplotExecute("vp = %g", vp);
	Plotter::instance().GnuplotExecute("ka = %g", ka);
	Plotter::instance().GnuplotExecute("cd = %g", cd);

	Plotter::instance().GnuplotExecute("set xrange[0:%g]", max_x + 1.0);
	Plotter::instance().GnuplotExecute("set yrange[0:]");

	Plotter::instance().GnuplotExecute("plot bt(x), pt(x)");
}
