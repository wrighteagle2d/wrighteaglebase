#ifndef __CommandSender_H__
#define __CommandSender_H__

#include "Thread.h"

class Observer;
class Agent;

class CommandSender: public Thread
{
public:
    /**
     * 构造函数和析构函数
     */
    CommandSender(Observer *pObserver);
    ~CommandSender();

    /**
     * 主循环函数
     */
    void StartRoutine();
    void Run(char *msg = 0);
    void RegisterAgent(Agent *agent) { mpAgent = agent; };

private:
    Observer  *mpObserver;
    Agent     *mpAgent;
};

#endif

