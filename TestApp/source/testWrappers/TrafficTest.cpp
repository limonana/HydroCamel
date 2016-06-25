#include <cv.h>
#include <highgui.h>
#include  "../BaseTestAlgorithm.h"
#include "../TestUtils.h"
#include "Traffic/Traffic.h"
#include "Utils/Utils.h"
//test images/b.jpg traffic -ShowSteps
using namespace cv;

class TrafficTest: public BaseTestAlgorithm {
public:
	~TrafficTest() {
		delete algo;
	}

	virtual string Name() const {
		return "traffic";
	}

	TrafficTest() {
		algo = new Traffic();
		algo->Init(true);
	}
	virtual map<string, Mat> getAllProcessImages() const {
		map<string, Mat> res;
		res["redkol"] = *algo->_redkol;
		res["hue"] = *algo->_hue;
		res["ballkol"] = *algo->_ballkol;
		res["greenkol"] = *algo->_greenkol;
		res["yellowkol"] = *algo->_yellowkol;
		res["green"] = *algo->_greenthresh;
		res["RedBallThresh"] = *algo->_redthreshball;
		res["RedSquareThresh"] = *algo->_redthreshsquare;
		res["yellow"] = *algo->_yellowthresh;
		res["redsquares"] = *algo->_imgredballs;
		res["yellowsquares"] = *algo->_imgyellowsquares;
		res["greensquares"] = *algo->_imggreensquares;
		res["greencontour"]=*algo->_greencontour;
		res["redsquarecontour"]=*algo->_redsquarecontour;
		res["redballcontour"]=*algo->_redballcontour;
		res["_yellowcontour"]=*algo->_yellowcontour;

		return res;
	}

	virtual vector<string> getImagesCategories() const {
		vector<string> res;
		res.push_back("red");
		res.push_back("ball");
		res.push_back("yellow");
		res.push_back("green");
		return res;
	}

	virtual map<string, Mat> getProcessImagesByCategories(
			vector<string> categories) const {
		map<string, Mat> res;
		for (uint i = 0; i < categories.size(); ++i) {
			if (categories.at(i).compare("red") == 0) {
				res["RedBallThresh"] = *algo->_redthreshball;
				res["RedSquareThresh"] = *algo->_redthreshsquare;
				res["redsquarecontour"]=*algo->_redsquarecontour;
				res["redballcontour"]=*algo->_redballcontour;
				res["redballs"] = *algo->_imgredballs;
				res["redsquare"] = *algo->_imgredsquare;
				res["redkol"] = *algo->_redkol;

			}
			if (categories.at(i).compare("ball") == 0) {
				res["RedBallThresh"] = *algo->_redthreshball;
				res["redballcontour"]=*algo->_redballcontour;
				res["redballs"] = *algo->_imgredballs;
				res["redball"] = *algo->_ballkol;
				res["hue"] = *algo->_hue;
			}

			if (categories.at(i).compare("green") == 0) {
				res["GreenThresh"] = *algo->_greenthresh;
				res["greencontour"]=*algo->_greencontour;
				res["greensquares"] = *algo->_imggreensquares;
				res["greenkol"] = *algo->_greenkol;

			}
			if (categories.at(i).compare("yellow") == 0) {
				res["YellowThresh"] = *algo->_yellowthresh;
				res["_yellowcontour"]=*algo->_yellowcontour;
				res["yellowsquares"] = *algo->_imgyellowsquares;
				res["yellowkol"] = *algo->_yellowkol;

			}

		}
		return res;
	}

	virtual map<string, string> getParams() {
		map<string, string> res;
		res["percentKolamanThresh"] = "the percent Kolaman Area";
		res["Redalpha"] = "the Red alpha";

		//res["minRadius"] = "Min Radius";
		//res["maxRadius"] = "Max Radius";

		res["Greenalpha"] = "the Green alpha";
		res["Yellowalpha"] = "the Yellow alpha";
		res["minRedKol"] = "the min hue value of the red color";
		res["maxRedKol"] = "the max hue value of the red color";
		res["minGreenKol"] = "the min hue value of the green color";
		res["maxGreenKol"] = "the max hue value of the green color";
		res["minYellowKol"] = "the min hue value of the yellow color";
		res["maxYellowKol"] = "the max hue value of the yellow color";
		//res["lowDiff"] = "the hue difference between different colors";
		//res["upperDiff"] =
	//			"the hue difference between picked color and the water color";
	//	res["border"] = "number of pixels of the border";
		res["Area"] =
				"the minimum percent of the contor area from the rectangle area";
		res["minRectPercent"] =
				"the minumum percent of pixels from the area of the square";
		res["NormalArea"] =
				"the minimum percent of area from total image that count as no noise";
		res["maxrectratio"] = "maxrectratio";
		res["minrectratio"] = "minrectratio";
		res["minsizerect"] = "minsizerect";
		res["Erosiontype"] = "Erosion Type";
		res["Erosionsize"] = "Erosion Size";
		res["Dilationtype"] = "Dilation Type";
		res["Dilationsize"] = "Dilation size";
		res["redDev"] = "red Deviation";
		res["greenDev"] = "green Deviation";
		res["yellowDev"] = "yellow Deviation";


		return res;
	}

