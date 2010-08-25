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

#include <cstring>
#include <math.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "Net.h"
#include "Types.h"
#include "Utilities.h"

#define BINARY

#define SCAN_PARAMS(p, i, d) if (fscanf(fp, "%d %d %d", &p, &i, &d) < 1) { Assert(0); }
#define SCAN_TRAIN(patterns, input_size, desire_size, input, desire) \
	for (int i = 0; i < patterns; ++i){ \
		for (int j = 0; j < input_size; ++j){ \
        if (fscanf(fp, "%lf", &input[i][j]) < 1) { Assert(0); } \
		} \
		for (int j = 0; j < desire_size; ++j){ \
        if (fscanf(fp, "%lf", &desire[i][j]) < 1) { Assert(0); } \
		} \
	}

Net::Net(int layers, int *units)
{
	mUnits = 0;
	mLogName = 0;
	Construct(layers, units);
}

Net::Net(const char *fname)
{
	mUnits = 0;
	mLogName = 0;
	Construct(fname);
}

Net::~Net()
{
	Destroy();
}

void Net::Construct(int layers, int *units)
{
	if (layers == 0)
		return;
	mLayers = layers;
	mUnits = new int[mLayers];
	if (units != 0){
		for (int i = 0; i < mLayers; ++i){
			mUnits[i] = units[i];
		}
	}

	Memaloc();
	SetDefaultValue();
}

void Net::Memaloc(){
	mWeight = new real**[mLayers];
	mDeltaWeight = new real**[mLayers];
	for (int i = 1; i < mLayers; ++i){
		mWeight[i] = new real*[mUnits[i]];
		mDeltaWeight[i] = new real*[mUnits[i]];
		for (int j = 0; j < mUnits[i]; ++j){
			mWeight[i][j] = new real[mUnits[i-1] + 1]; //one bias
			mDeltaWeight[i][j] = new real[mUnits[i-1] + 1]; //one bias
		}
	}

	mOutput = new real*[mLayers];
	mDelta = new real*[mLayers];
	for (int i = 1; i < mLayers; ++i){
		mOutput[i] = new real[mUnits[i]];
		mDelta[i] = new real[mUnits[i]];
	}
}

void Net::InitWeight()
{
	for (int i = 1; i < mLayers; ++i){
		for (int j = 0; j < mUnits[i]; ++j){
			for (int k = 0; k < mUnits[i-1]+1; ++k){
				mWeight[i][j][k] = small_rand();
				mDeltaWeight[i][j][k] = 0.0;
			}
		}
	}
}

void Net::Destroy()
{
	if (mUnits == 0)
		return;
	for (int i = 1; i < mLayers; ++i){
		for (int j = 0; j < mUnits[i]; ++j){
			delete[] mWeight[i][j];
			delete[] mDeltaWeight[i][j];
		}
		delete[] mWeight[i];
		delete[] mDeltaWeight[i];
	}
	delete[] mWeight;
	delete[] mDeltaWeight;

	for (int i = 1; i < mLayers; ++i){
		delete[] mOutput[i];
		delete[] mDelta[i];
	}
	delete[] mOutput;
	delete[] mDelta;

	delete[] mUnits;
	if (mLogName != 0)
		delete[] mLogName;
}

void Net::Save(const char *fname)
{
	if (mUnits == 0)
		return;
	FILE *fp;
	if ((fp = fopen(fname, "w")) == 0){
		perror("BPN::Save(char *fname)");
		exit(1);
	}
#ifdef BINARY
	fwrite(&mLayers, sizeof(mLayers), 1, fp);
	fwrite(mUnits, sizeof(*mUnits), mLayers, fp);
	fwrite(&mEta, sizeof(mEta), 1, fp);
	fwrite(&mAlpha, sizeof(mAlpha), 1, fp);
	for (int i = 1; i < mLayers; ++i){
		for (int j = 0; j < mUnits[i]; ++j){
			fwrite(mWeight[i][j], sizeof(*mWeight[i][j]), mUnits[i-1]+1, fp);
		}
	}
#else
	fprintf(fp, "#mLayers\n%d\n#mUnits\n", mLayers);
	for (int i = 0; i < mLayers; ++i){
		fprintf(fp, "%d ", mUnits[i]);
	}
	fprintf(fp, "\n#mEta\n%lf\n#mAlpha\n%lf\n#mWeight\n", mEta, mAlpha);
	for (int i = 1; i < mLayers; ++i){
		fprintf(fp, "#Layer %d\n", i);
		for (int j = 0; j < mUnits[i]; ++j){
			for (int k = 0; k < mUnits[i-1]+1; ++k){
				fprintf(fp, "%lf ", mWeight[i][j][k]);
			}
			fprintf(fp, "\n");
		}
		fprintf(fp, "\n");
	}
#endif
	fclose(fp);
}

