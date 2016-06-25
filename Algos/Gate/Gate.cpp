/*
 * Gate.cpp
 *
 *  Created on: Feb 26, 2013
 *      Author: raz
 */
#define _USE_MATH_DEFINES
#include <algorithm>
#include "Gate.h"
#include <cvaux.h>
#include <highgui.h>
#include <cxcore.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "../Utils/Utils.h"
#include "../Utils/ParamUtils.h"
#include "../Traffic/Traffic.h"


Gate::Gate(Traffic* itzik): BaseAlgorithm()
{
	_gate = NULL;
	_erosiontype = MORPH_RECT;
	_dilationtype = MORPH_RECT;
	//_itzik = itzik;
}

Gate::~Gate()
{


}

void Gate::ClearProcessData()
{
	if(this->_kolman!= NULL)
	{
		delete _kolman;
		_kolman = NULL;
	}
	if(this->_BW!= NULL)
	{
		delete _BW;
		_BW = NULL;
	}
	if(this->_contourImage!= NULL)
	{
		delete _contourImage;
		_contourImage = NULL;
	}
	if(this->_outputIm!= NULL)
	{
		delete _outputIm;
		_outputIm = NULL;
	}
	if(this->_rectIm!= NULL)
	{
		delete _rectIm;
		_rectIm = NULL;
	}
	if(this->_eroded!= NULL)
	{
		delete _eroded;
		_eroded = NULL;
	}
	if(this->_dilated!= NULL)
	{
		delete _dilated;
		_dilated = NULL;
	}
	if(this->_roiFrame!= NULL)
	{
		delete _roiFrame;
		_roiFrame = NULL;
	}

	if (this->_areaPercent!=NULL)
	{
		delete _areaPercent;
		_areaPercent = NULL;
	}
	if (this->_horizonPole!=NULL)
	{
		delete _horizonPole;
		_horizonPole = NULL;
	}
	if (this->_gateVerticalPoles!=NULL)
	{
		delete _gateVerticalPoles;
		_gateVerticalPoles = NULL;
	}
}

void Gate::InitProcessData()
{
	_kolman=NULL;
	_BW=NULL;
	_contourImage=NULL;
	_outputIm = NULL;
	_rectIm = NULL;
	_eroded = NULL;
	_dilated = NULL;
	_roiFrame = NULL;
	_areaPercent = NULL;
	_gateVerticalPoles = NULL;
	_horizonPole = NULL;
}


void Gate::SetDefaultParams()
{

	_treshBlockSize = 273;
	_minCurves = 5;
	_objectImageFactor = 0.003;
	_baseFactor = 0.4;
	_minAngle = 65;
	_maxAngle = 25;
	_areaSimilarity = 0.37;
	_baseWidthTOimageFactor = 0.15;
	_roiOriginX_TO_BaseWidthFactor = 0.5;
	_minPixelsInROI = 1;
	_YverticPolesMaxDiff = 0.5;
	_XverticPolesMaxDiff = 1;
	_XverticPolesMinDiff = 0.5;
	_ConnectVerticAreaSimilarityMin = 0.4;
	_ConnectVerticAreaSimilarityMax = 1;
	_greenAlpha = 140;
	_kolmanThresh = 0.56;
	_erosionsize = 3;
	_dilationsize = 5;
	_minRectHeight = 0.14;
	_XminDistVertHoriz = 0.3;
	_XmaxDistVertHoriz = 0.7;
	_YminDistVertHoriz = 0.35;
	_YmaxDistVertHoriz = 0.75;
	_minHorVerLeng = 1.5;
	_maxHorVerLeng = 6;

}

void Gate::Run(Mat& image)
{
	this->_gateVertices.clear();
	this->_gate = new RotatedRect();
	_gate->center.x = -1;
	_gate->center.y = -1;
	_isPole = false;
	_isTrafficGreen	= true;
	Run2(image,*_gate );
}

