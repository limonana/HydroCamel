#ifndef MAIN
#define MAIN
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
//#include "SimpleAlgo.cpp"
#include "testWrappers/PathTest.cpp"
#include "testWrappers/TrafficTest.cpp"
#include "testWrappers/ShadowTest.cpp"
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "testWrappers/BallTest.cpp"
#include "testWrappers/GateTest.cpp"
#include "testWrappers/BlackGateAdaptiveTest.cpp"
#include "testWrappers/TorpedoTest.cpp"
#include "tests.h"
#include "FrontCamera.h"
#include "configCommand.h"
#include "TestUtils.h"
#include "export.h"
#include "Utils/ParamUtils.h"
using namespace cv;
using namespace std;

const ushort commandPlace = 0;
const ushort algoPlace = 2;
const ushort dataPlace = 1;
map<string, BaseTestAlgorithm*> algorithms;

/*enum ConsoleColor
 {
 BLACK = 30,
 RED = 31,
 GREEN = 32,
 BROWN = 33,
 BLUE = 34,
 MAGENTA = 35,
 CYAN = 36,
 LIGHTGRAY = 37
 };*/

/*void SetColor(ConsoleColor c)
 {
 cout << "\033[0;" << c << "m";
 }
 void SetDefaultColor()
 {
 cout << "\033[0m";
 }*/

vector<string> spiltString(string str, char delimiter)
{
	istringstream iss(str);
	vector<string> tokens;
	std::string item;
	while (std::getline(iss, item, delimiter))
	{
		tokens.push_back(item);
	}
	return tokens;
}

string Optional()
{
	return "-ShowSteps [category 1]...[category n](Optional) -Args [args1=val1]..[argsN=valN](Optional) -Config [config1]...[config_N]";
}

void helpCommands()
{
	string startImg =
			"test [Image name] [algorithm name 1]...[algorithm name N] ";
	string startDir =
			"test [Directory] [algorithm name 1]...[algorithm name N] ";
	cout << endl;
	cout << startImg << Optional() << endl;
	cout << endl;
	cout << startDir << Optional() << endl;
	cout << endl;
	cout << "ShowSteps use for see images of the process of the algorithm."
			<< endl;
	cout << "the catergories are set by the developer of the algorithm."
			<< endl;
	cout << "config use for test with parameters from config file."
			<< "if no config file mentioned, use the default." << endl;

	cout << "config  left/right/bottom [algorithm name]" << endl;
	cout << "config live on image from camera" << endl;

	cout << "config  [Image path] [algorithm name]" << endl;
	cout
			<< "open window with trackbars in order to config the algorithm on one image."
			<< endl;
	cout
			<< "the properties window (CTRL+P) allow to control process images and parameters."
			<< endl;

	cout << endl;
	cout << "config  [Directory path] [algorithm name]" << endl;
	cout << "open window with trackbars in order to config the algorithm on "
			<< "the images in the directory." << endl;
	cout
			<< "the properties window (CTRL+P) allow to control process images and parameters."
			<< endl;
	cout << endl;

	cout << "export" << endl;
	cout << "for each algorithm save config file with it's name . " << endl
			<< "the config file include  the parameters of the algorithm."
			<< endl;
	cout << endl;

	cout << "export [algorithmName]" << endl;
	cout << "save config file with it's name . " << endl
			<< "the config file include  the parameters of the algorithm."
			<< endl;
	cout << endl;

	cout << "live [algorithm name 1]...[algorithm name N]" << endl;
	cout << "show result of algorithms on live images from camera.";
	cout << "*meantime show only left and right of front camera";
	//not avilabe yet
	//cout << start << " [Movie name] " << Optional() << endl <<endl;

	cout << "Args are arguments the algorithm use." << endl;
	cout << endl;

	cout << "frames [movieName]" << endl;
	cout << "divide the movie to frames with delay of 1 second." << endl;
	cout
			<< "save them in new folder with the name of the file inside the same directory."
			<< endl;
}

Point point1, point2; /* vertical points of the bounding box */
int drag = 0;
Rect rect; /* bounding box */
Mat selectCutFrame;
int select_flag = 0;

