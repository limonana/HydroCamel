#include "Utils.h"

Utils::Utils()
{
}

Utils::~Utils()
{
}

vector<int> Utils::CutDirections(const RotatedRect& rect, Size sz) {
	vector<Point> rectContor;
	Utils::GetContor(rect, rectContor);
	return Utils::isCut(rectContor, sz);
}

vector<int> Utils::isCut(const vector<Point>& contor, Size sz) {
	//find contours use a border of 1 pixel.
	vector<int> cutDirections;
	for (uint i = 0; i < contor.size(); ++i) {
		int x = contor[i].x;
		int y = contor[i].y;
		if (x <= 1) {
			cutDirections.push_back(LEFT);
		}
		if (y <= 1)
			cutDirections.push_back(UP);
		if (x >= sz.width - 2) //the last row / col is size-1=> 1 pixel border=> 2
			cutDirections.push_back(RIGHT);
		if (y >= sz.height - 2)
			cutDirections.push_back(DOWN);
	}
	return cutDirections;
}

void Utils::mergeVectors(const vector<vector<Point> >& arr,
		vector<Point>& merged)
{
	for (uint i = 0; i < arr.size(); i++)
	{
		if (arr[i].size() > 1)
			merged.insert(merged.end(), arr[i].begin(), arr[i].end());
	}
}

Mat* Utils::HueChannel(const Mat &image)
{
	Mat hsv(image.size(), image.type());
	cvtColor(image, hsv, CV_BGR2HSV);
	int ch[] =
	{ 0, 0 };
	Mat* gray = new Mat(image.size(), CV_8U);
	mixChannels(&hsv, 1, gray, 1, ch, 1);
	return gray;
}
Mat* Utils::SaturationChannel(const Mat &image)
{
	Mat hsv(image.size(), image.type());
	cvtColor(image, hsv, CV_BGR2HSV);
	int ch[] =
	{ 1, 0 };
	Mat* gray = new Mat(image.size(), CV_8U);
	mixChannels(&hsv, 1, gray, 1, ch, 1);
	return gray;
}
Mat* Utils::lightChannel(const Mat &image)
{
	Mat hsv(image.size(), image.type());
	cvtColor(image, hsv, CV_BGR2HLS);
	int ch[] =
	{ 1, 0 };
	Mat* gray = new Mat(image.size(), CV_8U);
	mixChannels(&hsv, 1, gray, 1, ch, 1);
	return gray;
}

Mat* Utils::RedChannel(const Mat &image)
{
	int ch[] =
	{ 2, 0 };
	Mat* gray = new Mat(image.size(), CV_8U);
	mixChannels(&image, 1, gray, 1, ch, 1);
	return gray;
}

Mat Utils::DeleteRedChannel(const Mat &image)
{
	int ch[] =
	{ 0, 0, 1, 1 };
	Mat BG = Mat(image.size(), image.type());
	mixChannels(&image, 1, &BG, 1, ch, 2);
	Mat zero = Mat::zeros(image.size(), CV_8UC1);
	int ch2[] =
	{ 0, 2 };
	mixChannels(&zero, 1, &BG, 1, ch2, 1);
	return BG;
}

void Utils::ShowImage(const Mat& img, const string& windowName)
{
	namedWindow(windowName, CV_WINDOW_NORMAL);
	imshow(windowName, img);
	char c = 'a';
	while (c != ' ')
		c = waitKey(0);
}

// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double Utils::angle(const Point& pt1, const Point& pt2, const Point& pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	double mechane = sqrt(
			(dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
	if (mechane == 0)
		throw logic_error("division by zero");
	return (dx1 * dx2 + dy1 * dy2) / mechane;
}

