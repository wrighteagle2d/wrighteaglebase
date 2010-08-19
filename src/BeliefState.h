#ifndef BELIEFSTATE_H_
#define BELIEFSTATE_H_

#include <map>
#include <set>
#include <vector>
#include "Geometry.h"
#include "ServerParam.h"
#include "PlayerParam.h"

class Agent;
class WorldState;

/**
* 假设人和球都是随机游动情况下的，关于人和球位置的信念空间
*/
class BeliefState {
public:
	/**
	* 将全场近似划分成1x1的格子
	*/
	static const unsigned int XSIZE = 100;
	static const unsigned int YSIZE = 60;

	static double XSTEP;
	static double YSTEP;

	class Field;

	/**
	* 单独记录球和每个球员出现的格子
	*/
	class Grid {
		/**
		* 本格子内出现某个球或人的概率
		*/
		ObjectArray<double> mAppearanceMap;

	public:
		Grid() {
			mAppearanceMap.fill(0.0);
		}

		const double & GetAppearProb(const ObjectIndex & i) const {
			return mAppearanceMap[i];
		}

		double & AppearProb(const ObjectIndex & i) {
			return mAppearanceMap[i];
		}

		/**
		* 格子的编号
		*/
		int mX, mY;

		/**
		* 格子的全局位置
		*/
		Vector mPos;

		Rectangular GetRectangular() const {
			return Rectangular(mPos.X() - 0.5 * XSTEP, mPos.X() + 0.5 * XSTEP, mPos.Y() - 0.5 * YSTEP, mPos.Y() + 0.5 * YSTEP);
		}

		void Appear(const Unum & i, const double & poss) {
			mAppearanceMap[i] += poss;
		}

		double GetAppearPossAll() {
			double poss = 0.0;

			poss += mAppearanceMap[0];
			for (Unum i = 1; i <= TEAMSIZE; ++i) {
				poss += mAppearanceMap[i] + mAppearanceMap[-i];
			}

			return poss;
		}

		void Spread(const Unum & i, BeliefState::Field & field)
		{
			RandomWalk(PlayerParam::instance().minAppearancePoss(), mX, mY, i, mAppearanceMap[i])(field);
		}

		void Erase(const Unum & i) {
			mAppearanceMap[i] = 0.0;
		}

		bool IsAppearPlayer() {
			for (Unum i = 1; i <= TEAMSIZE; ++i) {
				if (mAppearanceMap[i] > 0.0 || mAppearanceMap[-i] > 0.0) return true;
			}
			return false;
		}

		bool IsAppearBall() {
			return mAppearanceMap[0] > 0.0;
		}

		bool IsAppearSomething() {
			return IsAppearBall() || IsAppearPlayer();
		}

		/**
		* 随机游动模型
		*/
		class RandomWalk {
			const double mPossThr;

			const int mX;
			const int mY;

			const Unum mUnum;
			const double mPoss;

		public:
			RandomWalk(const double & poss_thr, const int & x, const int & y, const Unum & i, const double & poss):
				mPossThr ( poss_thr ),
				mX ( x ),
				mY ( y ),
				mUnum ( i ),
				mPoss ( poss )
			{
				Assert(poss > 0.0);
			}

			void operator()(BeliefState::Field & field)
			{
				if (mPoss < mPossThr) { //不扩散
					field.AppearAt(mUnum, mX, mY, mPoss);
					return;
				}

				int s = (mUnum == 0? 3: 1); //球速最大为3,人为1

				const int x_low  = mX - s;
				const int x_high = mX + s;
				const int y_low  = mY - s;
				const int y_high = mY + s;

				int spread_count = 0;
				std::list<std::pair<int, int> > spreads;

				for (int x = x_low; x <= x_high; ++x) {
					for (int y = y_low; y <= y_high; ++y) {
						if ((unsigned)x < XSIZE && (unsigned)y < YSIZE) {
							spread_count += 1;
							spreads.push_back( std::pair<int, int>(x, y) );
						}
					}
				}

				if (spread_count > 0) {
					double spread_poss = mPoss / spread_count;

					for (std::list<std::pair<int, int> >::iterator it = spreads.begin(); it != spreads.end(); ++it) {
						field.AppearAt(mUnum, it->first, it->second, spread_poss);
					}
				}
			}
		};
	};

	BeliefState();
	~BeliefState();

	void Maintain(WorldState & world_state, const Unum & self_unum, const bool & is_new_sight); //维护得到当前的信念状态
	void Spread(Agent & agent); //扩散一周期后

	void Show(const Time & time);

private:
	void InitField();
	void Spread(const Vector & ball_pos, const Vector & ball_vel, const bool & consider_kick);

public:
	class Field {
		friend class BeliefState;

