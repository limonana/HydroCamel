/*
 * Shadow.h
 *
 *  Created on: Jul 7, 2013
 *      Author: shani
 */

#ifndef SHADOW_H_
#define SHADOW_H_

#include "../BaseAlgorithm.h"
#include <cv.h>
using namespace cv;

class ShadowAlgorithm: public BaseAlgorithm
{
public:
	ShadowAlgorithm();
	virtual ~ShadowAlgorithm();

	//override
public:
	virtual void Run(Mat& image);
	virtual void findYellowShadows(const Mat& image);
	virtual void Load(map<string, string>& params);
	virtual void ToMesseges(vector<MissionControlMessage>& res);
	virtual void ClearProcessData();
	virtual void SetDefaultParams();
	virtual void Draw(Mat& draw);
protected:
	virtual void InitProcessData();
	virtual void InitResult();

private:
	int findNumber(Mat& ROI);
	float matchShadow(const Mat& BW, const Mat& shadowTemplate);
	void Rotate180(Mat& threshinside);
	bool isUpsideDown(Mat& TargetBW);
	void TO_BW(Mat& gray, Mat& BW);
	void GetTargets(Mat& yellow, vector<RotatedRect>& targets,
			vector<RotatedRect>& cuts,int minGray,Mat*& threshOut);
	void GetTargetBW(const Mat& yellow, RotatedRect rect, Mat& BW);
	void createMSG(MissionControlMessage& msg, RotatedRect& shadow);
	void findWhiteBorder(Mat& image);
	int identifyNumber(Mat& yellow, RotatedRect& target);
	bool ToBig(RotatedRect r,Size sz);

//process images
public:
	Mat* _yellow;
	Mat* _yellowBW;
	vector<Mat*> _BWROI;
	Mat* _L;
	Mat* _WhiteThresh;
	vector<Mat*> _insideWhiteBW;
	vector<Mat*> _insideWhiteGray;

//members
private:
	map<int, Mat> templates;
	int _realRatio;
	int _maxSizeGarbage;
public:
//parameters
	double _minGrade;
	double _minSizePercent;
	double _RatioDev;
	int _dilate;
	double _minYellow;
	int _threshWhite;
	double _retangleDev;

public:
	vector<pair<int, RotatedRect> > current_results;
	vector<RotatedRect> cut_results;

	//process data
	vector<int> _cutsAmount;
	vector<int> _targetsAmount;
	bool _yellowStatus;
};

#endif /* SHADOW_H_ */
