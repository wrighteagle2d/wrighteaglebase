#include "CommandSender.h"
#include "DynamicDebug.h"
#include "Observer.h"
#include "Agent.h"
#include "Logger.h"
#include "NetworkTest.h"

CommandSender::CommandSender(Observer *pObserver): mpObserver(pObserver), mpAgent(0)
{
}


CommandSender::~CommandSender()
{
}

void CommandSender::StartRoutine()
{
	static char msg[MAX_MESSAGE];

    while (mpObserver->WaitForCommandSend())
    {
        NetworkTest::instance().AddCommandSendBegin();

        strcpy(msg, "record_cmd: ");
        Run(msg);
        DynamicDebug::instance().AddMessage(msg, MT_Send);

        NetworkTest::instance().AddCommandSendEnd(mpObserver->CurrentTime());
    }
}

void CommandSender::Run(char *msg)
{
	if (mpAgent != 0){
		mpAgent->SendCommands(msg);
	}
	else {
		PRINT_ERROR("null agent");
	}
}