		ObjectArray<std::list<Grid*> > mAppearanceSet;
		Array<Array<Grid, YSIZE>,  XSIZE> mGrids;

	public:
		Field () {
			Init();
		}

		void Init() {
			XSTEP = ServerParam::PITCH_LENGTH / XSIZE;
			YSTEP = ServerParam::PITCH_WIDTH / YSIZE;

			for (unsigned int x = 0; x < XSIZE; ++x) {
				for (unsigned int y = 0; y < YSIZE; ++y) {
					Grid * const grid = & mGrids[x][y];

					grid->mX = x;
					grid->mY = y;
					grid->mPos.SetX(ToX(x));
					grid->mPos.SetY(ToY(y));

					Assert(ToXIdx(ToX(x) - FLOAT_EPS) == int(x));
					Assert(ToYIdx(ToY(y) - FLOAT_EPS) == int(y));
					Assert(ToXIdx(ToX(x) + FLOAT_EPS) == int(x));
					Assert(ToYIdx(ToY(y) + FLOAT_EPS) == int(y));
				}
			}
		}

		const Grid & GetGrid(const Vector & pos) const {
			return mGrids[ToXIdx(pos.X())][ToYIdx(pos.Y())];
		}

		std::list<Grid*> & AppearanceSet(const ObjectIndex & i)
		{
			return mAppearanceSet[i];
		}

		const std::list<Grid*> & GetAppearanceSet(const ObjectIndex & i) const
		{
			return mAppearanceSet[i];
		}

		void AppearAt(const Unum & i, const int & x, const int & y, const double & prob) {
			Grid * const grid = & mGrids[x][y];

			if (grid->GetAppearProb(i) < FLOAT_EPS) {
				AppearanceSet(i).push_back(grid);
			}

			grid->Appear(i, prob);
		}

		bool Kickable(Grid *grid) {
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					int x = grid->mX + dx;
					int y = grid->mY + dy;
					if ((unsigned)x < XSIZE && (unsigned)y < YSIZE) {
						if (mGrids[x][y].IsAppearPlayer()) {
							return true;
						}
					}
				}
			}

