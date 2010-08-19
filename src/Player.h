#ifndef __Player_H__
#define __Player_H__

#include "Client.h"

class DecisionTree;
class  BeliefState;

class Player: public Client
{
	DecisionTree *mpDecisionTree;

public:
    /**
     * 构造函数和析构函数
     */
    Player();
    ~Player();

    void Run();
    void SendOptionToServer();
};

#endif