void mouseHandler(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN && !drag)
	{
		/* left button clicked. ROI selection begins */
		point1 = Point(x, y);
		drag = 1;
	}

	if (event == CV_EVENT_MOUSEMOVE && drag)
	{
		/* mouse dragged. ROI being selected */
		Mat img1 = selectCutFrame.clone();
		point2 = Point(x, y);
		rectangle(img1, point1, point2, CV_RGB(255, 0, 0), 1, 8, 0);
		imshow("cutFrame", img1);
	}

	if (event == CV_EVENT_LBUTTONUP && drag)
	{
		point2 = Point(x, y);
		rect = Rect(point1.x, point1.y, x - point1.x, y - point1.y);
		drag = 0;
	}

	if (event == CV_EVENT_LBUTTONUP)
	{
		/* ROI selected */
		select_flag = 1;
		drag = 0;
	}
}

Rect getROIFromUser(VideoCapture& capture)
{
	cout << "select an area from the frame and then press any key." << endl;
	cout << "or select nothing and press any key." << endl;
	capture.read(selectCutFrame);

	//return the capture to beginning
	capture.set(CV_CAP_PROP_POS_FRAMES, 0);
	namedWindow("cutFrame");
	setMouseCallback("cutFrame", mouseHandler, NULL);
	imshow("cutFrame", selectCutFrame);
	cvWaitKey(0);
	destroyWindow("cutFrame");
	return rect;
}

