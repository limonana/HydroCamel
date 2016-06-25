#include <cv.h>
#include <highgui.h>
#include  "../BaseTestAlgorithm.h"
#include "../TestUtils.h"
#include "blackGate2/BlackGate_adaptive.h"
#include "Utils/Utils.h"

using namespace cv;

class BlackGate2Test: public BaseTestAlgorithm
{
public:
	/*~BlackGate2Test()
	{
		delete algo;
	}*/

	virtual string Name() const
	{
			return "blackGate";
	}

	BlackGate2Test()
	{
		algo = new BlackGate2();
		algo->Init(true);
	}
	virtual map<string,Mat> getAllProcessImages() const
	{
		map<string,Mat>  res;
		res["kolman"]= *algo->_kolman;
		res["BW"] = *algo->_BW;
		res["contours"] = *algo->_contourImage;
		res["rects"] = *algo->_rectIm;
		res["eroded"]= *algo->_eroded;


		return res;
	}

	virtual vector<string> getImagesCategories() const
	{
		vector<string> res;
		res.push_back("kolman");
		res.push_back("BW");
		res.push_back("contours");
		res.push_back("rects");
		res.push_back("eroded");
		return res;
	}

	virtual map<string, Mat> getProcessImagesByCategories(vector<string> categories) const
	{
		map<string, Mat> res;
		for (uint i = 0; i < categories.size(); ++i)
		{
			if (categories.at(i).compare("kolman") == 0) res["kolman"] = *algo->_kolman;
			if (categories.at(i).compare("BW") == 0) res["BW"] = *algo->_BW;
			if (categories.at(i).compare("contours") == 0) res["contours"] = *algo->_contourImage;
			if (categories.at(i).compare("rects") == 0) res["rects"] = *algo->_rectIm;
			if (categories.at(i).compare("eroded") == 0) res["eroded"] = *algo->_eroded;

		}
		return res;
	}

	virtual vector<ConfigParam> getConfigParams() const
	{
		vector<ConfigParam>  res;
		res.push_back(ConfigParam{"AreaRatio" , algo->_areaSimilarity*100,100});
		res.push_back(ConfigParam{"kolmanTreshold" , algo->_kolmanThresh*100,100});
		res.push_back(ConfigParam{"orangeHue" , algo->_OrangeHue,180});
		res.push_back(ConfigParam{"minAngle" , algo->_minAngle,90});
		res.push_back(ConfigParam{"maxAngle" , algo->_maxAngle,90});
		res.push_back(ConfigParam{"minCurves" , algo->_minCurves,200});
		res.push_back(ConfigParam{"objectImageRatio" , algo->_objectImageRatio*1000,100});
		res.push_back(ConfigParam{"heightWidthRatio" , algo->_heightWidthRatio*100,300});
		res.push_back(ConfigParam{"erosionsize" , algo->_erosionsize,50});
		res.push_back(ConfigParam{"dilationsize" , algo->_dilationsize,50});
		res.push_back(ConfigParam{"difPoleCenters" , algo->_difPoleCenters,10});
		res.push_back(ConfigParam{"AreaMaxRatio" , algo->_maxRatio*100,100});
		res.push_back(ConfigParam{"minRectAreaSize" , algo->_minRectAreaSize*100,100});
		res.push_back(ConfigParam{"blockSize" , algo->_blockSize,1000});
		res.push_back(ConfigParam{"isHorizonRec" , algo->_isHorizonRec,65});


		return res;
	}

	virtual map<string, string> getResults() const
	{
		map<string, string> res;
		if (algo->_gate->center.x != -1 and algo->_gate->center.y != -1 )
		{
			ostringstream stream;
			stream <<"Gate center ";
			res[stream.str()] = BlackGateDescription(algo->_gate);
		}
		return res;
	}
	string BlackGateDescription( RotatedRect* square ) const
	{
		ostringstream res;
		res << "center point: " << square->center <<endl;
		return res.str();
	}

	//draw the results on the image
	virtual void DrawResults(Mat& draw)
	{
		algo->Draw(draw);
	}





	virtual map<string, string> getProcessData() const
	{
		map<string, string> res;
		for (uint i = 0; i < algo->_verticalRects->size(); ++i)
		{
			ostringstream tmp;
			tmp << "vertical rectangle's center " << (i + 1);
			string name = tmp.str();
			tmp.clear();
			tmp << algo->_verticalRects->at(i).center;
			res[name] = tmp.str();
		}
		for (uint i = 0; i < algo->_areaPercent->size(); ++i)
		{
			ostringstream tmp;
			tmp << "area percent " << (i + 1);
			string name = tmp.str();
			tmp.clear();
			tmp << algo->_areaPercent->at(i);
			res[name] = tmp.str();
		}
		for (uint i = 0; i < algo->_gateVertices.size(); ++i)
		{
			ostringstream tmp;
			tmp << "gate vertice" << (i + 1);
			string name = tmp.str();
			tmp.clear();
			tmp << algo->_gateVertices.at(i);
			res[name] = tmp.str();
		}
		ostringstream tmp;
		if(algo->_isCompleteGate)
			tmp << " found the GATE!!! "<< endl;
		if((algo->_isCompleteGate == false) and (algo->_verticalRects != NULL) )
			tmp << "ALMOST found the GATE!!! "<< endl;
		else tmp << "no Gate this frame"<<endl;
		string name = tmp.str();
		res[name] = tmp.str();
		return res;
	}


protected:
	virtual BaseAlgorithm* getAlgorithm()
	{
		return algo;
	}
private:
	BlackGate2*  algo;
};
