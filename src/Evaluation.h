#ifndef __Evaluation_H__
#define __Evaluation_H__

#include "Geometry.h"
class Net;

class Evaluation
{
	Evaluation();

public:
	~Evaluation();

	static Evaluation & instance();

	double EvaluatePosition(const Vector & pos, bool ourside);

private:
	Net *mSensitivityNet;
};

#endif

