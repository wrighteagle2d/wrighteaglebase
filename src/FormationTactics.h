/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2016                                                    *
 * Copyright (c) 1998-2016 WrightEagle 2D Soccer Simulation Team,                   *
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
