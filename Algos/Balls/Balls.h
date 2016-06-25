/*
 * Balls.h
 *
 *  Created on: Feb 26, 2013
 *      Author: robosub
 */

#ifndef BALLS_H_
#define BALLS_H_
#include <cv.h>
using namespace cv;
class Balls {
public:
	Balls();
	void Run(Mat& frame,Mat& draw);
	virtual ~Balls();
	int MaxRadius(vector<Vec3f>& circles);
	void DrawCircles(vector<Vec3f> gcircles,vector<Vec3f> rcircles,vector<Vec3f> ycircles, Mat& frame,int maxidxRadiusG,int maxidxRadiusR,int maxidxRadiusY);
	void FindGreen(Mat& Hue_scale, int greenminh,int greenmaxh,Mat& greenthresh,Mat& se21,Mat& se11);
	void FindRed(Mat& Hue_scale, int redminh,int redmaxh,Mat& redthresh,Mat& se21,Mat& se11) ;
	void FindYellow(Mat& Hue_scale, int yellowminh,int yellowmaxh,Mat& yellowthresh,Mat& se21,Mat& se11);
	void ClearProcessData();
	void SetDefaultParams();
	//processData
    vector<Vec3f>* _gcircles;
    vector<Vec3f>* _rcircles;
    vector<Vec3f>* _ycircles;
	//process images
	Mat* _frame;
	Mat* _greenthresh;
	Mat* _redthresh;
	Mat* _yellowthresh;
	//configuration
		bool _inTest;
		int _redminh;
		int _redmaxh;

		int _greenminh;
		int _greenmaxh;

		int _yellowminh;
		int _yellowmaxh;

		int _inverseRatio;
		int _mindistDivide;
		int _upcannyThresh;
		int _centerThresh;
		int _minRadius;
		int _maxRadius;

		int _maxidxRadiusG;
		int _maxidxRadiusR;
		int _maxidxRadiusY;

};

#endif /* BALLS_H_ */
