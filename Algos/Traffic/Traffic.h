/*
 * Traffic.h
 *
 *  Created on: Mar 17, 2013
 *      Author: itzik
 */

#ifndef TRAFFIC_H_
#define TRAFFIC_H_
#include <cv.h>
#include "../BaseAlgorithm.h"
using namespace cv;
class Traffic : public BaseAlgorithm
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
	//configuration
	int _inverseRatio;
	int _mindistDivide;
	int _upcannyThresh;
	int _centerThresh;
	int _minRadius;
	int _maxRadius;
	int _maxidxRadiusR;
	int _maxmeancircle;
	int _maxmeanfakecircle;
	int _minmeanfakecircle;
	int _borderPixels;
	int _lowDif;
	int _upperDif;
	int _param1;// used for the (internally called) canny edge detector
	int _param2;//sets the minimum number of “votes” that an accumulator cell needs to qualify as a possible circle.
	double _percentNormalArea;
	double _percentKolamanThresh;
	double _minRectPrecent;
	double _maxBufferPercent;
	int _maxsizerect;
	int _minsizerect;
	double _maxrectratio;
	double _minrectratio;
	double _rectratio;
	bool _redexist;
	bool _greenexist;
	bool _yellowexist;
	int _redDev;
	int _greenDev;
	int _yellowDev;
	int _redalpha;
	int _greenalpha;
	int _yellowalpha;
	int _redminh;
	int _redmaxh;
	int _greenminh;
	int _greenmaxh;
	int _yellowminh;
	int _yellowmaxh;

	int _erosiontype;
	int _erosionsize;
	int _dilationtype;
	int _dilationsize;

	//process images
	Mat* _ballkol;
	Mat* _hue;
	Mat* _redkol;
	Mat* _greenkol;
	Mat* _yellowkol;
	Mat* _greenthresh;
	Mat* _redthreshball;
	Mat* _redthreshsquare;
	Mat* _yellowthresh;

	Mat* _greencontour;
	Mat* _redsquarecontour;
	Mat* _redballcontour;
	Mat* _yellowcontour;

	Mat* _imgredballs;
	Mat* _imgredsquare;
	Mat* _imgyellowsquares;
	Mat* _imggreensquares;

	//processData
    vector<Vec3f>* _rcircles;
	vector<double>* _redsquarePercent;
	vector<double>* _yellowPercent;
	vector<double>* _greenPercent;
	vector<RotatedRect>* _redsquares;
	vector<RotatedRect>* _yellowsquares;
	vector<RotatedRect>* _greensquares;

	Traffic();
	~Traffic();
	void ClearProcessData();
	void SetDefaultParams();
	void Run2(const Mat& image,Vec3f& redballres,RotatedRect& redrsquareres,RotatedRect& yellowres,RotatedRect& greenres);
	double PercentRectangle( const vector<Point>& contour , RotatedRect& outRect);
	int  MaxRadius(vector<Vec3f>& circles,const Mat& image);
	bool isRed( vector<Point> countor, const Mat& hueLayer );
	bool isYellow( vector<Point> countor, const Mat& hueLayer );
	bool isGreen( vector<Point> countor, const Mat& hueLayer );
	void ThreshStruct(Mat& Hue_scale, int minh,int maxh,Mat& colorthresh,int flag);
	void drawRect(Mat& draw,const RotatedRect& square,Scalar color);
	void DrawCircle(Vec3f& rcircles, Mat& frame);
	void Draw(Mat& draw);
private:
	void findBuoys( const Mat& image,Vec3f& redball,RotatedRect& redsquare,RotatedRect& yellowsquares,RotatedRect& greensquares);
	bool AreaCheck(const Mat& image,vector<Point>& contour);
	void static drawAngle( float angle, int length,const Point& center,Mat& draw );
	void static drawPoint( const Point& center, Mat& draw );
	void fixAngle( RotatedRect& rect );
	void PaintRed(Mat& hue );
	void PaintYellow(Mat& hue );
	void PaintGreen(Mat& hue );
public:
    Vec3f RedBall_current_results;
    Vec3f Greenb_current_results;
    Vec3f Yellowb_current_results;
    Vec3f Redb_current_results;
	RotatedRect RedSquare_current_results;
	RotatedRect Yellow_current_results;
	RotatedRect Green_current_results;
};
#endif /* TRAFFIC_H_ */
