#ifndef __Coach_H__
#define __Coach_H__

#include "Client.h"
#include "Types.h"
#include "Utilities.h"

using namespace std;
class HeteroManager;

class Coach: public Client
{
public:
	/**
	* 构造函数和析构函数
	*/
	Coach();
	~Coach();

	void Run();
	void SendOptionToServer();
};

#endif
