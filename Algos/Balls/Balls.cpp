/*
 * Balls.cpp
 *
 *  Created on: Feb 26, 2013
 *      Author: Itzik Mizrachy
 */

#include "Balls.h"
#include <cvaux.h>
#include <highgui.h>
//#include <photo_c.h>
#include "../Utils/Utils.h"
#include <cxcore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include "onMouse.cpp"

Balls::Balls() {
	// TODO Auto-generated constructor stub
	SetDefaultParams();
	_inTest = false;
	this->_greenthresh = NULL;
	this->_redthresh = NULL;
	this->_yellowthresh = NULL;
    this->_gcircles=NULL;
    this->_rcircles=NULL;
    this->_ycircles=NULL;

}

Balls::~Balls() {
	// TODO Auto-generated destructor stub
	ClearProcessData();

}
void Balls::SetDefaultParams()
{
	_redminh =6;
	_redmaxh = 31;

	_greenminh = 77;
	_greenmaxh = 83;

	_yellowminh = 30;
	_yellowmaxh = 73;

	_inverseRatio = 1;
	_mindistDivide = 4;
	_upcannyThresh = 100;
	_centerThresh = 50;
	_minRadius = 5;
	_maxRadius = 400;

}

void Balls::ClearProcessData()
	{
		if (this->_greenthresh!= NULL)
		{
			delete _greenthresh;
			_greenthresh = NULL;
		}
		if (this->_redthresh!= NULL)
		{
			delete _redthresh;
			_redthresh = NULL;
		}
		if (this->_yellowthresh!= NULL)
		{
			delete _yellowthresh;
			_yellowthresh = NULL;
		}
		 if (_gcircles != NULL)
		{
			 delete _gcircles;
			  _gcircles = NULL;
			    		}
			  // _gcircles = new vector<Vec3f>();
			    if (_rcircles != NULL)
			    		{
			    			delete _rcircles;
			    			_rcircles = NULL;
			    		}
			  //  _rcircles = new vector<Vec3f>();
			    if (_ycircles != NULL)
			    		{
			    			delete _ycircles;
			    			_ycircles = NULL;
			    		}
			   // _ycircles = new vector<Vec3f>();

	}
