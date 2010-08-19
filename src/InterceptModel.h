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
