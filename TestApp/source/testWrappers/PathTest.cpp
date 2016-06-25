#include <cv.h>
#include <highgui.h>
#include  "../BaseTestAlgorithm.h"
#include "../TestUtils.h"
#include "Path/PathAlgorithm.h"
#include "Utils/Utils.h"

using namespace cv;

class PathTest: public BaseTestAlgorithm {
public:
	/*~PathTest()
	 {
	 }*/

	virtual string Name() const {
		return "path";
	}

	PathTest() {
		algo = new PathAlgorithm();
		algo->Init(true);
	}

	virtual map<string, Mat> getAllProcessImages() const {
		map<string, Mat> res;
		res["kolman"] = *algo->_kolman;
		if (algo->_counterImage!=NULL)
			res["counters"] = *algo->_counterImage;
		if (algo->_thresh!=NULL)
			res["thresh"] = *algo->_thresh;
		return res;
	}

	virtual vector<string> getImagesCategories() const {
		vector<string> res;
		res.push_back("kolman");
		res.push_back("thresh");
		res.push_back("counters");
		return res;
	}

	virtual map<string, Mat> getProcessImagesByCategories(
			vector<string> categories) const {
		map<string, Mat> res;
		for (uint i = 0; i < categories.size(); ++i) {
			if (categories.at(i).compare("counters") == 0)
			{
				if (algo->_counterImage!=NULL)
					res["counters"] = *algo->_counterImage;
			}
			if (categories.at(i).compare("kolman") == 0)
				res["kolman"] = *algo->_kolman;
			if (categories.at(i).compare("thresh") == 0)
			{
				if (algo->_thresh!=NULL)
					res["thresh"] = *algo->_thresh;
			}
		}
		return res;
	}

	virtual vector<ConfigParam> getConfigParams() const {
		vector<ConfigParam> res;
		res.push_back(
				ConfigParam { "NormalArea", (int)round(algo->_percentNormalArea * 1000),
						1000 });
		res.push_back(ConfigParam { "Area", (int)round(algo->_areaSimilarity * 100), 100 });
		res.push_back(
				ConfigParam { "RatioPercent", (int)round(algo->_minRatio * 100), 700 });
		res.push_back(ConfigParam { "OrangeHue", algo->_OrangeHue, 360 });
		res.push_back(ConfigParam { "kolmanDev", (int)round(algo->_kolmanDev*100), 1000 });
		res.push_back(ConfigParam { "erode", algo->_erode, 50 });
		return res;
	}

	virtual map<string, string> getResults() const {
		map<string, string> res;

		for (uint i = 0; i < algo->current_results.size(); ++i) {
			ostringstream stream;
			stream << "path " << (i + 1);
			res[stream.str()] = PathDescription(algo->current_results.at(i));
		}

		return res;
	}

	//draw the results on the image
	virtual void DrawResults(Mat& draw) {
		algo->drawPath(draw, algo->current_results);
	}

	string PathDescription(RotatedRect& path) const {
		ostringstream res;
		res << "center point: " << path.center << endl;
		res << "angle (of X axis): " << (90 - path.angle) << endl;
		vector<Point> points;
		Utils::GetContor(path, points);
		res << "edge points: " << points.at(0) << " " << points.at(1) << " "
				<< points.at(2) << " " << points.at(3) << endl;
		return res.str();
	}

	virtual map<string, string> getProcessData() const {
		map<string, string> res;
		for (uint i = 0; i < algo->_areaPercent->size(); ++i) {
			ostringstream tmp;
			tmp << "area percent " << (i + 1);
			string name = tmp.str();
			tmp.clear();
			tmp << algo->_areaPercent->at(i);
			res[name] = tmp.str();
		}
		for (uint i = 0; i < algo->_ratios->size(); ++i) {
			ostringstream tmp;
			tmp << "ratios " << (i + 1);
			string name = tmp.str();
			tmp.clear();
			tmp << algo->_ratios->at(i);
			res[name] = tmp.str();
		}

		return res;
	}
protected:
	virtual BaseAlgorithm* getAlgorithm() {
		return algo;
	}

private:
	PathAlgorithm* algo;
};
