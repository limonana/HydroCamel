#ifndef PATH_ALGO_H
#define PATH_ALGO_H
#include <cv.h>
#include "../BaseAlgorithm.h"
using namespace cv;
class PathAlgorithm : public BaseAlgorithm
{
	//override
public:
	virtual void Run(Mat& image);
	virtual void Load(map<string,string>& params);
	virtual void ToMesseges(vector<MissionControlMessage>& res);
	virtual void ClearProcessData();
	virtual void SetDefaultParams();
	virtual void Draw(Mat& draw);
protected:
	virtual void InitProcessData();
	virtual void InitResult();

public:
	//orangeColor= [238 118 0]; RGB
	//30  100  93 hsv
	//configuration
	double _areaSimilarity;
	double _percentNormalArea;
	double _minRatio;
	int _OrangeHue;
	double _kolmanDev;
	int _erode;

	//process images
	Mat* _counterImage;
	Mat* _kolman;
	Mat* _thresh;


	//processData
	vector<double>* _areaPercent;
	vector<double>* _ratios;

	PathAlgorithm();
	~PathAlgorithm();
	static void drawPath(Mat& draw, const vector<RotatedRect>& squares);
	void Run2(const Mat& image, RotatedRect& path1, RotatedRect& path2);

private:
	void findPaths(const Mat& image, vector<RotatedRect>& squares);
	bool isOrange(vector<Point> countor, const Mat& hueLayer);
	static void drawAngle(float angle, int length, const Point& center, Mat& draw);
	bool isRectangle(const vector<Point>& contour, RotatedRect& outRect, Size imageSize);
	static void drawPoint(const Point& center, Mat& draw);
	vector<RotatedRect>::iterator findMaxArea(vector<RotatedRect>& paths);
	void filterByShape(vector<vector<Point> >& contors, const Mat& image,
			Mat& thresh, Mat& kolman32F, vector<RotatedRect>& squares);
	void getPathObjects(Mat& thresh, const Mat& image, Mat& kolman32F,
			vector<RotatedRect>& squares);
	void filterByColor(const vector<vector<Point> >& contors,const Mat& kolman32F,vector<vector<Point> >& filteredContors);
public:
	vector<RotatedRect> current_results;
};
#endif
