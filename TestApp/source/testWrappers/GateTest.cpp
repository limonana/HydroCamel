#include <cv.h>
#include <highgui.h>
#include  "../BaseTestAlgorithm.h"
#include "../TestUtils.h"
#include "Gate/Gate.h"
#include "Traffic/Traffic.h"
#include "Utils/ParamUtils.h"
#include "Utils/Utils.h"

using namespace cv;

class GateTest: public BaseTestAlgorithm
{
public:
	/*~GateTest()
		 {
		 }*/

	virtual string Name() const
	{
			return "Gate";
	}

	GateTest()
	{
		_trafficAlgo = new Traffic();
		_trafficAlgo->Init(true);
		//Traffic/config must contain a valid traffic.config file
		map<string,string> params;
		ParamUtils::ReadDefaultConfigFile("traffic",params,true);
		_trafficAlgo->Load(params);
		algo = new Gate(_trafficAlgo);
		algo->Init(true);
	}
	virtual map<string,Mat> getAllProcessImages() const
	{
		map<string,Mat>  res;
		res["grayChannell"]= *algo->_kolman;
		res["BW"]= *algo->_BW;
		res["eroded"]= *algo->_eroded;
		res["dilated"]= *algo->_dilated;
		res["contours"] = *algo->_contourImage;
		res["roi"]= *algo->_roiFrame;
		res["rects"] = *algo->_rectIm;
		return res;
	}

	virtual vector<string> getImagesCategories() const
	{
		vector<string> res;
		res.push_back("grayChannell");
		res.push_back("BW");
		res.push_back("eroded");
		res.push_back("dilated");
		res.push_back("contours");
		res.push_back("roi");
		res.push_back("rects");
		return res;
	}

	virtual map<string, Mat> getProcessImagesByCategories(vector<string> categories) const
	{
		map<string, Mat> res;
		for (uint i = 0; i < categories.size(); ++i)
		{
			if (categories.at(i).compare("grayChannell") == 0) res["grayChannell"] = *algo->_kolman;
			if (categories.at(i).compare("BW") == 0) res["BW"] = *algo->_BW;
			if (categories.at(i).compare("eroded") == 0) res["eroded"] = *algo->_eroded;
			if (categories.at(i).compare("dilated") == 0) res["dilated"] = *algo->_dilated;
			if (categories.at(i).compare("contours") == 0) res["contours"] = *algo->_contourImage;
			if (categories.at(i).compare("roi") == 0) res["roi"] = *algo->_roiFrame;
			if (categories.at(i).compare("rects") == 0) res["rects"] = *algo->_rectIm;
		}
		return res;
	}

	virtual vector<ConfigParam> getConfigParams() const
	{
		vector<ConfigParam>  res;
		res.push_back(ConfigParam{"treshBlockSize" , algo->_treshBlockSize,1000});
		res.push_back(ConfigParam{"minAngle" , algo->_minAngle,90});
		res.push_back(ConfigParam{"maxAngle" , algo->_maxAngle,90});
		res.push_back(ConfigParam{"objectImageFactor" , algo->_objectImageFactor*1000,100});
		res.push_back(ConfigParam{"decision-roi_OrAllPic" , algo->_baseFactor*100,100});
		res.push_back(ConfigParam{"AreaRatio" , algo->_areaSimilarity*100,100});
		res.push_back(ConfigParam{"minWidthHorizonPole" , algo->_baseWidthTOimageFactor*100,100});
		res.push_back(ConfigParam{"originXInRoi" , algo->_roiOriginX_TO_BaseWidthFactor*100,100});
		res.push_back(ConfigParam{"minPixInObjInROI" , algo->_minPixelsInROI,100});
		res.push_back(ConfigParam{"maxYDifVertPoles" , algo->_YverticPolesMaxDiff*100,100});
		res.push_back(ConfigParam{"maxXDifVertPoles" , algo->_XverticPolesMaxDiff*100,150});
		res.push_back(ConfigParam{"minXDifVertPoles" , algo->_XverticPolesMinDiff*100,80});
		res.push_back(ConfigParam{"min2VerticAreaSim" , algo->_ConnectVerticAreaSimilarityMin*100,70});
		res.push_back(ConfigParam{"max2VerticAreaSim" , algo->_ConnectVerticAreaSimilarityMax*100,100});
		res.push_back(ConfigParam{"erosionsize" , algo->_erosionsize,30});
		res.push_back(ConfigParam{"dilationsize" , algo->_dilationsize,30});
		res.push_back(ConfigParam{"minVertRectHeight" , algo->_minRectHeight*100,30});
		res.push_back(ConfigParam{"Xmin_H_V_Dist" , algo->_XminDistVertHoriz*100,80});
		res.push_back(ConfigParam{"Xmax_H_V_Dist" , algo->_XmaxDistVertHoriz*100,100});
		res.push_back(ConfigParam{"Ymin_H_V_Dist" , algo->_YminDistVertHoriz*100,100});
		res.push_back(ConfigParam{"Ymax_H_V_Dist" , algo->_YmaxDistVertHoriz*100,100});
		res.push_back(ConfigParam{"min_H_V_Leng" , algo->_minHorVerLeng*100,250});
		res.push_back(ConfigParam{"max_H_V_Leng" , algo->_maxHorVerLeng*100,700});
		res.push_back(ConfigParam{"minCurves" , algo->_minCurves,20});
		res.push_back(ConfigParam{"greenAlpha" , algo->_greenAlpha,260});
		res.push_back(ConfigParam{"kolmanThresh" , algo->_kolmanThresh*100,100});

		return res;
	}

