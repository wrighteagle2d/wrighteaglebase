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

#include "Geometry.h"
#include "Utilities.h"

/**
 * 求与直线是否相交
 * \param l line
 * \param intersection_dist will be set to the distance from origin to
 *                          intersection point
 * \return if intersect
 */
bool Ray::Intersection(const Line & l, double & dist) const
{
	SinCosT value = SinCos(mDirection);

    double tmp = l.A() * Cos(value) + l.B() * Sin(value);
	if (fabs(tmp) < FLOAT_EPS)// 如果平行
    {
        return false;
    }
    else
    {
        dist = (-l.C() - l.A() * mOrigin.X() - l.B() * mOrigin.Y()) / tmp; //正弦定理
        return dist >= 0;
    }
}

/**
 * 求与直线是否相交
 * \param l line
 * \return -1000 if not intersect
 *         distance from origin to intersection point otherwise
 */
double Ray::Intersection(const Line & l) const
{
	double dist;
	return Intersection(l, dist) ? dist : -1000.0;
}

/**
 * 求与直线是否相交
 * \param l line
 * \param point will be set to the intersection point
 * \return if intersect
 */
bool Ray::Intersection(const Line & l, Vector & point) const
{
    double dist;
    bool ret = Intersection(l, dist);
    if (ret) {
    	point = GetPoint(dist);
    }
    return ret;
}

/**
 * 求与射线是否相交
 * \param r ray
 * \param point will be set to the intersection point
 * \return if intersect
 */
bool Ray::Intersection(const Ray &r, Vector & point) const
{
    Line l1(*this);
    Line l2(r);

    if (l1.IsSameSlope(l2)) {
        return false;
    }

    if (!l1.Intersection(l2, point)) {
        return false;
    }

    return IsInRightDir(point) && r.IsInRightDir(point);
}

/**
 * 求与射线是否相交
 * \param r ray
 * \param intersection_dist will be set to the distance from origin to
 *                          intersection point
 * \return if intersect
 */
bool Ray::Intersection(const Ray &r, double & dist) const
{
	Vector point;
	bool ret = Intersection(r, point);
    dist = point.Dist(mOrigin);
    return ret;
}

/**
 * 得到一条射线上离这个点最近的点
 */
Vector Ray::GetClosestPoint(const Vector& point) const
{
    Line l(*this);
    Vector closest_point = l.GetProjectPoint(point);
    return fabs(GetNormalizeAngleDeg((closest_point - mOrigin).Dir() - mDirection)) < 90? closest_point:this->Origin();
}

/**
 * 判断一点的垂足是否在两点之间
 */
bool Line::IsInBetween(const Vector & pt, const Vector & end1, const Vector & end2) const
{
	Assert(IsOnLine(end1) && IsOnLine(end2));

	Vector project_pt = GetProjectPoint(pt);
	double dist2 = end1.Dist2(end2);

	return (project_pt.Dist2(end1) < dist2+FLOAT_EPS && project_pt.Dist2(end2) < dist2+FLOAT_EPS);
}

Vector Line::Intersection(const Line &l) const
{
	Vector point;
	if (Intersection(l, point)) {
		return point;
	}
	return Vector(0.0, 0.0); //as WE2008
}

/**
 * 求与直线是否相交
 * \param l line
 * \param point will be set to the intersection point
 * \return if intersect
 */
bool Line::Intersection(const Line & l, Vector & point) const
{
    if (IsSameSlope(l)) {
        return false;
    }

    if (fabs(mB) > 0.0) {
    	if (fabs(l.B()) > 0.0) {
            point.SetX((mC * l.B() - mB * l.C()) / (l.A() * mB - mA * l.B()));
            point.SetY(GetY(point.X()));
    	}
    	else {
    		point =  Vector(-l.C() / l.A(), GetY(-l.C() / l.A()));
    	}
    }
    else {
    	point =  Vector(-mC / mA, l.GetY(-mC / mA));
    }

    return true;
}


/**
 * 求与射线是否相交
 * \param r ray
 * \param point will be set to the intersection point
 * \return if intersect
 */
bool Line::Intersection(const Ray & r, Vector & point) const
{
    Line l(r);

    if (!Intersection(l, point)) {
        return false;
    }
    else {
        return r.IsInRightDir(point);
    }
}

/**
 * 得到直线上两点间距离这个点最近的点
 */
Vector Line::GetClosestPointInBetween(const Vector & pt, const Vector & end1, const Vector & end2) const
{
	Assert(IsOnLine(end1) && IsOnLine(end2));

	if (IsInBetween(pt, end1, end2))
		return GetProjectPoint(pt);
	else if (end1.Dist2(pt) < end2.Dist2(pt))
		return end1;
	else
		return end2;
}

Vector Rectangular::Intersection(const Ray &r) const
{
	Vector point;
	bool ret = Intersection(r, point);
	if (!ret) return r.Origin(); //as WE2008
	return point;
}

