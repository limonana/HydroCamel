#include  "BaseTestAlgorithm.h"
#include <cv.h>
#include <highgui.h>
using namespace cv;

class SimpleAlgo: public BaseTestAlgorithm
{
	Mat* image;
	
	virtual vector<Mat*> getAllProcessImages()
	{
		vector<Mat*> arr(1);
		arr[0] = image;
		return arr;
	}	

	virtual vector<string> getImagesCategories()
	{
		return vector<string> ();
	}

	virtual void Run(Mat& image, vector<string>& args)
	{
		this->image = &image;
	}

	//return the images of the process by level
	virtual vector<Mat*> getImages(vector<string> level)
	{
		vector<Mat*> arr(1);
		arr[0] = image;
		return arr;
	}

	virtual map<string, string> getParams()
	{
		map<string, string> a;
		a["param"] = "param vlaue";
		return a;
	}

	virtual map<string, string> getArgsAndValues()
	{
		map<string, string> a;
		a["arg"] = "arg vlaue";
		return a;
	}

	virtual map<string, string> getResults()
	{
		map<string, string> a;
		a["resName"] = "res value";
		return a;
	}

	//draw the results on the image
	virtual void DrawResults(const char* windowName)
	{
		//imshow(windowName, *image);
	}

};