double Utils::ColorPercent(const Mat& hueLayer, const vector<Point>& contour,
		int minHue, int maxHue)
{
	return ColorPercent(hueLayer, contour, minHue, maxHue, 180);
}
double Utils::ColorPercent(const Mat& grey, const vector<Point>& contour,
		int minHue, int maxHue, int maxVal)
{
	MatND hist;
	float chosenAmount = colorAmountInner(grey, contour, minHue, maxHue, maxVal,
			hist);
	double allPixels = 0;
	for (int i = 0; i <= maxVal; ++i)
	{
		allPixels += hist.at<float>(i);
	}

	if (allPixels == 0)
		throw logic_error("no pixels at given range");
	double precent = chosenAmount / allPixels;
	return precent;
}

int Utils::ColorAmount(const Mat& hueLayer, const vector<Point>& contour,
		int minHue, int maxHue, int maxVal)
{
	MatND hist;
	return colorAmountInner(hueLayer, contour, minHue, maxHue, maxVal, hist);
}

double Utils::BlackPercent(const Mat& binaryImage, const vector<Point>& counter)
{
	//TODO: the loop inside is waise from 1 to 255.
	return Utils::ColorPercent(binaryImage, counter, 0, 0, 255);
}

float Utils::getMaxKolmanValue(float alpha, int maxR, int maxG, int maxB)
{
	float maxValue = -256;
	for (int R = 0; R <= 1; ++R)
		for (int G = 0; G <= 1; ++G)
			for (int B = 0; B <= 1; ++B)
			{
				float kol = calcKolman(alpha, R * maxR, G * maxG, B * maxB);
				if (kol > maxValue)
					maxValue = kol;
			}
	return maxValue;
}

float Utils::calcKolman(float alpha, int R, int G, int B)
{
	float beta;
	if (alpha < 181)
		beta = alpha + 180;
	else
		beta = alpha - 180;
	return calcKolmanInner(alpha, R, G, B) - calcKolmanInner(beta, R, G, B);
}

float Utils::calcKolmanInner(float alpha, int R, int G, int B)
{
	if (alpha == 0 || alpha == 360)
	{
		return R;
	}
	if (alpha > 0 && alpha < 120)
	{
		return ((120 - alpha) * R + alpha * G) / 120;
	}
	if (alpha == 120)
	{
		return G;
	}
	if (alpha > 120 && alpha < 240)
	{
		return (((240 - alpha) * G + (alpha - 120) * B) / 120);
	}
	if (alpha == 240)
	{
		return B;
	}
	if (alpha > 240 && alpha < 360)
	{
		return ((360 - alpha) * B + (alpha - 240) * R) / 120;
	}
}

double Utils::kolmanPercent(float val, float alpha)
{
	float maxAlpha = Utils::getMaxKolmanValue(alpha, 255, 255, 255);
	if (maxAlpha == 0)
		throw logic_error("max value for kolman can't be zero");
	return (val + maxAlpha) / (2 * maxAlpha);
}

double Utils::KolmanColorPercent(float alpha, Mat& kolman32F,
		const vector<Point>& shape)
{
	Mat mask = Mat::zeros(kolman32F.size(), CV_8UC1);
	vector<vector<Point> > c;
	c.push_back(shape);
	drawContours(mask, c, 0, Scalar(255), -1);
	Scalar avg = mean(kolman32F, mask);
	return kolmanPercent(avg(0), alpha);
}

double Utils::MeanOfShape(const Mat& gray,const vector<Point>& shape)
{
	vector<vector<Point> > c;
	Mat mask = Mat::zeros(gray.size(),CV_8UC1);
	c.push_back(shape);
	drawContours(mask, c, 0, Scalar(255), -1);
	Scalar avg = mean(gray, mask);
	return avg(0);
}

void Utils::GetMask(Size sz, const RotatedRect& rect, Mat& mask)
{
	mask = Mat::zeros(sz, CV_8UC1);
	vector<Point> rectContor;
	Utils::GetContor(rect, rectContor);
	vector<vector<Point> > c;
	c.push_back(rectContor);
	drawContours(mask, c, 0, Scalar(255), -1);

}