//==============================================================================
bool Rectangular::Intersection(const Ray &r, Vector &point) const
{
    if (!IsWithin(r.Origin())) {
        return false; // do not deal with this condition
    }

    int n = 0;
    Array<Vector, 4> points; // there may be 4 intersections

    if (TopEdge().Intersection(r, points[n])) {
	    if (IsWithin(points[n])) {
		    n++;
	    }
    }

    if (BottomEdge().Intersection(r, points[n])) {
	    if (IsWithin(points[n])) {
		    n++;
	    }
    }

    if (LeftEdge().Intersection(r, points[n])) {
	    if (IsWithin(points[n])) {
		    n++;
	    }
    }

    if (RightEdge().Intersection(r, points[n])) {
	    if (IsWithin(points[n])) {
		    n++;
	    }
    }

    if (n == 0) {
        return false;
    }
    else if (n == 1) {
        point = points[0];
        return true;
    }
    else if (n >= 2) {
	    int max_idx     = 0;
        double max_dist2 = points[0].Dist2(r.Origin());

	    for (int j = 1; j < n; ++j) {
            double dist2 = points[j].Dist2(r.Origin());
		    if (dist2 > max_dist2) {
			    max_idx     = j;
			    max_dist2    = dist2;
		    }
	    }

        point = points[max_idx];
	    return true;
    }

    return false;
}

int	Circle::Intersection(const Ray &r, double &t1, double &t2, const double & buffer) const
{
    Vector rel_center = (mCenter - r.Origin()).Rotate(-r.Dir());

    if ((mRadius + buffer) <= fabs(rel_center.Y()))
    {
	    return 0;
    }
    else if (mRadius <= fabs(rel_center.Y()))
    {
        t1 = rel_center.X() - 0.13;
        t2 = rel_center.X() + 0.13;
	    return 2;
    }
    else
    {
        double dis = Sqrt(mRadius * mRadius - rel_center.Y() * rel_center.Y());
        t1 = rel_center.X() - dis;
        t2 = rel_center.X() + dis;
	    return 2;
    }
}

int	Circle::Intersection(const Circle &C, Vector &v1, Vector &v2, const double & buffer = FLOAT_EPS) const
{
    double  d, a, b, c, p, q, r;
    double  cos_value[2], sin_value[2];
    if (mCenter.Dist(C.Center()) <= buffer
        && fabs(mRadius-C.Radius())<= buffer ) {
        return -1;
    }

    d = mCenter.Dist(C.Center());
    if  (d > mRadius + C.Radius()
        || d < fabs (mRadius - C.Radius())) {
        return 0;
    }

    a = 2.0 * mRadius * (mCenter.X() - C.Center().X());
    b = 2.0 * mRadius * (mCenter.Y() - C.Center().Y());
    c = C.Radius() * C.Radius() - mRadius * mRadius
        - mCenter.Dist2(C.Center());
    p = a * a + b * b;
    q = -2.0 * a * c;
    if  (fabs(d - mRadius - C.Radius()) <= buffer
        || fabs(d - fabs (mRadius - C.Radius()) <= buffer )) {
        cos_value[0] = -q / p / 2.0;
        sin_value[0] = sqrt (1 - cos_value[0] * cos_value[0]);

        v1.SetX(mRadius * cos_value[0] + mCenter.X());
        v1.SetY(mRadius * sin_value[0] + mCenter.Y());

        if  (fabs(v1.Dist2( C.Center()) -
                           C.Radius() * C.Radius()) >= buffer) {
           v1.SetY(mCenter.Y() - mRadius * sin_value[0]);
        }
        return 1;
    }

    r = c * c - b * b;
    cos_value[0] = (sqrt (q * q - 4.0 * p * r) - q) / p / 2.0;
    cos_value[1] = (-sqrt (q * q - 4.0 * p * r) - q) / p / 2.0;
    sin_value[0] = sqrt (1 - cos_value[0] * cos_value[0]);
    sin_value[1] = sqrt (1 - cos_value[1] * cos_value[1]);

    v1.SetX(mRadius * cos_value[0] + mCenter.X());
    v2.SetX(mRadius * cos_value[1] + mCenter.X());
    v1.SetY(mRadius * sin_value[0] + mCenter.Y());
    v2.SetY(mRadius * sin_value[1] + mCenter.Y());

    if  ( fabs(v1.Dist2( C.Center()) -
                       C.Radius() * C.Radius())>=buffer) {
       v1.SetY(mCenter.Y() - mRadius * sin_value[0]);
    }

    if  (fabs(v2.Dist2( C.Center()) -
                       C.Radius() * C.Radius())>= buffer) {
       v2.SetY(mCenter.Y() - mRadius * sin_value[0]);
    }

    if  (v1.Dist(v2) <= buffer) {
        if  (v1.Y() > 0) {
            v2.SetY(-v2.Y());
        } else {
            v1.SetY(-v1.Y());
        }
    }
    return 2;
}