void DivideToFrames(const char* filePath)
{
	char tmp[200];
	strcpy(tmp, filePath);
	const char* dir = dirname(tmp);
	char tmp2[200];
	strcpy(tmp2, filePath);
	char* fileName = basename(tmp2);

	string fullDir;
	fullDir.append(dir);
	fullDir.append("/");
	fullDir.append(fileName);

	vector<string> v = spiltString(fullDir, '.');
	const char* createDir = v.at(0).c_str();
	int res = mkdir(createDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (res < 0)
	{
		if (errno != EEXIST)
		{
			ostringstream s;
			s << "can't create directory " << createDir;
			s << " " << strerror(errno);
			throw s.str();
		}
	}

	VideoCapture capture(filePath);
	if (capture.isOpened())
	{
		Rect ROI = getROIFromUser(capture);
		Mat image;
		cout << "start to divide to frames" << endl;
		int i = 1;
		while (true)
		{
			bool sucseed = capture.read(image);
			if (!sucseed)
				break;
			if (ROI.area() > 0)
				image = image(ROI);
			ostringstream name;
			name << createDir << "/" << i << ".png";
			string file = name.str();
			if (!imwrite(file, image))
				throw "can't save image";

			++i;
			capture.set(CV_CAP_PROP_POS_MSEC, i * 250);
		}
	}
	capture.release();
}

void InitAlgorithms()
{
	vector<BaseTestAlgorithm*> list;
	list.push_back(new PathTest());
	//list.push_back(new BlackGate2Test());
	list.push_back(new GateTest());
	//list.push_back(new BallTest());
	BaseTestAlgorithm* y=new TrafficTest();
	list.push_back(y);
	list.push_back(new TorpedoTest());
	list.push_back(new ShadowTest());

	for (uint i = 0; i < list.size(); ++i)
		algorithms[list.at(i)->Name()] = list.at(i);
}

void clearAlgorithms()
{
	for (map<string, BaseTestAlgorithm*>::iterator it = algorithms.begin();
			it != algorithms.end(); ++it)
		delete it->second;
}

vector<string> getCommand()
{
	string strCmd;
	getline(cin, strCmd);
	vector<string> tokens = spiltString(strCmd, ' ');
	return tokens;
}
vector<string> untilNextOptional(vector<string>& cmd)
{
	vector<string> res;
	while (!cmd.empty())
	{
		if (cmd.at(0).at(0) == '-')
			break;
		else
		{
			res.push_back(cmd.at(0));
			cmd.erase(cmd.begin());
		}
	}
	return res;
}
BaseTestAlgorithm* getAlgorithm(const string& name)
{
	try
	{
		return algorithms.at(name);
	} catch (std::exception& ex)
	{
		throw "algorithm " + name + " not exists";
	}
}
vector<BaseTestAlgorithm*> getAlgorithms(vector<string>& cmd)
{
	//assume the algos start at the begining
	BaseTestAlgorithm* algo;
	vector<BaseTestAlgorithm*> res;
	vector<string> names = untilNextOptional(cmd);
	for (uint i = 0; i < names.size(); ++i)
	{
		algo = getAlgorithm(names[i]);
		res.push_back(algo);
	}
	return res;
}

void HandleLive(vector<string>& cmd)
{
	cmd.erase(cmd.begin());
	vector<BaseTestAlgorithm*> algos = getAlgorithms(cmd);
	CamerasController cam;
	cam.Init();
	CameraStatus res = cam.GetStatus(Left);
	if (res != Working)
	{
		cerr << "problem front in camera" << endl;
		return;
	}

	namedWindow("left");
	namedWindow("right camera");
	try
	{
		int key;
		do
		{
			Mat* imageLeft = cam.Read(Left);
			Mat drawLeft = *imageLeft;
			for (uint i = 0; i < algos.size(); ++i)
			{
				RunAlgoSimple(algos.at(i), *imageLeft, map<string, string>(),
						drawLeft);
			}
			delete imageLeft;
			imshow("left", drawLeft);
			Mat* imageRight = cam.Read(Right);
			Mat drawRight = *imageRight;
			for (uint i = 0; i < algos.size(); ++i)
			{
				RunAlgoSimple(algos.at(i), *imageRight, map<string, string>(),
						drawRight);
			}
			delete imageRight;
			imshow("right", drawRight);

			//read in frquency by the time algorithms take
			//TODO::mabye change to cvStartWindowThread
			key = cvWaitKey(0);
		} while (key != 'c');
	} catch (CamerasException& ex)
	{
		cerr << "probleam reading from camera";
		cerr << ex.what() << endl;
	} catch (...)
	{
		cerr << "unknown problem" << endl;
	}
	cam.Close(true);
}

void readTestCmd(vector<string> cmd, string &dataplace,
		map<string, string> &args, vector<string> &categories, bool& showSteps,
		bool& config, vector<string> &configFiles,
		vector<BaseTestAlgorithm*>& algos)
{
	dataplace = cmd.at(dataPlace);
	for (uint i = 0; i < algoPlace; ++i)
		cmd.erase(cmd.begin());
	algos = getAlgorithms(cmd);
	showSteps = false;
	config = false;
	if (!cmd.empty())
	{
		if (cmd.size() >= 2 && cmd.at(0).compare("-Args") == 0)
		{
			cmd.erase(cmd.begin());
			vector<string> argsList = untilNextOptional(cmd);
			for (uint i = 0; i < argsList.size(); ++i)
			{
				vector<string> tmp = spiltString(argsList[i], '=');
				args[tmp[0]] = tmp[1];
			}
		}

		if (cmd.size() >= 1 && cmd.at(0).compare("-ShowSteps") == 0)
		{
			showSteps = true;
			cmd.erase(cmd.begin());
			if (!cmd.empty())
				categories = untilNextOptional(cmd);
		}

		if (cmd.size() >= 1 && cmd.at(0).compare("-config") == 0)
		{
			config = true;
			cmd.erase(cmd.begin());
			if (!cmd.empty())
				configFiles = untilNextOptional(cmd);
		}
	}
}

void HandleConfig(vector<string>& cmd)
{
	string algoName = cmd.at(2);
	BaseTestAlgorithm* algo = getAlgorithm(algoName);
	string camera = cmd.at(1);
	try
	{
		if (camera.compare("bottom") == 0)
		{
			createLiveTrackbarWindow(Bottom, algo);
			return;
		}
		if (camera.compare("left") == 0)
		{
			createLiveTrackbarWindow(Left, algo);
			return;
		}
		if (camera.compare("right") == 0)
		{
			createLiveTrackbarWindow(Right, algo);
			return;
		}

		string path = cmd.at(1);
		if (TestUtils::isFile(path))
		{
			createTrackbarWindow(path, algo);
			return;
		}
		if (TestUtils::isDirectory(path))
		{
			createMultiWindowConfig(path, algo);
			return;
		}
		throw invalid_argument("invalid file name or directory");

	}
	catch(invalid_argument&)
	{
		throw;
	}
	catch (Exception&)
	{
		destroyAllWindows();
	}
}

void HandleExport(vector<string>& cmd)
{
	if (cmd.size() == 2) //export [algoName]
	{
		BaseTestAlgorithm* algo = getAlgorithm(cmd[1]);
		ExportAlgorithm(*algo);
	}
	else //export
	{
		map<string, BaseTestAlgorithm*>::iterator it;
		for (it = algorithms.begin(); it != algorithms.end(); ++it)
			ExportAlgorithm(*(it->second));

	}

}

void HandleTest(vector<string>& cmd)
{
	vector<BaseTestAlgorithm*> algos;
	vector<string> categories;
	map<string, string> args;
	string path;
	bool showSteps;
	bool config;
	vector<string> configFiles;
	readTestCmd(cmd, path, args, categories, showSteps, config, configFiles,
			algos);
	if (algos.empty())
	{
		map<string, BaseTestAlgorithm*>::iterator it;
		for (it = algorithms.begin(); it != algorithms.end(); ++it)
			algos.push_back(it->second);
	}

	map<string, map<string, string> > AlgoParams;
	if (config)
	{
		if (configFiles.empty())
		{
			vector<BaseTestAlgorithm*>::iterator it;
			for (it = algos.begin(); it != algos.end(); ++it)
			{
				BaseTestAlgorithm* curr_algo = *it;
				//load config file in order to make sure it is valid
				string algoName = curr_algo->Name();
				string configFile = ParamUtils::GetDefaultConfigFile(algoName,
						true);
				configFiles.push_back(configFile);
			}
		}
		else
		{
			uint i;
			for (i = 0; i < configFiles.size(); ++i)
			{
				configFiles[i] = ParamUtils::GetConfigFilePath(algos[i]->Name(),
						configFiles[i]);
			}
			for (uint j = i; j < algos.size(); ++j)
			{
				cout << "no config file mentioned for " << algos[j]->Name()
						<< ".";
				cout << "use default config file instead" << endl;
				configFiles.push_back(
						ParamUtils::GetDefaultConfigFile(algos[j]->Name(),
								true));
				cout << configFiles[j] << endl;
			}

		}

		cout << configFiles.size() << endl;
		for (uint i = 0; i < configFiles.size(); ++i)
		{
			BaseTestAlgorithm* curr_algo = algos[i];
			try
			{
				curr_algo->Load(configFiles[i]);
				AlgoParams[curr_algo->Name()] = curr_algo->getArgsAndValues();
			} catch (ParameterException& e)
			{
				cerr << configFiles[i]
						<< " is not valid. use default params instead." << endl;
			} catch (exception& e)
			{
				string err = e.what();
				cerr << err << endl;
				cerr << "use default params instead." << endl;
			} catch (string& err)
			{
				cerr << err << endl;
				cerr << "use default params instead." << endl;
			}
		}
	}
	else
	{
		vector<BaseTestAlgorithm*>::iterator it;
		for (it = algos.begin(); it != algos.end(); ++it)
		{
			AlgoParams[(*it)->Name()] = args;
		}
	}

	if (TestUtils::isFile(path))
	{
		RunOneImageFile(path, algos, AlgoParams, showSteps, categories);
	}
	else
	{
		//data is directory
		vector<string> files = TestUtils::getFilesInDirectory(path);
		for (uint i = 0; i < files.size(); ++i)
		{
			//SetColor(GREEN);
			cout << files.at(i) << ":" << endl;
			//SetColor(BLACK);
			RunOneImageFile(path + "/" + files.at(i), algos, AlgoParams,
					showSteps, categories);
		}
	}
	int key;
	do
	{
		key = cvWaitKey(100);
	} while (key != ' ');

	cvDestroyAllWindows();
//cout<<"what now?";
//string afterCmd;
//cin>>afterCmd;
}

int main(int argc, char** argv)
{
	printf("Welcome to the Hydro Camel vision team tests app. :-) \n");
	printf("to see the available commands type \"help\" any time.  \n");
	InitAlgorithms();
	bool quit = false;
	while (!quit)
	{
		cout << ">";
		vector<string> cmd = getCommand();

		try
		{
			if (cmd.at(commandPlace).compare("quit") == 0)
			{
				quit = true;
				continue;
			}
			if (cmd.at(commandPlace).compare("frames") == 0)
			{
				string file = cmd.at(commandPlace + 1);
				DivideToFrames(file.c_str());
				cout << "finish divide to frames" << endl;
				continue;
			}

			if (cmd.at(commandPlace).compare("help") == 0)
			{
				helpCommands();
				continue;
			}

			if (cmd.at(commandPlace).compare("test") == 0)
			{
				HandleTest(cmd);
				continue;
			}
			if (cmd.at(commandPlace).compare("live") == 0)
			{
				HandleLive(cmd);
				continue;
			}
			if (cmd.at(commandPlace).compare("config") == 0)
			{
				if (cmd.size() != 3)
					throw invalid_argument("error in command format");
				HandleConfig(cmd);
				continue;
			}
			if (cmd.at(commandPlace).compare("export") == 0)
			{
				if (cmd.size() > 2)
					throw "error in command format";
				HandleExport(cmd);
				continue;
			}

			cout << " not known command: " << cmd.at(commandPlace) << endl;
		} catch (exception& e)
		{
			cerr << e.what() << endl;
		} catch (string& ex)
		{
			cerr << ex << endl;
		} catch (char* ex)
		{
			cerr << ex << endl;
		}
	}
	return 0;
}

#endif
