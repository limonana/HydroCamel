/*
 * Gate.cpp
 *
 *  Created on: Feb 25, 2013
 *      Author: Itzik Mizrachy
 */
#include "Traffic.h"
#include <highgui.h>
#include "math.h"
#include "../Utils/ParamUtils.h"

#include "../Utils/Utils.h"

Traffic::~Traffic() {
	ClearProcessData();

}

void Traffic::ClearProcessData() {
	if (this->_imgredballs != NULL) {
		delete _imgredballs;
		_imgredballs = NULL;
	}
	if (this->_imgyellowsquares != NULL) {
		delete _imgyellowsquares;
		_imgyellowsquares = NULL;
	}
	if (this->_imggreensquares != NULL) {
		delete _imggreensquares;
		_imggreensquares = NULL;
	}
	if (this->_hue != NULL) {
		delete _hue;
		_hue = NULL;
	}
	if (this->_redkol != NULL) {
		delete _redkol;
		_redkol = NULL;
	}
	if (this->_ballkol != NULL) {
		delete _ballkol;
		_ballkol = NULL;
	}
	if (this->_greenkol != NULL) {
		delete _greenkol;
		_greenkol = NULL;
	}
	if (this->_yellowkol != NULL) {
		delete _yellowkol;
		_yellowkol = NULL;
	}
	if (this->_greenPercent != NULL) {
		delete _greenPercent;
		_greenPercent = NULL;
	}
	if (this->_yellowPercent != NULL) {
		delete _yellowPercent;
		_yellowPercent = NULL;
	}
	if (this->_redsquarePercent != NULL) {
		delete _redsquarePercent;
		_redsquarePercent = NULL;
	}

	if (this->_greenthresh != NULL) {
		delete _greenthresh;
		_greenthresh = NULL;
	}
	if (this->_redthreshball != NULL) {
		delete _redthreshball;
		_redthreshball = NULL;
	}
	if (this->_redthreshsquare != NULL) {
		delete _redthreshsquare;
		_redthreshsquare = NULL;
	}
	if (this->_yellowcontour != NULL) {
		delete _yellowcontour;
		_yellowcontour = NULL;
	}

	if (this->_greencontour != NULL) {
		delete _greencontour;
		_greencontour = NULL;
	}
	if (this->_redsquarecontour != NULL) {
		delete _redsquarecontour;
		_redsquarecontour = NULL;
	}
	if (this->_redballcontour != NULL) {
		delete _redballcontour;
		_redballcontour = NULL;
	}
	if (this->_yellowthresh != NULL) {
		delete _yellowthresh;
		_yellowthresh = NULL;
	}

	if (this->_redsquares != NULL) {
		delete _redsquares;
		_redsquares = NULL;
	}
	if (this->_yellowsquares != NULL) {
		delete _yellowsquares;
		_yellowsquares = NULL;
	}
	if (this->_greensquares != NULL) {
		delete _greensquares;
		_greensquares = NULL;
	}

}

