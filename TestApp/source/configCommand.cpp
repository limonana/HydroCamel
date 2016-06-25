#include "configCommand.h"
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "BaseTestAlgorithm.h"
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include "tests.h"
#include "export.h"
#include "TestUtils.h"
#include "Utils/ParamUtils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "CamerasController.h"
#include "Utils/Utils.h"
using namespace cv;
using namespace std;

const int overlaySeconds = 5;
const int overlayErrorTime = 10;
Mat image;
vector<ConfigParam> configParams;
string windowName;
string imageName;
BaseTestAlgorithm* configAlgo;
bool ShowSteps;
vector<string> chosenCategories;
vector<string> allCategories;
map<string, Mat> name2image;

void CloseStepsWindows() {
	map<string, Mat>::iterator it;
	map<string, Mat> processImages = configAlgo->getAllProcessImages();
	for (it = processImages.begin(); it != processImages.end(); ++it) {
		stringstream windowNameStream;

		windowNameStream << imageName << ": " << it->first;
		string windowName = windowNameStream.str();
		destroyWindow(windowName);
	}
}

/*void OpenStepsWindows()
 {
 map<string, Mat>::iterator it;
 map<string, Mat> processImages = configAlgo->getAllProcessImages();
 for (it = processImages.begin(); it != processImages.end(); ++it)
 {
 stringstream windowNameStream;

 windowNameStream << imageName << ": " << it->first;
 string windowName = windowNameStream.str();
 namedWindow(windowName);
 loadWindowParameters(windowName);
 }
 }*/

void RunCallback(int, void*) {
	Mat draw(image.size(), image.type());
	image.copyTo(draw);

	map<string, string> argsValues = configAlgo->getArgsAndValues(configParams);
	// open only steps windows that defined by the categories
	RunAlgo(configAlgo, image, draw, argsValues, imageName, windowName,
			ShowSteps, chosenCategories, true);

}

void ShowStepCallback(int state, void* usrData) {
	ShowSteps = (state == 1);
	if (!ShowSteps)
		CloseStepsWindows();
	RunCallback(0, 0);
}

void ExportCallBack(int state, void* usrData) {
	string fileName = ExportAlgorithm(*configAlgo);
	stringstream msg;
	msg << fileName << " saved.";
	//displayOverlay(windowName, msg.str(), overlaySeconds * 1000);
}

void CategoriesCallBack(int state, void* usrData) {
	string category = *(string*) usrData;
	if (state) {
		chosenCategories.push_back(category);
	} else {
		vector<string>::iterator foundIt;
		foundIt = find(chosenCategories.begin(), chosenCategories.end(),
				category);
		if (foundIt != chosenCategories.end())
			chosenCategories.erase(foundIt);
	}

	//close all steps windows and the correct will open at change of trackbar
	CloseStepsWindows();

	//OpenStepsWindows();
	RunCallback(0, 0);
}

void createCategoiresCheckboxs(BaseTestAlgorithm* algo) {
	allCategories = algo->getImagesCategories();
	for (uint i = 0; i < allCategories.size(); ++i) {
		string& category = allCategories[i];
		createButton(category, CategoriesCallBack, (void*) &category,
				CV_CHECKBOX, false);
	}
}

void changeConfigParams(vector<ConfigParam>& params) {
	for (uint i = 0; i < params.size(); ++i) {
		configParams[i].Value = params[i].Value;
		setTrackbarPos(configParams[i].Name, windowName, configParams[i].Value);
	}
}

