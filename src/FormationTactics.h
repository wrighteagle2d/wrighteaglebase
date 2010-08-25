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

#ifndef FORMATIONTACTIC_H_
#define FORMATIONTACTIC_H_

#include "Geometry.h"
#include <vector>
#include <string>
#include <map>
#include <utility>


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
