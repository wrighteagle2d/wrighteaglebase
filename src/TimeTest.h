#ifndef __TimeTest_H__
#define __TimeTest_H__


#include <vector>
#include <cstring>
#include "Utilities.h"

/**
 * 测试函数所花时间的接口
 * Interface to calculate the time cost of a function.
 */
#define TIMETEST(func_name) TimeTestFunc time_test_func(func_name);


/**
 * TimeCost.
 */
struct TimeCost
{
    /**
     * Constructor.
     */
    TimeCost()
    {
        mNum        = 0;
        mAveCost    = 0;
        mMaxTime    = Time(-3, 0);
        mMaxCost    = 0;
        mMinTime    = Time(-3, 0);
        mMinCost    = 1000000;
    }

	/**
	 * 经过次数
	 * Number of times a function was called.
	 */
    long    mNum;

    /**
     * 平均耗时
     * Average cost.
     */
    double  mAveCost; //

    /**
     * 最大耗时对应的周期
     * Cycle which the highest cost was generated at.
     */
    Time    mMaxTime;

    /**
     * 最大耗时
     * Highest time cost.
     */
    long    mMaxCost;

    /**
     * 最小耗时对应的周期
     * Cycle which the lowest cost was generated at.
     */
    Time    mMinTime;

    /**
     * 最小耗时
     * Lowest time cost.
     */
    long    mMinCost;
};


/**
 * TimeRecord.
 */
struct TimeRecord
{
	/**
	 * Constructor.
	 */
    TimeRecord()
    {
        mBeginTime      = RealTime(0, 0);
        mCycleTimeCost  = 0;
    }

    /**
	 * 每周期的信息
	 * Time cost for every cycle.
	 */
    TimeCost    mEachCycle;

    /**
     * 每一次的信息
     * Time cost for each call.
     */
    TimeCost    mEachTime;

    /**
     * 每一次的开始时间
     * Begin time for each call.
     */
    RealTime    mBeginTime;

    /**
     * 每一周期总共花的时间
     * Total cost for each cycle.
     */
    long        mCycleTimeCost;
};


/**
 * TimeTest.
 */
class TimeTest
{
    TimeTest();

public:
    ~TimeTest();

    /**
     * 创建实例
     * Instacne.
     */
    static TimeTest & instance();

    /**
     * 新的周期到来后进行更新，传入的current_time为更新前的那个周期
     * Update when each cycle begins.
     */
    void Update(Time current_time);

    /**
     * 每次测试的开始
     * Begin of each test.
     */
    int Begin(std::string func_name);

    /**
     * 每次测试的结束
     * End of each test.
     */
    void End(int event_id);

    /**
     * 设置自己的号码，用于文件名的赋值
     * Set self unum, which record files will be named after.
     */
    inline void SetUnum(int unum) { mUnum = unum; }

private:
    std::vector<std::string>    mEventQueue; // 记录测试函数的名字
    std::vector<TimeRecord>     mRecordQueue; // 记录测试的结果
    std::vector<bool>           mIsExecute; // 记录当前周期是否被执行过
    std::vector<bool>           mIsBegin; // 用来保证Begin()和End()交替执行

    Time        mUpdateTime; // 上次update的周期
    int         mUnum; // 自己的号码
};


/**
 * TimeTestFunc
 */
class TimeTestFunc
{
public:
    TimeTestFunc(std::string func_name);
    ~TimeTestFunc();

private:
    int mEventID;
};


#endif