void Gate::Load(map<string, string>& params)
{

	ParamUtils::setParam(params, "treshBlockSize", _treshBlockSize);
	ParamUtils::setParam(params, "minCurves", _minCurves);
	double tmp;
	stringstream(params["objectImageFactor"]) >> tmp;
	_objectImageFactor = tmp/1000;
	ParamUtils::setParamPercent(params, "decision-roi_OrAllPic", _baseFactor);
	ParamUtils::setParam(params, "minAngle", _minAngle);
	ParamUtils::setParam(params, "maxAngle",_maxAngle);
	ParamUtils::setParamPercent(params, "AreaRatio", _areaSimilarity);
	ParamUtils::setParamPercent(params, "minWidthHorizonPole", _baseWidthTOimageFactor);
	ParamUtils::setParamPercent(params, "originXInRoi", _roiOriginX_TO_BaseWidthFactor);
	ParamUtils::setParam(params, "minPixInObjInROI", _minPixelsInROI);
	ParamUtils::setParamPercent(params, "maxYDifVertPoles", _YverticPolesMaxDiff);
	ParamUtils::setParamPercent(params, "maxXDifVertPoles", _XverticPolesMaxDiff);
	ParamUtils::setParamPercent(params, "minXDifVertPoles", _XverticPolesMinDiff);
	ParamUtils::setParamPercent(params, "min2VerticAreaSim", _ConnectVerticAreaSimilarityMin);
	ParamUtils::setParamPercent(params, "max2VerticAreaSim", _ConnectVerticAreaSimilarityMax);
	ParamUtils::setParam(params, "erosionsize", _erosionsize);
	ParamUtils::setParam(params, "dilationsize", _dilationsize);
	ParamUtils::setParam(params, "greenAlpha", _greenAlpha);
	ParamUtils::setParamPercent(params, "kolmanThresh", _kolmanThresh);
	ParamUtils::setParamPercent(params, "minVertRectHeight", _minRectHeight);
	ParamUtils::setParamPercent(params, "Xmin_H_V_Dist", _XminDistVertHoriz);
	ParamUtils::setParamPercent(params, "Xmax_H_V_Dist", _XmaxDistVertHoriz);
	ParamUtils::setParamPercent(params, "Ymin_H_V_Dist", _YminDistVertHoriz);
	ParamUtils::setParamPercent(params, "Ymax_H_V_Dist", _YmaxDistVertHoriz);
	ParamUtils::setParamPercent(params, "min_H_V_Leng", _minHorVerLeng);
	ParamUtils::setParamPercent(params, "max_H_V_Leng", _maxHorVerLeng);


}
void Gate::ToMesseges(vector<MissionControlMessage>& res)
{
	//Message:
	//additionalInformation is a boolean that determine is Traffic green or not.
	//first 4 points of bounds are the vertices points of the black gate in that order:
	//Bottom Left,Top Left ,Top Right ,Bottom Right.
	//intrestPoints contain the center point of the gate
	//if bounds is empty then center is just the horizon pole
	if(_gate->center.x != -1 and _gate->center.y != -1)
	{
		MissionControlMessage msg;
		msg.MissionCode = GreenGate;
		if(_isTrafficGreen)
			msg.additionalInformation = 1; //traffic is GREEN
		else msg.additionalInformation = 0; //traffic is RED -WAIT!!
		if (!_gateVertices.empty())
		{
			for (uint i = 0; i<4; ++i)
				msg.bounds.push_back(std::pair<int, int>(_gateVertices.at(i).x,_gateVertices.at(i).y));
		}
		msg.intrestPoints.push_back(std::pair<int,int>(_gate->center.x,_gate->center.y));
		res.push_back(msg);
	}
	else if(_isPole)
	{
		MissionControlMessage msg;
		msg.intrestPoints.push_back(std::pair<int,int>(_horizonPole->center.x,_horizonPole->center.y));
		res.push_back(msg);
	}
}

