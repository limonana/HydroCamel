/*
 * Shadow.cpp
 *
 *  Created on: Jul 7, 2013
 *      Author: shani
 */

#include "ShadowAlgorithm.h"
#include "../Utils/Utils.h"
#include "../Utils/ParamUtils.h"

void ShadowAlgorithm::GetTargets(Mat& gray, vector<RotatedRect>& targets,
		vector<RotatedRect>& cuts, int minGray, Mat*& threshOut)
{
	Mat orgThrsh, thresh;
	threshold(gray, orgThrsh, 0, 255, THRESH_OTSU);

	//add dilate in order to close shapes that are divided by the red
	dilate(orgThrsh, thresh, Mat(), Point(-1, -1), _dilate);
	if (_inTest)
	{
		threshOut = Utils::CopyImage(thresh);
	}
	vector<vector<Point> > c;
	findContours(thresh, c, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (uint i = 0; i < c.size(); ++i)
	{
		if (contourArea(c[i]) < _maxSizeGarbage)
			continue;

		double whiteP = 1 - Utils::BlackPercent(orgThrsh, c[i]);
		/*if (whiteP < 0.5)
			continue;*/
		if (Utils::MeanOfShape(gray, c[i]) < whiteP * minGray)
			continue;

		RotatedRect rect = minAreaRect(c[i]);

		if (Utils::CutDirections(rect, gray.size()).size() > 0)
		{
			cuts.push_back(rect);
			continue;
		}

		if (rect.size.area()
				< _minSizePercent * _minSizePercent * gray.size().area())
			continue;

		double minL = min(rect.size.height, rect.size.width);
		double maxL = max(rect.size.height, rect.size.width);
		double ratio = maxL / minL;
		if (ratio < (1 - _RatioDev) * _realRatio
				|| ratio > (1 + _RatioDev) * _realRatio)
			continue;

		targets.push_back(rect);
		if (_inTest)
		{
			Utils::drawRotatedRect(rect, *threshOut, Scalar(100), 3);
		}
	}
}

void ShadowAlgorithm::GetTargetBW(const Mat& gray, RotatedRect rect, Mat& BW)
{
	Mat ROIGray;
	Utils::GetROI(gray, rect, ROIGray);
	TO_BW(ROIGray, BW);
	//make the image to be long and not width
	if (BW.rows < BW.cols)
	{
		//TODO: change to  warppaffline?  - generic function
		Utils::Rotate90(BW);
	}

	bool upsideDown = isUpsideDown(BW);
	if (upsideDown) //we are upside down
	{
		//rotate 180
		Utils::Rotate90(BW);
		Utils::Rotate90(BW);
	}
	//the text should be in black and the background white
	int bgPixells = countNonZero(BW);
	if (bgPixells < 0.5 * BW.size().area()) //need to inverse the image
		bitwise_not(BW, BW);

	if (_inTest)
		_BWROI.push_back(Utils::CopyImage(BW));
}

void ShadowAlgorithm::findWhiteBorder(Mat& image)
{
	Mat LAB;
	cvtColor(image, LAB, CV_BGR2Lab);
	int ch[] =
	{ 0, 0 };
	Mat L(image.size(), CV_8U);
	mixChannels(&LAB, 1, &L, 1, ch, 1);
	normalize(L, L, 0, 255, NORM_MINMAX);
	if (_inTest)
	{
		_L = Utils::CopyImage(L);
	}
	//the white area is black and the holes is white so we can see cut holes
	Mat thresh;
	threshold(L, thresh, 255 - _threshWhite, 255, THRESH_BINARY_INV);
	if (_inTest)
	{
		_WhiteThresh = Utils::CopyImage(thresh);
	}

	vector<vector<Point> > c;
	findContours(thresh, c, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	for (uint i = 0; i < c.size(); ++i)
	{
		if (Utils::BlackPercent(thresh, c[i]) > _retangleDev)
			continue;

		if (_inTest)
		{
			drawContours(*_WhiteThresh, c, i, Scalar(100), 2);
		}

		RotatedRect r;
		if (ToBig(r,image.size()))
			continue;
		if (!Utils::isRectangle(c.at(i), r, image.size(), _retangleDev))
			continue;

		//because the backgornd is white find contours find the all image - ignore it
		if (ToBig(r, image.size()))
			continue;

		if (!Utils::CutDirections(r, image.size()).empty())
		{
			cut_results.push_back(r);
			continue;
		}
		/*Mat draw2 = image.clone();
		 Utils::drawRotatedRect(r, draw2, Scalar(0, 255, 0), 2);
		 drawContours(draw2, c, i, Scalar(0, 255, 255), 1);
		 Utils::ShowImage(draw2, "hole");*/
		if (r.size.area()
				< _minSizePercent * _minSizePercent * image.size().area())
			continue;

		Mat ROI;
		//get little smaller rectagnle so not see the white
		r.size.height -= 4;
		r.size.width -= 4;
		Utils::GetROI(L, r, ROI);
		normalize(ROI, ROI, 0, 255, NORM_MINMAX);
		if (_inTest)
			_insideWhiteGray.push_back(Utils::CopyImage(ROI));
		vector<RotatedRect> cuts, targets;
		Mat threshOut;
		Mat* p = &threshOut;
		GetTargets(ROI, targets, cuts, 0, p);
		if (_inTest)
		{
			_insideWhiteBW.push_back(p);
		}
		if (_inTest)
		{
			_targetsAmount.push_back(targets.size());
			_cutsAmount.push_back(cuts.size());
		}
		/*Mat draw = ROI.clone();
		 for (uint i = 0; i < targets.size(); ++i)
		 {
		 Utils::drawRotatedRect(targets[i], draw, Scalar(255), 2);
		 }

		 for (uint i = 0; i < cuts.size(); ++i)
		 {
		 Utils::drawRotatedRect(cuts[i], draw, Scalar(200), 2);
		 }
		 Utils::ShowImage(draw, "target");*/
		//chose the first
		RotatedRect target;
		if (!targets.empty())
			target = targets.front();
		else
		{
			if (cuts.empty())
				continue;
			target = cuts.front();
		}
		//because we far don't use the min grade
		int tmp = _minGrade;
		_minGrade = 0;
		int num = identifyNumber(ROI, target);
		_minGrade = tmp;
		if (num > 0)
			current_results.push_back(pair<int, RotatedRect>(num, r));
	}
}

void ShadowAlgorithm::Run(Mat& image)
{
	findYellowShadows(image);
	if (!cut_results.empty() || !current_results.empty()) //found something
	{
		_yellowStatus = true;
		return;
	}

	//find white border
	_yellowStatus = false;
	findWhiteBorder(image);
}

ShadowAlgorithm::ShadowAlgorithm()
{
	templates[10] = imread("10.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	templates[16] = imread("16.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	templates[37] = imread("37.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	templates[98] = imread("98.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	map<int, Mat>::iterator it;
	for (it = templates.begin(); it != templates.end(); ++it)
		if (it->second.empty())
			throw runtime_error("shadow image not loaded");

	_realRatio = 60 / 30;
	_maxSizeGarbage = 70;
}

ShadowAlgorithm::~ShadowAlgorithm()
{
	// TODO Auto-generated destructor stub
}

void ShadowAlgorithm::Load(map<string, string>& params)
{
	ParamUtils::setParamPercent(params, "minGrade", _minGrade);
	ParamUtils::setParamPercent(params, "minSizePercent", _minSizePercent);
	ParamUtils::setParamPercent(params, "RatioDev", _RatioDev);
	ParamUtils::setParam(params, "dilate", _dilate);
	ParamUtils::setParam(params, "minYellow", _minYellow);
	ParamUtils::setParam(params, "whiteThresh", _threshWhite);
	ParamUtils::setParamPercent(params, "retangleDev", _retangleDev);
}

int ShadowAlgorithm::identifyNumber(Mat& gray, RotatedRect& target)
{
	Mat ROI_BW;
	GetTargetBW(gray, target, ROI_BW);
	int num = findNumber(ROI_BW);
	return num;
}

void ShadowAlgorithm::findYellowShadows(const Mat& image)
{
	current_results.clear();
	cut_results.clear();
	Mat yellow;
	Utils::YellowChannel(image, yellow);
	if (_inTest)
		_yellow = Utils::CopyImage(yellow);
	double maxVal;
	minMaxLoc(yellow, NULL, &maxVal, NULL, NULL);
	if (maxVal < _minYellow)
		return;

	vector<RotatedRect> targets;
	vector<RotatedRect> cuts;
	GetTargets(yellow, targets, cuts, _minYellow, _yellowBW);
	for (uint i = 0; i < targets.size(); ++i)
	{
		if (ToBig(targets[i],yellow.size()))
					continue;
		int num = identifyNumber(yellow, targets[i]);
		if (num > 0)
			current_results.push_back(pair<int, RotatedRect>(num, targets[i]));
	}

	for (uint i = 0; i < cuts.size(); ++i)
	{
		if (ToBig(cuts[i],yellow.size()))
					continue;
		cut_results.push_back(cuts[i]);
	}
}

void ShadowAlgorithm::createMSG(MissionControlMessage& msg, RotatedRect& shadow)
{
	msg.MissionCode = Shadow;
	vector<Point> bounds;
	Utils::FillRectBounds(shadow, msg);
	if (shadow.size.height >= shadow.size.width)
		msg.bounds.push_back(
				std::pair<int, int>(shadow.size.height, shadow.size.width));
	else
		msg.bounds.push_back(
				std::pair<int, int>(shadow.size.width, shadow.size.height));

	msg.intrestPoints.push_back(
			std::pair<int, int>(shadow.center.x, shadow.center.y));
}
void ShadowAlgorithm::ToMesseges(vector<MissionControlMessage>& res)
{
	//Message:
	//additionalInformation is the number written on the shadow or 0 if the shadow is cut.
	//first 4 points of bounds are the edge points of the shadow in that order:
	//Left Bottom,Top Left ,Top Right ,Bottom Right.
	//the 5th point of bounds is a pair of height and width of the shadow.
	//the first is the height .
	//intrestPoints contain the center point of the shadow
	vector<pair<int, RotatedRect> >::iterator it;
	for (it = current_results.begin(); it != current_results.end(); ++it)
	{
		RotatedRect& shadow = (*it).second;
		MissionControlMessage msg;
		createMSG(msg, shadow);

		msg.additionalInformation = (*it).first;

		res.push_back(msg);
	}

	for (uint i = 0; i < cut_results.size(); ++i)
	{
		RotatedRect& shadow = cut_results[i];
		MissionControlMessage msg;
		createMSG(msg, shadow);
		msg.additionalInformation = 0;
		res.push_back(msg);
	}

}

void ShadowAlgorithm::ClearProcessData()
{
	if (_yellowBW != NULL)
	{
		delete _yellowBW;
		_yellowBW = NULL;
	}
	if (_yellow != NULL)
	{
		delete _yellow;
		_yellow = NULL;
	}
	if (!_BWROI.empty())
	{
		for (uint i = 0; i < _BWROI.size(); ++i)
		{
			Mat* image = _BWROI.at(i);
			if (image != NULL)
				delete image;
		}
		_BWROI.clear();
	}

	if (!_insideWhiteBW.empty())
	{
		for (uint i = 0; i < _insideWhiteBW.size(); ++i)
		{
			Mat* image = _insideWhiteBW.at(i);
			if (image != NULL)
				delete image;
		}
		_insideWhiteBW.clear();
	}

	if (!_insideWhiteGray.empty())
	{
		for (uint i = 0; i < _insideWhiteGray.size(); ++i)
		{
			Mat* image = _insideWhiteGray.at(i);
			if (image != NULL)
				delete image;
		}
		_insideWhiteGray.clear();
	}

	if (_WhiteThresh != NULL)
	{
		delete _WhiteThresh;
		_WhiteThresh = NULL;
	}

	if (_L != NULL)
	{
		delete _L;
		_L = NULL;
	}
	_targetsAmount.clear();
	_cutsAmount.clear();
}

void ShadowAlgorithm::SetDefaultParams()
{
	_minSizePercent = 0.1;
	_minGrade = 0.5;
	_dilate = 1;
	_RatioDev = 0.2;
	_minYellow = 100;
	_threshWhite = 40;
	_retangleDev = 0.2;
}

void ShadowAlgorithm::Draw(Mat& draw)
{
	vector<pair<int, RotatedRect> >::iterator it;
	for (it = current_results.begin(); it != current_results.end(); ++it)
	{
		Utils::drawRotatedRect((*it).second, draw, Scalar(0, 255, 0),
				draw.cols / 50);
		stringstream stream;
		stream << it->first;
		putText(draw, stream.str(), (*it).second.center, FONT_HERSHEY_SIMPLEX,
				draw.cols / 100, Scalar(255, 0, 0), 2);
	}

	for (uint i = 0; i < cut_results.size(); ++i)
	{
		Utils::drawRotatedRect(cut_results[i], draw, Scalar(0, 0, 255),
				draw.cols / 50);
	}
}

void ShadowAlgorithm::InitProcessData()
{
	_yellowBW = NULL;
	_yellow = NULL;
	_BWROI.clear();
	_L = NULL;
	_WhiteThresh = NULL;
	_targetsAmount.clear();
	_cutsAmount.clear();
	_insideWhiteBW.clear();
}

void ShadowAlgorithm::InitResult()
{
	current_results.clear();
	cut_results.clear();
}

void ShadowAlgorithm::Rotate180(Mat& threshinside)
{
	transpose(threshinside, threshinside);
	flip(threshinside, threshinside, 1);
	transpose(threshinside, threshinside);
	flip(threshinside, threshinside, 1);
}

bool ShadowAlgorithm::isUpsideDown(Mat& TargetBW)
{
	Mat topHalf, bottomHalf;
	Rect firstHalfRect, secondHalfRect;
	firstHalfRect.x = 0;
	firstHalfRect.y = 0;
	firstHalfRect.height = TargetBW.rows / 2;
	firstHalfRect.width = TargetBW.cols;
	secondHalfRect.x = 0;
	secondHalfRect.y = TargetBW.rows / 2;
	secondHalfRect.height = TargetBW.rows / 2;
	secondHalfRect.width = TargetBW.cols;
	topHalf = TargetBW(firstHalfRect);
	bottomHalf = TargetBW(secondHalfRect);
	//background is white
	//top half have more background
	bool upsideDown = countNonZero(topHalf) < countNonZero(bottomHalf);
	return upsideDown;
}

void ShadowAlgorithm::TO_BW(Mat& gray, Mat& BW)
{
	normalize(gray, gray, 0, 255, NORM_MINMAX);
	threshold(gray, BW, 0, 255, THRESH_OTSU);
}

int ShadowAlgorithm::findNumber(Mat& BW)
{
	float max = 0;
	int number = -1;
	float grade;

	map<int, Mat>::iterator it;
	for (it = templates.begin(); it != templates.end(); ++it)
	{
		grade = matchShadow(BW, it->second);
		if (grade < _minGrade)
			continue;
		if (grade > max)
		{
			max = grade;
			number = it->first;
		}
	}
	return number;
}

float ShadowAlgorithm::matchShadow(const Mat& BW, const Mat& shadowTemplate)
{
	Mat shadow;
	resize(shadowTemplate, shadow, BW.size());
	Mat res;
	matchTemplate(BW, shadow, res, CV_TM_CCORR_NORMED);
	return res.at<float>(0, 0);
}

bool ShadowAlgorithm::ToBig(RotatedRect rect, Size sz)
{
	double minL = min(rect.size.height, rect.size.width);
	double maxL = max(rect.size.height, rect.size.width);
	int diffAll = 10;
	return (minL >= sz.height - diffAll) || (minL >= sz.width - diffAll)
			|| (maxL >= sz.height - diffAll) || (maxL >= sz.width - diffAll);
}
