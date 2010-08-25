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

#include "WorldModel.h"
#include "Observer.h"
#include "WorldState.h"
#include "InfoState.h"

WorldModel::WorldModel() {
	mpHistoryState[0] = new HistoryState;
	mpWorldState[0] = new WorldState(mpHistoryState[0]); //队友的世界状态

	mpHistoryState[1] = new HistoryState;
	mpWorldState[1] = new WorldState(mpHistoryState[1]); //供反算时用的，对手的世界状态
}

WorldModel::~WorldModel() {
	delete mpWorldState[0];
	delete mpHistoryState[0];

	delete mpWorldState[1];
	delete mpHistoryState[1];
}

void WorldModel::Update(Observer *observer)
{
	//存储一下当前的世界
	mpHistoryState[0]->UpdateHistory(*mpWorldState[0]);
	mpHistoryState[1]->UpdateHistory(*mpWorldState[1]);

	mpWorldState[0]->UpdateFromObserver(observer); //自己方决策使用的世界状态
	mpWorldState[1]->GetReverseFrom(mpWorldState[0]); //可供反算对手时使用的世界状态
}

const WorldState & WorldModel::GetWorldState(bool reverse) const
{
	return *(reverse? mpWorldState[1]: mpWorldState[0]);
}

WorldState & WorldModel::World(bool reverse)
{
	return *(reverse? mpWorldState[1]: mpWorldState[0]);
}