			return false;
		}

		/**
		 * 按照给定的模型进行扩散，扩散结果保存在result_field，扩散完成后，清空自己
		 * @param ball_vel
		 * @param result_field
		 */
		void Spread(const Vector & ball_pos, const Vector & ball_vel, const bool & consider_kick, Field & result_field)
		{
			const int ball_set_size = mAppearanceSet[0].size();

			for (std::list<Grid*>::iterator it = mAppearanceSet[0].begin(); it != mAppearanceSet[0].end(); ++it) {
				Assert((*it)->GetAppearProb(0) > 0.0);
				if (consider_kick && Kickable(*it)) { //有人可以踢到球
					(*it)->Spread(0, result_field);
				}
				else { //球自由运动
					double prob = (*it)->GetAppearProb(0);

					if (ball_set_size == 1) {
						Vector expected_pos = ball_pos + ball_vel;

						int expected_x = ToXIdx(expected_pos.X());
						int expected_y = ToYIdx(expected_pos.Y());

						prob *= 0.5;
						for (int dx = -1; dx <= 1; ++dx) {
							for (int dy = -1; dy <= 1; ++dy) {
								if (dx != 0 || dy != 0) {
									int x = MinMax(0, expected_x + dx, int(XSIZE) - 1);
									int y = MinMax(0, expected_y + dy, int(YSIZE) - 1);
									result_field.AppearAt(0, x, y, prob / 8.0);
								}
							}
						}

						result_field.AppearAt(0, expected_x, expected_y, prob);
					}
					else {
						Vector expected_pos = (*it)->mPos + ball_vel;

						int expected_x = ToXIdx(expected_pos.X());
						int expected_y = ToYIdx(expected_pos.Y());

						if (ball_set_size <= 9) { //特殊处理，增加随机性
							for (int dx = -1; dx <= 1; ++dx) {
								for (int dy = -1; dy <= 1; ++dy) {
									int x = MinMax(0, expected_x + dx, int(XSIZE) - 1);
									int y = MinMax(0, expected_y + dy, int(YSIZE) - 1);
									result_field.AppearAt(0, x, y, prob / 9.0);
								}
							}
						}
						else {
							result_field.AppearAt(0, expected_x, expected_y, prob);
						}
					}
				}
				(*it)->Erase(0); //清除老的field
			}
			mAppearanceSet[0].clear();

			for (Unum i = 1; i <= TEAMSIZE; ++i) {
				for (std::list<Grid*>::iterator it = mAppearanceSet[i].begin(); it != mAppearanceSet[i].end(); ++it) {
					Assert((*it)->GetAppearProb(i) > 0.0);
					(*it)->Spread(i, result_field);
					(*it)->Erase(i); //清除老的field
				}
				mAppearanceSet[i].clear();

				for (std::list<Grid*>::iterator it = mAppearanceSet[-i].begin(); it != mAppearanceSet[-i].end(); ++it) {
					Assert((*it)->GetAppearProb(-i) > 0.0);
					(*it)->Spread(-i, result_field);
					(*it)->Erase(-i); //清除老的field
				}
				mAppearanceSet[-i].clear();
			}
		}

		/**
		 *  视觉范围之内清空
		 * @param pos
		 * @param left
		 * @param right
		 */
		void Clean(const Vector & pos, const AngleDeg & left, const AngleDeg & right, const Unum & i)
		{
			static const double radius = Sqrt(Sqr(XSTEP * 0.5) + Sqr(YSTEP * 0.5)) * 0.9;

			std::list<BeliefState::Grid*> & set = AppearanceSet(i);
			std::list<BeliefState::Grid*>::iterator it = set.begin();
			bool adjust = false;

			while (it != set.end()) {
				const Vector rel_pos = ((*it)->mPos - pos);
				const double dist = rel_pos.Mod();
				const AngleDeg dir_buffer = ASin(radius / dist);

				if (dist < ServerParam::instance().visibleDistance() - radius || IsAngleDegInBetween(left + dir_buffer, rel_pos.Dir(), right - dir_buffer)) {
					Assert((*it)->GetAppearProb(i) > 0.0);
					(*it)->Erase(i);
					it = set.erase(it);
					adjust = true;
					continue;
				}
				++it;
			}

			if (adjust) {
				AdjustProb(i);
			}
		}

		/**
		 * 调整概率，使总和为一
		 * 这里 AppearanceSet(i) 是同步的
		 * @param i
		 */
		void AdjustProb(Unum i) {
			const std::list<Grid*> & set = GetAppearanceSet(i);

			if (!set.empty()) {
				double sum_prob = 0.0;

				for (std::list<Grid*>::const_iterator it = set.begin(); it != set.end(); ++it) {
					Assert((*it)->GetAppearProb(i) > 0.0);
					sum_prob += (*it)->GetAppearProb(i);
				}

				double plus_prob = (1.0 - sum_prob) / set.size();
				if (plus_prob > 0.0) {
					for (std::list<Grid*>::const_iterator it = set.begin(); it != set.end(); ++it) {
						(*it)->AppearProb(i) += plus_prob;
					}
				}
				else {
					Assert(0);
				}
			}
		}

		/**
		 * 清除 i 的所有出现
		 * @param i
		 */
		void ClearAppearance(Unum i){
		    std::list<Grid*> & set = AppearanceSet(i);

		    for (std::list<Grid*>::iterator it = set.begin(); it != set.end(); ++it) {
		        (*it)->Erase(i);
		    }
		    set.clear();
		}

		void Dump(std::ostream & out) {
			double poss, sum_poss = 0.0;
			double ball_poss = 0.0;
			out << std::endl;
			out << std::endl;

			for (unsigned int y = YSIZE - 1; y > 0; --y) {
				for (unsigned int x = 0; x < XSIZE; ++x) {
					Grid *grid = & mGrids[x][y];
					poss = grid->GetAppearPossAll();
					sum_poss += poss;
					ball_poss += grid->GetAppearProb(0);
					out << poss << ' ';
				}
				out << std::endl;
			}
			out << "# sum_poss " << sum_poss << std::endl;
			out << "# ball_poss " << ball_poss << std::endl;
		}

		void Log(Unum i); //在sightlog上标记球员i的信念状态
	};

public:
	const std::list<Grid*> & GetAppearanceSet(Unum i) const {
		return mField[mCurField].GetAppearanceSet(i);
	}

	Vector GetExpectedPos(Unum i) const;

	const Grid & GetGrid(const Vector & pos) const {
		return mField[mCurField].GetGrid(pos);
	}

	static int ToXIdx(const double & x) { //返回x所在格子的x方向编号
		return MinMax(0, int((x + ServerParam::PITCH_LENGTH * 0.5) / XSTEP), int(XSIZE) - 1);
	}

	static int ToYIdx(const double & y) { //返回y所在格子的y方向编号
		return MinMax(0, int((y + ServerParam::PITCH_WIDTH * 0.5) / YSTEP), int(YSIZE) - 1);
	}

	static double ToX(int x) {
		return x * XSTEP + 0.5 * XSTEP - ServerParam::PITCH_LENGTH * 0.5;
	}

	static double ToY(int y) {
		return y * YSTEP + 0.5 * YSTEP - ServerParam::PITCH_WIDTH * 0.5;
	}

public:
	int  mCurField;
	Array<Field, 2> mField;
};

#endif /* BELIEFSTATE_H_ */
