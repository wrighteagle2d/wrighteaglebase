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

#ifndef __Utilities_H__
#define __Utilities_H__

#ifdef WIN32
#include <Winsock2.h>
#else
#include <sys/time.h>
#endif
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <map>
#include "Types.h"

inline bool IsInvalid(const double & x)
{
#ifndef WIN32
	return isnan(x) || isinf(x);
#else
	return _isnan(x) || _isinf(x);
#endif
}

inline double Rint(const double & x)
{
#ifndef WIN32
	return rint(x);
#else
	return floor(x) + 0.5;
#endif
}

inline double Sqr(const double & x)
{
	return x * x;
}

#ifdef _FAST_SQRT
/**
 * Faster sqrt algorithm. 4 times faster than std::sqrt.
 * 相对精度 1e-4.
 */
inline double Sqrt(const double x)
{
    Assert(!IsInvalid(x));
	Assert(x >= 0.0);

	union { double x; int64_t i; } u = { x };
	u.i = 0x5fe6ec85e7de30da - (u.i >> 1);
	return x*u.x*(1.5 - 0.5*x*u.x*u.x);
}
#else
inline double Sqrt(const double & x)
{
    Assert(!IsInvalid(x));
	Assert(x >= 0.0);

	return sqrt(x);
}
#endif

template<typename _Tp>
inline const _Tp&
Max(const _Tp& x, const _Tp& y)
{
	return std::max(x, y);
}

template<typename _Tp>
inline const _Tp&
Min(const _Tp& x, const _Tp& y)
{
	return std::min(x, y);
}

template<typename _Tp>
inline const _Tp&
MinMax(const _Tp& min, const _Tp& x, const _Tp& max)
{
	return Min(Max(min, x), max);
}

template<typename _Tp>
inline int
Sign(const _Tp& x)
{
	return x >= 0? 1: -1;
}

inline AngleDeg Rad2Deg(const AngleRad & x)
{
	return x * 180.0 / M_PI;
}

inline AngleRad Deg2Rad(const AngleDeg & x)
{
	return x * M_PI / 180.0;
}

inline double Sin(const AngleDeg & x)
{
	return sin(Deg2Rad(x));
}

inline double Cos(const AngleDeg & x)
{
	return cos(Deg2Rad(x));
}

inline SinCosT SinCos(const AngleDeg & x)
{
	double sine, cosine;

	sincos(Deg2Rad(x), & sine, & cosine); //faster way to calculate sine and cosine of the same angle x simultaneously

	return std::make_pair(sine, cosine);
}

inline const double & Sin(const SinCosT & value)
{
	return value.first;
}

inline const double & Cos(const SinCosT & value)
{
	return value.second;
}

inline double Tan(const AngleDeg & x)
{
	return tan(Deg2Rad(x));
}

inline AngleDeg ACos(const double & x)
{
	return ((x) >= 1.0 - 0.000006 ? 0.0 : ((x) <= -1.0 + 0.000006 ? 180.0 : (Rad2Deg(acos(x)))));
}

inline AngleDeg ASin(const double & x)
{
	return ((x) >= 1.0 - 0.000006 ? 90.0 : ((x) <= -1.0 + 0.000006 ? -90.0 : (Rad2Deg(asin(x)))));
}

inline AngleDeg ATan(const double & x) //[-90.0, 90.0]
{
	return (Rad2Deg(atan(x)));
}

inline AngleDeg ATan2(const double & y, const double & x) //[-180.0, 180.0]
{
	return ((fabs(x) < 0.000006 && fabs(y) < 0.000006) ? 0 : (Rad2Deg(atan2(y, x))));
}

inline AngleDeg GetNormalizeAngleDeg(AngleDeg ang, const AngleDeg & min_ang = -180.0)
{
	if (ang < min_ang) {
		do {
			ang += 360.0;
		} while (ang < min_ang);
	}
	else {
		const AngleDeg max_ang = 360.0 + min_ang;

		while (ang >= max_ang){
			ang -= 360.0;
		}
	}
	return ang;
}

inline void NormalizeAngleDeg(AngleDeg & ang, const AngleDeg & min_ang = -180.0)
{
	ang = GetNormalizeAngleDeg(ang, min_ang);
}

inline AngleRad GetNormalizeAngleRad(AngleRad ang, const AngleRad & min_ang = -M_PI)
{
	while (ang > 2.0 * M_PI + min_ang)
	{
		ang -= 2.0 * M_PI;
	}
	while (ang < min_ang)
	{
		ang += 2.0 * M_PI;
	}
	return ang;
}

