/*
 * BlackGate2.cpp
 *
 *  Created on: Mar 17, 2013
 *      Author: raz
 */

#include "BlackGate_adaptive.h"
#include <algorithm>
#include <cvaux.h>
#include <highgui.h>
#include <cxcore.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "../Utils/Utils.h"
#include "../Utils/ParamUtils.h"
BlackGate2::BlackGate2() : BaseAlgorithm()
{
	//SetDefaultParams();
	_gate = NULL;
	_erosiontype = MORPH_RECT;
	_dilationtype = MORPH_RECT;
}

BlackGate2::~BlackGate2()
{

}



void BlackGate2::ClearProcessData()
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
		_contourImage=NULL;
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
	if (this->_areaPercent!=NULL)
	{
		delete _areaPercent;
		_areaPercent = NULL;
	}
	if (this->_outputIm!=NULL)
	{
		delete _outputIm;
		_outputIm = NULL;
	}
	if (this->_rectIm!=NULL)
	{
		delete _rectIm;
		_rectIm = NULL;
	}
	if (this->_verticalRects!=NULL)
	{
		delete _verticalRects;
		_verticalRects = NULL;
	}
}



void BlackGate2::InitProcessData()
{
	_kolman=NULL;
	_BW=NULL;
	_contourImage=NULL;
	_areaPercent=NULL;
	_outputIm = NULL;
	_rectIm = NULL;
	_verticalRects = NULL;
	_eroded = NULL;
	_dilated = NULL;
}
void BlackGate2::fixResults(Point offset)
{
	if(_gate->center.x != -1)
	{
		_gate->center.x += offset.x;
		_gate->center.y += offset.y;
	}
	if(!_gateVertices.empty())
	{
		for(uint i = 0; i < 4;i++)
		{
			_gateVertices[i].x += offset.x;
			_gateVertices[i].y += offset.y;
		}
	}
}


void BlackGate2::SetDefaultParams()
{
	_areaSimilarity = 0.5;
	_minCurves = 4;
	_objectImageRatio = 0.003;
	_minAngle = 70;
	_maxAngle = 20;
	_OrangeHue = 30;
	_kolmanThresh = 0.45;
	_heightWidthRatio = 1.6;
	_erosionsize = 2;
	_dilationsize = 9;
	_difPoleCenters = 5;
	_maxRatio = 0.32;
	_minRectAreaSize = 0.1;
	_blockSize = 299;
	_isHorizonRec = 65;
	_c =0;
}

void BlackGate2::Run(Mat& image)
{
	this->_gateVertices.clear();
	this->_gate = new RotatedRect();
	this->_isCompleteGate = false;
	_gate->center.x = -1;
	_gate->center.y = -1;

	Run2(image,*_gate );
}
void BlackGate2::Load(map<string, string>& params)
{
	ParamUtils::setParam(params, "minAngle", _minAngle);
	ParamUtils::setParam(params, "maxAngle",_maxAngle);
	ParamUtils::setParam(params, "orangeHue",_OrangeHue);
	double tmp;
	stringstream(params["objectImageRatio"]) >> tmp;
	_objectImageRatio = tmp/1000;
	ParamUtils::setParam(params, "minCurves", _minCurves);
	ParamUtils::setParamPercent(params, "AreaRatio", _areaSimilarity);
	ParamUtils::setParamPercent(params, "kolmanTreshold", _kolmanThresh);
	ParamUtils::setParamPercent(params, "heightWidthRatio", _heightWidthRatio);
	ParamUtils::setParam(params, "erosionsize", _erosionsize);
	ParamUtils::setParam(params, "dilationsize", _dilationsize);
	ParamUtils::setParam(params, "difPoleCenters", _difPoleCenters);
	ParamUtils::setParamPercent(params, "AreaMaxRatio", _maxRatio);
	ParamUtils::setParamPercent(params, "minRectAreaSize", _minRectAreaSize);
	ParamUtils::setParam(params, "blockSize", _blockSize);
	ParamUtils::setParam(params, "isHorizonRec", _isHorizonRec);

}
void BlackGate2::ToMesseges(vector<MissionControlMessage>& res)
{
		//additionalInformation is a boolean that determine is the gate found too high.
		//if it is too high it could say we dont see the whole gate. if "1"- the gate is OK
		//first 4 points of bounds are the vertices points of the black gate in that order:
		//Bottom Left,Top Left ,Top Right ,Bottom Right.
		//intrestPoints contain the center point of the gate
	if(_gate->center.x != -1 and _gate->center.y != -1){
			MissionControlMessage msg;
			double tmp1,tmp2;
			int height,width;
			msg.MissionCode = BlackGate;
			if(_isCompleteGate)
				msg.additionalInformation = 1; //found the gate
			else msg.additionalInformation = 0; //gate could be partial because it is to high
			if (!_gateVertices.empty())
			{
			for (uint i = 0; i<4; ++i)
				msg.bounds.push_back(std::pair<int, int>(_gateVertices.at(i).x,_gateVertices.at(i).y));
			}
			msg.intrestPoints.push_back(std::pair<int,int>(_gate->center.x,_gate->center.y));

			res.push_back(msg);
	}
}

