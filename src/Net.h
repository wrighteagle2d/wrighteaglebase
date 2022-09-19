/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D) * BASE SOURCE CODE RELEASE 2016 *
 * Copyright (c) 1998-2016 WrightEagle 2D Soccer Simulation Team, * Multi-Agent
 *Systems Lab.,                                * School of Computer Science and
 *Technology,               * University of Science and Technology of China *
 * All rights reserved. *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without *
 * modification, are permitted provided that the following conditions are met: *
 *     * Redistributions of source code must retain the above copyright *
 *       notice, this list of conditions and the following disclaimer. *
 *     * Redistributions in binary form must reproduce the above copyright *
 *       notice, this list of conditions and the following disclaimer in the *
 *       documentation and/or other materials provided with the distribution. *
 *     * Neither the name of the WrightEagle 2D Soccer Simulation Team nor the *
 *       names of its contributors may be used to endorse or promote products *
 *       derived from this software without specific prior written permission. *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED    * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *PURPOSE ARE           * DISCLAIMED. IN NO EVENT SHALL WrightEagle 2D Soccer
 *Simulation Team BE LIABLE    * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *EXEMPLARY, OR CONSEQUENTIAL       * DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *PROCUREMENT OF SUBSTITUTE GOODS OR       * SERVICES; LOSS OF USE, DATA, OR
 *PROFITS; OR BUSINESS INTERRUPTION) HOWEVER       * CAUSED AND ON ANY THEORY OF
 *LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,    * OR TORT (INCLUDING
 *NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF * THIS SOFTWARE,
 *EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                *
 ************************************************************************************/

#ifndef BPN_H_
#define BPN_H_

typedef double real;

class Net {
  int mLayers; /// number of layers(including input layer)
  int *mUnits; /// the number of units of each layer

  real ***mWeight;      /// weight of each conjuction between units
  real ***mDeltaWeight; /// delta weight of each conjuction between units
  real **mDelta;        /// delta value of each unit
  real **mOutput;       /// output value of each unit
  real *mDesire;        /// desired output value of output layer

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

  void
  Run(real *input,
      real *output); /// calc outout of input and return sum of square error
  real Train(real *input,
             real *desire); /// train network with sample <Input, Desire>
  void TrainOnFile(const char *fname);
  real TestOnFile(const char *fname);
  real Error();

  void SetLearningRate(real rate);
  void SetAlpha(real a);
  void SetDesiredError(real d);
  void SetLogName(const char *);
  void SetMaxEpochs(int);

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
