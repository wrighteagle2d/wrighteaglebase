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

#ifndef __BALLSTATE_H__
#define __BALLSTATE_H__

#include "BaseState.h"

class BallState : public MobileState
{
public:
    BallState():
    	MobileState( ServerParam::instance().ballDecay(), ServerParam::instance().ballSpeedMax())
    {

    }

    void GetReverseFrom(const BallState & o)
    {
    	UpdatePos(o.GetPos().Rotate(180.0), o.GetPosDelay(), o.GetPosConf());
    	UpdateVel(o.GetVel().Rotate(180.0), o.GetVelDelay(), o.GetVelConf());
    }

private:
    /** 暂时有何种其它信息待定*/
};

#endif
