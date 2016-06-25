/*
 * BaseAlgorithm.h
 *
 *  Created on: Apr 17, 2013
 *      Author: shani
 */

#ifndef BASEALGORITHM_H_
#define BASEALGORITHM_H_
#include <stdio.h>
#include <cv.h>
#include "MissionControlMessage.h"
using namespace std;
using namespace cv;

/*enum ImageInputType
{
	Regular, Hue
};*/

/*
 * each algorithm derive this class.
 * the algorithm must contain the current results of the algorithm.
 * when creating an algorithm you MUST INIT it.
 */
class BaseAlgorithm
{
	/** TO OVERRIDE **/
public:
	/* get a image and run the algorithm on it.
	 * the algorithm is allowed to change the image.
	 */
	virtual void Run(Mat& image) = 0;

	/* set the parameters of the algorithm.
	 * params is containing ALL the parameters of the algorithm
	 * (copy the code from your test class, use the ParamUtils instead
	 * of TestUtils)
	 */
	virtual void Load(map<string, string>& params) = 0;

	//TODO: not sure if neeeded
	/*virtual ImageInputType GetInputType();*/

	/*
	 * return your results in the format of mission control.
	 * each object we find is a new Message.
	 * IMPRTANT: only add to the vector
	 * the message contain "abstract" attributes.
	 * need to fit your attributes to this generic format.
	 * the format is important, need to define exactly and told to yuval.
	 * (even order inside array sometimes,
	 * for example bounds contain 4 edge of rectangle in specific order:
	 * leftup,rightup,rightdown,leftdown)
	 */
	virtual void ToMesseges(vector<MissionControlMessage>& res) =0;

	//TODO: not sure if needed
	/* void Draw(vector<Msg> messages)*/

	/**
	 * delete the process data and images.
	 */
	virtual void ClearProcessData()=0;

	/**
	 * set default value to the parameters of the algorithm.
	 * use only for tests.
	 */
	virtual void SetDefaultParams() =0;

	/*
	 * draw on the given images the last results of the algorithm.
	 * use on the vision application.
	 */
	virtual void Draw(Mat& draw) =0;
protected:
	/**
	 * init the process data and images.
	 */
	virtual void InitProcessData()=0;

	/* init the current results of the algorithm */
	virtual void InitResult()=0;

	virtual void fixResults(Point offset)
	{

	}

public:
	virtual ~BaseAlgorithm();
	void Init(bool testState);
	void MakeCopyAndRun(const Mat& image);
	void SetOffset(Point offset);
	vector<string> Logs;

protected:
	bool _inTest;

private:
	Point _offset;

};

#endif /* BASEALGORITHM_H_ */
