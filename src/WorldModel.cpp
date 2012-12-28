/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2013                                                    *
 * Copyright (c) 1998-2013 WrightEagle 2D Soccer Simulation Team,                   *
 *                         Multi-Agent Systems Lab.,                                *
 *                         School of Computer Science and Technology,               *
 *                         University of Science and Technology of China            *
 * All rights reserved.                                                             *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without               *
 * modification, are permitted provided that the following conditions are met:      *
 *     * Redistributions of source code must retain the above copyright             *
 *       notice, this list of conditions and the following disclaimer.              *
 *     * Redistributions in binary form must reproduce the above copyright          *
 *       notice, this list of conditions and the following disclaimer in the        *
 *       documentation and/or other materials provided with the distribution.       *
 *     * Neither the name of the WrightEagle 2D Soccer Simulation Team nor the      *
 *       names of its contributors may be used to endorse or promote products       *
 *       derived from this software without specific prior written permission.      *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED    *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
 * DISCLAIMED. IN NO EVENT SHALL WrightEagle 2D Soccer Simulation Team BE LIABLE    *
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL       *
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR       *
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER       *
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,    *
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF *
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                *
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

