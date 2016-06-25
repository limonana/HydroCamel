#include "PathAlgorithm.h"
#include <highgui.h>
#include "math.h"
#include "../Utils/Utils.h"
#include "../Utils/ParamUtils.h"

void PathAlgorithm::Run(Mat& image)
{
	current_results.clear();
	RotatedRect r1,r2;
	Run2(image, r1,r2);
	if (r1.size.height >0)
		current_results.push_back(r1);
	if (r2.size.height >0)
			current_results.push_back(r2);
}
void PathAlgorithm::Load(map<string, string>& params)
{
	ParamUtils::setParamPercent(params, "Area", _areaSimilarity);
	ParamUtils::setParam(params, "OrangeHue", _OrangeHue);
	ParamUtils::setParamPercent(params,"RatioPercent",_minRatio);
	ParamUtils::setParamPercent(params,"kolmanDev",_kolmanDev);
	ParamUtils::setParam(params,"erode",_erode);
	stringstream a(params["NormalArea"]);
	int tmp;
	a >> tmp;
	_percentNormalArea = (double)tmp / 1000;
}
PathAlgorithm::~PathAlgorithm()
{

}

void PathAlgorithm::ClearProcessData()
{
	if (this->_thresh)
	{
		delete _thresh;
		_thresh = NULL;
	}
	if (this->_kolman!= NULL)
		{
			delete _kolman;
			_kolman = NULL;
		}
	if (this->_counterImage != NULL)
	{
		delete _counterImage;
		_counterImage = NULL;
	}
	if (this->_areaPercent != NULL)
	{
		delete _areaPercent;
		_areaPercent = NULL;
	}
	if (this->_ratios != NULL)
	{
		delete _ratios;
		_ratios = NULL;
	}
}

void PathAlgorithm::InitProcessData()
{
	_kolman = NULL;
	_counterImage = NULL;
	_areaPercent = NULL;
	_ratios = NULL;
	_thresh = NULL;
}

void PathAlgorithm::SetDefaultParams()
{
	_areaSimilarity = 0.70;
	_percentNormalArea = 0.001;
	_minRatio = 1.50;
	_OrangeHue = 30;
	//_kolmanThresh = 0.4;
	_erode =5;
}

vector<RotatedRect>::iterator PathAlgorithm::findMaxArea(vector<RotatedRect>& paths)
{
	vector<RotatedRect>::iterator maxIt = paths.begin();
	vector<RotatedRect>::iterator it;
	for (it=paths.begin(); it!=paths.end();++it)
	{
		if (it->size.width > maxIt->size.width)
		{
			maxIt = it;
		}
	}
	return maxIt;
}
void PathAlgorithm::Run2(const Mat& image, RotatedRect& path1, RotatedRect& path2)
{
	if (_inTest)
	{
		this->_areaPercent = new vector<double>();
		this->_ratios = new vector<double>();
	}
	vector<RotatedRect> tmp;
	findPaths(image, tmp);
	if (tmp.size()>10) //kolman failed - just crap
		return;
	//filter paths if more then 2
	if (tmp.size()>2)
	{
		vector<RotatedRect>::iterator maxIt =findMaxArea(tmp);
		path1=*maxIt;
		tmp.erase(maxIt);
		path2 = *findMaxArea(tmp);

	}
	else
	{
		if (tmp.size()==1)
		{
			path1=tmp[0];
			return;
		}
		if (tmp.size() == 2)
		{
			if (tmp[0].size.width > tmp[1].size.width)
			{
				path1=tmp[0];
				path2=tmp[1];
			}
			else
			{
				path1=tmp[1];
				path2=tmp[0];
			}
		}
	}

	float factor = 0.6;
	//use width because when cut height is changing
	if (path2.size.width  < path1.size.width * factor)
	{
		RotatedRect empty;
		path2 = empty;
	}
}

void PathAlgorithm::drawPath(Mat& draw, const vector<RotatedRect>& squares)
{
	for (uint i = 0; i < squares.size(); i++)
	{
		const RotatedRect& square = squares[i];

		Utils::drawRotatedRect(square, draw, Scalar(0, 255, 255), 1);
		int l = MAX(square.size.width,square.size.height);
		//we draw the angle from X axis and the rectangle contains the
		//angle from Y axis
		//TODO: change  to my object/Yuval object who contains the data as i need it.
		drawAngle(90 - square.angle, l, square.center, draw);
		drawPoint(square.center, draw);
	}
}

