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

#include "BehaviorFormation.h"
#include "VisualSystem.h"
#include "Formation.h"
#include "Dasher.h"
#include "BasicCommand.h"
#include "BehaviorPosition.h"
#include "Agent.h"
#include "PositionInfo.h"
#include "Logger.h"
#include "Evaluation.h"

const BehaviorType BehaviorFormationExecuter::BEHAVIOR_TYPE = BT_Formation;

namespace
{
bool ret = BehaviorExecutable::AutoRegister<BehaviorFormationExecuter>();
}

BehaviorFormationExecuter::BehaviorFormationExecuter(Agent & agent) :
	BehaviorExecuterBase<BehaviorDefenseData>(agent)
{
	Assert(ret);
}

BehaviorFormationExecuter::~BehaviorFormationExecuter(void)
{
}

bool BehaviorFormationExecuter::Execute(const ActiveBehavior & beh)
{
	Logger::instance().LogGoToPoint(mSelfState.GetPos(), beh.mTarget, "@Formation");

	return Dasher::instance().GoToPoint(mAgent, beh.mTarget, beh.mBuffer, beh.mPower, false, true);
}

BehaviorFormationPlanner::BehaviorFormationPlanner(Agent & agent):
	BehaviorPlannerBase<BehaviorDefenseData>( agent)
{
}

BehaviorFormationPlanner::~BehaviorFormationPlanner()
{
}

void BehaviorFormationPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	ActiveBehavior formation(mAgent, BT_Formation);

	formation.mBuffer = 1.0;
	formation.mPower = mSelfState.CorrectDashPowerForStamina(ServerParam::instance().maxDashPower());
	formation.mTarget = mAnalyser.mHome[mSelfState.GetUnum()];

	if (mWorldState.GetPlayMode() > PM_Opp_Mode) {
		while (formation.mTarget.Dist(mBallState.GetPos()) < ServerParam::instance().offsideKickMargin() + 0.5) {
			formation.mTarget.SetX(formation.mTarget.X() - 0.5);
		}

		if (mWorldState.GetPlayMode() == PM_Opp_Offside_Kick) {
			formation.mTarget.SetX(Min(formation.mTarget.X(), mBallState.GetPos().X()));
		}
		else if (mWorldState.GetPlayMode() == PM_Opp_Goal_Kick) {
			if (ServerParam::instance().oppPenaltyArea().IsWithin(formation.mTarget)) {
				if (mSelfState.GetPos().X() < ServerParam::instance().oppPenaltyArea().Left()) {
					if (mSelfState.GetPos().Y() < ServerParam::instance().oppPenaltyArea().Top()) {
						formation.mTarget = ServerParam::instance().oppPenaltyArea().TopLeftCorner();
					}
					else if (mSelfState.GetPos().Y() > ServerParam::instance().oppPenaltyArea().Bottom()) {
						formation.mTarget = ServerParam::instance().oppPenaltyArea().BottomLeftCorner();
					}
					else {
						formation.mTarget.SetX(Min(formation.mTarget.X(), ServerParam::instance().oppPenaltyArea().Left() - 0.5));
					}
				}
				else {
					if (mSelfState.GetPos().Y() < ServerParam::instance().oppPenaltyArea().Top()) {
						formation.mTarget.SetY(Max(formation.mTarget.Y(), ServerParam::instance().oppPenaltyArea().Top() - 0.5));
					}
					else if (mSelfState.GetPos().Y() > ServerParam::instance().oppPenaltyArea().Bottom()) {
						formation.mTarget.SetY(Min(formation.mTarget.Y(), ServerParam::instance().oppPenaltyArea().Bottom() + 0.5));
					}
					else {
						formation.mTarget = mSelfState.GetPos(); //do nothing
					}
				}
			}
		}
	}

	formation.mEvaluation = Evaluation::instance().EvaluatePosition(formation.mTarget, false);

	behavior_list.push_back(formation);
}

