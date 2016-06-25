/*
 * BlackGate2.h
 *
 *  Created on: Mar 17, 2013
 *      Author: raz
 */

#ifndef BlackGate2_H_
#define BlackGate2_H_
#include <cv.h>
#include "../BaseAlgorithm.h"



using namespace cv;
using namespace std;


// this is the second algorithm- find red poles
class BlackGate2: public BaseAlgorithm
{
public:

	virtual void Run(Mat& image);
	virtual void Load(map<string,string>& params);
	virtual void ToMesseges(vector<MissionControlMessage>& res);
protected:
	virtual void InitProcessData();
	virtual void InitResult();
	virtual void fixResults(Point offset);
public:
	BlackGate2();
	virtual ~BlackGate2();
	void Run2(Mat& frame,RotatedRect& gate);

	void findGates(Mat frame,double area,vector<Point>& contour,vector<RotatedRect>& gates,vector<int>& index,int temp);
	void SetDefaultParams();
	void ClearProcessData();
	void Draw(Mat& draw);
	void findGateVertices(Mat frame,RotatedRect& gate,vector<vector<Point> > contours,int& gateIdx,vector<Point>& gateVertices);
	void findBoundGates(Mat frame,double area,vector<vector<Point> >& contours,vector<RotatedRect>& gates,vector<int>& index,int temp);
	RotatedRect box2RotatedRect(const Rect rect);
	bool isVerticalRectangle( const vector<Point>& contour,RotatedRect& outRect,Mat frame);
	bool isFindPoles(Mat& frame,vector<RotatedRect>& Rects,RotatedRect& gate,vector<Point>& gateVertices);
	bool isVertic(vector<Point>& rectContour);
	bool isFindGate(Mat frame,vector<RotatedRect>& gates,vector<vector<Point> > contours,vector<int> index,int& gateIdx);
	//configuration
	double _areaSimilarity;
	int _minCurves;
	int _OrangeHue;
	int _erosiontype ;
	int _erosionsize ;
	int _dilationtype ;
	int _dilationsize ;
	int _difPoleCenters;
	double _minRectAreaSize;
	double _heightWidthRatio;
	double _kolmanThresh;
	double _objectImageRatio;
	double _minAngle;
	double _maxAngle;
	double _maxRatio;
	int _blockSize;
	int _c;
	double _isHorizonRec;

	//process images-for test only
	Mat* _kolman;
	Mat* _BW;
	Mat* _contourImage;
	Mat* _outputIm;
	Mat* _rectIm;
	Mat* _eroded;
	Mat* _dilated;


	//processData
	vector<double>* _areaPercent;
	vector<RotatedRect> *_verticalRects;

	//output data
	vector<Point> _gateVertices;
	RotatedRect* _gate;
	bool _isCompleteGate;


private:
};

#endif /* BlackGate2_H_ */