void Utils::DrawHistogram(const Mat& gray, int binSize, Mat& draw)
{
	if (binSize == 0)
		throw invalid_argument("bin size can't be zero");

	int histSize = 256 / binSize;
	if (histSize == 0)
		throw invalid_argument("hist size can't be zero, bin Size is larger then 256");

	/// Set the ranges ( for B,G,R) )
	float range[] =
	{ 0, 256 };
	const float* histRange =
	{ range };

	bool uniform = true;
	bool accumulate = false;

	Mat hist;

	/// Compute the histograms:
	calcHist(&gray, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform,
			accumulate);

	int hist_w = 255 * 2;
	int hist_h = 400;
	int bin_w = cvRound((double) hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage,
				Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
				Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
				Scalar(255, 0, 0), 2, 8, 0);
	}

	draw = histImage;
}


void Utils::YellowChannel(const Mat& image,Mat& yellow)
{
	vector<Mat> BGR;
		split(image, BGR);
		BGR[0].convertTo(BGR[0], CV_32F, 1.0 / 255, 0);
		BGR[1].convertTo(BGR[1], CV_32F, 1.0 / 255, 0);
		BGR[2].convertTo(BGR[2], CV_32F, 1.0 / 255, 0);

		Mat black(image.size(), CV_32F);
		for (int x = 0; x < image.size().width; ++x)
			for (int y = 0; y < image.size().height; ++y)
				black.at<float>(y, x) = 1
						- std::max(
								std::max(BGR[0].at<float>(y, x),
										BGR[1].at<float>(y, x)),
								BGR[2].at<float>(y, x));

		yellow = Mat(image.size(),CV_32F);
		for (int x = 0; x < image.size().width; ++x)
			for (int y = 0; y < image.size().height; ++y) {
				float k = black.at<float>(y, x);
				yellow.at<float>(y, x) = (1 - (BGR[0].at<float>(y, x)) - k)
						/ (1 - k);
			}
		yellow.convertTo(yellow, CV_8UC1, 255, 0);
}

void Utils::GetROI(const Mat& image, RotatedRect rect, Mat& ROI)
{
	Rect frame = rect.boundingRect();
	Size sz = frame.size();
	Point center = Point(frame.x+sz.width/2,frame.y+sz.height/2);
	Mat frameImage;
		getRectSubPix(image,sz,center,frameImage);
		//Utils::ShowImage(frameImage,"frame");
		rect.center.x -= frame.x;
		rect.center.y -= frame.y;

		// matrices we'll use
		Mat M, rotated;
		// get angle and size from the bounding box
		float angle = rect.angle;
		Size rect_size = rect.size;
		// thanks to http://felix.abecassis.me/2011/10/opencv-rotation-deskewing/
		//TODO: understand why this is needed
		if (rect.angle < -45.)
		{
			angle += 90.0;
			swap(rect_size.width, rect_size.height);
		}
		// get the rotation matrix
		M = getRotationMatrix2D(rect.center, angle, 1.0);
		// perform the affine transformation
		warpAffine(frameImage, rotated, M, frameImage.size(), INTER_CUBIC);
		// crop the resulting image
		getRectSubPix(rotated, rect_size, rect.center, ROI);
}

void Utils::Rotate90(Mat& image)
{
	transpose(image, image);
	flip(image, image, 1);
}

bool Utils::isRectangle(const vector<Point>& contor, RotatedRect& rect,
		Size imageSize, double deviation)
{
	rect = minAreaRect(contor);
	Mat mask;
	Utils::GetMask(imageSize,rect,mask);
	int rotatedRectArea = countNonZero(mask);
	return (contourArea(contor) / rotatedRectArea > 1 -deviation);
}

