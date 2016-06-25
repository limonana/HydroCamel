/*
 * TorpedoAlgo.cpp
 *
 *  Created on: Feb 25, 2013
 *      Author: shani
 */

#include "TorpedoAlgo.h"
#include "../Utils/Utils.h"
#include "../Utils/ParamUtils.h"
using namespace std;

Rect TorpedoAlgo::nonCutRect(RotatedRect& r) {
	Rect res = r.boundingRect();
	int maxL = max(res.height, res.width);
	res.height = maxL;
	res.width = maxL;
}

void TorpedoAlgo::DeleteImages(map<COLOR, vector<Mat*> >& images) {
	map<COLOR, vector<Mat*> >::iterator it;
	for (it = images.begin(); it != images.end(); ++it) {
		if (!it->second.empty()) {
			for (int i = 0; i < it->second.size(); ++i) {
				if (it->second[i] != NULL)
					delete it->second[i];
			}
		}
		it->second.clear();
	}
	images.clear();
}

void TorpedoAlgo::DeleteImages(map<COLOR, Mat*>& images) {
	map<COLOR, Mat*>::iterator it;
	for (it = images.begin(); it != images.end(); ++it) {
		if (it->second != NULL) {
			delete it->second;
		}
	}
	images.clear();
}

void TorpedoAlgo::ClearProcessData() {
	DeleteImages(_contours);
	DeleteImages(_gray);
}

void TorpedoAlgo::SetDefaultParams() {
	_redHue = 0;
	_redKolmanThresh = 0.2;
	_dknownPBig = 0.3;
	_dknownPSmall = 0.6;
	_sqaureEpsilon = 0.5;
	_dKnownPObject = 0.2;
	_threshJumpRed = 20;
	_threshJumpGreen = 10;
	_backgroundThresh = 0.7;
	_sizeGarbege = 150;
	_noisePercent = 0.3;
	_erodeAmount = 2;
	_openAmount = 1;
	//_convexThresh = 0.2;
	_dOptimalProp = 0.1;
}

TorpedoAlgo::TorpedoAlgo() :
		BaseAlgorithm() {
	_paintColors[BLUE] = Scalar(255, 0, 0);
	_paintColors[YELLOW] = Scalar(0, 217, 255);
	_paintColors[GREEN] = Scalar(0, 255, 0);
	_paintColors[RED] = Scalar(0, 0, 255);
}

TorpedoAlgo::~TorpedoAlgo() {
	ClearProcessData();
}

void TorpedoAlgo::threshOtsu(const Mat& kolman, Mat& thresh) {
	//paint the object in white
	threshold(kolman, thresh, 0, 255, THRESH_OTSU);
	int whitePixels = countNonZero(thresh);
	if (whitePixels > 0.5 * kolman.size().area()) //OTSU is opposite
		threshold(thresh, thresh, 254, 255, THRESH_BINARY_INV);

	//Utils::ShowImage(thresh, "otsu thresh");
}

