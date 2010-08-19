#ifndef CLIENT_H_
#define CLIENT_H_

class Observer;
class WorldModel;
class Agent;
class Parser;
class CommandSender;

class Client {
	friend class Player;
	friend class Coach;

	Observer        *mpObserver;
	WorldModel      *mpWorldModel;
	Agent           *mpAgent;

	Parser 		    *mpParser;
	CommandSender   *mpCommandSender;

public:
	Client();
	virtual ~Client();

	/**
	* 动态调试时球员入口函数
	*/
	void RunDynamicDebug();

	/**
	* 正常比赛时的球员入口函数
	*/
	void RunNormal();

	/**
	* 正常比赛时的球员主循环函数
	*/
	void MainLoop();

	/**
	 * 创建Agent，并完成相关调用
	 */
	void ConstructAgent();

	/**
	* 球员决策函数，每周期执行1次
	*/
	virtual void Run() = 0;

	/**
	* 给server发送一些选项，如synch_see,eye_on等
	*/
	virtual void SendOptionToServer() = 0;
};

#endif /* CLIENT_H_ */