void Traffic::SetDefaultParams() {

	_inverseRatio = 1;
	_mindistDivide = 4;
	_upcannyThresh = 100;
	_centerThresh = 50;
	_minRadius = 5;
	_maxRadius = 100;
	_maxmeancircle = 75;
	_maxmeanfakecircle=100;
	_minmeanfakecircle=76;
	_redexist = false ;
	_greenexist = false;
	_yellowexist = false;
	_redminh = 177;
	_redmaxh = 180;
	//_greenminh = 60;
	//_greenmaxh = 67;
	//_yellowminh = 119;
	//_yellowmaxh = 121;
	_redDev=40;
	_greenDev=40;
	_yellowDev=40;
	//_redminh = 157;
	//_redmaxh = 255;
	_greenminh = 72;
	_greenmaxh = 74;
	_yellowminh = 26;
	_yellowmaxh = 30;
	_param1	= 19;
	_param2	= 20;
	_redalpha = 3;
	_greenalpha = 120;
	_yellowalpha = 60;
	//_maxrectratio = 10; //max ratio between height (the longer) and width
	//_minrectratio = 1.1; //min ratio between height (the longer) and width

	_maxrectratio = 5; //max ratio between height (the longer) and width
	_minrectratio = 0; //min ratio between height (the longer) and width


	_minsizerect = 408; //min area of rectangle
	_lowDif = 40;
	_upperDif = 40;
	_borderPixels = 5;
	_percentNormalArea = 0.8; //to determine if the color exist in the picture before kolaman
	_percentKolamanThresh = 0.50; //to determine the maximum size of rect
	_minRectPrecent = 0.2; //min rectangle area percent
	_maxBufferPercent = 0.2;
	_erosionsize = 3;
	_dilationsize = 10;

}
Traffic::Traffic() :
		BaseAlgorithm() {
	_erosiontype = MORPH_RECT;
	_dilationtype = MORPH_RECT;
}
void Traffic::Run(Mat& image) {
	if (_inTest) {
		this->_redsquarePercent = new vector<double>();
		this->_yellowPercent = new vector<double>();
		this->_greenPercent = new vector<double>();
		this->_redsquares = new vector<RotatedRect>();
		this->_yellowsquares = new vector<RotatedRect>();
		this->_greensquares = new vector<RotatedRect>();
	}
	RedBall_current_results[0] = 0;
	RedSquare_current_results.center.x = -1;
	Green_current_results.center.x = -1;
	Yellow_current_results.center.x = -1;
	Run2(image, RedBall_current_results, RedSquare_current_results,
			Yellow_current_results, Green_current_results);
}
void Traffic::InitProcessData() {
	_redkol = NULL;
	_hue = NULL;
	_ballkol = NULL;
	_greenkol = NULL;
	_yellowkol = NULL;
	_greenthresh = NULL;
	_redthreshball = NULL;
	_redthreshsquare = NULL;
	_yellowthresh = NULL;
	_greencontour = NULL;
	_redsquarecontour = NULL;
	_redballcontour = NULL;
	_yellowcontour = NULL;

	_imgredballs = NULL;
	_imgyellowsquares = NULL;
	_imggreensquares = NULL;
}
void Traffic::Load(map<string, string>& params) {
	ParamUtils::setParamPercent(params, "percentKolamanThresh",
			_percentKolamanThresh);
	ParamUtils::setParam(params, "Redalpha", _redalpha);
	ParamUtils::setParam(params, "Greenalpha", _greenalpha);
	ParamUtils::setParam(params, "Yellowalpha", _yellowalpha);
	ParamUtils::setParam(params, "minRedKol", _redminh);
	ParamUtils::setParam(params, "maxRedKol", _redmaxh);
	ParamUtils::setParam(params, "minGreenKol", _greenminh);
	ParamUtils::setParam(params, "maxGreenKol", _greenmaxh);
	ParamUtils::setParam(params, "minYellowKol", _yellowminh);
	ParamUtils::setParam(params, "maxYellowKol", _yellowmaxh);
	ParamUtils::setParam(params, "redDev", _redDev);
	ParamUtils::setParam(params, "greenDev", _greenDev);
	ParamUtils::setParam(params, "yellowDev", _yellowDev);

	ParamUtils::setParam(params, "minRadius", _minRadius);
	ParamUtils::setParam(params, "maxRadius", _maxRadius);
	ParamUtils::setParam(params, "param1", _param1);
	ParamUtils::setParam(params, "param2", _param2);
	ParamUtils::setParam(params, "inverseRatio", _inverseRatio);
	ParamUtils::setParam(params, "maxmeanfakecircle", _maxmeanfakecircle);
	ParamUtils::setParam(params, "minmeanfakecircle", _minmeanfakecircle);
	ParamUtils::setParam(params, "maxmeancircle", _maxmeancircle);

	//ParamUtils::setParam(params, "lowDiff", _lowDif);
	//ParamUtils::setParam(params, "upperDiff", _upperDif);
	//ParamUtils::setParam(params, "border", _borderPixels);
	ParamUtils::setParamPercent(params, "maxrectratio", _maxrectratio);
	ParamUtils::setParamPercent(params, "minrectratio", _minrectratio);
	ParamUtils::setParam(params, "minsizerect", _minsizerect);
	ParamUtils::setParamPercent(params, "minRectPercent", _minRectPrecent);
	ParamUtils::setParamPercent(params, "NormalArea", _percentNormalArea);
	//ParamUtils::setParam(params, "Erosiontype", _erosiontype);
	ParamUtils::setParam(params, "Erosionsize", _erosionsize);
	//ParamUtils::setParam(params, "Dilationtype", _dilationtype);
	ParamUtils::setParam(params, "Dilationsize", _dilationsize);
}
void Traffic::InitResult() {
}

void Traffic::Run2(const Mat& image, Vec3f& redballres,
		RotatedRect& redsquareres, RotatedRect& yellowres,
		RotatedRect& greenres) {
	if (_inTest) {
		this->_redsquarePercent = new vector<double>();
		this->_yellowPercent = new vector<double>();
		this->_greenPercent = new vector<double>();
		this->_redsquares = new vector<RotatedRect>();
		this->_yellowsquares = new vector<RotatedRect>();
		this->_greensquares = new vector<RotatedRect>();
	}
	findBuoys(image, redballres, redsquareres, yellowres, greenres);
}

