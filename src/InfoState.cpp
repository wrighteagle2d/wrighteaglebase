#include "InfoState.h"
#include "InterceptInfo.h"
#include "PositionInfo.h"


InfoState::InfoState(WorldState *world_state)
{
	mpPositionInfo  = new PositionInfo( world_state, this );
	mpInterceptInfo = new InterceptInfo( world_state, this );
}

InfoState::~InfoState()
{
	delete mpPositionInfo;
	delete mpInterceptInfo;
}

PositionInfo & InfoState::GetPositionInfo() const
{
	mpPositionInfo->Update();
	return *mpPositionInfo;
}

InterceptInfo & InfoState::GetInterceptInfo() const
{
	mpInterceptInfo->Update();
	return *mpInterceptInfo;
}