inline void NormalizeAngleRad(AngleRad & ang, const AngleRad & min_ang = -M_PI)
{
	ang = GetNormalizeAngleRad(ang, min_ang);
}

inline AngleDeg GetAngleDegDiffer(const AngleDeg & ang1, const AngleDeg & ang2)
{
	return fabs(GetNormalizeAngleDeg(ang1 - ang2));
}

inline AngleRad GetAngleRadDiffer(const AngleRad & ang1, const AngleRad & ang2)
{
	return fabs(GetNormalizeAngleRad(ang1 - ang2));
}

/**
 * 返回角度gamma是否位于由alpha按顺时针方向旋转到beta所围成的扇形区间内
 * @param alpha
 * @param gamma
 * @param beta
 * @return
 */
inline bool IsAngleDegInBetween(const AngleDeg & alpha, const AngleDeg & gamma, const AngleDeg & beta)
{
	return GetNormalizeAngleDeg(gamma, alpha) <= GetNormalizeAngleDeg(beta, alpha);
}

/**
 * 返回gamma是否离alpha更近
 * @param alpha
 * @param gamma
 * @param beta
 * @return true如果更近
 */
inline bool IsAngleDegCloseToLeft(const AngleDeg & alpha, const AngleDeg & gamma, const AngleDeg & beta)
{
	return GetAngleDegDiffer(gamma, alpha) < GetAngleDegDiffer(gamma, beta);
}

/**
 * Sum of infinite geometry series
 * \param first_term
 * \param r common ratio
 */
inline double SumInfGeomSeries(const double & first_term, const double & r)
{
	return first_term / (1.0 - r);
}

inline double drand( double low, double high )
{
    if ( low > high ) std::swap( low, high );
    if ( high - low < 1.0e-10 ) return (low + high) * 0.5;
    return low + drand48() * (high - low);
}


/**
 * 下面四个函数都是得到系统时间，但用的地方不同，一定要注意
 *
 * GetRealTime()用在动态调试时不会经过的地方，即下面3个函数不能用的地方
 * GetRealTimeParser()用在Parser::Parse()及其调用的所有函数中
 * GetRealTimeDecision()用在Player::Decision()及其调用的所有函数中
 * GetRealTimeCommandSend()用在CommandSend::Run()及其调用的所有函数中
 */
timeval GetRealTime();
timeval GetRealTimeParser();
timeval GetRealTimeDecision();
timeval GetRealTimeCommandSend();

/**
 * 安全数组，Debug版本会检查越界访问
 * _Tp 数据域类型
 * _Nm 数组大小
 * _Zero 标记是否在构造函数内清零
 */
template<typename _Tp, std::size_t _Nm, bool _Zero = false>
class Array
{
	_Tp _M_instance[_Nm ? _Nm : 1];

public:
	Array() {
		if (_Zero) {
			bzero();
		}
	}

	Array(const _Tp & x) {
		fill(x);
	}

	_Tp &
	operator[](const std::size_t & i) {
		Assert(i < _Nm);
		return _M_instance[i];
	}

	const _Tp &
	operator[](const std::size_t & i) const  {
		Assert(i < _Nm);
		return _M_instance[i];
    }

	void bzero() {
		memset(_M_instance, 0, sizeof(_M_instance));
	}

	void fill(const _Tp & x) {
		std::fill(_M_instance, _M_instance + (_Nm ? _Nm : 1), x);
	}

	template<typename _Function>
	_Function
	for_each(_Function f) {
		return std::for_each(_M_instance, _M_instance + (_Nm ? _Nm : 1), f);
	}

	_Tp * instance() {
		return _M_instance;
	}
};

/**
 * 球员相关数组，有效下标：1..TEAMSIZE
 */
template <typename _Tp, bool _Zero = false>
class PlayerArray
{
	Array<_Tp, TEAMSIZE, _Zero> _M_array;

public:
	void bzero() {
		_M_array.bzero();
	}

	void fill(const _Tp & x) {
		_M_array.fill(x);
	}

	template<typename _Function>
	_Function
	for_each(_Function f) {
		return _M_array.for_each(f);
	}

	_Tp &	operator[](const Unum & i) {
		return _M_array[i - 1];
	}

	const _Tp & operator[](const Unum & i) const {
		return _M_array[i - 1];
	}
};

/**
 * 存放关于场上对象（球员和球）数据的数组
 * 有效下标：-TEAMSIZE..TEAMSIZE
 */