void Traffic::findBuoys(const Mat& image, Vec3f& redball,
		RotatedRect& redsquare, RotatedRect& yellowsquare,
		RotatedRect& greensquare) {
	//Mat* hue = Atlas::HueChannel(image);
	Mat redkol;
	Mat ballkol;
	Mat yellowkol;
	Mat greenkol;
	Mat kolman32F;
	_hue = Utils::HueChannel(image);
	//redkol = *Utils::SaturationChannel(image);
	//greenkol = *Utils::SaturationChannel(image);
	ballkol = *Utils::SaturationChannel(image);
	ballkol=255-ballkol;
	greenkol=ballkol;
	redkol=ballkol;
	//_redexist = Utils::Kolaman(_redalpha, image, kolman32F, redkol,
	//		_percentKolamanThresh);

	//_greenexist = Utils::Kolaman(_greenalpha, image, kolman32F, greenkol,
	//		_percentKolamanThresh);
	//_yellowexist = Utils::Kolaman(_yellowalpha, image, kolman32F, yellowkol,
	//		_percentKolamanThresh);
	Utils::YellowChannel(image,yellowkol);
	normalize(yellowkol, yellowkol, 0, 255, NORM_MINMAX);

	//Utils::ShowImage(redkol, "redkol");
	//Utils::ShowImage(greenkol, "greenkol");
	//Utils::ShowImage(yellowkol, "yellowkol");
	//Utils::ShowImage(*hue, "hue");
	_maxsizerect = _percentNormalArea * image.size().area();
	if (_inTest) {
		_redkol = new Mat();
		redkol.copyTo(*_redkol);
		_ballkol = new Mat();
		ballkol.copyTo(*_ballkol);
		_greenkol = new Mat();
		greenkol.copyTo(*_greenkol);
		_yellowkol = new Mat();
		yellowkol.copyTo(*_yellowkol);
	}

	Mat *redthreshball = new Mat();
	Mat *redthreshsquare = new Mat();
	Mat *yellowthresh = new Mat();
	Mat *greenthresh = new Mat();
	vector<vector<Point> > redballcontours;
	vector<vector<Point> > redsquarecontours;
	vector<vector<Point> > yellowcontours;
	vector<vector<Point> > greencontours;

	//Mat huescale;
	//redkol->copyTo(huescale);
	ThreshStruct(redkol, _redminh, _redmaxh, *redthreshball, 1);
	if (_inTest) {
		_redthreshball = new Mat();
		(*redthreshball).copyTo(*_redthreshball);
	}
	//Utils::ShowImage(*redthreshball, "redball");
	findContours(*redthreshball, redballcontours, CV_RETR_TREE,
			CV_CHAIN_APPROX_SIMPLE);
	ThreshStruct(redkol, _redminh, _redmaxh, *redthreshsquare, 0);
	if (_inTest) {
		_redthreshsquare = new Mat();
		(*redthreshsquare).copyTo(*_redthreshsquare);
	}
	//Utils::ShowImage(*redthreshsquare, "redsquare");
	findContours(*redthreshsquare, redsquarecontours, CV_RETR_TREE,
			CV_CHAIN_APPROX_SIMPLE);

	ThreshStruct(yellowkol, _yellowminh, _yellowmaxh, *yellowthresh, 0);
	if (_inTest) {
		_yellowthresh = new Mat();
		(*yellowthresh).copyTo(*_yellowthresh);
	}
	//Utils::ShowImage(*yellowthresh, "yellow");
	findContours(*yellowthresh, yellowcontours, CV_RETR_TREE,
			CV_CHAIN_APPROX_SIMPLE);

	ThreshStruct(greenkol, _greenminh, _greenmaxh, *greenthresh, 0);
	if (_inTest) {
		_greenthresh = new Mat();
		(*greenthresh).copyTo(*_greenthresh);
	}
	//Utils::ShowImage(*greenthresh, "green");
	findContours(*greenthresh, greencontours, CV_RETR_TREE,
			CV_CHAIN_APPROX_SIMPLE);

	if (_inTest) {
		_greencontour = new Mat();
		image.copyTo(*_greencontour);
		_redsquarecontour = new Mat();
		image.copyTo(*_redsquarecontour);
		_redballcontour = new Mat();
		image.copyTo(*_redballcontour);
		_yellowcontour = new Mat();
		image.copyTo(*_yellowcontour);

		//drawContours(*_greenthresh, greencontours, 0, Scalar(255, 255, 255), 3);
		for (uint i = 0; i < greencontours.size(); ++i)
			drawContours(*_greencontour, greencontours, i,
					Scalar(255, 255, 255), 2);
		//	Utils::ShowImage(*_greenthresh, "green contours");

		//drawContours(*_yellowthresh, yellowcontours, 0, Scalar(255, 255, 255),3);
		for (uint i = 0; i < yellowcontours.size(); ++i)
			drawContours(*_yellowcontour, yellowcontours, i,
					Scalar(255, 255, 255), 2);
		//		Utils::ShowImage(*_yellowthresh, "yellow contours");

		//	drawContours(*_redthreshball, redballcontours, 0, Scalar(255, 255, 255), 3);
		for (uint i = 0; i < redballcontours.size(); ++i)
			drawContours(*_redballcontour, redballcontours, i,
					Scalar(255, 255, 255), 2);
		//drawContours(*_redthreshsquare, redsquarecontours, 0, Scalar(255, 255, 255), 3);
		for (uint i = 0; i < redsquarecontours.size(); ++i)
			drawContours(*_redsquarecontour, redsquarecontours, i,
					Scalar(255, 255, 255), 2);
		//Utils::ShowImage(*_redthresh, "red contours");
		_imgredballs = new Mat();
		image.copyTo(*_imgredballs);
		_imgredsquare = new Mat();
		image.copyTo(*_imgredsquare);
		_imgyellowsquares = new Mat();
		image.copyTo(*_imgyellowsquares);
		_imggreensquares = new Mat();
		image.copyTo(*_imggreensquares);
	}
	vector<Point> approx;
	double match_area = 0;
	int match_area_idx = -1;
	double RedSquareareaPercent[redsquarecontours.size()];
	double RedBallareaPercent[redballcontours.size()];
	double YellowareaPercent[yellowcontours.size()];
	double GreenareaPercent[greencontours.size()];
	vector<Vec3f> rcircles;
	RotatedRect redRect;
	RotatedRect greenRect;
	RotatedRect yellowRect;

		GaussianBlur( ballkol, ballkol, Size(9, 9), 2, 2 );
		HoughCircles(ballkol, rcircles, CV_HOUGH_GRADIENT, _inverseRatio,
				ballkol.rows / 8, _param1, _param2, 0, 0);
		if (!rcircles.empty()) {
			_maxidxRadiusR = MaxRadius(rcircles, image);
			if (_maxidxRadiusR != -1)
			RedBall_current_results = rcircles.at(_maxidxRadiusR);

		} else
			_maxidxRadiusR = -1;
  if (_redexist) {
		match_area = 0;
		match_area_idx = -1;
		for (uint i = 0; i < redsquarecontours.size(); ++i) {
			approxPolyDP(Mat(redsquarecontours[i]), approx,
					arcLength(Mat(redsquarecontours.at(i)), true) * 0.02, true);
			double area = fabs(contourArea(Mat(approx)));
			if ((area > _minsizerect)
					&& (AreaCheck(image, redsquarecontours.at(i))))
			{
				double shapeHue = Utils::MeanOfShape(*_hue,redsquarecontours[i]);

			//	if (shapeHue*2 >  _redalpha - _redDev && shapeHue*2 < _redalpha + _redDev)
				{
					RedSquareareaPercent[i] = PercentRectangle(
							redsquarecontours.at(i), redRect);
					if (_inTest)
									_redsquares->push_back(redRect);
								if (RedSquareareaPercent[i] > match_area) {
									match_area = RedSquareareaPercent[i];
									match_area_idx = i;
								}
								if (RedSquareareaPercent[i] == match_area) {
									if (redsquarecontours.at(i).at(0).y
											< redsquarecontours.at(match_area_idx).at(0).y) {
										match_area = RedSquareareaPercent[i];
										match_area_idx = i;
									}
								}

				}
			}
		}
		if (match_area_idx != -1)
			redsquare = minAreaRect(redsquarecontours.at(match_area_idx));
		if (_inTest && match_area_idx != -1) {
			vector<Point> rectCountor;
			vector<vector<Point> > b;
			for (uint i = 0; i < _redsquares->size(); ++i) {
				Utils::GetContor(_redsquares->at(i), rectCountor);
				b.push_back(rectCountor);
			}
			drawContours(*_imgredsquare, b, 0, Scalar(0, 255, 255));
			//Utils::ShowImage(*_imgyellowsquares, "yellow squares");
			_redsquarePercent->push_back(RedSquareareaPercent[match_area_idx]);
		}
	}
	if (_yellowexist) {
		match_area = 0;
		match_area_idx = -1;
		for (uint i = 0; i < yellowcontours.size(); ++i) {
			approxPolyDP(Mat(yellowcontours[i]), approx,
					arcLength(Mat(yellowcontours.at(i)), true) * 0.02, true);
			double area = fabs(contourArea(Mat(approx)));
			if ((area > _minsizerect)
					&& (AreaCheck(image, yellowcontours.at(i)))) {
				double shapeHue = Utils::MeanOfShape(*_hue,yellowcontours[i]);
				//if (shapeHue*2 >  _yellowalpha - _yellowDev && shapeHue*2 < _yellowalpha + _yellowDev)
				{
					YellowareaPercent[i] = PercentRectangle(
							yellowcontours.at(i), yellowRect);
					if (_inTest)
									_yellowsquares->push_back(yellowRect);
								if (YellowareaPercent[i] > match_area) {
									match_area = YellowareaPercent[i];
									match_area_idx = i;
								}
								if (YellowareaPercent[i] == match_area) {
									if (yellowcontours.at(i).at(0).y
											< yellowcontours.at(match_area_idx).at(0).y) {
										match_area = YellowareaPercent[i];
										match_area_idx = i;
									}
								}

				}

			}
		}
		if (match_area_idx != -1)
			yellowsquare = minAreaRect(yellowcontours.at(match_area_idx));
		if (_inTest && match_area_idx != -1) {
			vector<Point> rectCountor;
			vector<vector<Point> > b;
			for (uint i = 0; i < _yellowsquares->size(); ++i) {
				Utils::GetContor(_yellowsquares->at(i), rectCountor);
				b.push_back(rectCountor);
			}
			drawContours(*_imgyellowsquares, b, 0, Scalar(0, 255, 255));
			//Utils::ShowImage(*_imgyellowsquares, "yellow squares");
			_yellowPercent->push_back(YellowareaPercent[match_area_idx]);
		}
	}
	if (_greenexist) {
		match_area = 0;
		match_area_idx = -1;
		for (uint i = 0; i < greencontours.size(); ++i) {
			approxPolyDP(Mat(greencontours.at(i)), approx,
					arcLength(Mat(greencontours.at(i)), true) * 0.02, true);
			double area = fabs(contourArea(Mat(approx)));
			if ((area > _minsizerect)
					&& (AreaCheck(image, greencontours.at(i)))) {

				double shapeHue = Utils::MeanOfShape(*_hue,greencontours[i]);
			//				if (shapeHue*2 >  _greenalpha - _greenDev && shapeHue*2 < _greenalpha + _greenDev)
							{
								GreenareaPercent[i] = PercentRectangle(
										greencontours.at(i), greenRect);
								if (_inTest)
												_greensquares->push_back(greenRect);
											if (GreenareaPercent[i] > match_area) {
												match_area = GreenareaPercent[i];
												match_area_idx = i;
											}
											if (GreenareaPercent[i] == match_area) {
												if (greencontours.at(i).at(0).y
														< greencontours.at(match_area_idx).at(0).y) {
													match_area = GreenareaPercent[i];
													match_area_idx = i;
												}
											}

							}
			}
		}
		if (match_area_idx != -1)
			greensquare = minAreaRect(greencontours.at(match_area_idx));
		if (_inTest && match_area_idx != -1) {

			vector<Point> rectCountor;
			vector<vector<Point> > b;
			for (uint i = 0; i < _greensquares->size(); ++i) {
				Utils::GetContor(_greensquares->at(i), rectCountor);
				b.push_back(rectCountor);
			}
			drawContours(*_imggreensquares, b, 0, Scalar(0, 255, 255));
			//Utils::ShowImage(*_imggreensquares, "green squares");
			_greenPercent->push_back(GreenareaPercent[match_area_idx]);
		}
	}
	delete redthreshball;
	delete redthreshsquare;
	delete yellowthresh;
	delete greenthresh;

}
void Traffic::ThreshStruct(Mat& Hue_scale, int minh, int maxh, Mat& colorthresh,
		int flag) {
	if (flag == 0) //square
			{
		Mat erosion, dilation, erode_element, dilate_element;
		inRange(Hue_scale, minh, maxh, colorthresh);
		//Utils::ShowImage(colorthresh, "colorthresh1");
		erode_element = getStructuringElement(_erosiontype,
				Size(2 * _erosionsize + 1, 2 * _erosionsize + 1),
				Point(_erosionsize, _erosionsize));
		erode(colorthresh, erosion, erode_element);
		//Utils::ShowImage(erosion, "erode");
		dilate_element = getStructuringElement(_dilationtype,
				Size(2 * _dilationsize + 1, 2 * _dilationsize + 1),
				Point(_dilationsize, _dilationsize));
		dilate(erosion, colorthresh, dilate_element);
		//Utils::ShowImage(colorthresh, "dilate");
	} else if (flag == 1) //ball
			{
		Mat closed, open;
		Mat se21 = getStructuringElement(MORPH_ELLIPSE, Size(11, 11),
				Point(10, 10));
		Mat se11 = getStructuringElement(MORPH_ELLIPSE, Size(11, 11),
				Point(5, 5));
		inRange(Hue_scale, minh, maxh, colorthresh);
		// Utils::ShowImage(thresholded,"thresholded");
		morphologyEx(colorthresh, closed, MORPH_CLOSE, se21);
		//Utils::ShowImage(closed,"closed");
		morphologyEx(closed, open, MORPH_OPEN, se11);
		//Utils::ShowImage(open,"open");
		open.copyTo(colorthresh);
	}
}

