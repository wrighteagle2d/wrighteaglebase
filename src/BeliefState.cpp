#include "BeliefState.h"
#include "WorldState.h"
#include "Agent.h"
#include "VisualSystem.h"
#include "TimeTest.h"
#include "Logger.h"
#include <fstream>
#include <sstream>

double BeliefState::XSTEP = ServerParam::PITCH_LENGTH / XSIZE;
double BeliefState::YSTEP = ServerParam::PITCH_WIDTH / YSIZE;

BeliefState::BeliefState(): mCurField(0)
{
	XSTEP = ServerParam::PITCH_LENGTH / XSIZE;
	YSTEP = ServerParam::PITCH_WIDTH / YSIZE;
}

BeliefState::~BeliefState()
{
}

void BeliefState::Maintain(WorldState & world_state, const Unum & self_unum, const bool & is_new_sight)
{
	//TIMETEST("BeliefState::Maintain");

	ObjectArray<int> delay;
	ObjectArray<bool, true> manual_update;
	ObjectArray<Vector> expected_pos;

	{
		if (world_state.GetBall().GetPosConf() > FLOAT_EPS) {
			delay[0] = world_state.GetBall().GetPosDelay();
			if (delay[0] <= 1) {
				manual_update[0] = true;
				expected_pos[0] = world_state.GetBall().GetPos();
			}
		}
		else {
			if (world_state.GetBall().GetPosDelay() > 15) {
				mField[mCurField].ClearAppearance(0); //drop it
			}
		}

		for (int i = -TEAMSIZE; i <= TEAMSIZE; ++i) {
			if (!i || i == self_unum) continue;

			if (world_state.GetPlayer(i).IsAlive()) {
				delay[i] = world_state.GetPlayer(i).GetPosDelay();
				if (delay[i] <= 1) {
					manual_update[i] = true;
					expected_pos[i] = world_state.GetPlayer(i).GetPos();
				}
			}
			else {
				if (world_state.GetPlayer(i).GetPosDelay() > 30) {
					mField[mCurField].ClearAppearance(i); //drop it
				}
			}
		}
	}

	const PlayerState & self_state = world_state.GetTeammate(self_unum);
	const ViewWidth view_width = self_state.GetViewWidth();
	const AngleDeg view_angle = sight::ViewAngle(view_width);
	const AngleDeg left = self_state.GetNeckGlobalDir() - view_angle * 0.5;
	const AngleDeg right = left + view_angle;
	const Vector self_pos = self_state.GetPos();

	/**
	* 1. 视觉区域内全部清空 -- 这里认为只要在视觉区域内肯定能识别出来（但目前的身份识别算法还有bug）
	*/
	if (is_new_sight) {
		for (int i = -TEAMSIZE; i <= TEAMSIZE; ++i) {
			if (!manual_update[i] && i != self_unum) {
				mField[mCurField].Clean(self_pos, left, right, i);
			}
		}
	}

	/**
	* 2. 看到的或听到的重置位置
	*/
	for (int i = -TEAMSIZE; i <= TEAMSIZE; ++i) {
		if (manual_update[i]) {
			mField[mCurField].ClearAppearance(i);

			const int expected_x = ToXIdx(expected_pos[i].X());
			const int expected_y = ToYIdx(expected_pos[i].Y());

			if (delay[i] == 0) {
				mField[mCurField].AppearAt(i, expected_x, expected_y, 1.0);
			}
			else {
				Assert(delay[i] == 1);

				int step = 1;
				if (!i) { //is ball
					Grid *grid = & mField[mCurField].mGrids[expected_x][expected_y];
					if (mField[mCurField].Kickable(grid)) {
						step = 3;
					}
				}

				const double prob = 1.0 / Sqr(2 * step + 1);

				for (int dx = -step; dx <= step; ++dx) {
					for (int dy = -step; dy <= step; ++dy) {
						int x = MinMax(0, expected_x + dx, int(XSIZE) - 1);
						int y = MinMax(0, expected_y + dy, int(YSIZE) - 1);
						mField[mCurField].AppearAt(i, x, y, prob);
					}
				}

				if (is_new_sight) {
					mField[mCurField].Clean(self_pos, left, right, i); //修正

					std::list<BeliefState::Grid*> & set = mField[mCurField].AppearanceSet(i);
					if (set.empty()) { //由于精度关系，有可能为空
						mField[mCurField].AppearAt(i, expected_x, expected_y, 1.0); //修正一下
					}
				}
			}
		}

		//mField[mCurField].Log(i);
	}
}

Vector BeliefState::GetExpectedPos(Unum i) const
{
	const std::list<Grid*> & set = GetAppearanceSet(i);
	Vector pos(0.0, 0.0);

	for (std::list<BeliefState::Grid*>::const_iterator it = set.begin(); it != set.end(); ++it) {
		pos += (*it)->mPos * (*it)->GetAppearProb(i);
	}

	return pos;
}

void BeliefState::Spread(Agent & agent)
{
	const WorldState & world_state = agent.GetWorldState();
	const Vector ball_pre_vel = agent.GetBallVelWithQueuedActions() / ServerParam::instance().ballDecay();

	bool consider_kick = false;
	if (
			world_state.GetPlayMode() == PM_Play_On
			|| world_state.GetPlayMode() == PM_Opp_Penalty_Taken
			|| world_state.GetPlayMode() == PM_Our_Penalty_Taken
		) {
		PositionInfo & position_info = agent.GetInfoState().GetPositionInfo();
		const Unum kickable_player = position_info.GetPlayerWithBall();
		consider_kick = kickable_player != 0 && kickable_player != agent.GetSelfUnum();
	}

	Spread(world_state.GetBall().GetPos(), ball_pre_vel, consider_kick);
}

void BeliefState::Spread(const Vector & ball_pos, const Vector & ball_vel, const bool & consider_kick)
{
	const int result_field = 1 - mCurField;

	mField[mCurField].Spread(ball_pos, ball_vel, consider_kick, mField[result_field]);

	mCurField = result_field;
}

void BeliefState::Field::Log(Unum i)
{
	if (PlayerParam::instance().SaveDecLog()) {
		const std::list<BeliefState::Grid*> & set = GetAppearanceSet(i);

		for (std::list<BeliefState::Grid*>::const_iterator it = set.begin(); it != set.end(); ++it) {
			SightLogger::Color color = SightLogger::White;

			const double prob = (*it)->GetAppearProb(i);
			if (prob > 0.1) {
				color = SightLogger::Purple;
			}
			else if (prob > 0.05) {
				color = SightLogger::Red;
			}
			else if (prob > 0.025) {
				color = SightLogger::Orange;
			}
			else if (prob > 0.0125) {
				color = SightLogger::Yellow;
			}
			else if (prob > 0.00625) {
				color = SightLogger::Cyan;
			}

			Logger::instance().LogRectangular((*it)->GetRectangular(), color);
		}
	}
}

void BeliefState::Show(const Time & time)
{
	std::ofstream file("pp");
	mField[mCurField].Dump(file);
	file.close();

	std::stringstream cmd;
	cmd << "set title \'" << time << "\'";

	Plotter::instance().GnuplotExecute(cmd.str().c_str());

	Plotter::instance().GnuplotExecute("set pm3d map");
	Plotter::instance().GnuplotExecute("set zrange [0.0:1.2]");
	Plotter::instance().GnuplotExecute("splot \"pp\" matrix");

	WaitFor(200);
}
