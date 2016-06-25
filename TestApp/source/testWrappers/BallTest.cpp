/*
 * BallTest.cpp
 *
 *  Created on: Feb 26, 2013
 *      Author: robosub
 */
#include "../BaseTestAlgorithm.h"
#include <cv.h>
#include "Balls/Balls.h"
#include "../TestUtils.h"
using namespace cv;

class BallTest: public BaseTestAlgorithm {
	Balls* algo;
public:
	BallTest() {
		algo = new Balls();
	}

	virtual string Name() const
	{
		return "balls";
	}

	void Run(Mat& image, map<string, string> args) {
		algo->ClearProcessData();
		algo->SetDefaultParams();
		algo->_inTest = true;

		if (args.size() > 0) {
			/*TestUtils::setParam(args, "minOrangeHue", algo->_minOrangeHue);
			 TestUtils::setParam(args, "maxOrangeHue", algo->_maxOrangeHue);
			 TestUtils::setParam(args, "lowDiff", algo->_lowDif);
			 TestUtils::setParam(args, "upperDiff", algo->_upperDif);
			 TestUtils::setParam(args, "border", algo->_borderPixels);
			 TestUtils::setParam(args, "Area", algo->_areaSimilarity);
			 TestUtils::setParam(args, "Orange", algo->_minOrangePrecent);
			 TestUtils::setParam(args, "NormalArea", algo->_percentNormalArea);
			 TestUtils::setParam(args, "orangeBuffer", algo->_maxBufferPercent);*/
		}
		Mat draw = image;
		algo->Run(image,draw);
	}
	virtual map<string, Mat> getAllProcessImages() const {
		map<string, Mat> res;
		res["_greenthresh"] = *algo->_greenthresh;
		res["_redthresh"] = *algo->_redthresh;
		res["_yellowthresh"] = *algo->_yellowthresh;
		return res;
	}

	virtual vector<ConfigParam> getConfigParams() const{
		vector<ConfigParam> res;
		ostringstream tmp;
		/*res["minOrangeHue"] = TestUtils::getString(algo->_minOrangeHue);
		 res["maxOrangeHue"] = TestUtils::getString(algo->_maxOrangeHue);
		 res["lowDiff"] = TestUtils::getString(algo->_lowDif);
		 res["upperDiff"] = TestUtils::getString(algo->_upperDif);
		 res["border"] = TestUtils::getString(algo->_borderPixels);
		 res["Area"] = TestUtils::getString(algo->_areaSimilarity);
		 res["Orange"] = TestUtils::getString(algo->_minOrangePrecent);
		 res["NormalArea"] = TestUtils::getString(algo->_percentNormalArea);
		 res["orangeBuffer"] = TestUtils::getString(algo->_maxBufferPercent);*/
		return res;
	}

	virtual map<string, string> getResults() const {
		map<string, string> res;
		if (algo->_gcircles != NULL) {
			for (uint i = 0; i < algo->_gcircles->size(); ++i) {
				ostringstream stream;
				stream << "ball " << (i + 1);
				ostringstream stream2;
				stream2 << algo->_gcircles->at(i)[0] << ","
						<< algo->_gcircles->at(i)[1];
				stream2 << "," << algo->_gcircles->at(i)[2];
				res[stream.str()] = stream2.str();
			}
		}
		return res;
	}

	//draw the results on the image
	virtual void DrawResults(Mat& draw) {
		if (algo->_gcircles != NULL) {
			algo->DrawCircles(*algo->_gcircles,*algo->_rcircles,*algo->_ycircles, draw,algo->_maxidxRadiusG,algo->_maxidxRadiusR,algo->_maxidxRadiusY);
			void DrawCircles(vector<Vec3f> gcircles,vector<Vec3f> rcircles,vector<Vec3f> ycircles, Mat& frame,int maxidxRadiusG,int maxidxRadiusR,int maxidxRadiusY);

		}
	}

	//TODO
	virtual map<string, string> getProcessData() const{
		map<string, string> res;
		return res;
	}
	virtual vector<string> getImagesCategories() const{
		vector<string> res;
		return res;
	}

	virtual map<string, Mat> getProcessImagesByCategories(vector<string> categories) const{
		map<string, Mat> res;
		for (uint i = 0; i < categories.size(); ++i) {
			if (categories.at(i).compare("_greenthresh"))
				res["_greenthresh"] = *algo->_greenthresh;
			if (categories.at(i).compare("_redthresh"))
				res["_redthresh"] = *algo->_redthresh;
			if (categories.at(i).compare("_yellowthresh"))
				res["_yellowthresh"] = *algo->_yellowthresh;
		}
		return res;
	}

	virtual map<string, string> getParams() {
		map<string, string> res;
		return res;
	}

	/*virtual BaseAlgorithm* getAlgorithm()
	{
		return algo;
	}*/

};