bool Traffic::AreaCheck(const Mat& image, vector<Point>& contour) {
	double area = contourArea(contour);
	//check if area is big enough
	if (area < _maxsizerect) {
		RotatedRect rect;
		//check if area is rectangle
		if (PercentRectangle(contour, rect) > _minRectPrecent) {
			//fix angle
			Utils::fixAngle(rect);
			_rectratio = rect.size.height / rect.size.width;
			//check if area is rectangle and vertical
			if (((abs(rect.angle) <= 12)||(abs(rect.angle) >= 80)) && (_rectratio < _maxrectratio)
					&& (_rectratio > _minrectratio)) {
				return true;
			} else
				return false;

		} else
			return false;
	} else
		return false;
}
int Traffic::MaxRadius(vector<Vec3f>& circles,const Mat& image) {
	int Maxidx = -1, MaxValue = 0;
	if (!circles.empty()) {
		for (size_t i = 0; i < (circles).size(); i++) {
			int radiusz = cvRound((circles)[i][2]);
			int x=cvRound((circles)[i][0]);
			int y=cvRound((circles)[i][1]);
			float teta=(2*3.14)/12;
			vector<Point> ballcontour;
			for (int j=0;j<12;j++)
			{
				Point center(x+radiusz*cos(teta*j),y+radiusz*sin(teta*j));
				ballcontour.push_back(center);
			}
		//	Mat hue;
		//	_hue->copyTo(hue);
			double circlemean=Utils::MeanOfShape(*_hue,ballcontour);
			//printf("Ball! p=%f\n\r",circlemean);
			if (_inTest) {
					vector<vector<Point> > b;
					b.push_back(ballcontour);
					drawContours(*_imgredballs, b, 0, Scalar(0, 255, 255));
				}
			if ((cvRound((circles).at(i)[2])) > MaxValue && circlemean<_maxmeancircle) {
				Maxidx = i;
				MaxValue = (circles).at(i)[2];
			}
			else if (circlemean<_maxmeanfakecircle && circlemean>_minmeanfakecircle)   {
				Point  one(x-2*radiusz,y+radiusz);
				Point two(x+2*radiusz,y+radiusz);
				Point three(x-2*radiusz,y+radiusz+2*radiusz);
				Point four(x+2*radiusz,y+radiusz+2*radiusz);
				RotatedRect roi(Point (x,y+radiusz),Size (2*radiusz,2*radiusz),0);
				Mat ROI;
				Mat *HUE;
				Utils::GetROI(image, roi,ROI);
				//HUE = Utils::HueChannel(ROI);
				//Utils::ShowImage(*_hue,"HUEEEE");
				double redper=Utils::ColorPercent(*_hue,ballcontour,_redminh*2, _redmaxh*2,360);
				double greenper=Utils::ColorPercent(*_hue,ballcontour,_greenminh*2, _greenmaxh*2,360);
				double yellowper=Utils::ColorPercent(*_hue,ballcontour,_yellowminh*2, _yellowmaxh*2,360);
				if (redper>greenper && redper>yellowper &&redper>0.018)
					RedSquare_current_results=roi;
				else if (greenper>redper && greenper>yellowper &&greenper>0.018)
					Green_current_results=roi;
				else if (yellowper>greenper && yellowper>redper &&yellowper>0.018)
					Yellow_current_results=roi;
				/*else if (RedSquare_current_results.center.x == -1)
					RedSquare_current_results=roi;
				else if (Green_current_results.center.x == -1)
					Green_current_results=roi;
				else if (Yellow_current_results.center.x == -1)
					Yellow_current_results=roi;
				*/
				//Utils::ShowImage(ROI,"ROI");
				drawRect(*_imgredballs, roi, Scalar(0, 120, 120));
			}
		}
	}
	return Maxidx;
}
bool Traffic::isRed(vector<Point> countor, const Mat& hueLayer) {
//maxBuffer because the rectangle contains part of the water.
//after orange,come yellow and then green.
	double percent = Utils::ColorPercent(hueLayer, countor, _redminh,
			_redmaxh * (1 + _maxBufferPercent));
	if (_inTest) {
		_redsquarePercent->push_back(percent);

	}
	return (percent >= _minRectPrecent);
}
bool Traffic::isYellow(vector<Point> countor, const Mat& hueLayer) {
//maxBuffer because the retangle contains part of the water.
//after orange,come yellow and then green.
	double percent = Utils::ColorPercent(hueLayer, countor, _yellowminh,
			_yellowmaxh * (1 + _maxBufferPercent));
	if (_inTest) {
		_yellowPercent->push_back(percent);
	}
	return (percent >= _minRectPrecent);
}
bool Traffic::isGreen(vector<Point> countor, const Mat& hueLayer) {
//maxBuffer because the retangle contains part of the water.
//after orange,come yellow and then green.
	double percent = Utils::ColorPercent(hueLayer, countor, _greenminh,
			_greenmaxh * (1 + _maxBufferPercent));
	if (_inTest) {
		_greenPercent->push_back(percent);
	}
	return (percent >= _minRectPrecent);
}
void Traffic::drawRect(Mat& draw, const RotatedRect& square, Scalar color) {
	if (square.center.x != -1) {
		Utils::drawRotatedRect(square, draw, color, 1);
		int l = MAX(square.size.width,square.size.height);
//we draw the angle from X axis and the rectangle contains the
//angle from Y axis
//TODO: change  to my object/Yuval object who contains the data as i need it.
		drawAngle(90 - square.angle, l, square.center, draw);
		drawPoint(square.center, draw);
	}
}

