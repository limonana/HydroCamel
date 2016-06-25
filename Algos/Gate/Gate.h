/*
 * Gate.h
 *
 *  Created on: Feb 26, 2013
 *      Author: raz
 */

#ifndef GATE_H_
#define GATE_H_
#include <cv.h>
#include "../BaseAlgorithm.h"
#include "../Traffic/Traffic.h"

using namespace cv;
using namespace std;

class Gate: public BaseAlgorithm
{
public:
	virtual void Run(Mat& image);
	virtual void Load(map<string,string>& params);
	virtual void ToMesseges(vector<MissionControlMessage>& res);
protected:
	virtual void InitProcessData();
	virtual void InitResult();
public:
	Gate(Traffic* itzik); //Traffic must be initialized and the parameters are loaded.
	void Run2(Mat& frame,RotatedRect& gate);


	////this function finds for each object the smallest rectangle covering it
	// and check the ratio between the object area and the rectangle area
	//and to check if there was a full detection or only partial
	bool IsGate(double area,vector<Point>& contour,RotatedRect& gate );
	bool isHorizontalRectangle( const vector<Point>& contour,RotatedRect& outRect,int frameWidth,int& rectWidth,int& rectHeight);
	bool isHorizon(vector<Point>& rectContour,int& rectWidth,int& rectHeight);
	bool isVerticalRectangle( const vector<Point>& contour,RotatedRect& outRect,Mat roiFrame);
	bool isVertic(vector<Point>& rectContour,int& rectHeight);
	bool isInRoi(vector<Point>& contour,Rect roi);
	bool isConnectedVertical(Mat frame,vector<RotatedRect>& verticalPoles,RotatedRect& gate,vector<Point>& gateVertices, RotatedRect horizonPole,int horizonRectWidth);
	bool isFindGate(Mat frame,vector<RotatedRect>& verticalPoles,RotatedRect& gate,vector<Point>& gateVertices, RotatedRect horizonPole,int horizonRectWidth);// check if 2 connected poles found or 1 or none
	virtual ~Gate();
	void Draw(Mat& draw);
	void SetDefaultParams();
	void ClearProcessData();
	void findHorizonPole(vector<RotatedRect> horizonRects,RotatedRect& horizonPole,vector<vector<Point> > contours,vector<int>  index,vector<int> rectWidth,int& horizonRectWidth,vector<int> rectHeight,int& horizonRectHeight);
	void getRoi(Mat& frame,RotatedRect& horizonPole,Rect &roi,int horizonRectWidth,int horizonRectHeight,int& newOriginX,int& newOriginY);
	void findGateVertices(RotatedRect horizonPole,vector<RotatedRect> verticalPoles,vector<Point>& gateVertices,RotatedRect& gate,int flag);
	void findVerticalPoles(vector<RotatedRect> verticalPoles,vector<RotatedRect>& gateVerticPoles,vector<vector<Point> > contours,vector<int> index,RotatedRect& horizonPole);

	//process images-for test only
	Mat* _kolman;
	Mat* _BW;
	Mat* _contourImage;
	Mat* _outputIm;
	Mat* _rectIm;
	Mat* _eroded;
	Mat* _dilated;
	Mat* _roiFrame;

	bool _isPole;// flag for indicating if horizon pole been found.
	bool _isTrafficGreen;//flag for indicating if found gate with 2 poles or three.
	//processData -
	int _kerRowNum;// # of rows for morphological kernell
	int _kerColNum;// # of colums for morphological kernell
	int _treshBlockSize;
	int _C;//const to add from avg for treshold
	int _kerC;//const to add/subtract from kernellColNum
	int _minCurves;//below this num, contour is noise
	int _minPixelsInROI;//above this amount of pixels object is in roi
	int _greenAlpha;
	int _erosiontype;
	int _erosionsize;
	int _dilationtype;
	int _dilationsize;
	double _kolmanThresh;
	double _baseFactor;//above this factor mul with frame width roi is all the image
	double _roiOriginX_TO_BaseWidthFactor;//beyond this factor mul with base width roi origin x is zero
	double _objectImageFactor;//below this factor mul with frame size object is noise
	double _baseWidthTOimageFactor;// beyond this factor mul frame width, object width is large enough to be horizon pole
	double _minAngle;//beyond this angle the rect is horizon nor vertic
	double _maxAngle;//below this angle the rect is horizon nor vertic
	double _areaSimilarity;// beyond this ratio object is a rectangle
	double _YverticPolesMaxDiff;//beyond this factor mul frame width,2 rectangles r not connected
	double _XverticPolesMaxDiff;//beyond this factor mul frame width,2 rectangles r not connected
	double _XverticPolesMinDiff;//below this factor mul frame width,2 rectangles r not connected
	double _ConnectVerticAreaSimilarityMin;//beyond this ratio 2 rects are suspect to be connected as vertic poles
	double _ConnectVerticAreaSimilarityMax;//below this ratio 2 rects are suspect to be connected as vertic poles
	double _minRectHeight;// minimum height factor from image height required to be taken as vertical pole
	double _XminDistVertHoriz;//minimum distance in x axes between horiz pole and vertical pole
	double _XmaxDistVertHoriz;//maximum distance in x axes between horiz pole and vertical pole
	double _YminDistVertHoriz;//minimum distance in y axes between horiz pole and vertical pole
	double _YmaxDistVertHoriz;//maximum distance in x axes between horiz pole and vertical pole
	double _minHorVerLeng;// minimum length ratio between horizon pole to vertical pole;
	double _maxHorVerLeng;// maximum length ratio between horizon pole to vertical pole;
	vector<double>* _areaPercent;
	vector<double> _vertAreaPercent;
	RotatedRect* _horizonPole;
	vector<RotatedRect>* _gateVerticalPoles;
	//Traffic* _itzik;

	//output data
	RotatedRect* _gate;
	vector<Point> _gateVertices;

private:

};

#endif /* GATE_H_ */