template <typename _Tp, bool _Zero = false>
class ObjectArray
{
	Array<_Tp, 2 * TEAMSIZE + 1, _Zero> _M_array; //0:ball, 1...11:teammate, 12...22:opponent

public:
	void bzero() {
		_M_array.bzero();
	}

	void fill(const _Tp & x) {
		_M_array.fill(x);
	}

	_Tp &	operator[](const ObjectIndex & i /*-11...11*/ ) {
		return i >= 0? _M_array[i]: _M_array[TEAMSIZE - i];
	}

	const _Tp & operator[](const ObjectIndex & i /*-11...11*/ ) const {
		return i >= 0? _M_array[i]: _M_array[TEAMSIZE - i];
	}

	_Tp & GetOfBall() {
		return (*this)[0];
	}

	const _Tp & GetOfBall() const {
		return (*this)[0];
	}

	_Tp & GetOfObject(const ObjectIndex & i /*-11...11*/ ) {
		return (*this)[i];
	}

	const _Tp & GetOfObject(const ObjectIndex & i /*-11...11*/ ) const {
		return (*this)[i];
	}

	_Tp & GetOfTeammate(const ObjectIndex & i /*1...11*/ ) {
		Assert(i > 0);
		return (*this)[i];
	}

	const _Tp & GetOfTeammate(const ObjectIndex & i /*1...11*/ ) const {
		Assert(i > 0);
		return (*this)[i];
	}

	_Tp & GetOfOpponent(const ObjectIndex & i /*1...11*/ ) {
		Assert(i > 0);
		return (*this)[-i];
	}

	const _Tp & GetOfOpponent(const ObjectIndex & i /*1...11*/ ) const {
		Assert(i > 0);
		return (*this)[-i];
	}
};

class RealTime
{
public:
	explicit RealTime(long tv_sec = 0, long tv_usec = 0) {
		mTime.tv_sec = tv_sec;
		mTime.tv_usec = tv_usec;
	}

	RealTime(const timeval &t): mTime(t) { }

	timeval GetTime() const { return mTime; }
	long GetSec() const { return mTime.tv_sec; }
	long GetUsec() const { return mTime.tv_usec; }

	const RealTime & operator = (const timeval &t) { mTime = t; return *this; }
	RealTime operator + (const RealTime &t) const;
	RealTime operator + (int msec) const;
	RealTime operator - (int msec) const;
	int operator - (const RealTime &t) const;
	long Sub( const RealTime &t);

	bool operator < (const RealTime &t) const;
	bool operator > (const RealTime &t) const;
	bool operator == (const RealTime &t) const;
	bool operator != (const RealTime &t) const;

	friend std::ostream & operator << (std::ostream &os, RealTime t);

private:
	timeval mTime;

	static timeval mStartTime;
	static const long ONE_MILLION;
};

class Time
{
public:
	explicit Time(int t = -3, int s = 0): mT(t), mS(s) {}

	int T() const { return mT; }
	int S() const { return mS; }
	void SetT(int t) { mT = t; }
	void SetS(int s) { mS = s; }

	const Time & operator=(const int a) { mT = a;  mS = 0; return *this; }
	Time operator+(const int a) const { return Time(mT + a, 0); }
	Time operator-(const int a) const;
	int operator-(const Time &a) const;

	void operator+=(const int a) { *this = *this + a; }
	void operator-=(const int a) { *this = *this - a; }
	void operator-=(const Time &a) { *this = *this - a; }
	const Time & operator++() { *this += 1; return *this; }
	const Time & operator--() { *this -= 1; return *this; }
	int operator%(const int a) const { return (mT + mS) % a; }

	bool operator==(const Time &a) const { return (mS == a.S()) && (mT == a.T()); }
	bool operator!=(const Time &a) const { return (mS != a.S()) || (mT != a.T()); }
	bool operator<(const Time &a) const { return (mT < a.T()) || (mT == a.T() && mS < a.S()); }
	bool operator<=(const Time &a) const { return (mT < a.T()) || ( mT == a.T() && mS <= a.S()); }
	bool operator>(const Time &a) const { return (mT > a.T()) || (mT == a.T() && mS > a.S()); }
	bool operator>=(const Time &a) const { return (mT > a.T()) || (mT == a.T() && mS >= a.S()); }
	bool operator!() const { return (mS == 0) && (mT == 0); }
	friend std::ostream & operator<<(std::ostream & os, const Time& t) { return os << '(' << t.T() << ':' << t.S() << ')'; }

private:
	int mT;
	int mS;
};