void TorpedoAlgo::threshYellow(const Mat& image, Mat& thresh) {
	Mat yellow;
	Utils::YellowChannel(image,yellow);
	normalize(yellow, yellow, 0, 255, NORM_MINMAX);
	if (_inTest) {
		_gray[YELLOW] = Utils::CopyImage(yellow);
	}
	//filter the image
	Scalar mean1, dev1;
	Mat bw;
	meanStdDev(yellow, mean1, dev1);
	threshold(yellow, bw, mean1(0) + dev1(0), 255, THRESH_BINARY); //maybe otsu
	erode(bw, bw, Mat(), Point(-1, -1), _erodeAmount);
	//chose the most yellow target
	vector<vector<Point > > c;
	vector<Vec4i> hierarchy;
	findContours(bw.clone(), c, hierarchy,CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
	int bestContor = -1;
	int bestMean = 0;
	for (uint i=0; i<c.size(); ++i)
	{
		if (contourArea(c[i]) < _sizeGarbege)
			continue;
		Mat mask = Mat::zeros(image.size(),CV_8UC1);
		drawContours(mask,c,i,Scalar(255),-1);
		//because holes
		mask = mask & bw;
		Scalar curr_mean = mean(yellow,mask);
		if (curr_mean(0)> bestMean)
		{
			bestMean = curr_mean(0);
			bestContor = i;
		}
	}
	if (bestContor == -1)
	{
		thresh = Mat::zeros(image.size(),CV_8UC1);
	}
	else
	{
		thresh = Mat::zeros(image.size(),CV_8UC1);
		drawContours(thresh ,c,bestContor,Scalar(255),-1);
		thresh = thresh & bw;
	}
}

void TorpedoAlgo::threshBlue(const Mat& image, Mat& thresh) {

	vector<Mat> BGR;
	split(image, BGR);
	Mat& blue = BGR[0];
	normalize(blue, blue, 0, 255, NORM_MINMAX);
	threshold(blue, thresh, 0, 255, THRESH_OTSU);
	int whitePixels = countNonZero(thresh);
	if (whitePixels > 0.5 * image.size().area()) //OTSU is opposite
		threshold(thresh, thresh, 254, 255, THRESH_BINARY_INV);

	if (_inTest) {
		_gray[BLUE] = Utils::CopyImage(blue);
	}
}

void TorpedoAlgo::findGreenTarget(const Mat& image, Target& target,
		const Target& knwonTarget) {
	Mat* hue = Utils::HueChannel(image);
	equalizeHist(*hue, *hue);

	if (_inTest) {
		_gray[GREEN] = Utils::CopyImage(*hue);
	}
	Mat thresh;
	Scalar mean, dev;
	meanStdDev(*hue, mean, dev);
	int threshVal = mean(0);
	//opposite yellow so the yellow part will be black
	threshold(*_yellowThresh, *_yellowThresh, 254, 255, THRESH_BINARY_INV);
	while (threshVal > 0) {
		//thresh from threval to zero
		threshold(*hue, thresh, threshVal, 255, THRESH_BINARY_INV);
		//delete the ywllow parts
		bitwise_and(thresh, *_yellowThresh, thresh);
		erode(thresh, thresh, Mat(), Point(-1, -1), _erodeAmount);

		FindTargetOfColor(GREEN, image, thresh, target, knwonTarget);
		threshVal -= _threshJumpGreen;
	}
	delete hue;
	delete _yellowThresh;
}

void TorpedoAlgo::findRedTarget(const Mat& image, Target& target,
		const Target& knwonTarget) {
	Mat kolman32F;
	Mat kolman(image.size(), CV_8UC1);
	bool colorExist = Utils::Kolaman((float) _redHue, image, kolman32F, kolman,
			_redKolmanThresh); //TODO change back
	if (_inTest) {
		_gray[RED] = Utils::CopyImage(kolman);
	}
	if (!colorExist) {
		return;
	}
	//disable otsu
	//Utils::ShowImage(kolman, "kolman");
	/*Mat thresh(image.size(), CV_8UC1);
	 threshOtsu(kolman, thresh);
	 FindHolesOfColor(RED, image, thresh, target, knwonTarget);
	 if (_inTest)
	 _redThreshs.push_back(thresh);*/
	Mat thresh;
	Scalar mean, dev;
	meanStdDev(kolman, mean, dev);
	int threshVal = mean(0) + dev(0);
	while (threshVal < 255) {
		threshold(kolman, thresh, threshVal, 255, THRESH_BINARY);
		erode(thresh, thresh, Mat(), Point(-1, -1), _erodeAmount);
		FindTargetOfColor(RED, image, thresh, target, knwonTarget);
		threshVal += _threshJumpRed;
	}
}

void TorpedoAlgo::findTargets(const Mat& image, map<COLOR, Target>& targets) {
	Target knownTarget;
	Mat thresh;

	threshYellow(image, thresh);
	_yellowThresh = new Mat();
	thresh.copyTo(*_yellowThresh);

	FindTargetOfColor(YELLOW, image, thresh, targets[YELLOW], knownTarget);
	/*Mat* hue = Utils::HueChannel(image);
	vector<Point> yellowContor;
	Utils::GetContor(targets[YELLOW].object, yellowContor);
	if (Utils::ColorPercent(*hue, yellowContor, 40, 170) < 0.5) {
		Target empty;
		targets[YELLOW] = empty;
	}*/

	if (!targets[YELLOW].empty()) {
		knownTarget = targets[YELLOW];
	}

	findRedTarget(image, targets[RED], knownTarget);
	if (knownTarget.empty())
		knownTarget = targets[RED];

	Mat newImage = image.clone();
	/*if (!targets[RED].empty() && targets[RED].NumOfHoles() > 0)
	 //deleteObject(newImage, targets[RED]);*/

	/*if (!targets[YELLOW].empty() && targets[YELLOW].NumOfHoles() > 0)
	 //deleteObject(newImage, targets[YELLOW]);*/
	findGreenTarget(image, targets[GREEN], knownTarget);

	/*threshBlue(newImage, thresh);
	 FindHolesOfColor(BLUE, image, thresh, targets[BLUE], knownTarget);*/

}

void TorpedoAlgo::Run(Mat& image) {
	current_results.clear();
	findTargets(image, current_results);
}

void TorpedoAlgo::Load(map<string, string>& params) {
	ParamUtils::setParam(params, "redHue", _redHue);
	ParamUtils::setParamPercent(params, "redKolmanPercent", _redKolmanThresh);
	ParamUtils::setParamPercent(params, "sizeObjectThresh", _backgroundThresh);
	ParamUtils::setParam(params, "sizeGarbege", _sizeGarbege);
	ParamUtils::setParamPercent(params, "noisePercent", _noisePercent);
	ParamUtils::setParam(params, "openAmount", _openAmount);
	ParamUtils::setParam(params, "erodeAmount", _erodeAmount);
	ParamUtils::setParamPercent(params, "distance_from_known_obj",
			_dKnownPObject);
	ParamUtils::setParamPercent(params, "distance_from_big_hole", _dknownPBig);
	ParamUtils::setParamPercent(params, "distance_from_small_hole",
			_dknownPSmall);
	ParamUtils::setParamPercent(params, "epsilon_rect", _sqaureEpsilon);
	ParamUtils::setParam(params, "red_thresh_jump", _threshJumpRed);
	ParamUtils::setParam(params, "green_thresh_jump", _threshJumpGreen);
	ParamUtils::setParamPercent(params, "diff_prop", _dOptimalProp);

	//ParamUtils::setParamPercent(params, "convex_thresh", _convexThresh);
}

void TorpedoAlgo::createMSG(RotatedRect rect, MissionControlMessage& msg) {
	msg.MissionCode = Torpedo;
	Utils::FillRectBounds(rect, msg);
	msg.bounds.push_back(std::pair<int, int>(rect.size.height, rect.size.width));
	msg.intrestPoints.push_back(
			std::pair<int, int>(rect.center.x,
					rect.center.y));
}

void TorpedoAlgo::ToMesseges(vector<MissionControlMessage>& res) {
//Message: each message is a hole.
//additionalInformation first digit is the number representing the color of the object containing the hole
//first 4 points of bounds are the edge points of the hole in that order:
//Left Bottom,Top Left ,Top Right ,Bottom Right.
//the 5th point of bounds is a pair height (first) and width (second) of the hole.
//intrestPoints contain the center point of the hole.
	//TODO: insert which hole is the bigger
	map<COLOR, Target>::iterator it;
	for (it = current_results.begin(); it != current_results.end(); ++it) {
		Target& t = it->second;
		if (t.empty())
			continue;

		MissionControlMessage msg;
		createMSG(t.object, msg);
		msg.additionalInformation = it->first * 10 + 0;
		res.push_back(msg);

		if (t.HasBigHole()) {
			MissionControlMessage msg;
			createMSG(t.bigHole, msg);
			msg.additionalInformation = it->first * 10 + 1;
			res.push_back(msg);
		}
		if (t.HasSmallHole()) {
			MissionControlMessage msg;
			createMSG(t.smallHole, msg);
			msg.additionalInformation = it->first * 10 + 1;
			res.push_back(msg);
		}

	}
}

void TorpedoAlgo::InitProcessData() {
	_gray.clear();
	_contours.clear();
}

void TorpedoAlgo::InitResult() {
	current_results.clear();
}

bool TorpedoAlgo::IsBackground(int rectH, int rectW, int imageH, int imageW) {
	double percentHeight = (double) (rectH) / imageH;
	double percentWidth = (double) (rectW) / imageW;
	return ((percentHeight > _backgroundThresh)
			|| (percentWidth > _backgroundThresh));
}

void TorpedoAlgo::createHoleThresh(const Mat& thresh, const RotatedRect& target,
		Mat& holeThresh, Rect& frame) {
	RotatedRect smallTarget = target;
	smallTarget.size.width = 0.8 * target.size.width;
	smallTarget.size.height = 0.8 * target.size.height;

	//crate a mask
	Mat mask;
	Utils::GetMask(thresh.size(), smallTarget, mask);

	//create white mat because the holes are black
	Mat threshZoom = Mat::ones(thresh.size(), CV_8UC1);
	threshZoom.convertTo(threshZoom, CV_8UC1, 255, 0);

	//copy only the target to new mat
	thresh.copyTo(threshZoom, mask);
	//Utils::ShowImage(threshZoom,"threshMask");
	frame = smallTarget.boundingRect();
	fitRectToFrame(frame, thresh.size().height, thresh.size().width);
	//fit the mat to the area we looking at
	threshZoom = threshZoom(frame);

	//paint the object in black and the holes in white.
	//findcountors assume black border so this allows to
	//find holes when we are close and the holes are cut by the frame.
	//inverse the image
	threshold(threshZoom, holeThresh, 254, 255, THRESH_BINARY_INV);
	//want to get rid of small holes (white) because we search for holes
	// so use open (first make more black and then white)
	//need to be careful not to close the holes
	//there is a filter of shape for ignoring small holes.
	if (_openAmount > 0)
		//TODO: don't know if more efficient to set the mat instead of iterations
		morphologyEx(holeThresh, holeThresh, MORPH_OPEN, Mat(), Point(-1, -1),
				_openAmount);
	//Utils::ShowImage(holeThresh, "holethresh");
}

void TorpedoAlgo::findBestHoles(COLOR color,
		const vector<vector<Point> >& countors, const Mat& holeThresh,
		Target& target, const Target& knownTarget, Point offset) {
	for (uint i = 0; i < countors.size(); ++i) {
		/*Mat draw = Mat::zeros(holeThresh.size(),CV_8SC3);
		 drawContours(draw,countors,i,Scalar(0,0,255),3);
		 Utils::ShowImage(draw,"hole c");*/
		int area = contourArea(countors[i]);
		if (area < _sizeGarbege)
			continue;
		RotatedRect curr_Hole = minAreaRect(countors[i]);
		double propHole = (double) SizeTarget(curr_Hole)
				/ SizeTarget(target.object);
		double optimalPropSmall = 17.8 / 60;
		double optimalPropBig = 30.5 / 60;
		double optimal;
		if (!target.HasBigHole())
			optimal = optimalPropBig;
		else
			optimal = optimalPropSmall;

		if (abs(propHole - optimal) > _dOptimalProp)
			continue;

		vector<int> cuts = Utils::CutDirections(curr_Hole, holeThresh.size());
		if (cuts.empty())
			cuts = Utils::isCut(countors[i], holeThresh.size());

		if (cuts.empty() && !isSquare(curr_Hole, _sqaureEpsilon))
			continue;

		if (cuts.empty() && knownTarget.NumOfHoles() > 0) {
			double dBigHole = abs(knownTarget.distanceBig(curr_Hole));
			double dSmallHole = abs(knownTarget.distanceSmall(curr_Hole));
			int knownBigSize = SizeTarget(knownTarget.bigHole);
			int knownSmallSize;
			if (knownTarget.HasSmallHole())
				knownSmallSize = SizeTarget(knownTarget.smallHole);
			else {
				knownSmallSize = 1;
				dSmallHole = 0;
			}
			if ((dBigHole / knownBigSize) > _dknownPBig
					&& (dSmallHole / knownSmallSize) > _dknownPSmall)
				continue;
		}
		if (IsWhite(holeThresh, countors[i], _noisePercent)) {
			if (_inTest) {
				Mat& draw = *_contours.at(color).back();
				drawContours(draw, countors, i, Scalar(100, 100, 100), -1,
						CV_AA, noArray(), 0, offset);
			}
			if (target.NumOfHoles() < 2) {
				target.setHole(curr_Hole);
				if (_inTest) {
					Mat& draw = *_contours.at(color).back();
					drawContours(draw, countors, i, _paintColors[color], 2,
							CV_AA, noArray(), 0, offset);
				}
			} else {
				int dSmallHole = target.distanceSmall(curr_Hole);
				RotatedRect empty;
				if (dSmallHole > 0) //curr_hole is bigger then the small hole
						{
					if (_inTest) {
						Mat& draw = *_contours.at(color).back();
						drawContours(draw, countors, i, _paintColors[color], 2,
								CV_AA, noArray(), 0, offset);
					}
					target.smallHole = empty;
					target.setHole(curr_Hole);
					continue;
				}

			}

		}

	}
}

void TorpedoAlgo::FindTargetOfColor(COLOR color, const Mat& image,
		const Mat& objectThresh, Target& target, const Target& knownTarget) {
	vector<RotatedRect> optinalTargets;
	getTargets(color, objectThresh, optinalTargets, knownTarget);
	if (optinalTargets.empty())
		return;

	for (uint i = 0; i < optinalTargets.size(); ++i) {
		Rect frame;
		Mat holeThresh;
		createHoleThresh(objectThresh, optinalTargets[i], holeThresh, frame);
		vector<vector<Point> > countors;
		vector<Vec4i> hierarchy;
		findContours(holeThresh.clone(), countors, hierarchy, CV_RETR_LIST,
				CV_CHAIN_APPROX_SIMPLE);
		//for tests
		/*Mat draw = Mat::zeros(holeThresh.size(), CV_8UC3);
		 for (uint j = 0; j < countors.size(); ++j)
		 drawContours(draw, countors, j, Scalar(0, 0, 255), 5);
		 Utils::ShowImage(draw,"holes contours");*/
		Target curr_target;
		curr_target.object = optinalTargets[i];
		findBestHoles(color, countors, holeThresh, curr_target, knownTarget,
				Point(frame.x, frame.y));
		//fix holes
		if (curr_target.HasBigHole()) {
			curr_target.bigHole.center.x += frame.x;
			curr_target.bigHole.center.y += frame.y;
		}
		if (curr_target.HasSmallHole()) {
			curr_target.smallHole.center.x += frame.x;
			curr_target.smallHole.center.y += frame.y;
		}

		/*if (target.NumOfHoles() ==2)
		 {
		 int dx = abs(target.bigHole.center.x - target.smallHole.center.x);
		 int dy = abs(target.bigHole.center.y - target.smallHole.center.y;)
		 if (dx < 0 || dy < target.)
		 }*/
		if (target.empty()) {
			target = curr_target;
			continue;
		}
		// if there is known target chose the one that is the similar
		if (!knownTarget.empty()) {
			int dKnownCurr = knownTarget.Distance(curr_target);
			int dKnownOld = knownTarget.Distance(target);
			if (dKnownCurr < dKnownOld) {
				/*Mat draw = Mat::zeros(image.size(),CV_8UC3);
				 Utils::drawSqureCounter(curr_target.object,draw,Scalar(0,0,255),3);
				 Utils::ShowImage(draw,"knwon");*/
				target = curr_target;
				continue;
			}
		}
		//chose the one with most holes
		if (curr_target.NumOfHoles() > target.NumOfHoles()) {
			target = curr_target;
			continue;
		}

		if (curr_target.NumOfHoles() == target.NumOfHoles()) {
			if (curr_target.area() > target.area())
				target = curr_target;
			/*double optimalPropSmall = 17.8 / 60;
			 double optimalPropBig = 30.5 / 60;
			 //chose the one with propertions close to defined
			 double prop_curr_small = 0;
			 double prop_curr_big = 0;
			 double prop_target_small = 0;
			 double prop_target_big = 0;
			 if (curr_target.HasBigHole())
			 {

			 prop_curr_big = (double) SizeTarget(curr_target.bigHole)
			 / SizeTarget(curr_target.object);
			 prop_target_big = (double) SizeTarget(target.bigHole)
			 / SizeTarget(target.object);
			 }
			 if (curr_target.HasSmallHole())
			 {
			 prop_curr_small = (double) SizeTarget(curr_target.smallHole)
			 / SizeTarget(curr_target.object);
			 prop_target_small = (double) SizeTarget(target.smallHole)
			 / SizeTarget(target.object);
			 }
			 double dPropCurr = abs(prop_curr_big - optimalPropBig)
			 + abs(prop_curr_small - optimalPropSmall);
			 double dTargetCurr = abs(prop_target_big - optimalPropBig)
			 + abs(prop_target_small - optimalPropSmall);
			 if (dPropCurr < dTargetCurr)
			 {
			 /*Mat draw = Mat::zeros(image.size(),CV_8UC3);
			 Utils::drawSqureCounter(curr_target.object,draw,Scalar(0,0,255),3);
			 Utils::ShowImage(draw,"knwon");*/
			//target = curr_target;
			//}
		}
	}
}

bool TorpedoAlgo::IsWhite(const Mat& binaryImage, const vector<Point>& counter,
		double noisePercent) {
//no black pixel suppose to be inside the hole
//but because of noise could be black pixels (cutted contors)
//TODO: maye there is more efficent way then doing histogram
	return Utils::ColorPercent(binaryImage, counter, 0, 0, 255) <= noisePercent;
}

void TorpedoAlgo::Draw(RotatedRect& hole, COLOR color, Mat& draw,
		bool whiteLine) {
	//draw rectangle
	if (whiteLine)
		Utils::drawRotatedRect(hole,draw,Scalar(255, 255, 255),3);
	Utils::drawRotatedRect(hole,draw,_paintColors[color],1);
	//draw center
	const Point& center = hole.center;
	rectangle(draw, center, center, _paintColors[color], 2);
}

bool TorpedoAlgo::insidenHole(const Rect& obj, const vector<Rect>& holes) {
	vector<Rect>::const_iterator it;
	for (it = holes.begin(); it != holes.end(); ++it) {
		Rect intersection = obj & *it;
		if (intersection.width > 0)
			return true;
	}
	return false;
}

void TorpedoAlgo::getTargets(COLOR color, const Mat& thresh,
		vector<RotatedRect>& targets, const Target& knownTarget) {
	vector<vector<Point> > contors;
	findContours(thresh.clone(), contors, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
	if (_inTest) {
		Mat draw = Mat::zeros(thresh.size(), CV_8UC3);
		for (uint i = 0; i < contors.size(); ++i)
			drawContours(draw, contors, i, Scalar(255, 255, 255), -1);
		_contours[color].push_back(Utils::CopyImage(draw));
	}

	for (uint i = 0; i < contors.size(); ++i) {
		int cArea = contourArea(contors[i]);
		if (cArea < _sizeGarbege)
			continue;

		if (Utils::BlackPercent(thresh, contors[i]) > 0.9)
			continue;

		//for test
		/*Mat draw = Mat::zeros(thresh.size(), CV_8UC3);
		 drawContours(draw, contors, i, _paintColors[color], 5);
		 Utils::ShowImage(draw, "contors");*/
		RotatedRect curr_target = minAreaRect(contors[i]);
		/*vector<Point> convexHole;
		 convexHull(contors[i], convexHole);
		 double dConvexPercent  = contourArea(convexHole)/curr_target.size.area();
		 cout << dConvexPercent << endl;
		 if (dConvexPercent < _convexThresh)
		 continue;*/

		if (IsBackground(curr_target.size.height, curr_target.size.width,
				thresh.size().height, thresh.size().width))
			continue;
		vector<int> cuts = Utils::CutDirections(curr_target, thresh.size());
		if (cuts.empty())
			cuts = Utils::isCut(contors[i], thresh.size());
		/*if (!cuts.empty())
			continue;*/
		/*if (!cuts.empty()) {
			Rect r = curr_target.boundingRect();
			bool cutLeft = find(cuts.begin(), cuts.end(), LEFT) != cuts.end();
			bool cutRight = find(cuts.begin(), cuts.end(), RIGHT) != cuts.end();
			bool cutUP = find(cuts.begin(), cuts.end(), UP) != cuts.end();
			bool cutDOWN = find(cuts.begin(), cuts.end(), DOWN) != cuts.end();
			if ((cutLeft || cutRight) && !(cutUP || cutDOWN)) {
				if (r.width > r.height) {
					cuts.clear();
				}
			}
			if (!(cutLeft || cutRight) && (cutUP || cutDOWN)) {
				if (r.height > r.width)
					cuts.clear();
			}
		}*/

		/*if (cuts.empty() && !isSquare(curr_target, _sqaureEpsilon))
			continue;*/
		if (!isSquare(curr_target, _sqaureEpsilon))
			continue;

		if (/*cuts.empty() && */!knownTarget.empty()) {
			double dObject = abs(knownTarget.distanceObject(curr_target));
			int knownSize = SizeTarget(knownTarget.object);
			if (dObject / knownSize > _dKnownPObject)
				continue;
		}
		if (_inTest) {
			Mat& draw = *_contours.at(color).back();
			drawContours(draw, contors, i, _paintColors[color], 5);
		}
		targets.push_back(curr_target);
	}
}

bool TorpedoAlgo::IsHoleOf(const Rect& object, const Rect& hole) {
	Rect intersection = object & hole;
	int intersectionArea = intersection.height * intersection.width;
	int holeArea = hole.width * hole.height;
	return (intersectionArea >= 0.7 * holeArea);
}

void TorpedoAlgo::deleteObject(Mat& image, const Target& target) {
	Mat mask = Mat::zeros(image.size(), CV_8UC1);
	if (target.HasBigHole()) {
		Mat tmpMask;
		Utils::GetMask(image.size(), target.bigHole, tmpMask);
		mask = mask | tmpMask;
	}
	if (target.HasSmallHole()) {
		Mat tmpMask;
		Utils::GetMask(image.size(), target.bigHole, tmpMask);
		mask = mask | tmpMask;
	}
	//TODO: not sure if use the hole mean of the image mean
	Scalar holesMean = mean(image, mask);
	vector<Point> targetContor;
	Utils::GetContor(target.object, targetContor);
	vector<vector<Point> > c;
	c.push_back(targetContor);
	drawContours(image, c, 0, holesMean, -1);
}

void TorpedoAlgo::fitRectToFrame(Rect& rect, int imageH, int imageW) {
	if (rect.x < 0) {
		rect.width += rect.x;
		rect.x = 0;
	}
	if (rect.y < 0) {
		rect.height += rect.y;
		rect.y = 0;
	}
	if (rect.y + rect.height > imageH) {
		rect.height = imageH - rect.y;
	}
	if (rect.x + rect.width > imageW) {
		rect.width = imageW - rect.x;
	}
}

bool TorpedoAlgo::isSquare(RotatedRect& rect, double epsilon) {
	double maxL = max(rect.size.width, rect.size.height);
	double minL = min(rect.size.width, rect.size.height);
	return (maxL / minL < 1 + epsilon);
}

int TorpedoAlgo::SizeTarget(const RotatedRect& r) {
	return max(r.size.height, r.size.width);
}

void TorpedoAlgo::fixResults(Point offset)
{
	map<COLOR,Target>::iterator it;
	for (it= current_results.begin(); it!=current_results.end(); ++it)
	{
		it->second.bigHole.center.x +=offset.x;
		it->second.bigHole.center.y +=offset.y;
		it->second.smallHole.center.x +=offset.x;
		it->second.smallHole.center.y +=offset.y;
		it->second.object.center.x +=offset.x;
		it->second.object.center.y +=offset.y;
	}
}

string TorpedoAlgo::colorToString(COLOR color) {
	switch (color) {
	case BLUE:
		return "BLUE";
	case GREEN:
		return "GREEN";
	case YELLOW:
		return "YELLOW";
	case RED:
		return "RED";
	default:
		return "";
	}
}

void TorpedoAlgo::Draw(Mat& draw) {
	map<COLOR, Target>::iterator it;
	for (it = current_results.begin(); it != current_results.end(); ++it) {
		if (it->second.empty())
			continue;
		Draw(it->second.object, it->first, draw, false);

		if (it->second.HasBigHole()) {
			Draw(it->second.bigHole, it->first, draw, true);
		}
		if (it->second.HasSmallHole()) {
			Draw(it->second.smallHole, it->first, draw, true);
		}
	}
}
