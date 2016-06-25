/*
 * BaseAlgorithm.cpp
 *
 *  Created on: Apr 17, 2013
 *      Author: shani
 */

#include "BaseAlgorithm.h"

void BaseAlgorithm::Init(bool testState)
{
	_inTest = testState;
	InitResult();
	if (_inTest)
	{
		SetDefaultParams();
		InitProcessData();
	}
	_offset = Point(0,0);
}

BaseAlgorithm::~BaseAlgorithm()
{
}

void BaseAlgorithm::MakeCopyAndRun(const Mat& image)
{
	try
	{
		Mat copy(image);
		image.copyTo(copy);
		Logs.clear();
		Run(copy);
		fixResults(_offset);
	} catch (std::exception& e)
	{
		cerr << e.what() << endl;
	}
}

void BaseAlgorithm::SetOffset(Point offset)
{
	_offset = offset;
}


