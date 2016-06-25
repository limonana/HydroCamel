#include <cv.h>
#include <highgui.h>
#include  "../BaseTestAlgorithm.h"
#include "../TestUtils.h"
#include "Shadow/ShadowAlgorithm.h"
#include "Utils/Utils.h"

using namespace cv;

class ShadowTest: public BaseTestAlgorithm
{
public:
	/*~PathTest()
	 {
	 }*/

	virtual string Name() const
	{
		return "shadow";
	}

	ShadowTest()
	{
		algo = new ShadowAlgorithm();
		algo->Init(true);
	}

	void getImages(const vector<Mat*>& images, string name,
			map<string, Mat>& res) const
	{
		for (uint i = 0; i < images.size(); ++i)
		{
			stringstream title;
			title << name << " " << i + 1;
			res[title.str()] = *(images.at(i));
		}
	}

	virtual map<string, Mat> getAllProcessImages() const
	{
		map<string, Mat> res;
		if (algo->_yellow != NULL)
			res["yellow"] = *algo->_yellow;
		if (algo->_yellowBW != NULL)
			res["yellowBW"] = *algo->_yellowBW;
		getImages(algo->_BWROI, "BWROI", res);
		getImages(algo->_insideWhiteBW, "insideBW", res);
		getImages(algo->_insideWhiteGray, "insideGray", res);
		if (algo->_WhiteThresh != NULL)
			res["whiteThresh"] = *algo->_WhiteThresh;
		if (algo->_L != NULL)
			res["LUMA"] = *algo->_L;
		return res;
	}

	virtual vector<string> getImagesCategories() const
	{
		vector<string> res;
		res.push_back("yellow");
		res.push_back("yellowBW");
		res.push_back("BW_ROI");
		res.push_back("LUMA");
		res.push_back("whiteThresh");
		res.push_back("insideWhiteBW");
		res.push_back("insideWhiteGray");
		return res;
	}

	virtual map<string, Mat> getProcessImagesByCategories(
			vector<string> categories) const
	{
		map<string, Mat> res;
		for (uint i = 0; i < categories.size(); ++i)
		{
			if (categories.at(i).compare("yellow") == 0)
			{
				if (algo->_yellow != NULL)
					res["yellow"] = *algo->_yellow;
			}
			if (categories.at(i).compare("LUMA") == 0)
			{
				if (algo->_L != NULL)
					res["LUMA"] = *algo->_L;
			}
			if (categories.at(i).compare("whiteThresh") == 0)
			{
				if (algo->_WhiteThresh != NULL)
					res["whiteThresh"] = *algo->_WhiteThresh;
			}
			if (categories.at(i).compare("yellowBW") == 0)
			{
				if (algo->_yellowBW != NULL)
					res["yellowBW"] = *algo->_yellowBW;
			}

			if (categories.at(i).compare("BW_ROI") == 0)
			{
				if (!algo->_BWROI.empty())
				{
					getImages(algo->_BWROI, "BWROI", res);
				}
			}

			if (categories.at(i).compare("insideWhiteBW") == 0)
			{
				if (!algo->_insideWhiteBW.empty())
				{
					getImages(algo->_insideWhiteBW, "insideBW", res);
				}
			}

			if (categories.at(i).compare("insideWhiteGray") == 0)
			{
				if (!algo->_insideWhiteGray.empty())
				{
					getImages(algo->_insideWhiteGray, "insideGray", res);
				}
			}
		}
		return res;
	}

	virtual vector<ConfigParam> getConfigParams() const
	{
		vector<ConfigParam> res;
		res.push_back(ConfigParam
		{ "minGrade", (int) round(algo->_minGrade * 100), 100 });
		res.push_back(ConfigParam
		{ "minSizePercent", (int) round(algo->_minSizePercent * 100), 100 });
		res.push_back(ConfigParam
		{ "RatioDev", (int) round(algo->_RatioDev * 100), 100 });
		res.push_back(ConfigParam
		{ "dilate", algo->_dilate, 20 });
		res.push_back(ConfigParam
		{ "minYellow", algo->_minYellow, 255 });
		res.push_back(ConfigParam
		{ "whiteThresh", algo->_threshWhite, 255 });
		res.push_back(ConfigParam
		{ "retangleDev", (int) round(algo->_retangleDev * 100), 100 });
		return res;
	}

	virtual map<string, string> getResults() const
	{
		map<string, string> res;
		if (algo->current_results.size() > 0)
		{
			ostringstream stream;
			vector<pair<int, RotatedRect> >::iterator it;
			for (it = algo->current_results.begin();
					it != algo->current_results.end(); ++it)
			{
				stream << (*it).first;
				stream << ",";
			}
			res["shadows numbers:"] = stream.str();
		}

		ostringstream stream;
		stream << algo->cut_results.size();
		res["cuts"] = stream.str();
		return res;
	}

	//draw the results on the image
	virtual void DrawResults(Mat& draw)
	{
		algo->Draw(draw);
	}

	virtual map<string, string> getProcessData() const
	{
		map<string, string> res;
		ostringstream stream;
		for (uint i = 0; i < algo->_targetsAmount.size(); ++i)
			stream << algo->_targetsAmount.at(i) << ",";
		res["target amounts"] = stream.str();

		ostringstream stream2;
		for (uint i = 0; i < algo->_cutsAmount.size(); ++i)
			stream2 << algo->_cutsAmount.at(i) << ",";
		res["cuts amounts"] = stream2.str();
		if (algo->_yellowStatus)
			res["state"] ="yellow";
		else
			res["state"] ="white";
		return res;
	}
protected:
	virtual BaseAlgorithm* getAlgorithm()
	{
		return algo;
	}

private:
	ShadowAlgorithm* algo;
};
