#ifndef UTILS_H
#define UTILS_H
#include <cv.h>
#include <highgui.h>
#include <highgui.h>
#include "../MissionControlMessage.h"
using namespace cv;
using namespace std;
enum CutDirection
{
	UP =1, DOWN =2, LEFT =3, RIGHT=4
};
class Utils {
public:
	Utils();
	~Utils();
	static vector<int> isCut(const vector<Point>& contor, Size sz);
	static vector<int> CutDirections(const RotatedRect& rect, Size sz);
	static void GetROI(const Mat& image, RotatedRect rect, Mat& ROI);
	static void mergeVectors(const vector<vector<Point> >& arr,
			vector<Point>& merged);
	static Mat* HueChannel(const Mat &image);
	static Mat* SaturationChannel(const Mat &image);
	static Mat* lightChannel(const Mat &image);
	static Mat* RedChannel(const Mat &image);
	static void YellowChannel(const Mat & image,Mat& yellow);
	static Mat DeleteRedChannel(const Mat &image);
	static Mat ScaledChannel(const Mat &image, int ScaleParam);
	static bool Kolaman(float alpha, const Mat &image, Mat &Kolaman_32F,
			Mat &kol_strech, double minthresh);
	static double ColorPercent(const Mat& hueLayer,
			const vector<Point>& contour, int minHue, int maxHue);
	static double ColorPercent(const Mat& hueLayer,
			const vector<Point>& contour, int minHue, int maxHue, int maxVal);
	static int ColorAmount(const Mat& hueLayer, const vector<Point>& contour,
			int minHue, int maxHue, int maxVal);
	static void ShowImage(const Mat& img, const string& windowName);
	static double angle(const Point& pt1, const Point& pt2, const Point& pt0);
	static void drawRotatedRect(const RotatedRect &square, Mat& draw,
			Scalar color, int thickness=1);
	static void GetContor(const RotatedRect& square, vector<Point>& pointsVec);

	//take only 1 channel images
	//paint an area with pixels inside a range.
	//the lowDIfference is the difference between pixel found in range and minimum value that still count to be part of the area.
	//the upperDIfference is the difference between pixel found in range and maximum value that still count to be part of the area.
	//ATTENTION: if you paint with value that can be count as part of the area,it could lead to wrong results.
	static void PaintContinuesArea(Mat& gray, int lowRange, int highRange,
			int lowerDifference, int upperDifference, int paintValue,
			bool diffFromFoundPixel);
	static void fixAngle(RotatedRect& rect);
	static void FindHeightWidth(vector<Point>& vertices, double& height,
			double& width); //find rect width and height, where
	/*
	 * fill the message bounds with the edge points of the the rectangle
	 * the bounds are in this order : Left Bottom,Top Left ,Top Right ,Bottom Right.
	 */
	static void FillRectBounds(const Rect& rect, MissionControlMessage& msg);
	/*
	 * fill the message bounds with the edge points of the the rectangle
	 * the bounds are in this order : Left Bottom,Top Left ,Top Right ,Bottom Right.
	 */
	static void FillRectBounds(const RotatedRect& rect,
			MissionControlMessage& msg);
	/*
	 * ASSUMING ONLY 4 points
	 * fill the message bounds with the points
	 * the bounds are in this order : Left Bottom,Top Left ,Top Right ,Bottom Right.
	 */
	static void FillRectBounds(RotatedRect& rect, vector<Point>& bounds);
	static void FillBounds(const vector<Point>& bounds,
			MissionControlMessage& msg);
	static void orderBounds(vector<Point>& bounds);
	static Mat* CopyImage(const Mat& image);
	static double BlackPercent(const Mat& binaryImage,
			const vector<Point>& counter);
	static float getMaxKolmanValue(float alpha, int maxR, int maxG, int maxB);
	static float calcKolman(float alpha, int R, int G, int B);
	static double KolmanColorPercent(float alpha,Mat& kolman32F,const vector<Point>& shape);
	static double kolmanPercent(float val,float alpha);
	static void GetMask(Size sz, const RotatedRect& rect,Mat& mask);
	static double MeanOfShape(const Mat& gray,const vector<Point>& shape);
	static void DrawHistogram(const Mat& gray,int binSize, Mat& draw);
	static void Rotate90(Mat& image);
	static bool isRectangle(const vector<Point>& contor,RotatedRect& rect,Size imageSize,double deviation);
private:
	static void KolmanFormula(float alpha, const Mat& RED, const Mat& GREEN,
			const Mat& BLUE, Mat& res);
	static float calcKolmanInner(float alpha, int R, int G, int B);
	static int colorAmountInner(const Mat& hueLayer,
			const vector<Point>& contour, int minHue, int maxHue, int maxVal,
			MatND& hist);
};

#endif