void Balls::Run(Mat& frame,Mat& draw)
{
	// Create a window in which the captured images will be presented
	    //
	    Mat *Hue = Utils::HueChannel(frame);
	    //Utils::ShowImage(*Hue,"Hue");
	    //Mat *Hue_scale = Utils::ScaledChannel(*Hue,180);
	    //Utils::ShowImage(*Hue_scale,"Hue_scale");
		Mat Huescale;
		Hue->copyTo(Huescale);
	    Mat se21=getStructuringElement(MORPH_ELLIPSE,Size(11,11),Point(10,10));
	    Mat se11=getStructuringElement(MORPH_ELLIPSE,Size(11,11),Point(5,5));
	    Mat closed;
	    Mat open;
		Mat greenthresh;
		Mat redthresh;
		Mat yellowthresh;
	    FindGreen(Huescale,_greenminh, _greenmaxh, greenthresh,se21,se11);
	    //Utils::ShowImage(greenthresh,"green");
	    FindRed(Huescale,_redminh, _redmaxh, redthresh,se21,se11);
	    //Utils::ShowImage(redthresh,"red");
	    FindYellow(Huescale,_yellowminh, _yellowmaxh,yellowthresh,se21,se11);
	    //Utils::ShowImage(yellowthresh,"yellow");
	    if (_inTest)
	    	    				{
	    	    					_greenthresh = new Mat();
	    	    					(&greenthresh)->copyTo(*_greenthresh);
	    	    					_redthresh = new Mat();
	    	    					(&redthresh)->copyTo(*_redthresh);
	    	    					_yellowthresh = new Mat();
	    	    					(&yellowthresh)->copyTo(*_yellowthresh);
	    	    				}
	    vector<Vec3f> gcircles;
	    vector<Vec3f> rcircles;
	    vector<Vec3f> ycircles;
	    HoughCircles( greenthresh, gcircles, CV_HOUGH_GRADIENT, _inverseRatio, greenthresh.rows/32, 200, 10, 1, 100 );
	    HoughCircles( redthresh, rcircles, CV_HOUGH_GRADIENT, _inverseRatio, redthresh.rows/32, 200, 10, 1, 100 );
	    HoughCircles( yellowthresh, ycircles, CV_HOUGH_GRADIENT, _inverseRatio, yellowthresh.rows/32, 200, 10, 1, 100 );
	    _maxidxRadiusG = MaxRadius(gcircles);
	    _maxidxRadiusR = MaxRadius(rcircles);
	    _maxidxRadiusY = MaxRadius(ycircles);
	    if (_inTest)
	    				{
							//this->_gcircles = new vector<Vec3f>();
							//this->_rcircles = new vector<Vec3f>();
							//this->_ycircles = new vector<Vec3f>();
	    					DrawCircles(gcircles,rcircles,ycircles, draw,_maxidxRadiusG,_maxidxRadiusR,_maxidxRadiusY);
	    				}
		this->_frame=&draw;
		//Utils::ShowImage(frame,"final");
		//		if (_inTest)
//				{
//					_Gaussthresh = new Mat();
//					(&frame)->copyTo(*_Gaussthresh);
//				}

//	   // if (_inTest)
//		{
//	    	_yellowthresh = new Mat();
//			Hue_scale->copyTo(*_yellowthresh);
//		}
	    // Memory for hough circles
	    // CvMemStorage* storage = cvCreateMemStorage(0);
	    // hue detector works better with some smoothing of the image
//		if (_circles != NULL)
//		{
//			delete _circles;
//			_circles = NULL;
//		}
//		_circles = new vector<Vec3f>();
	   // HoughCircles( open, *_circles, CV_HOUGH_GRADIENT, _inverseRatio, thresholded.rows/32, 200, 10, 1, 100 );
//		_maxidxRadius = MaxRadius(_circles);
//		if ((*_circles).size()>0){
//		drawCircle(*_circles, frame,_maxidxRadius);
//		}
//		if (_inTest)
//				{
//					_Gaussthresh = new Mat();
//					(&frame)->copyTo(*_Gaussthresh);
//				}
}
int Balls::MaxRadius(vector<Vec3f>& circles){
	int Maxidx=0,MaxValue=0;
	if (!circles.empty()){
		for (size_t i = 0; i <(circles).size(); i++){
			if ((cvRound((circles)[i][2])) > MaxValue){
				Maxidx = i;
				MaxValue=(circles)[i][2];
			}
		}
	}
	return Maxidx;
}
void Balls::DrawCircles(vector<Vec3f> gcircles,vector<Vec3f> rcircles,vector<Vec3f> ycircles, Mat& frame,int maxidxRadiusG,int maxidxRadiusR,int maxidxRadiusY) {
		if (!gcircles.empty()){
		Point center(cvRound((gcircles)[maxidxRadiusG][0]), cvRound((gcircles)[maxidxRadiusG][1]));
		int radius = cvRound((gcircles)[maxidxRadiusG][2]);
		//printf("Ball! p=%f\n\r",Point.center);
		// circle center
		circle(frame, center, 3, Scalar(255, 0, 0), -1, 8, 0);
		// circle outline
		circle(frame, center, radius, Scalar(0, 0, 255), 3, 8, 0);
		}
		if (!rcircles.empty()){
		Point center(cvRound((rcircles)[maxidxRadiusR][0]), cvRound((rcircles)[maxidxRadiusR][1]));
		int radius = cvRound((rcircles)[maxidxRadiusR][2]);
		//printf("Ball! p=%f\n\r",Point.center);
		// circle center
		circle(frame, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		// circle outline
		circle(frame, center, radius, Scalar(0,255, 0), 3, 8, 0);
		}
		if (!ycircles.empty()){
		Point center(cvRound((ycircles)[maxidxRadiusY][0]), cvRound((ycircles)[maxidxRadiusY][1]));
		int radius = cvRound((ycircles)[maxidxRadiusY][2]);
		//printf("Ball! p=%f\n\r",Point.center);
		// circle center
		circle(frame, center, 3, Scalar(0, 0, 255), -1, 8, 0);
		// circle outline
		circle(frame, center, radius, Scalar(255, 0, 0), 3, 8, 0);
		}
}
void Balls::FindGreen(Mat& Hue_scale, int greenminh,int greenmaxh,Mat& greenthresh,Mat& se21,Mat& se11) {
    Mat closed,open;
	inRange(Hue_scale, greenminh, greenmaxh, greenthresh);
	// Utils::ShowImage(greenthresh,"thresholded");
	morphologyEx(greenthresh,closed,MORPH_CLOSE,se21);
	//Utils::ShowImage(closed,"closed");
	morphologyEx(closed,open,MORPH_OPEN,se11);
	//Utils::ShowImage(open,"open");
	open.copyTo(greenthresh);
}
void Balls::FindRed(Mat& Hue_scale, int redminh,int redmaxh,Mat& redthresh,Mat& se21,Mat& se11) {
    Mat closed,open;
	inRange(Hue_scale, redminh, redmaxh, redthresh);
	// Utils::ShowImage(thresholded,"thresholded");
	morphologyEx(redthresh,closed,MORPH_CLOSE,se21);
	//Utils::ShowImage(closed,"closed");
	morphologyEx(closed,open,MORPH_OPEN,se11);
	//Utils::ShowImage(open,"open");
	open.copyTo(redthresh);
}
void Balls::FindYellow(Mat& Hue_scale, int yellowminh,int yellowmaxh,Mat& yellowthresh,Mat& se21,Mat& se11) {
    Mat closed,open;
	inRange(Hue_scale, yellowminh, yellowmaxh, yellowthresh);
	// Utils::ShowImage(thresholded,"thresholded");
	morphologyEx(yellowthresh,closed,MORPH_CLOSE,se21);
	//Utils::ShowImage(closed,"closed");
	morphologyEx(closed,open,MORPH_OPEN,se11);
	//Utils::ShowImage(open,"open");
	open.copyTo(yellowthresh);
}