int Utils::colorAmountInner(const Mat& hueLayer, const vector<Point>& contour,
		int minHue, int maxHue, int maxVal, MatND& hist)
{
	Mat mask = Mat::zeros(hueLayer.size(), CV_8UC1);
	vector<vector<Point> > contors;
	contors.push_back(contour);
	drawContours(mask, contors, 0, Scalar(255), -1);
	//Utils::ShowImage(mask,"mask");
	int channel = 0;
	int histSize[] =
	{ maxVal + 1 };
	float hranges[] =
	{ 0, maxVal + 1 };
	const float* ranges[] =
	{ hranges };
	calcHist(&hueLayer, 1, &channel, mask, hist, 1, histSize, ranges);
	int chosenHue[] =
	{ minHue, maxHue };
	double chosenAmount = 0;
	for (int i = chosenHue[0]; i <= chosenHue[1]; i += 2)
	{
		//TODO: understand why taking float from histogram
		chosenAmount += hist.at<float>(i / 2);
	}
	return chosenAmount;
}

void Utils::drawRotatedRect(const RotatedRect &square, Mat& draw, Scalar color,
		int thickness)
{
	vector<Point> pointsVec;
	GetContor(square, pointsVec);
	vector<vector< Point> > c;
	c.push_back(pointsVec);
	drawContours(draw,c,0,color,thickness);
}

void Utils::GetContor(const RotatedRect &square, vector<Point>& pointsVec)
{
	pointsVec.clear();
	Point2f tmpPoints[4];
	square.points(tmpPoints);
	for (int i = 0; i < 4; i++)
	{
		pointsVec.push_back(tmpPoints[i]);
	}
}
Mat Utils::ScaledChannel(const Mat &image, int ScaleParam)

{
	double minVal, maxVal;
	int minIdx = 0;
	int maxIdx = 255;
	minMaxIdx(image, &minVal, &maxVal, &minIdx, &maxIdx);
	if ((maxVal - minVal) ==0)
		throw runtime_error("range of image is zero");

	double alpha = ScaleParam / (maxVal - minVal);
	double beta = -ScaleParam * minVal / (maxVal - minVal);

	Mat scaled = Mat(image.size(), CV_8U);
	image.convertTo(scaled, CV_8U, alpha, beta);
	return scaled;
}

void Utils::PaintContinuesArea(Mat& gray, int lowRange, int highRange,
		int lowerDifference, int upperDifference, int paintValue,
		bool diffFromFoundPixel)
{
	//TODO: search in some jumps and not every pixel
	for (int y = 0; y < gray.rows; ++y)
	{
		for (int x = 0; x < gray.cols; ++x)
		{
			uchar pixelVal = gray.at<uchar>(Point(x, y));
			if ((lowRange <= pixelVal) && (pixelVal <= highRange))
			{
				if (diffFromFoundPixel)
				{
					floodFill(gray, Point(x, y), Scalar(paintValue), NULL,
							Scalar(lowerDifference), Scalar(upperDifference),
							CV_FLOODFILL_FIXED_RANGE);
					//Utils::ShowImage(gray,"flood fill");
				}
				else
				{
					floodFill(gray, Point(x, y), Scalar(paintValue), NULL,
							Scalar(lowerDifference), Scalar(upperDifference));
					//Utils::ShowImage(gray, "flood fill");
				}
			}
		}
	}
}

void Utils::fixAngle(RotatedRect& rect)
{
	//needed for the angle to be between y and the long side of the rectangle
	if (rect.size.width > rect.size.height)
	{
		rect.angle += 90;
		//swap between hight and width
		swap(rect.size.width,rect.size.height);
	}
}

void Utils::KolmanFormula(float alpha, const Mat& RED, const Mat& GREEN,
		const Mat& BLUE, Mat& res)
{
	if (alpha == 0 || alpha == 360)
	{
		RED.convertTo(res, CV_32FC1);
	}
	if (alpha > 0 && alpha < 120)
	{
		addWeighted(RED, (120 - alpha) / 120, GREEN, alpha / 120, 0, res,
				CV_32FC1);
	}
	if (alpha == 120)
	{
		GREEN.convertTo(res, CV_32FC1);
	}
	if (alpha > 120 && alpha < 240)
	{
		addWeighted(GREEN, (240 - alpha) / 120, BLUE, (alpha - 120) / 120, 0,
				res, CV_32FC1);
	}
	if (alpha == 240)
	{
		BLUE.convertTo(res, CV_32FC1);
	}
	if (alpha > 240 && alpha < 360)
	{
		addWeighted(BLUE, (360 - alpha) / 120, RED, (alpha - 240) / 120, 0, res,
				CV_32FC1);
	}
}

