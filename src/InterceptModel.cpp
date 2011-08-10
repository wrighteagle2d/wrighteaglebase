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

/**
 * 最佳截球点（与当前球速无关，是截球窗口变化时收缩成的那个点，也就是外切点） -- 这里不考虑cd
 * @param relpos
 * @param vp 球员最大速度
 * @param ka 球员可踢范围半径
 * @param fix 是跑动延迟的修正（即player不能全速跑，用全速跑计算，要加个修正）
 * @return
 */
double InterceptModel::CalcPeakPoint(const Vector & relpos, const double & vp, const double & ka, const double fix)
{
	static const double MINERROR = 0.01;

	const double alpha = ServerParam::instance().ballDecay();
	const double ln_alpha = ServerParam::instance().logBallDecay();
	const double x0 = relpos.X();
	const double y0 = relpos.Y();

	if (x0 < 0.0){
		return 150.0;
	}

	if (fabs(y0) < ka){
		return -1.0; //不可能穿越
	}

	double s, p, alpha_p, f, dfdx, last_f = 1000.0, x, last_x = x0;
	int iteration_cycle = 0;

	x = x0;
	do {
		iteration_cycle += 1;
		if (iteration_cycle > 10){
			break;
		}
		s = Sqrt((x - x0) * (x - x0) + y0 * y0);
		p = (s - ka)/vp + fix;
		if (p < 0.0) p = 0.0;
		alpha_p = pow(alpha, (p));
		f = 1.0 - alpha_p * (1 - x * (x - x0) * ln_alpha / (s * vp));

		if(fabs(f) > fabs(last_f)){
			return last_x; //没有切点
		}
		else{
			last_f = f;
		}
		last_x = x;
		dfdx = ln_alpha / vp * ((x - x0) * (f - 1.0) / s + alpha_p * ((x + x - x0) / s - x * (x - x0) * (x - x0) / (s * s * s)));
        dfdx = fabs(dfdx) < FLOAT_EPS? (Sign(dfdx) * FLOAT_EPS): dfdx;
		x = x - f/dfdx;
	} while (fabs(f) > MINERROR);

	return x;
}


double InterceptModel::CalcGoingThroughSpeed(const PlayerState & player, const Ray & ballcourse, const double & distance, const double fix)
{
    Vector rel_pos = (player.GetPredictedPos() - ballcourse.Origin()).Rotate(-ballcourse.Dir());
    double kick_area = (player.IsGoalie())? ServerParam::instance().maxCatchableArea(): player.GetKickableArea();
    double peak = CalcPeakPoint(rel_pos, player.GetEffectiveSpeedMax(), kick_area, fix);
	double gtspeed = 0.0;

	if (peak < 0)
    {
        double d = Max(0.0, kick_area * kick_area - rel_pos.Y() * rel_pos.Y());
		gtspeed = rel_pos.X() + Sqrt(d) + 0.06;
		gtspeed = Max(gtspeed , 1.2);
		return gtspeed;
	}
    else if (peak < distance)
    {
        double cycletopoint = Dasher::instance().RealCycleNeedToPoint(player, ballcourse.GetPoint(peak));
        gtspeed = ServerParam::instance().GetBallSpeed((int)(cycletopoint), peak);

        if (player.IsGoalie() && player.GetUnum() > 0 && player.GetBodyDirConf() > PlayerParam::instance().minValidConf())
        {
            Ray ray(player.GetPredictedPos(), player.GetBodyDir());
			Vector pt;
			if (ballcourse.Intersection(Line(ray), pt))
            {
				double c2p = Dasher::instance().RealCycleNeedToPoint(player, pt);
				double pk = pt.Dist(ballcourse.Origin());
                double gtspd =  ServerParam::instance().GetBallSpeed((int)ceil(c2p), pk);
                gtspeed = Max(gtspeed, gtspd);
			}
		}

        if (gtspeed < ServerParam::instance().ballSpeedMax())
        {
    		double cycletopoint = Dasher::instance().RealCycleNeedToPoint(player, ballcourse.GetPoint(distance));
            double speed = ServerParam::instance().GetBallSpeed((int)ceil(cycletopoint), distance);
    		gtspeed = Max(gtspeed, speed);
    	}
	}
    else
    {
		double cycletopoint = Dasher::instance().RealCycleNeedToPoint(player, ballcourse.GetPoint(distance));
        gtspeed = ServerParam::instance().GetBallSpeed((int)ceil(cycletopoint), distance);
	}

	return gtspeed;
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