void Gate::Run2(Mat& frame,RotatedRect& gate)
{
	if(_inTest)
	{
		this->_kolman = new Mat();
		this->_BW = new Mat();
		this->_eroded = new Mat();
		this->_dilated = new Mat();
		this->_contourImage = new Mat();
		this->_rectIm = new Mat();
		this->_outputIm = new Mat();
		this->_roiFrame = new Mat();
		this->_areaPercent = new vector<double>();
	}
	this->_horizonPole = new RotatedRect();
	this->_gateVerticalPoles = new vector<RotatedRect>();
/*
// A.taking "smart" grayscale channell
	//Mat tempFrame = Utils::DeleteRedChannel(frame);//delete red channel from each frame.
	Mat* hueChannel = Utils::HueChannel(frame);//converting to HSV and taking only hueChannel
	//from frame without red channel
//Utils::ShowImage(*hueChannel,"hue");

//B. scaling grayScale channel to be robust for different light conditions.
	Mat gray;
	hueChannel->copyTo(gray);
	Mat ScaledGray = Utils::ScaledChannel(gray,180);//making the hueChannel robust for light change
													//by linear transformation
//Utils::ShowImage(ScaledGray ,"scale");
*/

//B: color mapping

	Mat kolman;
	Mat kolman32F;
	bool OrangeExists =  true; //Utils::Kolaman(_greenAlpha, frame, kolman32F, kolman,_kolmanThresh);
	if (_inTest)
	{
		_kolman = new Mat();
		kolman.copyTo(*_kolman);
	}

	//color map with yellow
	//Utils::YellowChannel(frame,kolman);
	kolman = *Utils::HueChannel(frame);
	normalize(kolman, kolman, 0, 255, NORM_MINMAX);
	//Utils::ShowImage(kolman,"gray");

	if (!OrangeExists)
	{
		if(_inTest)
		{
			frame.copyTo(*_BW);
			frame.copyTo(*_eroded);
			frame.copyTo(*_dilated);
			frame.copyTo(*_contourImage);
			frame.copyTo(*_rectIm);
			frame.copyTo(*_outputIm);
			frame.copyTo(*_roiFrame);
		}
		return;
	}

//C.tresholding the scaled huechannel
	Mat BW(frame.size().width, frame.size().height, CV_8UC1);
	Mat eroded(frame.size().width, frame.size().height, CV_8UC1);
	Mat dilated(frame.size().width, frame.size().height, CV_8UC1);
	Mat erode_element, dilate_element;
	if (_treshBlockSize <=1)
		_treshBlockSize =3;
	if (_treshBlockSize%2 ==0)
		++_treshBlockSize;
	Scalar mean,dev;
	meanStdDev(kolman,mean,dev);
	_C = dev(0);
		adaptiveThreshold(kolman,BW,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV,_treshBlockSize,-_C);
		//Utils::ShowImage(BW,"BW");
//D. morphologic operation
	erode_element = getStructuringElement(_erosiontype,
	Size(2 * _erosionsize + 1, 2 * _erosionsize + 1),
	Point(_erosionsize, _erosionsize));
	erode(BW, eroded, erode_element);
	dilate_element = getStructuringElement(_dilationtype,
			Size(2 * _dilationsize + 1, 2 * _dilationsize + 1),
			Point(_dilationsize, _dilationsize));
	dilate(eroded, dilated, dilate_element);
	Mat tempBW;// image saved for last step - finding vertical poles
	dilated.copyTo(tempBW);
	//Utils::ShowImage(dilated,"dilate");

	if(_inTest)
	{
		kolman.copyTo(*_kolman);
		BW.copyTo(*_BW);
		eroded.copyTo(*_eroded);
		dilated.copyTo(*_dilated);
	}

//E. find all object contours
	vector<vector<Point> > contours;
	findContours(dilated,contours,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);
	if (_inTest)
	{
		frame.copyTo(*_contourImage);
		frame.copyTo(*_rectIm);
		frame.copyTo(*_outputIm);
		for (uint i=0; i<contours.size();++i)
			drawContours(*_contourImage,contours,i,Scalar(255,255,255),1);
//Utils::ShowImage(*_contourImage,"contours");
	}

//F.*******find horizontal pole********
	vector<RotatedRect> horizonRects;
	RotatedRect horizonPole;
	horizonPole.center.x = -1;
	horizonPole.center.y = -1;
	vector<int> index;
	vector<int>  rectWidth;
	vector<int>  rectHeight;
	int horizonRectWidth,horizonRectHeight;
	for (uint i=0,j=0; i<contours.size();++i)
	{
		//filter noise
		if (contours.at(i).size() < _minCurves ) continue;
		double area = contourArea(contours.at(i));
		if (area < _objectImageFactor *frame.size().area())
			continue;

		//check if object is a vertical rectangle
		RotatedRect rect;
		rectWidth.push_back(0);
		rectHeight.push_back(0);
		if (isHorizontalRectangle(contours.at(i),rect,frame.size().width,rectWidth.at(j),rectHeight.at(j)))
		{
			if(Utils::MeanOfShape(kolman,contours.at(i))> mean(0)) continue;
			if(rect.size.area() > 0.15*frame.size().area()) continue;
			index.push_back(i);
			horizonRects.push_back(rect);
			++j;
		}
	}
//G. check if horizontal pole is found
	findHorizonPole(horizonRects,horizonPole,contours,index,rectWidth,horizonRectWidth,rectHeight,horizonRectHeight);

	if((horizonPole.center.x == -1) or (horizonPole.center.y == -1) )
	{
		_isPole = false;
		if(_inTest) frame.copyTo(*_roiFrame);
		return;
	}
	_isPole = true;
	*_horizonPole = horizonPole;

//H. select ROI based on horizon pole
	Rect roi;
	Mat roiFrame,roiFrameColor;
	Point newOrigin;
	if(max(horizonPole.size.width,horizonPole.size.height) < _baseFactor*frame.size().width )
	{
		getRoi(frame,horizonPole,roi,horizonRectWidth,horizonRectHeight,newOrigin.x,newOrigin.y);
		roiFrame = Mat(tempBW,roi);
		roiFrameColor = Mat(frame,roi);
//Utils::ShowImage(roiFrame,"roi");

	}
	else
	{
		tempBW.copyTo(roiFrame);
		frame.copyTo(roiFrameColor);
		roi.x =  0;roi.y = 0;roi.height = tempBW.size().height;roi.width = tempBW.size().width;
	}
	if(_inTest)
		roiFrame.copyTo(*_roiFrame);

//I. find 2 vertical poles (or one)

	//check for each contour  if it is the gate poles
	vector<int> vertIndex;
	int j = 0;
	vector<RotatedRect> verticalPoles;
	for (uint i=0; i<contours.size();++i)
	{
		//filter noise
		if (contours.at(i).size() < _minCurves ) continue;
		double area = contourArea(contours.at(i));
		//double a = _objectImageFactor *frame.size().area();
		if (area < _objectImageFactor *roiFrame.size().area())
			continue;
		if(!isInRoi(contours.at(i),roi))continue;
			//check if object is a vertical rectangle
		RotatedRect rect;
		if (isVerticalRectangle(contours.at(i),rect,roiFrame))
		{
			verticalPoles.push_back(rect);
			vertIndex.push_back(i);
			++j;
		}
	}
	vector<RotatedRect> gateVerticPoles;
	findVerticalPoles(verticalPoles,gateVerticPoles,contours,vertIndex,horizonPole);
	if(isFindGate(roiFrameColor,gateVerticPoles,gate,_gateVertices,horizonPole, horizonRectWidth))
	{
		if(gateVerticPoles.size() < 2)
			//found gate with 1 left vertical pole
			findGateVertices(horizonPole,gateVerticPoles,_gateVertices,gate,0);
		else
			////found gate with 2 vertical poles
		findGateVertices(horizonPole,gateVerticPoles,_gateVertices,gate,1);
		if(_inTest)
		{
			_gateVerticalPoles->swap(gateVerticPoles);
			//Draw(*_outputIm);
	//Utils::ShowImage(*_outputIm,"gateCenter");
		}
	}

}






	bool Gate::isHorizontalRectangle( const vector<Point>& contour,RotatedRect& outRect,int frameWidth,int& rectWidth,int& rectHeight)
	{
		outRect = minAreaRect(contour);
		vector<Point> rectContour;
		Utils::GetContor(outRect,rectContour);
		double area= contourArea(contour);
		double rectArea  = contourArea(rectContour);
		double maxArea = max(area,rectArea);
		double minArea = min(area,rectArea);

		if((abs(outRect.angle) >= _minAngle) or (abs(outRect.angle) <= _maxAngle) )
		{
			if(!(isHorizon(rectContour,rectWidth,rectHeight))) return false;
			if(rectWidth <= _baseWidthTOimageFactor*frameWidth)return false;
			if ((_inTest) and ((minArea/maxArea) > _areaSimilarity))
			{
				_areaPercent->push_back(minArea/maxArea);
				vector<vector<Point> > c;
				c.push_back(rectContour);
				drawContours(*_rectIm,c,0,Scalar(0,0,255));
//Utils::ShowImage(*_contourImage ,"rect");
			}
			if((minArea/maxArea) > _areaSimilarity)
				return true ;
		}
		return false;
	}


	bool Gate::isHorizon(vector<Point>& rectContour,int& rectWidth,int& rectHeight)
	{
		int minIdx = -1,minIdy = -1,maxIdx = 0 ,maxIdy = 0,tempX,tempY,height,width;
		for (uint i=0; i<rectContour.size();++i)
		{
			tempX = rectContour[i].x;
			tempY = rectContour[i].y;
			maxIdx = max(tempX,maxIdx);
			maxIdy = max(tempY,maxIdy);
			minIdx = min(tempX,minIdx);
			minIdy = min(tempY,minIdy);
			if(minIdx == -1) minIdx = rectContour[i].x;
			if(minIdy == -1) minIdy = rectContour[i].y;
		}
		width = maxIdx - minIdx;
		height = maxIdy - minIdy;
		if(height <= width)
		{
			rectWidth = width;
			rectHeight = height;
			return true;
		}
		return false;

	}

	void Gate::findHorizonPole(vector<RotatedRect> horizonRects,RotatedRect& horizonPole,vector<vector<Point> > contours,vector<int> index,vector<int> rectWidth,int& horizonRectWidth,vector<int>  rectHeight,int& horizonRectHeight)
	{
		if(horizonRects.empty()) return;
		vector<double> matchArea;
		vector<double> matchRatio;
		double maxMatch = 0;
		double maxMatchRat = 0;
		double maxMatchAre = 0;
		int maxIndexAre = -1;
		int maxIndexRat = -1;
		int maxIndex = -1;
		int j = 0;
		int tempHeight;
		int tempWidth;
		for (uint i=0; i<horizonRects.size();++i)
		{
			vector<Point> rectContour;
			Utils::GetContor(horizonRects.at(i),rectContour);
			double area= contourArea(contours.at(index.at(i)));
			double rectArea  = contourArea(rectContour);
			double maxArea = max(area,rectArea);
			double minArea = min(area,rectArea);
			matchArea.push_back(minArea/maxArea);
			maxMatchAre = max(maxMatchAre,matchArea.at(i));
			if(maxMatchAre == matchArea.at(i)) maxIndexAre= i;
			int height= min(horizonRects.at(i).size.height,horizonRects.at(i).size.width);
			int width= max(horizonRects.at(i).size.height,horizonRects.at(i).size.width);
			matchRatio.push_back(width/height);
			maxMatchRat = max(maxMatchRat,matchRatio.at(i));
			if(maxMatchRat == matchRatio.at(i)) maxIndexRat= i;

		}
		if(maxIndexRat == -1 and maxIndexAre != -1)
			maxIndex = maxIndexAre ;
		if(maxIndexAre == -1 and maxIndexRat != -1)
			maxIndex = maxIndexRat ;
		if(maxIndexAre == maxIndexRat )
			maxIndex = maxIndexRat ;
		else
		{
			if(matchArea.at(maxIndexAre)>0.7) maxIndex = maxIndexAre;
			if(matchRatio.at(maxIndexRat)>3) maxIndex = maxIndexRat;
			else maxIndex = maxIndexAre;
		}
		if(maxIndexAre == -1 and maxIndexRat == -1)
			maxIndex = -1 ;
		if(maxIndex== -1) return;
		horizonPole = horizonRects.at(maxIndex);
		horizonRectWidth = rectWidth.at(maxIndex);
		horizonRectHeight = rectHeight.at(maxIndex);
	}

	void Gate::getRoi(Mat& frame,RotatedRect& horizonPole,Rect &roi,int horizonRectWidth,int horizonRectHeight,int& newOriginX,int& newOriginY)
	{
		int height,width,poleWidth;
		float teta;
		if (abs(horizonPole.angle) >= _minAngle) teta = (2*M_PI/360)*(90 - abs(horizonPole.angle));
		else teta = (2*M_PI/360)*abs(horizonPole.angle);
		poleWidth = horizonRectWidth/cos(teta);
		if(horizonPole.center.x < _roiOriginX_TO_BaseWidthFactor * poleWidth)
			newOriginX = 0;
		else newOriginX = horizonPole.center.x - _roiOriginX_TO_BaseWidthFactor * poleWidth;
		if(horizonPole.center.y < poleWidth)
			newOriginY = 0;
		else newOriginY = horizonPole.center.y - poleWidth;
		if(frame.size().width - newOriginX < poleWidth)
			width = frame.size().width - newOriginX ;
		else width = poleWidth;
		if(frame.size().height - newOriginY < poleWidth + horizonRectHeight)
			height = frame.size().height - newOriginY ;
		else height = poleWidth + horizonRectHeight;
		Rect temp(newOriginX,newOriginY,width,height);
		roi.x =  temp.x;roi.y =  temp.y;roi.height = temp.height;roi.width = temp.width;
	}






	bool Gate::isVerticalRectangle( const vector<Point>& contour,RotatedRect& outRect,Mat roiFrame)
	{
		outRect= minAreaRect(contour);
		vector<Point> rectContour;
		Utils::GetContor(outRect,rectContour);
		double area= contourArea(contour);
		double rectArea  = contourArea(rectContour);
		double maxArea = max(area,rectArea);
		double minArea = min(area,rectArea);
		int rectHeight;
		if((abs(outRect.angle) >= _minAngle) or (abs(outRect.angle) <= _maxAngle))
		{
			if(!(isVertic(rectContour,rectHeight))) return false;
			if(rectHeight <= _minRectHeight*roiFrame.size().height)return false;

			if ((_inTest) and ((minArea/maxArea) > _areaSimilarity))
			{
				_vertAreaPercent.push_back(minArea/maxArea);
				vector<vector<Point> > c;
				c.push_back(rectContour);
				drawContours(*_rectIm,c,0,Scalar(0,255,0));// horizon rects drawn in red
//Utils::ShowImage(*_contourImage ,"rect");
			}
			return (minArea/maxArea) > _areaSimilarity;
		}
		return false;

	}
	bool Gate::isVertic(vector<Point>& rectContour,int& rectHeight)
	{
		int minIdx = -1,minIdy = -1,maxIdx = 0 ,maxIdy = 0,tempX,tempY,height,width;
		for (uint i=0; i<rectContour.size();++i)
		{
			tempX = rectContour.at(i).x;
			tempY = rectContour.at(i).y;
			maxIdx = max(tempX,maxIdx);
			maxIdy = max(tempY,maxIdy);
			minIdx = min(tempX,minIdx);
			minIdy = min(tempY,minIdy);
			if(minIdx == -1) minIdx = rectContour.at(i).x;
			if(minIdy == -1) minIdy = rectContour.at(i).y;
		}
		width = maxIdx - minIdx;
		height = maxIdy - minIdy;
		rectHeight = height;
		if(height >= width)return true;
		return false;

	}

	void Gate::findVerticalPoles(vector<RotatedRect> verticalPoles,vector<RotatedRect>& gateVerticPoles,vector<vector<Point> > contours,vector<int> index,RotatedRect& horizonPole)
	{
		if(verticalPoles.empty()) return;
		vector<double> matchArea;
		double maxMatch = 0;
		double tempMatch = 0;
		double maxArea,minArea,area,rectArea;
		bool right = false;
		bool left = false;
		int maxIndex1 = -1;
		int maxIndex2 = -1;
		int tempDeltaX,tempDeltaY;
		Size horizSize = horizonPole.size;
		Size vertSize;
		int horizWidth =  max(horizSize.height,horizSize.width);
		int vertHeight = 0;
		for (uint i=0; i< verticalPoles.size();++i)
		{
			vector<Point> rectContour;
			Utils::GetContor(verticalPoles.at(i),rectContour);
			area= contourArea(contours.at(index.at(i)));
			rectArea  = contourArea(rectContour);
			maxArea = max(area,rectArea);
			minArea = min(area,rectArea);
			matchArea.push_back(minArea/maxArea);
			tempMatch = max(maxMatch,matchArea.at(i));
			if(tempMatch == matchArea.at(i))
			{
				tempDeltaX = abs(verticalPoles.at(i).center.x - horizonPole.center.x);
				tempDeltaY = abs(verticalPoles.at(i).center.y - horizonPole.center.y);
				if(tempDeltaX >= _XminDistVertHoriz*horizWidth and tempDeltaX <= _XmaxDistVertHoriz*horizWidth)
				{
					if(tempDeltaY >= _YminDistVertHoriz*horizWidth and tempDeltaY <= _YmaxDistVertHoriz*horizWidth)
					{
						vertSize = verticalPoles.at(i).size;
						vertHeight = max(vertSize.height,vertSize.width);
						if(horizWidth/vertHeight >= _minHorVerLeng and horizWidth/vertHeight <= _maxHorVerLeng)
						{
							maxMatch = tempMatch;
							maxIndex1 = i;
						}
					}
				}
			}
		}
		if(maxIndex1 == -1) return;
		if(verticalPoles.at(maxIndex1).center.x - horizonPole.center.x > 0)
			right = true;
		else left = true;
		maxMatch = 0;
		tempMatch = 0;
		for (uint i=0; i< verticalPoles.size();++i)
		{
			if(i==maxIndex1) continue;
			vector<Point> rectContour;
			Utils::GetContor(verticalPoles[i],rectContour);
			area= contourArea(contours.at(index[i]));
			rectArea  = contourArea(rectContour);
			maxArea = max(area,rectArea);
			minArea = min(area,rectArea);
			matchArea[i] = minArea/maxArea;
			tempMatch = max(maxMatch,matchArea.at(i));
			if(tempMatch == matchArea.at(i))
			{
				tempDeltaX = abs(verticalPoles.at(i).center.x - horizonPole.center.x);
				tempDeltaY = abs(verticalPoles.at(i).center.y - horizonPole.center.y);
				if(tempDeltaX >= _XminDistVertHoriz*horizWidth and tempDeltaX <= _XmaxDistVertHoriz*horizWidth)
				{
					if(tempDeltaY >= _YminDistVertHoriz*horizWidth and tempDeltaY <= _YmaxDistVertHoriz*horizWidth)
					{
						vertSize = verticalPoles.at(i).size;
						vertHeight = max(vertSize.height,vertSize.width);
						if(horizWidth/vertHeight >= _minHorVerLeng and horizWidth/vertHeight <= _maxHorVerLeng)
						{
							if(verticalPoles.at(i).center.x - horizonPole.center.x > 0)
							{
								if(right) continue;
								maxMatch = tempMatch;
								maxIndex2 = i;
							}
							else
							{
								if(left) continue;
								maxMatch = tempMatch;
								maxIndex2 = i;
							}

						}
					}
				}
			}
		}
		gateVerticPoles.push_back(verticalPoles.at(maxIndex1));
		if(maxIndex2 == -1) return;
		right = true;
		left = true;
		gateVerticPoles.push_back(verticalPoles.at(maxIndex2));
	}


	bool Gate::isInRoi(vector<Point>& contour,Rect roi)
	{
		int j = 0;
		for (uint i=0; i<contour.size();++i)
		{
			if((contour.at(i).x > roi.x) and (contour.at(i).x < roi.width + roi.x) )
				if((contour.at(i).y > roi.y) and (contour.at(i).y < roi.height + roi.y) )++j ;
			if(j>_minPixelsInROI ) return true;
		}
		return false;

	}

	bool Gate::isConnectedVertical(Mat frame,vector<RotatedRect>& verticalPoles,RotatedRect& gate,vector<Point>& gateVertices, RotatedRect horizonPole,int horizonRectWidth)
	{
		//check correlation between 2 vertic rects

		//if _isTrafficGreen true-2 green connected was sent
		//else - check if red
		// if verticalPoles.size < 2 only one option
		if(_isTrafficGreen)
		{//check if vertical connected poles are related to horizon pole

			if(abs(verticalPoles[0].center.y - verticalPoles[1].center.y ) <= _YverticPolesMaxDiff*horizonRectWidth )
				if(abs(verticalPoles[0].center.x - verticalPoles[1].center.x ) <= _XverticPolesMaxDiff*horizonRectWidth
				and  abs(verticalPoles[0].center.x - verticalPoles[1].center.x ) >= _XverticPolesMinDiff*horizonRectWidth )
				{
					vector<Point> tempRectContour1;
					vector<Point> tempRectContour2;
					double tempArea1, tempArea2;
					Utils::GetContor(verticalPoles[0],tempRectContour1);
					Utils::GetContor(verticalPoles[1],tempRectContour2);
					tempArea1 = contourArea(tempRectContour1);
					tempArea2 = contourArea(tempRectContour2);
					double maxArea = max(tempArea1,tempArea2);
					double minArea = min(tempArea1,tempArea2);

					if(minArea/maxArea >= _ConnectVerticAreaSimilarityMin and minArea/maxArea <= _ConnectVerticAreaSimilarityMax)
					return true;
				}
		}
		//*********just temporary.
		//*********ITZIK - when u configure your algo- delete the "else" line and uncomment the "else" below
		else
			//Gate::findGateVertices(horizonPole,verticalPoles,_gateVertices,gate,0);
		//else //didnt find 2 parallel poles
			//check traffic
		{
			Gate::findGateVertices(horizonPole,verticalPoles,_gateVertices,gate,0);
			if(_inTest)
			_gateVerticalPoles->swap(verticalPoles);
			return false;
			//end of algorithm - found right hand pole and horizon only
			/*
			Mat copyFrame = frame.clone();
			_itzik->Run(copyFrame);
			int flag = -1;
			RotatedRect redBouy = _itzik->RedSquare_current_results;
			RotatedRect ItzikBouy;
			if(redBouy.center.x == -1)
			{
				ItzikBouy = _itzik->Green_current_results;
				if(ItzikBouy.center.x != -1)
					_isTrafficGreen = true;
			}
			else
				ItzikBouy  = redBouy;

			if(ItzikBouy.center.x == -1)
			{
				//itzik didnt find, continue with out
				Gate::findGateVertices(horizonPole,verticalPoles,_gateVertices,gate,0);
				if(_inTest)
					_gateVerticalPoles->swap(verticalPoles);
				return false;
				//end of algorithm - found right hand pole and horizon only
			}
			if(abs(verticalPoles.at(0).center.y - ItzikBouy.center.y ) <= _YverticPolesMaxDiff*horizonRectWidth )
				if(abs(verticalPoles.at(0).center.x - ItzikBouy.center.x ) <= _XverticPolesMaxDiff*horizonRectWidth
				and  abs(verticalPoles.at(0).center.x - ItzikBouy.center.x ) >= _XverticPolesMinDiff*horizonRectWidth )
				{
					vector<Point> tempRectContour1;
					vector<Point> tempRectContour2;
					double tempArea1, tempArea2;
					Utils::GetContor(verticalPoles.at(0),tempRectContour1);
					Utils::GetContor(ItzikBouy,tempRectContour2);
					tempArea1 = contourArea(tempRectContour1);
					tempArea2 = contourArea(tempRectContour2);
					double maxArea = max(tempArea1,tempArea2);
					double minArea = min(tempArea1,tempArea2);
					if(minArea/maxArea >= _ConnectVerticAreaSimilarityMin and minArea/maxArea <= _ConnectVerticAreaSimilarityMax)
					{
						verticalPoles.push_back(ItzikBouy);
						return true;
					}
				}*/
		}
		return false;//**ITZIK - delete this after unmarking
	}

	bool Gate::isFindGate(Mat frame,vector<RotatedRect>& verticalPoles,RotatedRect& gate,vector<Point>& gateVertices, RotatedRect horizonPole,int horizonRectWidth)
	{
		if(verticalPoles.empty())
			return false;
		if(verticalPoles.size() < 2  )//found 1 pole
		{
			//if found only left pole - no need to check with itzik!
/*			if(verticalPoles.at(0).center.x - horizonPole.center.x < 0)
			{
				Gate::findGateVertices(horizonPole,verticalPoles,_gateVertices,gate,0);
				return false;// Gate found with 1 left pole!
			}
*/

			//else *****check if itzik found rect because i.e - call traffic

			_isTrafficGreen = false;
			//check corelation with traffic by function to horizon pole
			return isConnectedVertical(frame,verticalPoles,gate,gateVertices,horizonPole,horizonRectWidth);
		}
		return isConnectedVertical(frame,verticalPoles,gate,gateVertices,horizonPole,horizonRectWidth);


	}

	void Gate::findGateVertices(RotatedRect horizonPole,vector<RotatedRect> verticalPoles,vector<Point>& gateVertices,RotatedRect& gate,int flag)
	{
		if(flag)// 2 poles found - or 2 green or 1 red 1 green
		{
			vector<Point> bounds1;
			vector<Point> bounds2;
			vector<Point> bounds3;
			vector<Point> bounds;
			vector<Point> vertices;
			vector<vector<Point> > arr;
			Utils::GetContor(verticalPoles.at(0),bounds1);
			Utils::GetContor(verticalPoles.at(1),bounds2);
			Utils::GetContor(horizonPole,bounds3);
			arr.push_back(bounds1);
			arr.push_back(bounds2);
			arr.push_back(bounds3);
			Utils::mergeVectors(arr,bounds);
			RotatedRect tempGate = minAreaRect(bounds);
			Point2f tmpPoints[4];
			tempGate.points(tmpPoints);
			for (int i = 0; i < 4; i++)
			{
				vertices.push_back(tmpPoints[i]);
			}
			Utils::orderBounds(vertices);
			gateVertices.push_back(vertices.at(0));
			gateVertices.push_back(vertices.at(1));
			gateVertices.push_back(vertices.at(2));
			gateVertices.push_back(vertices.at(3));
			gate.center = tempGate.center;

		}
		else //found 1 right hand green pole
		{
			vector<Point> bounds1;
			vector<Point> bounds2;
			vector<Point> bounds;
			vector<vector<Point> > arr;
			Utils::GetContor(verticalPoles.at(0),bounds1);
			Utils::GetContor(horizonPole,bounds2);
			arr.push_back(bounds1);
			arr.push_back(bounds2);
			Utils::mergeVectors(arr,bounds);
			Rect tempGate = boundingRect(bounds);
			gateVertices.push_back(Point(tempGate.x,tempGate.y+tempGate.height));
			gateVertices.push_back(Point(tempGate.x,tempGate.y));
			gateVertices.push_back(Point(tempGate.x+tempGate.width,tempGate.y));
			gateVertices.push_back(Point(tempGate.x+tempGate.width,tempGate.y+tempGate.height));
			gate.center = Point((2*tempGate.x + tempGate.width)/2,(2*tempGate.y + tempGate.height)/2);
		}
	}

	void Gate::InitResult()
	{
		if (this->_gate!=NULL)
		{
			delete _gate;
			_gate = NULL;
		}

	}

	void Gate::Draw(Mat& draw)
	{
		if(_gateVertices.size()>0)
		{
			vector<vector<Point> > c;
			c.push_back(_gateVertices);
			drawContours(draw,c,0,Scalar(0,0,255));
			if(_isTrafficGreen == true)
					line(draw,_gate->center,_gate->center,Scalar(0,255,0),3);
			else line(draw,_gate->center,_gate->center,Scalar(0,0,255),3);
			circle(draw,_gate->center,6,Scalar(255,255,255),2);
			}
		else{
				if(_isPole)
				{
					vector<Point> temp;
					Utils::GetContor(*_horizonPole,temp);
					vector<vector<Point> > d;
					d.push_back(_gateVertices);
					drawContours(draw,d,0,Scalar(0,255,0));
					line(draw,_gate->center,_gate->center,Scalar(0,0,255),3);

				}

			}
	}
