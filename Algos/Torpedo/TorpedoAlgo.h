#ifndef TORPEDOALGO_H_
#define TORPEDOALGO_H_
#include <cv.h>
#include "../BaseAlgorithm.h"
using namespace std;
using namespace cv;

enum COLOR
{
	BLUE = 1, YELLOW = 2, GREEN = 3, RED = 4
};

struct Target
{
	int NumOfHoles() const
	{
		int count = 0;
		if (bigHole.size.height > 0)
			++count;
		if (smallHole.size.height > 0)
			++count;
		return count;
	}

	void setHole(RotatedRect hole)
	{
		int nHoles = NumOfHoles();
		if (nHoles == 2)
			return;
		if (nHoles == 1)
		{
			RotatedRect tmpHole;
			if (bigHole.size.height > 0)
				tmpHole = bigHole;
			else
				tmpHole = smallHole;
			if (hole.size.area() > tmpHole.size.area())
			{
				bigHole = hole;
				smallHole = tmpHole;
			}
			else
			{
				bigHole = tmpHole;
				smallHole = hole;
			}
			return;
		}
		bigHole = hole;
	}

	int Distance(Target& t) const
	{
		int d = abs(distanceObject(t.object));
		if (HasBigHole() && t.HasBigHole())
			d += abs(distanceBig(t.bigHole));
		if (HasSmallHole() && t.HasSmallHole())
			d += abs(distanceBig(t.smallHole));
		return d;
	}

	int area()
	{
		return object.size.area() - smallHole.size.area() - bigHole.size.area();
	}

	int distance(const RotatedRect& a, const RotatedRect& b) const
	{
		int maxA = max(a.size.width, a.size.height);
		int maxB = max(b.size.width, b.size.height);
		return (maxA - maxB);
	}

	int distanceObject(RotatedRect& o) const
	{
		return distance(o, object);
	}

	int distanceBig(RotatedRect& hole) const
	{
		return distance(hole, bigHole);
	}

	int distanceSmall(RotatedRect& hole) const
	{
		return distance(hole, bigHole);
	}
	bool HasBigHole() const
	{
		return bigHole.size.height > 0;
	}

	bool HasSmallHole() const
	{
		return smallHole.size.height > 0;
	}

	bool empty() const
	{
		return (object.size.width == 0);
	}

	RotatedRect object;
	RotatedRect bigHole;
	RotatedRect smallHole;
};

class TorpedoAlgo: public BaseAlgorithm
{
	//override
public:
	virtual void Run(Mat& image);
	virtual void Load(map<string, string>& params);
	virtual void ToMesseges(vector<MissionControlMessage>& res);
	virtual void ClearProcessData();
	virtual void SetDefaultParams();
	virtual void Draw(Mat& draw);
protected:
	virtual void InitProcessData();
	virtual void InitResult();
	virtual void fixResults(Point offset);

private:
	map<COLOR, Scalar> _paintColors;

public:
	map<COLOR, Target> current_results;
public:
	TorpedoAlgo();
	virtual ~TorpedoAlgo();
	void DrawHoles(const vector<Target>& holes, Mat& draw);
	//process images
	map<COLOR, Mat*> _gray;
	//map<COLOR,vector<Mat*> > _threshs;
	map<COLOR,vector<Mat*> > _contours;

	//params
	int _redHue;
	double _redKolmanThresh;
	int _sizeGarbege; //below this size the object is just a noise
	double _backgroundThresh; //minimum size for background
	double _noisePercent; //how much noise inside hole
	int _openAmount; //open on holes thresh
	int _erodeAmount; //erode on object thresh
	int _threshJumpRed;
	int _threshJumpGreen;
	double _dknownPBig;
	double _dknownPSmall;
	double _sqaureEpsilon;
	double _dKnownPObject;
	double _dOptimalProp;

private:
	void findTargets(const Mat& image, map<COLOR, Target>& targets);
	void FindTargetOfColor(COLOR color, const Mat& image,
			const Mat& objectThresh, Target& target, const Target& knownTarget);
	//TODO: move to utils
	bool IsWhite(const Mat& binaryImage, const vector<Point>& counter,
			double noisePercent);
	bool IsBackground(int rectH, int rectW, int imageH, int imageW);
	void drawColorCountors(const Mat& image,
			const vector<vector<Point> >& countors, COLOR& color);
	void DeleteImages(map<COLOR, vector<Mat*> >& images);
	void DeleteImages(map<COLOR, Mat*>& images);
	bool insidenHole(const Rect& hole, const vector<Rect>& holes);
	void insertRealObjects(map<COLOR, vector<Rect> >& objects,
			map<COLOR, vector<Rect> >& holes, vector<Target>& res);
	void getTargets(COLOR color,const Mat& thresh, vector<RotatedRect>& targets,
			const Target& knownTarget);
	bool IsHoleOf(const Rect& object, const Rect& hole);
	void threshGreen(const Mat& image, Mat& thresh);
	void threshBlue(const Mat& image, Mat& thresh);
	void threshOtsu(const Mat& image, Mat& thresh);
	void threshYellow(const Mat& image, Mat& thresh);
	void deleteObject(Mat& img, const Target& target);
	void findBestHoles(COLOR color,const vector<vector<Point> >& countors,
			const Mat& thresh, Target& target, const Target& knownTarget,Point offset);
	void createHoleThresh(const Mat& thresh, const RotatedRect& target,
			Mat& holeThresh, Rect& frame);

	void fitRectToFrame(Rect& rect, int imageH, int imageW);
	void findRedTarget(const Mat& image, Target& target,
			const Target& knwonTarget);
	void createMSG(RotatedRect rect, MissionControlMessage& msg);
	void Draw(RotatedRect& hole, COLOR color, Mat& draw, bool centerPoint);
	bool isSquare(RotatedRect& rect, double epsilon);
	Rect nonCutRect(RotatedRect& r);
	int SizeTarget(const RotatedRect& r);
	void findGreenTarget(const Mat& image, Target& target,
			const Target& knwonTarget);
	Mat* _yellowThresh;
public:
	string colorToString(COLOR color);
};
#endif