//this function gets an image and the required thresholds (hue/alpha+its RGB)
//the function returns a grayscale image in kolaman space
//according to the given thresholds
//exist is true only if the minthresh exists in frame
bool Utils::Kolaman(float alpha, const Mat &image, Mat &Kolaman_32F,
		Mat &kol_strech, double minthresh)
{
	float beta;
	double minVal;
	double maxVal;
	bool exist;

	Mat Color(image.size(), CV_32FC1);
	Mat Oppon(image.size(), CV_32FC1);
	Mat img_R(image.size(), CV_8UC1);
	Mat img_G(image.size(), CV_8UC1);
	Mat img_B(image.size(), CV_8UC1);
	//BGR format
	int ch1[] =
	{ 2, 0 };
	mixChannels(&image, 1, &img_R, 1, ch1, 1);
	int ch2[] =
	{ 1, 0 };
	mixChannels(&image, 1, &img_G, 1, ch2, 1);
	int ch3[] =
	{ 0, 0 };
	mixChannels(&image, 1, &img_B, 1, ch3, 1);
	//alpha calculation
	KolmanFormula(alpha, img_R, img_G, img_G, Color);

	if (alpha < 181)
		beta = alpha + 180;
	else
		beta = alpha - 180;
	KolmanFormula(beta, img_R, img_G, img_G, Oppon);
	//kolaman space
	Kolaman_32F = Color - Oppon;
	// find the maximum and minimum values and their locations
	minMaxLoc(Kolaman_32F, &minVal, &maxVal, NULL, NULL);

	double maxvalnorm = kolmanPercent(maxVal, alpha);
	exist = (maxvalnorm > minthresh);

	normalize(Kolaman_32F, kol_strech, 0, 255, NORM_MINMAX, CV_8UC1);
	return exist;
}
void Utils::FillRectBounds(const Rect& rect, MissionControlMessage& msg)
{
	vector<std::pair<int, int> > bounds(4);
	bounds[0] = std::pair<int, int>(rect.x, rect.y - rect.height);
	bounds[1] = std::pair<int, int>(rect.x, rect.y);
	bounds[2] = std::pair<int, int>(rect.x + rect.width, rect.y);
	bounds[3] = std::pair<int, int>(rect.x + rect.width, rect.y + rect.height);
	msg.bounds = bounds;
}

void Utils::FillRectBounds(const RotatedRect& rect, MissionControlMessage& msg)
{
	vector<Point> bounds;
	Utils::GetContor(rect, bounds);
	Utils::FillBounds(bounds, msg);

}

void Utils::FillBounds(const vector<Point>& bounds, MissionControlMessage& msg)
{
	Point tmp;
	//find first left
	tmp = bounds[0];
	int l1Index = 0;
	for (uint i = 1; i < 4; ++i)
		if (bounds[i].x < tmp.x)
		{
			tmp = bounds[i];
			l1Index = i;
		}
	//find second left
	tmp = Point(100000000, 100000000);
	int l2Index;
	for (uint i = 0; i < 4; ++i)
	{
		if (i == l1Index)
			continue;
		if (bounds[i].x < tmp.x)
		{
			tmp = bounds[i];
			l2Index = i;
		}
	}
	int r1Index = -1;
	int r2Index = -1;
	for (uint i = 0; i < 4; ++i)
	{
		if (i == l1Index || i == l2Index)
			continue;
		else
		{
			if (r1Index == -1)
				r1Index = i;
			else
				r2Index = i;
		}
	}
	Point BL, BR, TL, TR;
	if (bounds.at(l1Index).y > bounds.at(l2Index).y)
	{
		BL = bounds.at(l1Index);
		TL = bounds.at(l2Index);
	}
	else
	{
		TL = bounds.at(l1Index);
		BL = bounds.at(l2Index);
	}

	if (bounds.at(r1Index).y > bounds.at(r2Index).y)
	{
		BR = bounds.at(r1Index);
		TR = bounds.at(r2Index);
	}
	else
	{
		TR = bounds.at(r1Index);
		BR = bounds.at(r2Index);
	}

	msg.bounds.push_back(pair<int, int>(BL.x, BL.y));
	msg.bounds.push_back(pair<int, int>(TL.x, TL.y));
	msg.bounds.push_back(pair<int, int>(TR.x, TR.y));
	msg.bounds.push_back(pair<int, int>(BR.x, BR.y));
}