	virtual map<string, string> getResults() const
	{
		map<string, string> res;
		if (algo->_gate->center.x != -1 and algo->_gate->center.y != -1 )
		{
			ostringstream stream;
			stream <<"Gate center ";
			res[stream.str()] = GateDescription(algo->_gate);
		}
		return res;
	}
	string GateDescription( RotatedRect* square ) const
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
		for (uint i = 0; i < algo->_gateVerticalPoles->size(); ++i)
		{
			ostringstream tmp;
			tmp << "vertical rectangle's center " << (i + 1);
			string name = tmp.str();
			tmp.clear();
			tmp << algo->_gateVerticalPoles->at(i).center;
			res[name] = tmp.str();
		}

		for (uint i = 0; i < algo->_areaPercent->size(); ++i)
		{
			ostringstream tmp1;
			tmp1 << "horizon area percent " << (i + 1);
			string name = tmp1.str();
			tmp1.clear();
			tmp1 << algo->_areaPercent->at(i);
			res[name] = tmp1.str();
		}

		for (uint i = 0; i < algo->_vertAreaPercent.size(); ++i)
		{
			ostringstream tmp2;
			tmp2 << "area percent " << (i + 1);
			string name = tmp2.str();
			tmp2.clear();
			tmp2 << algo->_vertAreaPercent.at(i);
			res[name] = tmp2.str();
		}
		for (uint i = 0; i < algo->_gateVertices.size(); ++i)
		{
			ostringstream tmp3;
			tmp3 << "area percent " << (i + 1);
			string name = tmp3.str();
			tmp3.clear();
			tmp3 << algo->_gateVertices.at(i);
			res[name] = tmp3.str();
		}

		ostringstream tmp4;

		if (algo->_gate->center.x != -1 and algo->_gate->center.y != -1 )
			if(algo->_isTrafficGreen)
				tmp4 << "found the GATE GO GO GO !!! "<< endl;
			else tmp4 << "found the GATE BUT WAIT WAIT WAIT !!! "<< endl;
		else
		{
			if(algo->_isPole)
				tmp4 << "found only the base!!! "<< endl;
			else tmp4 << "no Gate this frame"<<endl;
		}
		string name = tmp4.str();
		res[name] = tmp4.str();
		if(algo->_isPole)
		{
			ostringstream tmp5;
			tmp5 << "horizontal rectangle's center " << endl;
			string name = tmp5.str();
			tmp5.clear();
			tmp5 << algo->_horizonPole->center;
			res[name] = tmp5.str();
		}
		return res;
	}


protected:
	virtual BaseAlgorithm* getAlgorithm()
	{
		return algo;
	}
private:
	Gate*  algo;
	Traffic* _trafficAlgo;
};