void Net::Construct(const char *fname)
{
	FILE *fp;
	if ((fp = fopen(fname, "r")) == 0){
		perror("BPN::Construct(char *fname)");
		exit(1);
	}
#ifdef BINARY
	if (fread(&mLayers, sizeof(mLayers), 1, fp) < 1)
    {
        Assert(0);
    }
	mUnits = new int[mLayers];
	if (fread(mUnits, sizeof(*mUnits), mLayers, fp) < 1)
    {
        Assert(0);
    }
    if (fread(&mEta, sizeof(mEta), 1, fp) < 1)
    {
        Assert(0);
    }
    if (fread(&mAlpha, sizeof(mAlpha), 1, fp) < 1)
    {
        Assert(0);
    }
	Memaloc();
	InitWeight();

	for (int i = 1; i < mLayers; ++i){
		for (int j = 0; j < mUnits[i]; ++j){
			if (fread(mWeight[i][j], sizeof(*mWeight[i][j]), mUnits[i-1]+1, fp) < 1)
            {
                Assert(0);
            }
		}
	}
#else
	fscanf(fp, "#mLayers\n%d\n#mUnits\n", &mLayers);
	mUnits = new int[mLayers];
	for (int i = 0; i < mLayers; ++i){
		fscanf(fp, "%d ", &mUnits[i]);
	}
	fscanf(fp, "\n#mEta\n%lf\n#mAlpha\n%lf\n#mWeight\n", &mEta, &mAlpha);
	int tmp;
	Memaloc();
	InitWeight();

	for (int i = 1; i < mLayers; ++i){
		fscanf(fp, "#Layer %d\n", &tmp);
		for (int j = 0; j < mUnits[i]; ++j){
			for (int k = 0; k < mUnits[i-1]+1; ++k){
				fscanf(fp, "%lf ", &mWeight[i][j][k]);
			}
			fscanf(fp, "\n");
		}
		fscanf(fp, "\n");
	}
#endif
	fclose(fp);
}

inline real Net::sigmoid(real s)
{
	return 1.0/(1.0 + exp(-s));
}

inline real Net::small_rand() //[-1.0, 1.0]
{
	return drand(-1.0, 1.0);
}

inline void Net::FeedForward()
{
	for (int i = 1; i < mLayers; ++i){
		for (int j = 0; j < mUnits[i]; ++j){
			mOutput[i][j] = mWeight[i][j][mUnits[i-1]];              //bias
			for (int k = 0; k < mUnits[i-1]; ++k){
				mOutput[i][j] += mOutput[i-1][k] * mWeight[i][j][k];
			}
			mOutput[i][j] = sigmoid(mOutput[i][j]);
		}
	}
}

inline void Net::BackProp()
{
	for (int i = 0; i < mUnits[mLayers-1]; ++i){
		mDelta[mLayers-1][i] = (mDesire[i] - mOutput[mLayers-1][i]) * mOutput[mLayers-1][i] * (1.0 - mOutput[mLayers-1][i]);
	}

	for (int i = mLayers-2; i >= 1; --i){
		for (int j = 0; j < mUnits[i]; ++j){
			mDelta[i][j] = 0.0;
			for (int k = 0; k < mUnits[i+1]; ++k){
				mDelta[i][j] += mDelta[i+1][k] * mWeight[i+1][k][j];
			}
			mDelta[i][j] *= mOutput[i][j] * (1.0 - mOutput[i][j]);
		}
	}
}

inline void Net::UpdateWeight()
{
	for (int i = 1; i < mLayers; ++i){
		for (int j = 0; j < mUnits[i]; ++j){
			for (int k = 0; k < mUnits[i-1]; ++k){
				mDeltaWeight[i][j][k] = mEta * mDelta[i][j] * mOutput[i-1][k] + mAlpha * mDeltaWeight[i][j][k];
				mWeight[i][j][k] += mDeltaWeight[i][j][k];
			}
			mDeltaWeight[i][j][mUnits[i-1]] = mEta * mDelta[i][j] + mAlpha * mDeltaWeight[i][j][mUnits[i-1]];
			mWeight[i][j][mUnits[i-1]] += mDeltaWeight[i][j][mUnits[i-1]];
		}
	}
}

