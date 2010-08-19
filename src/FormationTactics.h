#ifndef FORMATIONTACTIC_H_
#define FORMATIONTACTIC_H_

#include "Geometry.h"
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

enum FormationTacticType
{
    FTT_KickOffPosition = 0,

    FTT_MAX
};

struct RoleType;

class FormationTacticBase
{
public:
	FormationTacticBase();
	virtual ~FormationTacticBase() {}
	virtual void Initial(std::vector<std::string> & config, Unum * index2unum = 0, int * unum2index = 0);
	virtual FormationTacticBase * clone() = 0;

protected:
	static int IndexIsUnum[TEAMSIZE+1];

	Unum * Index2Unum;
	int *  Unum2Index;
};


class FormationTacticKickOffPosition : public FormationTacticBase
{
public:
	FormationTacticKickOffPosition();
	void Initial(std::vector<std::string> & config, Unum * index2unum = 0, int * unum2index = 0);
	Vector & operator()(Unum player ,bool is_our_kickoff );
	FormationTacticKickOffPosition * clone() { return new FormationTacticKickOffPosition(*this); }

private:
	Array<Array<Vector, 2>, TEAMSIZE> mKickOffPosition;
};

#endif /* FORMATIONTACTIC_H_ */
