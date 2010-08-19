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

#include "Evaluation.h"
#include "ServerParam.h"
#include "Agent.h"
#include "PositionInfo.h"
#include "Strategy.h"
#include "Net.h"
#include "WorldState.h"
#include "InfoState.h"

Evaluation::Evaluation()
{
	mSensitivityNet = new Net("data/sensitivity.net");
}

Evaluation::~Evaluation()
{
	delete mSensitivityNet;
}

Evaluation & Evaluation::instance()
{
    static Evaluation evaluation;
    return evaluation;
}

double Evaluation::EvaluatePosition(const Vector & pos, bool ourside)
{
	static double input[2];
	static double output[1];

	input[0] = pos.X() / (ServerParam::instance().PITCH_LENGTH * 0.5);
	input[1] = fabs(pos.Y()) / (ServerParam::instance().PITCH_WIDTH * 0.5) * 2.0 - 1.0;
	if (!ourside){
		input[0] *= -1.0;
	}
	mSensitivityNet->Run(input, output);

    return output[0];
}

