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

#ifndef BPN_H_
#define BPN_H_

typedef double real;

class Net {
	int mLayers;                   ///number of layers(including input layer)
	int  *mUnits;                  ///the number of units of each layer

	real ***mWeight;               ///weight of each conjuction between units
	real ***mDeltaWeight;          ///delta weight of each conjuction between units
	real **mDelta;                 ///delta value of each unit
	real **mOutput;                ///output value of each unit
	real *mDesire;                 ///desired output value of output layer

	real mEta;
	real mAlpha;
	real mDesiredError;
	int mMaxEpochs;

	char *mLogName;
public:
	Net(int Layers = 0, int *Units = 0);
	Net(const char *fname);
	~Net();

	void Construct(int layers, int *units);
	void Construct(const char *fname);
	void Destroy();
	void Save(const char *fname);

	void Run(real *input, real *output);               ///calc outout of input and return sum of square error
	real Train(real *input, real *desire);             ///train network with sample <Input, Desire>
	void TrainOnFile(const char *fname);
	real TestOnFile(const char *fname);
	real Error();

	void SetLearningRate(real rate);
	void SetAlpha(real a);
	void SetDesiredError(real d);
	void SetLogName(const char *);
	void SetMaxEpochs(int );
private:
	Net(const Net &);
	void Memaloc();
	void SetDefaultValue();

	void SetInput(real *input);
	void SetDesire(real *desire);
	void InitWeight();
	void FeedForward();
	void BackProp();
	void UpdateWeight();

	real sigmoid(real s);
	real small_rand();
};

#endif
