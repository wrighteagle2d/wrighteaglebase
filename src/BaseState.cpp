#include "BaseState.h"

void BaseState::UpdatePos(const Vector & pos, int delay, double conf)
{
    mPos.mValue = pos;
    mPos.mCycleDelay = delay;
    mPos.mConf = conf;
}

void BaseState::AutoUpdate(int delay_add, double conf_dec_factor)
{
	mPos.AutoUpdate(delay_add, conf_dec_factor);
}

void MobileState::UpdateVel(const Vector & vel, int delay, double conf)
{
    mVel.mValue = vel;
    mVel.mCycleDelay = delay;
    mVel.mConf = conf;

    ResetPredictor();
}

void MobileState::UpdatePos(const Vector & pos, int delay, double conf)
{
	BaseState::UpdatePos(pos, delay, conf);

	ResetPredictor();
}

void MobileState::AutoUpdate(int delay_add , double conf_dec_factor)
{
	BaseState::AutoUpdate(delay_add , conf_dec_factor);
	mVel.AutoUpdate(delay_add , conf_dec_factor);
}

