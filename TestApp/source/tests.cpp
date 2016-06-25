#ifndef TEST_CPP
#define TEST_CPP
#include "tests.h"
#include <cv.h>
#include "BaseTestAlgorithm.h"
#include "TestUtils.h"
#include <stdio.h>
using namespace std;
using namespace cv;

void printParameters(const map<string, string>& data, char delimiter)
{
	map<string, string>::const_iterator it;
	for (it = data.begin(); it != data.end(); ++it)
		cout << it->first << " " << delimiter << " " << it->second << endl;
	cout << endl;
}

void RunAlgoSimple(BaseTestAlgorithm* algo, Mat& image, map<string, string> args, Mat& draw)
{
	try
	{
	double t = (double) (cvGetTickCount());
	algo->Run(image, args);
	t = (double) (cvGetTickCount()) - t;
	double diffms = (t / ((double) (cvGetTickFrequency()) * 1000));
	cout << "execution time: " << diffms << " milliseconds" << endl;

	t = (double) (cvGetTickCount());
	algo->DrawResults(draw);
	t = (double) (cvGetTickCount()) - t;
	diffms = (t / ((double) (cvGetTickFrequency()) * 1000));
	cout << "draw time: " << diffms << " milliseconds" << endl;
	}
	catch (cv::Exception& e) {
		cerr << "problem in algorithms:" << e.what() << endl;
	}
}

void RunAlgo(BaseTestAlgorithm* algo, Mat& image, Mat& draw, const map<string, string>& args,
		const string& imageName,const string& windowName, bool showSteps, const vector<string>& categories, bool oneAlgo)
{
	RunAlgoSimple(algo, image, args, draw);
	namedWindow(windowName);
	imshow(windowName,draw);
	if (showSteps)
	{
		map<string, Mat> processImages;
		if (categories.empty())
			processImages = algo->getAllProcessImages();
		else
			processImages = algo->getProcessImagesByCategories(categories);

		for (map<string, Mat>::iterator it = processImages.begin(); it != processImages.end(); ++it)
		{
			stringstream windowNameStream;
			if (!oneAlgo)
				windowNameStream << algo->Name() << ":";
			windowNameStream << imageName << ": " << it->first;
			string windowName = windowNameStream.str();
			namedWindow(windowName, CV_WINDOW_AUTOSIZE);
			imshow(windowName, it->second);
		}
	}

	cout << endl;
	map<string, string> textResults = algo->getResults();
	if (textResults.size() > 0)
		printParameters(algo->getResults(), ':');
	else
		cout << "no results" << endl;

	cout << endl;
	map<string, string> processData = algo->getProcessData();
	if (processData.size() > 0)
	{
		cout << "process data:" << endl;
		printParameters(processData, ':');
	}
	cout << "arguments and values:" << endl;
	printParameters(algo->getArgsAndValues(), '=');
}

void RunOneImageFile(string imagePath, vector<BaseTestAlgorithm*> &algos,
		map<string,map<string,string> > Algo2Args,
		bool showSteps, vector<string> &categories)
{
	Mat image = TestUtils::LoadImage(imagePath);
	string imageName = TestUtils::getFileName(imagePath);
	Mat draw(image.size(), image.type());
	image.copyTo(draw);
	string windowName = "Results - " + imageName;
	for (uint i = 0; i < algos.size(); ++i)
	{
		cout << algos.at(i)->Name() << ":" << endl;
		map<string,string> args;
		string algoName = algos[i]->Name();
		if (Algo2Args.find(algoName) != Algo2Args.end())
			args = Algo2Args.at(algoName);

		RunAlgo(algos.at(i), image, draw, args, imageName,windowName,
				showSteps, categories);
	}
}
#endif
