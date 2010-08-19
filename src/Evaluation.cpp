#include "Evaluation.h"
#include "ServerParam.h"
#include "Agent.h"
#include "PositionInfo.h"
#include "Strategy.h"
#include "Net.h"
#include "WorldState.h"
#include "InfoState.h"

Evaluation::Evaluation()
{
	mSensitivityNet = new Net("data/sensitivity.net");
}

Evaluation::~Evaluation()
{
	delete mSensitivityNet;
}

Evaluation & Evaluation::instance()
{
    static Evaluation evaluation;
    return evaluation;
}

double Evaluation::EvaluatePosition(const Vector & pos, bool ourside)
{
	static double input[2];
	static double output[1];

	input[0] = pos.X() / (ServerParam::instance().PITCH_LENGTH * 0.5);
	input[1] = fabs(pos.Y()) / (ServerParam::instance().PITCH_WIDTH * 0.5) * 2.0 - 1.0;
	if (!ourside){
		input[0] *= -1.0;
	}
	mSensitivityNet->Run(input, output);

    return output[0];
}