void PathAlgorithm::filterByShape(vector<vector<Point> >& contors,
		const Mat& gray, Mat& thresh, Mat& kolman32F,
		vector<RotatedRect>& squares) {
	for (uint i = 0; i < contors.size(); ++i) {
		//filter noise
		if (contors.at(i).size() < 4)
			continue;

		double area = contourArea(contors.at(i));
		if (area < _percentNormalArea * gray.size().area())
			continue;

		if (Utils::BlackPercent(thresh, contors[i]) > 0.5)
			continue; //we found a black area instead of white


		RotatedRect rect;
		if (isRectangle(contors[i], rect, gray.size())) {
			squares.push_back(rect);
		}
	}

}

void PathAlgorithm::getPathObjects(Mat& thresh, const Mat& image,
		Mat& kolman32F, vector<RotatedRect>& squares) {
	vector < vector<Point> > contors;
	findContours(thresh, contors, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	if (_inTest) {
		if (_counterImage!= NULL)
			delete _counterImage;
		_counterImage = new Mat();
		image.copyTo(*_counterImage);
		for (uint i = 0; i < contors.size(); ++i)
			drawContours(*_counterImage, contors, i, Scalar(255, 255, 255), 2);
	}
	vector < vector<Point> > filteredContors;
	filterByColor(contors,kolman32F,filteredContors);
	filterByShape(filteredContors, image, thresh, kolman32F, squares);
}

void PathAlgorithm::findPaths(const Mat& image, vector<RotatedRect>& squares)
{
	Mat kolman;
	//TODO: no need for copy
	Mat kolman32F;
	Utils::Kolaman(_OrangeHue, image, kolman32F, kolman,0);
	if (_inTest)
	{
		_kolman = new Mat();
		kolman.copyTo(*_kolman);
	}

	Mat thresh;
	threshold(kolman,thresh,0,255,THRESH_OTSU);
	int erodeVal = _erode;
	if (((float) ((countNonZero(thresh))) / image.size().area()) > 0.5) {
		Scalar mean, dev;
		meanStdDev(kolman, mean, dev);
		threshold(kolman, thresh, 255 - dev(0), 255, THRESH_BINARY);
		erodeVal = 0;
	}
	erode(thresh, thresh, Mat(), Point(-1, 1), erodeVal);
	if (_inTest) {
		_thresh = new Mat();
		thresh.copyTo(*_thresh);
	}
	getPathObjects(thresh, image, kolman32F, squares);

	if (squares.size() == 0)
	{
		Scalar mean,dev;
		meanStdDev(kolman,mean,dev);
		threshold(kolman,thresh,255-dev(0),255,THRESH_BINARY);
		if (_inTest) {
				_thresh = new Mat();
				thresh.copyTo(*_thresh);
			}
		getPathObjects(thresh, image, kolman32F, squares);
	}
}

void PathAlgorithm::drawAngle(float angle, int length, const Point& center, Mat& draw)
{
	float PI = 3.14159265;
	// y is "opposite" because is start from top and not from bottom
	int y1 = center.y - sin(angle * PI / 180.0) * (length / 2);
	int x1 = center.x + cos(angle * PI / 180.0) * (length / 2);
	int y2 = center.y + sin(angle * PI / 180.0) * (length / 2);
	int x2 = center.x - cos(angle * PI / 180.0) * (length / 2);

	line(draw, Point(x1, y1), Point(x2, y2), Scalar(255, 0, 0), 2, CV_AA);
}

void PathAlgorithm::drawPoint(const Point& center, Mat& draw)
{
	rectangle(draw, center, center, Scalar(255, 255, 255), 3);
}

//find the retangle of the contor
//check similarity by area
bool PathAlgorithm::isRectangle(const vector<Point>& contour, RotatedRect& outRect, Size imageSize)
{
	outRect = minAreaRect(contour);
	Utils::fixAngle(outRect);
	//the height supposed to be  a lot larger then the width.
	//because we want to find even the edge of path we will satisfy with a rectnagle.
	double ratio = ((double) outRect.size.height / outRect.size.width);
	if (_inTest)
		_ratios->push_back(ratio);
	if (ratio <= _minRatio) return false;
	//fix rectangle out of frame so the mesure of area will be correct.
	//cannot satisfied only with similarity percent because the difference could be twice the size.
	//(example: path with low angle at the bottom of the frame)
	vector<Point> rectCountor;
	Utils::GetContor(outRect, rectCountor);
	//not supposed to be when angle is zero
	//and cannot calculate tan(90)
	if (outRect.angle != 0)
	{
		float PI = 3.14159265;
		double angleXRad = (90 - outRect.angle) * PI / 180.0; //the angle from x axis
		for (uint i = 0; i < rectCountor.size(); i++)
		{
			if (rectCountor[i].x < 0)
			{
				//when angle < 90 going the same direction : tan >0
				//but y start from up
				rectCountor[i].y -= tan(angleXRad) * (-rectCountor[i].x);
				rectCountor[i].x = 0;
			}
			if (rectCountor[i].x > imageSize.width)
			{
				//when angle < 90 going the oppsite direction : tan >0
				//but y start from up
				rectCountor[i].y += (rectCountor[i].x - imageSize.width) * tan(angleXRad);
				rectCountor[i].x = imageSize.width;
			}
			if (rectCountor[i].y < 0)
			{
				//when angle < 90 going the oppsite direction : tan >0
				rectCountor[i].x -= -rectCountor[i].y / tan(angleXRad);
				rectCountor[i].y = 0;
			}
			if (rectCountor[i].y > imageSize.height)
			{
				//when angle < 90 going the same direction : tan >0
				rectCountor[i].x += (rectCountor[i].y - imageSize.height) / tan(angleXRad);
				rectCountor[i].y = imageSize.height;
			}
		}
	}

	double rectArea = contourArea(rectCountor);
	double area = contourArea(contour);
	/*if (outRect.size.area()< rectArea)
		cerr << "the rectangle supposed to get smaller" << endl;*/

	double maxArea = max(area, rectArea);
	double minArea = min(area, rectArea);

	if (_inTest)
	{
		_areaPercent->push_back(minArea / maxArea);
		/*vector<vector<Point> > c;
		c.push_back(rectCountor);
		drawContours(*_counterImage, c, 0, Scalar(0, 0, 255));*/
	}
	return (minArea / maxArea) > _areaSimilarity;
}

void PathAlgorithm::InitResult()
{
	current_results.clear();
}

PathAlgorithm::PathAlgorithm() :
		BaseAlgorithm()
{
}

void PathAlgorithm::ToMesseges(vector<MissionControlMessage>& res)
{
	//Message:
	//additionalInformation is the angle between the long side of the path and X axis.
	//first 4 points of bounds are the edge points of the path in that order:
	//Left Bottom,Top Left ,Top Right ,Bottom Right.
	//the 5th point of bounds is a pair of height and width of the path.
	//the first is the longer .
	//intrestPoints contain the center point of the path
	for (uint i = 0; i < current_results.size(); ++i)
	{
		RotatedRect& path = current_results[i];
		MissionControlMessage msg;
		msg.MissionCode = Path;
		msg.additionalInformation = 90 - path.angle; //the angle between long side and X axis
		vector<Point> bounds;
		Utils::FillRectBounds(path, msg);
		if (path.size.height >= path.size.width)
			msg.bounds.push_back(std::pair<int, int>(path.size.height, path.size.width));
		else
			msg.bounds.push_back(std::pair<int, int>(path.size.width, path.size.height));
		msg.intrestPoints.push_back(std::pair<int, int>(path.center.x, path.center.y));

		res.push_back(msg);
	}
}

void PathAlgorithm::Draw(Mat& draw)
{
	PathAlgorithm::drawPath(draw, this->current_results);
}

void PathAlgorithm::filterByColor(const vector<vector<Point> >& contors,
		const Mat& kolman, vector<vector<Point> >& filteredContors)
{
	double avg = mean(kolman)(0);
	for (uint i=0; i<contors.size(); ++i)
	{
		double avgShape = Utils::MeanOfShape(kolman,contors.at(i));
		if (avgShape > avg + abs(_kolmanDev*avg))
			filteredContors.push_back(contors.at(i));
	}
}