inline void Net::SetInput(real *input)
{
	mOutput[0] = input;
}

inline void Net::SetDesire(real *desire)
{
	mDesire = desire;
}

void Net::SetLearningRate(real rate)
{
	mEta = rate;
}

void Net::SetAlpha(real a)
{
	mAlpha = a;
}

void Net::SetDesiredError(real d)
{
	mDesiredError = d;
}

void Net::SetLogName(const char *s)
{
	mLogName = new char[strlen(s)+1];
	strcpy(mLogName, s);
}

void Net::SetMaxEpochs(int m)
{
	mMaxEpochs = m;
}

void Net::SetDefaultValue()
{
	SetLearningRate(0.7);
	SetAlpha(0.0);
	SetDesiredError(0.01);
	SetMaxEpochs(1000);
	InitWeight();
}

void Net::Run(real *input, real *output)
{
	if (mUnits == 0)
		return;
	SetInput(input);
	FeedForward();
	if (output != 0){
		for (int i = 0; i < mUnits[mLayers-1]; ++i){
			output[i] = mOutput[mLayers-1][i];
		}
	}
}

real Net::Error()
{
	real error = 0.0;
	for (int i = 0; i < mUnits[mLayers-1]; ++i){
		error += (mDesire[i] - mOutput[mLayers-1][i]) * (mDesire[i] - mOutput[mLayers-1][i]);
	}
	return error;
}

real Net::Train(real *input, real *desire)
{
	if (mUnits == 0)
		return -1.0;
	SetInput(input);
	SetDesire(desire);
	FeedForward();
	BackProp();
	UpdateWeight();

	return Error();
}

void Net::TrainOnFile(const char *fname)
{
	if (mUnits == 0)
		return;
	FILE *fp;
	if ((fp = fopen(fname, "r")) == 0){
		perror("BPN::TrainOnFile(char *fname)");
		exit(1);
	}
	int patterns;
	int input_size;
	int desire_size;
	real **input;
	real **desire;

	SCAN_PARAMS(patterns, input_size, desire_size)

	input = new real*[patterns];
	desire = new real*[patterns];
	for (int i = 0; i < patterns; ++i){
		input[i] = new real[input_size];
		desire[i] = new real[desire_size];
	}

	SCAN_TRAIN(patterns, input_size, desire_size, input, desire)

	FILE *log_file = 0;
	if (mLogName != 0){
		log_file = fopen(mLogName, "w");
		if (log_file == 0){
			perror("fopen(mLogName)");
			exit(1);
		}
	}

	int epochs = 0;
	while(epochs <= mMaxEpochs){
		++epochs;
		real error = 0.0;
		for (int p = 0; p < patterns; ++p){
			error += Train(input[p], desire[p]);
		}
		if (mLogName){
			fprintf(log_file, "%d %f\n", epochs, error);
		}
		if (error < mDesiredError){
			break;
		}
	}
	if (mLogName != 0){
		fclose(log_file);
	}

	fclose(fp);
	for (int i = 0; i < patterns; ++i){
		delete[] input[i];
		delete[] desire[i];
	}
	delete[] input;
	delete[] desire;
}

real Net::TestOnFile(const char *fname)
{
	if (mUnits == 0)
		return -1.0;
	FILE *fp;
	if ((fp = fopen(fname, "r")) == 0){
		perror("BPN::TrainOnFile(char *fname)");
		exit(1);
	}
	int patterns;
	int input_size;
	int desire_size;
	real **input;
	real **desire;

	SCAN_PARAMS(patterns, input_size, desire_size)

	input = new real*[patterns];
	desire = new real*[patterns];
	for (int i = 0; i < patterns; ++i){
		input[i] = new real[input_size];
		desire[i] = new real[desire_size];
	}

	SCAN_TRAIN(patterns, input_size, desire_size, input, desire)

	real error = 0.0;
	for (int p = 0; p < patterns; ++p){
		SetDesire(desire[p]);
		Run(input[p], 0);
		error += Error();
	}

	for (int i = 0; i < patterns; ++i){
		delete[] input[i];
		delete[] desire[i];
	}
	delete[] input;
	delete[] desire;

	return error;
}