void Traffic::drawAngle(float angle, int length, const Point& center,
		Mat& draw) {
	float PI = 3.14159265;
// y is "opposite" because is start from top and not from bottom
	int y1 = center.y - sin(angle * PI / 180.0) * (length / 2);
	int x1 = center.x + cos(angle * PI / 180.0) * (length / 2);
	int y2 = center.y + sin(angle * PI / 180.0) * (length / 2);
	int x2 = center.x - cos(angle * PI / 180.0) * (length / 2);

	line(draw, Point(x1, y1), Point(x2, y2), Scalar(255, 0, 0), 2, CV_AA);
}

void Traffic::drawPoint(const Point& center, Mat& draw) {
	rectangle(draw, center, center, Scalar(255, 255, 255), 3);
}

//check similarity by area
double Traffic::PercentRectangle(const vector<Point>& contour,
		RotatedRect& outRect) {
	outRect = minAreaRect(contour);
	vector<Point> rectCountor;
	Utils::GetContor(outRect, rectCountor);
	double area = contourArea(contour);
	double rectArea = contourArea(rectCountor);
	double maxArea = max(area, rectArea);
	double minArea = min(area, rectArea);
	return (minArea / maxArea);
}
void Traffic::ToMesseges(vector<MissionControlMessage>& res) {
	//Message:
	if (RedBall_current_results[0] != 0) {
		MissionControlMessage redmsg;
		redmsg.MissionCode = Traffics;
		redmsg.additionalInformation = {0}; //0=red ball
		vector<Point> bounds;
		redmsg.bounds.push_back(
				std::pair<int, int>(RedBall_current_results[0],
						RedBall_current_results[1])); //x,y

		redmsg.intrestPoints.push_back(
				std::pair<int, int>(RedBall_current_results[2],
						RedBall_current_results[2])); //radius
		res.push_back(redmsg);
	}
	if (RedSquare_current_results.center.x != -1) {
		MissionControlMessage redmsg;
		redmsg.MissionCode = Traffics;
		redmsg.additionalInformation = {1}; //1=red square
		vector<Point> bounds;
		Utils::FillRectBounds(RedSquare_current_results, redmsg);
		if (RedSquare_current_results.size.height
				>= RedSquare_current_results.size.width)
			redmsg.bounds.push_back(
					std::pair<int, int>(RedSquare_current_results.size.height,
							RedSquare_current_results.size.width));
		else
			redmsg.bounds.push_back(
					std::pair<int, int>(RedSquare_current_results.size.width,
							RedSquare_current_results.size.height));
		redmsg.intrestPoints.push_back(
				std::pair<int, int>(RedSquare_current_results.center.x,
						RedSquare_current_results.center.y));
		res.push_back(redmsg);
	}
	if (Green_current_results.center.x != -1) {
		MissionControlMessage greenmsg;
		greenmsg.MissionCode = Traffics;
		greenmsg.additionalInformation = {2}; //2=green
		vector<Point> bounds;
		Utils::FillRectBounds(Green_current_results, greenmsg);
		if (Green_current_results.size.height
				>= Green_current_results.size.width)
			greenmsg.bounds.push_back(
					std::pair<int, int>(Green_current_results.size.height,
							Green_current_results.size.width));
		else
			greenmsg.bounds.push_back(
					std::pair<int, int>(Green_current_results.size.width,
							Green_current_results.size.height));
		greenmsg.intrestPoints.push_back(
				std::pair<int, int>(Green_current_results.center.x,
						Green_current_results.center.y));
		res.push_back(greenmsg);
	}
	if (Yellow_current_results.center.x != -1) {
		MissionControlMessage yellowmsg;
		yellowmsg.MissionCode = Traffics;
		yellowmsg.additionalInformation = {3}; //3=yellow
		vector<Point> bounds;
		Utils::FillRectBounds(Yellow_current_results, yellowmsg);
		if (Yellow_current_results.size.height
				>= Yellow_current_results.size.width)
			yellowmsg.bounds.push_back(
					std::pair<int, int>(Yellow_current_results.size.height,
							Yellow_current_results.size.width));
		else
			yellowmsg.bounds.push_back(
					std::pair<int, int>(Yellow_current_results.size.width,
							Yellow_current_results.size.height));
		yellowmsg.intrestPoints.push_back(
				std::pair<int, int>(Yellow_current_results.center.x,
						Yellow_current_results.center.y));
		res.push_back(yellowmsg);
	}
}
void Traffic::PaintRed(Mat& hue) {
	for (int y = 0; y < hue.rows; ++y) {
		for (int x = 0; x < hue.cols; ++x) {
			uchar pixelHue = hue.at<uchar>(Point(x, y));
			if ((_redminh / 2 <= pixelHue) && (pixelHue <= _redmaxh / 2)) {
				//fill the paths with black color because find counters need zero and non zero pixels
				//the difference calculate from the source pixel because it is easy
				//to define the differences but could change
				floodFill(hue, Point(x, y), Scalar(0), NULL,
						Scalar(_lowDif / 2), Scalar(_upperDif / 2),
						CV_FLOODFILL_FIXED_RANGE);

			}
		}
	}
}
void Traffic::PaintYellow(Mat& hue) {
	for (int y = 0; y < hue.rows; ++y) {
		for (int x = 0; x < hue.cols; ++x) {
			uchar pixelHue = hue.at<uchar>(Point(x, y));
			if ((_yellowminh / 2 <= pixelHue)
					&& (pixelHue <= _yellowmaxh / 2)) {
				//fill the paths with black color because find counters need zero and non zero pixels
				//the difference calculate from the source pixel because it is easy
				//to define the differences but could change
				floodFill(hue, Point(x, y), Scalar(0), NULL,
						Scalar(_lowDif / 2), Scalar(_upperDif / 2),
						CV_FLOODFILL_FIXED_RANGE);

			}
		}
	}
}
void Traffic::PaintGreen(Mat& hue) {
	for (int y = 0; y < hue.rows; ++y) {
		for (int x = 0; x < hue.cols; ++x) {
			uchar pixelHue = hue.at<uchar>(Point(x, y));
			if ((_greenminh / 2 <= pixelHue) && (pixelHue <= _greenmaxh / 2)) {
				//fill the paths with black color because find counters need zero and non zero pixels
				//the difference calculate from the source pixel because it is easy
				//to define the differences but could change
				floodFill(hue, Point(x, y), Scalar(0), NULL,
						Scalar(_lowDif / 2), Scalar(_upperDif / 2),
						CV_FLOODFILL_FIXED_RANGE);

			}
		}
	}
}
void Traffic::DrawCircle(Vec3f& rcircles, Mat& frame) {
	if (RedBall_current_results[0] != 0) {
		Point center(cvRound((rcircles)[0]), cvRound((rcircles)[1]));
		int radius = cvRound((rcircles)[2]);
		//printf("Ball! p=%f\n\r",Point.center);
		// circle center
		circle(frame, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		// circle outline
		circle(frame, center, radius, Scalar(0, 255, 0), 3, 8, 0);
	}
}
void Traffic::fixResults(Point offset) {
	if (RedBall_current_results[0] != 0) {
		RedBall_current_results[0] += offset.x;
		RedBall_current_results[1] += offset.y;

	}
	if (RedSquare_current_results.center.x != -1) {
		RedSquare_current_results.center.x += offset.x;
		RedSquare_current_results.center.y += offset.y;
	}
	if (Green_current_results.center.x != -1) {
		Green_current_results.center.x += offset.x;
		Green_current_results.center.y += offset.y;
	}
	if (Yellow_current_results.center.x != -1) {
		Yellow_current_results.center.x += offset.x;
		Yellow_current_results.center.y += offset.y;
	}
}
void Traffic::Draw(Mat& draw) {
	if (RedBall_current_results[0] != 0) {
		DrawCircle(RedBall_current_results, draw);
	}
	if (RedSquare_current_results.center.x != -1) {
		drawRect(draw, RedSquare_current_results, Scalar(255, 0, 0));
	}
	if (Green_current_results.center.x != -1) {
		drawRect(draw, Green_current_results, Scalar(0, 255, 0));
	}
	if (Yellow_current_results.center.x != -1) {
		drawRect(draw, Yellow_current_results, Scalar(0, 120, 120));
	}
}