void ConfigFileChange(int state, void* userdata) {
	if (state == 0)
		return;
	stringstream stream;
	string fileName = *((string*) userdata);
	stream << "config/" << configAlgo->Name() << "/" << fileName; // << ".config";
	string filePath = stream.str();
	try {
		configAlgo->Load(filePath);
		vector<ConfigParam> newParams = configAlgo->getConfigParams();
		changeConfigParams(newParams);
		RunCallback(0, 0);
	} catch (ParameterException& e) {
		stringstream error;
		error << "problem in " << fileName << ":";
		error << e.what();
		//displayOverlay(windowName, error.str(), overlayErrorTime * 1000);
	}
}
vector<string> files;
void createConfigFilesChoser(string algoName) {
	//check is the directory exist, if not create if
	string dir = "config/" + algoName;
	struct stat sb;
	int dirExist = stat(dir.c_str(), &sb) >= 0;
	if (!dirExist) {
		mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
	files = TestUtils::getFilesInDirectory(dir);

	for (uint i = 0; i < files.size(); ++i) {
		string configName = files[i];
		createButton(configName, ConfigFileChange, &files[i], CV_RADIOBOX,
				false);
	}
}
void createConfigWindow(BaseTestAlgorithm*& algo) {
	configParams = algo->getConfigParams();
	configAlgo = algo;
	ShowSteps = false;
	chosenCategories.clear();
	const int maxParamInWindow = 10;
	namedWindow(windowName, CV_WINDOW_NORMAL);
	/*if (configParams.size() <= maxParamInWindow)
		displayOverlay(windowName,
				"open the properties window (CTRL+P) in order to control process images",
				overlaySeconds * 1000);
	else
		displayOverlay(windowName,
				"open the properties window (CTRL+P) in order to control process images and parameters.",
				overlaySeconds * 1000);
*/
	//create properties windows
	createConfigFilesChoser(algo->Name());
	createButton("export", ExportCallBack);
	createButton("show steps", ShowStepCallback, NULL, CV_CHECKBOX, ShowSteps);
	createCategoiresCheckboxs(algo);
	for (uint i = 0; i < configParams.size(); ++i) {
		ConfigParam& p = configParams[i];
		if (configParams.size() <= maxParamInWindow)
			createTrackbar(p.Name, windowName, &p.Value, p.MaxValue,
					RunCallback);
		else
			createTrackbar(p.Name, "", &p.Value, p.MaxValue, RunCallback);
	}
	//use for showing the images with current params
	/*RunCallback(0, 0);
	 char c;
	 do
	 {
	 c = waitKey(0);
	 } while (c != ' ');
	 destroyAllWindows();*/
}

void createTrackbarWindow(string& imagePath, BaseTestAlgorithm* algo) {
	image = TestUtils::LoadImage(imagePath);
	imageName = TestUtils::getFileName(imagePath);
	windowName = imageName;
	createConfigWindow(algo);
	RunCallback(0, 0);
	char c;
	do {
		c = waitKey(0);
	} while (c != ' ');
	destroyAllWindows();
}
void MultiImageConfigCallback(int, void*) {
	map<string, string> argsValues = configAlgo->getArgsAndValues(configParams);
	map<string, Mat>::iterator it;
	for (it = name2image.begin(); it != name2image.end(); ++it) {
		Mat& image = it->second;
		string imageName = it->first;
		Mat draw(image.size(), image.type());
		image.copyTo(draw);

		// open only steps windows that defined by the categories
		RunAlgo(configAlgo, image, draw, argsValues, imageName, imageName,
				false, chosenCategories, true);
	}

}
void orderWindows(map<string, Mat> images) {
	int InitialX = 60;
	int InitialY = 20;
	int x = InitialX;
	int y = InitialY;
	map<string, Mat>::iterator it;
	int maxY = 0;
	for (it = images.begin(); it != images.end(); ++it) {
		string winName = it->first;
		namedWindow(winName, CV_WINDOW_AUTOSIZE);
		moveWindow(winName, x, y);
		//TODO: make all the window size the same
		x += it->second.size().width;
		maxY = max(maxY, it->second.size().height);
		map<string, Mat>::iterator nextIt = it;
		nextIt++;
		//TODO: consider the properites window
		if ((nextIt != images.end())
				&& (x + nextIt->second.size().width > 1400)) {
			y += maxY + 75;
			x = InitialX;
		}
		//displayOverlay(winName,
				//"open the properties window (CTRL+P) in order to control process images and parameters",
				//overlaySeconds * 1000);
	}
}

void createLiveTrackbarWindow(CameraType type, BaseTestAlgorithm* algo) {
	string cameraStr = cameraString(type);
	windowName = "live " + cameraStr;

	CamerasController camControl;
	camControl.Init(type);
	if (camControl.GetStatus(type) == NotWorking) {
		cerr << cameraStr << " not working";
		return;
	}
	createConfigWindow(algo);
	int key;
	do {
		try {
			Mat* pImg = camControl.Read(type);
			if (pImg == NULL)
				continue;
			pImg->copyTo(image);
			delete pImg;
			RunCallback(0, 0);
			key = cvWaitKey(1000 / 30);
		} catch (CamerasException& ex) {
			cerr << "problem in camera: " <<  ex.what() << std::endl;
		}
		catch(std::exception& ex)
		{
			cerr << "some problem in algorithm:";
			cerr << ex.what() << endl;
		}
		catch(...)
		{
			cerr << "unkown problem" << endl;
		}
	} while (key != ' ');
	destroyAllWindows();
	camControl.Close(type,true);
}

void createMultiWindowConfig(string dirPath, BaseTestAlgorithm* algo) {
	configParams = algo->getConfigParams();
	configAlgo = algo;
	ShowSteps = false;
	chosenCategories.clear();

	vector<string> files = TestUtils::getFilesInDirectory(dirPath);
	for (uint i = 0; i < files.size(); ++i) {
		string imagePath = dirPath + "/" + files[i];
		Mat m = TestUtils::LoadImage(imagePath);
		name2image[files[i]] = m;
	}

	orderWindows(name2image);
	createButton("export", ExportCallBack);

	//create trackbars in the properties window
	for (uint i = 0; i < configParams.size(); ++i) {
		ConfigParam& p = configParams[i];
		createTrackbar(p.Name, "", &p.Value, p.MaxValue,
				MultiImageConfigCallback);
	}

	MultiImageConfigCallback(0, 0);
	char c;
	do {
		c = waitKey(0);
	} while (c != ' ');
	destroyAllWindows();
}
