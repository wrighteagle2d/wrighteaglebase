/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2010                                                    *
 * Copyright (C) 1998-2010 WrightEagle 2D Soccer Simulation Team,                   *
 *                         Multi-Agent Systems Lab.,                                *
 *                         School of Computer Science and Technology,               *
 *                         University of Science and Technology of China, China.    *
 *                                                                                  *
 * This program is free software; you can redistribute it and/or                    *
 * modify it under the terms of the GNU General Public License                      *
 * as published by the Free Software Foundation; either version 2                   *
 * of the License, or (at your option) any later version.                           *
 *                                                                                  *
 * This program is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 * GNU General Public License for more details.                                     *
 *                                                                                  *
 * You should have received a copy of the GNU General Public License                *
 * along with this program; if not, write to the Free Software                      *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.  *
 ************************************************************************************/

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