void BlackGate2::Run2(Mat& frame,RotatedRect& gate)
{
	if(_inTest)
	{
		this->_contourImage = new Mat();
		this->_kolman = new Mat();
		this->_BW = new Mat();
		this->_outputIm= new Mat();
		this->_verticalRects = new vector<RotatedRect>();
		this->_areaPercent = new vector<double>();
		this->_rectIm= new Mat();
		this->_eroded = new Mat();
		this->_dilated = new Mat();

	}
	Mat kolman;
	Mat kolman32F;
	bool OrangeExists = Utils::Kolaman(_OrangeHue, frame, kolman32F, kolman,_kolmanThresh);
	if (_inTest)
	{
		_kolman = new Mat();
		kolman.copyTo(*_kolman);
	}

		if (!OrangeExists)
			return;

	//C. threshold orange color  and fill holes in orange area
//Utils::ShowImage(ScaledGray ,"filled");
	Mat BW;
	if (_blockSize <=1)
		_blockSize =3;
	if (_blockSize%2 ==0)
		++_blockSize;
	Scalar mean,dev;
	meanStdDev(kolman,mean,dev);
	_c = dev(0);
	adaptiveThreshold(kolman,BW,255,ADAPTIVE_THRESH_GAUSSIAN_C ,THRESH_BINARY,_blockSize,-_c);


//Utils::ShowImage(BW ,"BW");

	//morph!!
	Mat eroded(frame.size().width, frame.size().height, CV_8UC1);
	Mat dilated(frame.size().width, frame.size().height, CV_8UC1);
	Mat erode_element, dilate_element;

	erode_element = getStructuringElement(_erosiontype,
	Size(2 * _erosionsize + 1, 2 * _erosionsize + 1),
	Point(_erosionsize, _erosionsize));
	erode(BW, eroded, erode_element);
	dilate_element = getStructuringElement(_dilationtype,
			Size(2 * _dilationsize + 1, 2 * _dilationsize + 1),
			Point(_dilationsize, _dilationsize));
	dilate(eroded, BW, dilate_element);


	//D. finding object contours
	if(_inTest)
	{
		eroded.copyTo(*_eroded);
		BW.copyTo(*_BW);
	}
	vector<vector<Point> > contours;
	findContours(BW,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	if (_inTest)
	{
		frame.copyTo(*_contourImage);
		frame.copyTo(*_outputIm);
		frame.copyTo(*_rectIm);
		for (uint i=0; i<contours.size();++i)
			drawContours(*_contourImage,contours,i,Scalar(255,255,255),2);
	}
//Utils::ShowImage(*_contourImage,"contours");

	//E. find all vertical rectangles
	vector<RotatedRect> verticalRects;
	vector<RotatedRect> gates;
	vector<int> index;

	for (uint i=0; i<contours.size();++i)
	{

		//E.1. filter noise
		if (contours.at(i).size() < _minCurves ) continue;
		double area = contourArea(contours.at(i));
		if (area < _objectImageRatio *frame.size().area())
			continue;

		//E.2. check if object is a vertical rectangle
		RotatedRect rect;
		if (isVerticalRectangle(contours.at(i),rect,frame))
		{
				verticalRects.push_back(rect);
		}

		findGates(frame,area,contours.at(i),gates,index,i);
		findBoundGates(frame,area,contours,gates,index,i);



	}
	int gateIdx = -1;
	if(isFindGate(frame,gates,contours,index,gateIdx))
	{
		int a = gates.size();
		findGateVertices(frame,gates.at(gateIdx),contours,gateIdx,_gateVertices);
		gate.center = gates.at(gateIdx).center;
		//gate.center.y += 100;
		if(gate.center.y < (frame.rows)/4)
			_isCompleteGate = false;
		else _isCompleteGate = true;
		if (_inTest)
		{
			_verticalRects->swap(gates);
		}
		return;
	}
		//F. check if we found gate poles
	if(isFindPoles(frame,verticalRects,gate,_gateVertices))
	{
		//check if gate found is partial,meaning the center is to high
		if(gate.center.y < (frame.rows)/4)
			_isCompleteGate = false;
		else _isCompleteGate = true;
		if (_inTest)
		{
			_verticalRects->swap(verticalRects);

//Utils::ShowImage(*_outputIm,"gateCenter");
		}
	}
}

void BlackGate2::findGateVertices(Mat frame,RotatedRect& gate,vector<vector<Point> > contours,int& gateIdx,vector<Point>& gateVertices)
{
	vector<Point> bounds;
	Utils::FillRectBounds(gate,bounds);
	gateVertices.push_back(bounds.at(0));
	gateVertices.push_back(bounds.at(1));
	gateVertices.push_back(bounds.at(2));
	gateVertices.push_back(bounds.at(3));

}
RotatedRect BlackGate2::box2RotatedRect(const Rect rect)
{
	Point2f center(rect.x + rect.width/2,rect.y + rect.height/2);
	Size2f size = rect.size();
	float angle = 0;
	RotatedRect temprect(center,size,angle);
	return temprect;

}
void BlackGate2::findBoundGates(Mat frame,double area,vector<vector<Point> >& contours,vector<RotatedRect>& gates,vector<int>& index,int temp)
{
	vector<Point> contour = contours.at(temp);
	Rect rect = boundingRect(contour);
	float minRectSize = _minRectAreaSize * frame.size().area();
	int height = rect.width;
	int width = rect.height;
	double rectArea = rect.area();
	if(area/rectArea < _maxRatio)
	{
		if(rectArea > minRectSize )
		{
			if(height >= 0.6*_heightWidthRatio*width)
			{
				RotatedRect tempRotated = box2RotatedRect(rect);
				tempRotated.center.y += 60;
				vector<vector<Point> > c;
				vector<Point> rectContour;
				Utils::GetContor(tempRotated,rectContour);
				c.push_back(rectContour);
				contours.push_back(rectContour);
				drawContours(*_rectIm,c,0,Scalar(0,255,0),3);
				int j = contours.size();
				index.push_back(j-1);
				gates.push_back(tempRotated);
			}
		}
	}
}

void BlackGate2::findGates(Mat frame,double area,vector<Point>& contour,vector<RotatedRect>& gates,vector<int>& index,int temp)
{
	vector<Point> rectContour;
	RotatedRect rect = minAreaRect(contour);
	int height = max(rect.size.height,rect.size.width);
	int width  = min(rect.size.height,rect.size.width);
	Utils::GetContor(rect,rectContour);
	float minRectSize = _minRectAreaSize * frame.size().area();
	double rectArea  = contourArea(rectContour);
		if(area/rectArea < _maxRatio)
		{
			if(rectArea > minRectSize )
			{
				if(height >= _heightWidthRatio*width)
				{
					Utils::fixAngle(rect);
					if(abs(rect.angle) >_isHorizonRec)
					{
						vector<vector<Point> > c;
						c.push_back(rectContour);
						drawContours(*_rectIm,c,0,Scalar(0,255,0),3);
						gates.push_back(rect);
						index.push_back(temp);
					}
				}

			}
		}
}
bool BlackGate2::isFindGate(Mat frame,vector<RotatedRect>& gates,vector<vector<Point> > contours,vector<int> index,int& gateIdx)
{
	if(gates.empty()) return false;
	if(gates.size()<2)
	{
		gateIdx = 0;
		return true;
	}
	double maxArea = 0;

	for(uint i=0;i<index.size();++i)
	{
		vector<Point> rectContour;
		RotatedRect rect = minAreaRect(contours.at(index.at(i)));
		Utils::GetContor(rect,rectContour);
		double rectArea  = contourArea(rectContour);
		maxArea = max(maxArea,rectArea);
		if(maxArea == rectArea) gateIdx = i;
	}


	return true;
}


bool BlackGate2::isVerticalRectangle( const vector<Point>& contour,RotatedRect& outRect,Mat frame)
//check if a contour is a verticle rectangle
{
	outRect= minAreaRect(contour);
	vector<Point> rectContour;
	Utils::GetContor(outRect,rectContour);
	double area= contourArea(contour);
	double rectArea  = contourArea(rectContour);
	double maxArea = max(area,rectArea);
	double minArea = min(area,rectArea);
	if((abs(outRect.angle) >= _minAngle) or (abs(outRect.angle) <= _maxAngle))
	{
		if(!(isVertic(rectContour))) return false;
		if ((_inTest) and ((minArea/maxArea) > _areaSimilarity))
		{
			_areaPercent->push_back(minArea/maxArea);
			vector<vector<Point> > c;
			c.push_back(rectContour);
			drawContours(*_rectIm,c,0,Scalar(0,0,255));
//Utils::ShowImage(*_contourImage ,"rect");
		}
		return (minArea/maxArea) > _areaSimilarity;
	}
	return false;

}

bool BlackGate2::isFindPoles(Mat& frame,vector<RotatedRect>& Rects,RotatedRect& gate,vector<Point>& gateVertices)
//check if find poles
{
	int index1 = -1;
	int index2 = -1;
	double low_pole1 = 0;
	double low_pole2 = 0;
	double temp1 = 0;
	double temp2 = 0;

	//find lowest vertical rects
	for (uint i=0; i<Rects.size();++i)
	{
		temp1 = Rects.at(i).center.y;
		low_pole1 = max(temp1,low_pole1);
		if(temp1 == low_pole1)
			index1 = i;
	}
	for (uint i=0; i<Rects.size();++i)
	{
		if(i == index1)continue;
		temp2 = Rects.at(i).center.y;
		low_pole2 = max(temp2,low_pole2);
		if(temp2 == low_pole2)
			index2 = i;
	}
	if(!_difPoleCenters) _difPoleCenters++;
	if((index1 == -1) or (index2 == -1) or (abs(low_pole2-low_pole1)> (frame.rows)/_difPoleCenters))//if not find 2 verical rectangles or pole's center is not parallel
		return false;
	//find gate contour
	vector<Point> bounds1;
	vector<Point> bounds2;
	vector<Point> bounds;
	vector<Point> vertices;
	vector<vector<Point> > arr;
	Utils::GetContor(Rects.at(index1),bounds1);
	Utils::GetContor(Rects.at(index2),bounds2);
	arr.push_back(bounds1);
	arr.push_back(bounds2);
	Utils::mergeVectors(arr,bounds);
	RotatedRect tempGate = minAreaRect(bounds);
	Point2f tmpPoints[4];
	tempGate.points(tmpPoints);
	for (int i = 0; i < 4; i++)
	{
		vertices.push_back(tmpPoints[i]);
	}
	Utils::orderBounds(vertices);
	if(tempGate.size.area() >= _minRectAreaSize * frame.size().area())
			{
				gateVertices.push_back(vertices.at(0));
				gateVertices.push_back(vertices.at(1));
				gateVertices.push_back(vertices.at(2));
				gateVertices.push_back(vertices.at(3));
			}
	else return false;
	gate.center = tempGate.center;
	gate.center.y += 100;
	return true;


}

bool BlackGate2::isVertic(vector<Point>& rectContour)//confirm it is a vertical rect because rect.angle changes
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
	if(height >= _heightWidthRatio*width)return true;
	return false;

}

void BlackGate2::InitResult()
{
	if (this->_gate!=NULL)
	{
		delete _gate;
		_gate = NULL;
	}

}

void BlackGate2::Draw(Mat& draw)
{
	if(_gateVertices.size()>0)
	{
		vector<vector<Point> > c;
		c.push_back(_gateVertices);
		drawContours(draw,c,0,Scalar(0,0,255),3);
		if(_isCompleteGate == true)
			line(draw,_gate->center,_gate->center,Scalar(0,255,0),5);
		else line(draw,_gate->center,_gate->center,Scalar(0,0,255),5);
		circle(draw,_gate->center,6,Scalar(0,255,255),3);
	}
}
