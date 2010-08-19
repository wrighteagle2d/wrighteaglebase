#ifndef INFOSTATE_H_
#define INFOSTATE_H_

#include "WorldState.h"

class InfoState;
class InterceptInfo;
class PositionInfo;


/**
 * 这些是由 WorldState 计算出来得信息状态
 */
class InfoStateBase: public Updatable {
public:
	InfoStateBase(const WorldState *world_state, const InfoState *info_state):
		mpWorldState( world_state ),
		mpInfoState( info_state )
	{
	}
	virtual ~InfoStateBase(){}

	void Update() {
		UpdateAtTime(mpWorldState->CurrentTime());
	}

protected:
	const WorldState *mpWorldState;
	const InfoState *mpInfoState;
};

class InfoState {
	friend class Agent;
	InfoState(InfoState &);
public:
	InfoState(WorldState *world_state);
	virtual ~InfoState();

	PositionInfo & GetPositionInfo() const;
	InterceptInfo & GetInterceptInfo() const;

private:
	PositionInfo  *mpPositionInfo;
	InterceptInfo *mpInterceptInfo;
};

#endif /* INFOSTATE_H_ */