//

void Utils::FillRectBounds(RotatedRect& rect, vector<Point>& bounds)
{
	Utils::GetContor(rect, bounds);
	Utils::orderBounds(bounds);
}

void Utils::orderBounds(vector<Point>& bounds)
{

	Point tmp2;
	vector<int> tempDY(3);
	int tmpMinDY, minDY;
	//find first left
	tmp2 = bounds.at(0);
	int l1Index = 0;
	for (uint i = 1; i < 4; ++i)
		if (bounds[i].x < tmp2.x)
		{
			tmp2 = bounds[i];
			l1Index = i;
		}
	int j = 0;
	for (uint i = 0; i < 4; ++i)
	{
		if (i == l1Index)
			continue;
		tempDY[j] = abs(bounds.at(l1Index).y - bounds.at(i).y);
		j++;
	}
	tmpMinDY = min(tempDY.at(0), tempDY.at(1));
	minDY = min(tmpMinDY, tempDY.at(2));

	//find second left
	tmp2 = Point(100000000, 100000000);
	int l2Index;
	for (uint i = 0; i < 4; ++i)
	{
		if (i == l1Index)
			continue;

		if (bounds[i].x < tmp2.x)
		{
			//and (abs(bounds[l1Index].y - bounds[i].y) > minDY)) {
			tmp2 = bounds[i];
			l2Index = i;
		}
	}
	int r1Index = -1;
	int r2Index = -1;
	for (uint i = 0; i < 4; ++i)
	{
		if (i == l1Index || i == l2Index)
			continue;
		else
		{
			if (r1Index == -1)
				r1Index = i;
			else
				r2Index = i;
		}
	}
	Point BL, BR, TL, TR;
	if (bounds.at(l1Index).y > bounds.at(l2Index).y)
	{
		BL = bounds.at(l1Index);
		TL = bounds.at(l2Index);
	}
	else
	{
		TL = bounds.at(l1Index);
		BL = bounds.at(l2Index);
	}

	if (bounds.at(r1Index).y > bounds.at(r2Index).y)
	{
		BR = bounds.at(r1Index);
		TR = bounds.at(r2Index);
	}
	else
	{
		TR = bounds.at(r1Index);
		BR = bounds.at(r2Index);
	}
	bounds[0] = BL;
	bounds[1] = TL;
	bounds[2] = TR;
	bounds[3] = BR;
}

/*void Utils::FindHeightWidth(vector<Point>& vertices,double& height,double& width)
 {
 Utils::FillBounds(vertices);//get vertices to order(TL,BL,TR,BR)
 double tmp1x,tmp1y,tmp2x,tmp2y;
 tmp1x = vertices[1].x-vertices[0].x;
 tmp2x = vertices[2].x-vertices[0].x;
 tmp1y = vertices[1].y-vertices[0].y;
 tmp2y = vertices[2].y-vertices[0].y;
 height = sqrt(pow(tmp1x,2)+pow(tmp1y,2));
 width = sqrt(pow(tmp2x,2)+pow(tmp2y,2));

 }*/

Mat* Utils::CopyImage(const Mat& image)
{
	Mat* copy = new Mat(image.size(), image.type());
	image.copyTo(*copy);
	return copy;
}
