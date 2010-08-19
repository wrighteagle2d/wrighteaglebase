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