/**
 * bellow is parse utilities
 */
namespace parser {
inline double get_double(char **str_ptr){
	while (!isdigit(**str_ptr) && **str_ptr != '-' && **str_ptr != '+' && **str_ptr != '.' && **str_ptr) (*str_ptr)++;
	return strtod(*str_ptr, str_ptr);
}

inline double get_double(char *str){
	while (!isdigit(*str) && *str != '-' && *str != '+' && *str != '.' && *str) str++;
	return strtod(str, (char **) 0);
}

inline int get_int(char **str_ptr){
	while (!isdigit(**str_ptr) && **str_ptr != '-' && **str_ptr != '+' && **str_ptr != '.' && **str_ptr) (*str_ptr)++;
	return static_cast<int>(strtol(*str_ptr, str_ptr, 10));
}

inline int get_int(char *str){
	while (!isdigit(*str) && *str != '-' && *str != '+' && *str != '.' && *str) str++;
	return static_cast<int>(strtol(str, (char **) 0, 10));
}

inline void get_word(char **str_ptr){
	while ( !isalpha(**str_ptr) && **str_ptr) (*str_ptr)++;
}

inline void get_next_word(char **str_ptr){
	while ( isalpha(**str_ptr) ) (*str_ptr)++;
	get_word(str_ptr);
}
}

namespace sight {
inline AngleDeg ViewAngle(ViewWidth view_width)
{
	switch(view_width){
	case VW_Narrow: return 60.0;
	case VW_Normal: return 120.0;
	case VW_Wide: return 180.0;
	default: Assert(0); return 90.0;
	}
}

inline int SightDelay(ViewWidth view_width)
{
	switch(view_width) {
	case VW_Narrow: return 1;
	case VW_Normal: return 2;
	case VW_Wide: return 3;
	default: Assert(0); return 1;
	}
}
}

inline double Quantize( const double v, const double q )
{
    return Rint( v / q ) * q;
}

template <class Data> class TimedData
{
public:
	TimedData(Time time = Time(-3, 0))               { mTime = time; }
	TimedData(const Data &data, Time time)  { mData = data; mTime = time; }

	const Data & GetData() const        { return mData; }
	const Time & GetTime() const        { return mTime; }
	void SetData(const Data &data)      { mData = data; }
	void SetTime(const Time time)       { mTime = time; }

	void operator = (const Data &data)  { mData = data; }
	TimedData operator + (TimedData d)  { return TimedData(mData + d, mTime); }
	TimedData operator - (TimedData d)  { return TimedData(mData - d, mTime); }
	TimedData operator / (TimedData d)  { return TimedData(mData / d, mTime); }
	TimedData operator * (TimedData d)  { return TimedData(mData * d, mTime); }

private:
	Data mData;
	Time mTime;
};

/** Key Player Information */
class KeyPlayerInfo
{
public:
	Unum    mUnum;
	double  mValue;

	KeyPlayerInfo():
		mUnum ( 0 ),
		mValue ( 0.0 )
	{
	};

	bool operator < (const KeyPlayerInfo &other) const  { return mValue < other.mValue; }
	bool operator > (const KeyPlayerInfo &other) const  { return mValue > other.mValue; }
	bool operator == (const KeyPlayerInfo &other) const { return fabs(mValue - other.mValue) < FLOAT_EPS; }
};

class Updatable {
public:
	Updatable(): mUpdateTime(Time(-3, 0)) {}

	virtual ~Updatable(){}

	void UpdateAtTime(const Time & time) {
		if (mUpdateTime != time){
			mUpdateTime = time;
			UpdateRoutine();
		}
	}

private:
	virtual void UpdateRoutine() = 0;

private:
	Time mUpdateTime;
};

class ServerPlayModeMap {
public:
	static ServerPlayModeMap & instance();
	ServerPlayMode GetServerPlayMode(const std::string & str);
	const char * GetPlayModeString(ServerPlayMode spm);

private:
	ServerPlayModeMap();
	void Bind(const std::string & str, ServerPlayMode spm);

private:
	std::map<std::string, ServerPlayMode> mString2Enum;
	std::map<ServerPlayMode, std::string> mEnum2String;
};


template<class _Container, typename _Tp>
bool has(const _Container & x, const _Tp & key) {
	return std::find(x.begin(), x.end(), key) != x.end();
}

#endif
