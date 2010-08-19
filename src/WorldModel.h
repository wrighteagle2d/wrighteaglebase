#ifndef WORLDMODEL_H_
#define WORLDMODEL_H_

class Observer;
class WorldState;
class HistoryState;

/**
 * WorldModel 里面存放两对 WorldState，一对用于队友的决策，另一对用于对手的决策
 */
class WorldModel {
	WorldModel(WorldModel &);
public:
	WorldModel();
	virtual ~WorldModel();

	void Update(Observer *observer);

	const WorldState & GetWorldState(bool reverse) const;
	WorldState       & World(bool reverse);

private:
	WorldState *mpWorldState[2];
	HistoryState *mpHistoryState[2];
};

#endif /* WORLDMODEL_H_ */