	virtual vector<ConfigParam> getConfigParams() const {
		vector<ConfigParam> res;
		res.push_back(ConfigParam { "Redalpha", algo->_redalpha, 360 });
		res.push_back(ConfigParam { "Greenalpha", algo->_greenalpha, 360 });
		res.push_back(ConfigParam { "Yellowalpha", algo->_yellowalpha, 360 });
		res.push_back(ConfigParam { "minRedKol", algo->_redminh, 255 });
		res.push_back(ConfigParam { "maxRedKol", algo->_redmaxh, 255 });
		res.push_back(ConfigParam { "minGreenKol", algo->_greenminh, 255 });
		res.push_back(ConfigParam { "maxGreenKol", algo->_greenmaxh, 255 });
		res.push_back(ConfigParam { "minYellowKol", algo->_yellowminh, 255 });
		res.push_back(ConfigParam { "maxYellowKol", algo->_yellowmaxh, 255 });
		res.push_back(ConfigParam { "redDev", algo->_redDev, 255 });
		res.push_back(ConfigParam { "greenDev", algo->_greenDev, 255 });
		res.push_back(ConfigParam { "yellowDev", algo->_yellowDev, 255 });

		res.push_back(ConfigParam { "maxmeancircle", algo->_maxmeancircle, 255 });
		res.push_back(ConfigParam { "minRadius", algo->_minRadius, 500 });
		res.push_back(ConfigParam { "maxRadius", algo->_maxRadius, 500 });
		res.push_back(ConfigParam { "inverseRatio", algo->_inverseRatio, 100 });
		res.push_back(ConfigParam { "param1", algo->_param1, 500 });
		res.push_back(ConfigParam { "param2", algo->_param2, 500 });
		res.push_back(ConfigParam { "minmeanfakecircle", algo->_minmeanfakecircle, 255 });
		res.push_back(ConfigParam { "maxmeanfakecircle", algo->_maxmeanfakecircle, 255 });

		res.push_back(ConfigParam { "minRectPercent", algo->_minRectPrecent*100, 100 });
		res.push_back(
				ConfigParam { "percentKolamanThresh", algo->_percentKolamanThresh * 100, 100 });
		res.push_back(
				ConfigParam { "NormalArea", algo->_percentNormalArea * 100, 100 });

		res.push_back(ConfigParam { "maxrectratio", algo->_maxrectratio * 100,
				2000 });
		res.push_back(ConfigParam { "minrectratio", algo->_minrectratio * 100,
				2000 });
		res.push_back(ConfigParam { "minsizerect", algo->_minsizerect, 5000 });
		res.push_back(ConfigParam { "Erosionsize", algo->_erosionsize, 100 });
		res.push_back(ConfigParam { "Dilationsize", algo->_dilationsize, 100 });

		return res;
	}

	virtual map<string, string> getResults() const {
		map<string, string> res;

		ostringstream stream;
		stream << "ResRed ";
		res[stream.str()] = BallDescription(algo->RedBall_current_results);
		return res;
	}

	//draw the results on the image
	virtual void DrawResults(Mat& draw) {
		algo->DrawCircle(algo->RedBall_current_results, draw);
		algo->drawRect(draw, algo->RedSquare_current_results,Scalar(0, 255, 0));
		algo->drawRect(draw, algo->Yellow_current_results,Scalar(0, 255, 0));
		algo->drawRect(draw, algo->Green_current_results,Scalar(255, 0, 0));

	}
	string BallDescription(Vec3f rcircles) const {
		ostringstream res;
		res << "CenterPoint: " << rcircles[0] << rcircles[1] << endl;
		res << "Radius: " << rcircles[2] << endl;
		return res.str();
	}
	string SquareDescription(RotatedRect& square) const {
		ostringstream res;
		res << "center point: " << square.center << endl;
		res << "angle (of X axis): " << (90 - square.angle) << endl;
		vector<Point> points;
		Utils::GetContor(square, points);
		res << "edge points: " << points.at(0) << " " << points.at(1) << " "
				<< points.at(2) << " " << points.at(3) << endl;
		return res.str();
	}

	virtual map<string, string> getProcessData() const {
		map<string, string> res;
		ostringstream redstream;
		redstream << "red color percents";
		ostringstream yellowstream;
		yellowstream << "yellow color percents";
		ostringstream greenstream;
		greenstream << "green color percents";

		ostringstream streamred;
		for (uint i = 0; i < algo->_redsquarePercent->size(); ++i) {
			streamred << "Red:" << (algo->_redsquarePercent->at(i)) << ",";
		}
		res[redstream.str()] = streamred.str();
		ostringstream streamyellow;
		for (uint i = 0; i < algo->_yellowPercent->size(); ++i) {
			streamyellow << "Yellow:" << algo->_yellowPercent->at(i) << ",";
		}
		res[yellowstream.str()] = streamyellow.str();
		ostringstream streamgreen;
		for (uint i = 0; i < algo->_greenPercent->size(); ++i) {
			streamgreen << "Green:" << algo->_greenPercent->at(i) << ",";
		}
		res[greenstream.str()] = streamgreen.str();
		return res;
	}

protected:
	virtual BaseAlgorithm* getAlgorithm() {
		return algo;
	}
private:
	Traffic* algo;
};
