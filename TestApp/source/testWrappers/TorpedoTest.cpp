#define stringify( name ) # name
#include <cv.h>
#include <highgui.h>
#include  "../BaseTestAlgorithm.h"
#include "../TestUtils.h"
#include "Torpedo/TorpedoAlgo.h"
#include "Utils/Utils.h"
using namespace cv;

class TorpedoTest: public BaseTestAlgorithm
{
private:
	TorpedoAlgo* algo;

public:
	~TorpedoTest()
	{
		delete algo;
	}

	virtual string Name() const
	{
		return "torpedo";
	}

	TorpedoTest()
	{
		algo = new TorpedoAlgo();
		algo->Init(true);
	}

	void getImagesFromMap(const map<COLOR, Mat*> & images, const string title,
			map<string, Mat>& res) const
	{
		map<COLOR, Mat*>::const_iterator it;
		for (it = images.begin(); it != images.end(); ++it)
		{
			stringstream stream;
			stream << title;
			stream << " " << algo->colorToString(it->first);
			res[stream.str()] = *it->second;
		}
	}

	void getImagesFromVector(const vector<Mat*> & images, const string title,
			map<string, Mat>& res) const
	{
		for (int i = 0; i < images.size(); ++i)
		{
			stringstream stream;
			stream << title;
			stream << " " << (i + 1);
			res[stream.str()] = *images[i];
		}
	}

	virtual map<string, Mat> getAllProcessImages() const
	{
		vector<string> cat = getImagesCategories();
		return getProcessImagesByCategories(cat);
	}

	virtual vector<string> getImagesCategories() const
	{
		vector<string> res;
		/*res.push_back("red threshs");
		res.push_back("blue threshs");
		res.push_back("green threshs");
		res.push_back("yellow threshs");*/
		res.push_back("red countors");
		res.push_back("blue countors");
		res.push_back("green countors");
		res.push_back("yellow countors");
		res.push_back("gray");
		return res;
	}

	virtual map<string, Mat> getProcessImagesByCategories(
			vector<string> categories) const
	{
		map<string, Mat> res;
		for (uint i = 0; i < categories.size(); ++i)
		{
			/*if (categories.at(i).compare("red threshs") == 0)
			{
				getImagesFromVector(algo->_threshs[RED], "red thresh", res);
			}
			if (categories.at(i).compare("blue threshs") == 0)
			{
				getImagesFromVector(algo->_threshs[BLUE], "blue thresh", res);
			}
			if (categories.at(i).compare("yellow threshs") == 0)
			{
				getImagesFromVector(algo->_threshs[YELLOW], "yellow thresh",
						res);
			}
			if (categories.at(i).compare("green threshs") == 0)
			{
				getImagesFromVector(algo->_threshs[GREEN], "green thresh", res);
			}*/
			if (categories.at(i).compare("yellow countors") == 0)
			{
				getImagesFromVector(algo->_contours[YELLOW], "yellow contors",
						res);
			}
			if (categories.at(i).compare("blue countors") == 0)
			{
				getImagesFromVector(algo->_contours[BLUE], "blue countors",
						res);
			}
			if (categories.at(i).compare("red countors") == 0)
			{
				getImagesFromVector(algo->_contours[RED], "red countors", res);
			}

			if (categories.at(i).compare("green countors") == 0)
			{
				getImagesFromVector(algo->_contours[GREEN], "green countors",
						res);
			}

			if (categories.at(i).compare("gray") == 0)
			{
				//getImagesFromMap(*algo->_kolman, "kolman", res);
				getImagesFromMap(algo->_gray, "gray", res);
			}
		}
		return res;
	}

	virtual map<string, string> getParams() const
	{
		//TODO
		map<string, string> res;
		return res;
	}

	virtual vector<ConfigParam> getConfigParams() const
	{
		vector<ConfigParam> res;
		res.push_back(ConfigParam
		{ "redHue", algo->_redHue, 255 });
		res.push_back(ConfigParam
		{ "redKolmanPercent", algo->_redKolmanThresh * 100, 100 });
		res.push_back(ConfigParam
		{ "sizeObjectThresh", algo->_backgroundThresh * 100, 100 });
		res.push_back(ConfigParam
		{ "sizeGarbege", algo->_sizeGarbege, 2000 });
		res.push_back(ConfigParam
		{ "noisePercent", algo->_noisePercent * 100, 100 });
		res.push_back(ConfigParam
		{ "openAmount", algo->_openAmount, 30 });
		res.push_back(ConfigParam
		{ "erodeAmount", algo->_erodeAmount, 30 });
		res.push_back(ConfigParam
		{ "distance_from_known_obj", algo->_dKnownPObject * 100, 100 });
		res.push_back(ConfigParam
		{ "distance_from_big_hole", algo->_dknownPBig * 100, 100 });
		res.push_back(ConfigParam
		{ "distance_from_small_hole", algo->_dknownPSmall * 100, 100 });
		res.push_back(ConfigParam
		{ "epsilon_rect", algo->_sqaureEpsilon * 100, 100 });
		res.push_back(ConfigParam
		{ "red_thresh_jump", algo->_threshJumpRed, 50 });
		res.push_back(ConfigParam
		{ "green_thresh_jump", algo->_threshJumpGreen, 50 });
		res.push_back(ConfigParam
				{ "diff_prop", algo->_threshJumpGreen*100, 100 });
		return res;
	}

	virtual map<string, string> getResults() const
	{
		map<string, string> res;
		map<COLOR, Target>::iterator it;
		for (it = algo->current_results.begin();
				it != algo->current_results.end(); ++it)
		{
			if (it->second.empty())
				res[algo->colorToString(it->first)] = "not found";
			else
			{
				stringstream stream;
				stream << "found: " << it->second.NumOfHoles() << " holes.";
				res[algo->colorToString(it->first)] = stream.str();
			}
		}

		return res;
	}

	//draw the results on the image
	virtual void DrawResults(Mat& draw)
	{
		algo->Draw(draw);
	}

	string holeDescription(Target& hole) const
	{
		ostringstream res;
		/*res << "center point: (" << hole.center.x << "," << hole.center.y << ")" << endl;
		 res << "width: " << hole.width;
		 res << "height: " << hole.height;*/
		return res.str();
	}

	virtual map<string, string> getProcessData() const
	{
		map<string, string> res;
		return res;
	}

protected:
	virtual BaseAlgorithm* getAlgorithm()
	{
		return algo;
	}

};
