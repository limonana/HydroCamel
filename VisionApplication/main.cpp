#include <cv.h>
#include "BaseAlgorithm.h"
#include "CamerasController.h"
#include "MissionControl.h"
#include "Utils/ParamUtils.h"
#include "Path/PathAlgorithm.h"
#include "Traffic/Traffic.h"
#include "Torpedo/TorpedoAlgo.h"
#include "Gate/Gate.h"
#include "blackGate2/BlackGate_adaptive.h"
#include "Shadow/ShadowAlgorithm.h"
#include "Utils/Utils.h"
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/detail/thread_group.hpp>
using namespace cv;
using namespace boost;

CamerasController _cameraControl;
MissionControl _missionControl;
map<string, BaseAlgorithm*> _frontAlgos;
map<string, BaseAlgorithm*> _bottomAlgos;
map<string, bool> _enabledAlgorithms;
bool fake = false;
const Point _leftOffset(40,40);
int imageIndex;

void notify(const string& error)
{
	//notify about hardware problems
	cerr << error << endl;
}

void HandleExit()
{
	_cameraControl.Close(false);
	exit(-1);
}

void InitCameras()
{
	_cameraControl.Init();
	map<CameraType, CameraStatus> stat = _cameraControl.GetStatus();
	if (stat[Left] == NotWorking && stat[Right] == NotWorking)
	{
		notify("front camera not working");
		HandleExit();
	}
	else
	{
		if (stat[Left] == Working)
			cout << "left camera working" << endl;
		if (stat[Right] == Working)
			cout << "right camera working" << endl;
	}
	if (stat[Bottom] == NotWorking)
	{
		notify("bottom camera not working");
		HandleExit();
	}
	else
	{
		cout << "bottom camera working" << endl;
	}
}

void CreateAlgorithms()
{
	//HERE ALL OF US WILL ADD CODE
	_bottomAlgos["path"] = new PathAlgorithm();
	_bottomAlgos["path"]->Init(false);
	_bottomAlgos["shadow"] = new ShadowAlgorithm();
	_bottomAlgos["shadow"]->Init(false);

	_frontAlgos["torpedo"] = new TorpedoAlgo();
	_frontAlgos["torpedo"]->Init(false);
	//IMPORTANT: not remove traffic if using Gate
	_frontAlgos["traffic"] = new Traffic();
	_frontAlgos["traffic"]->Init(false);
	_frontAlgos["blackGate"] = new BlackGate2();
	_frontAlgos["blackGate"]->Init(false);
	//IMPORTANT: not remove traffic if using Gate
	_frontAlgos["Gate"] = new Gate((Traffic*)_frontAlgos.at("traffic"));
	_frontAlgos["Gate"]->Init(false);

}

bool LoadParameters(string Name, BaseAlgorithm* algo)
{
	map<string, string> args;
	if (!ParamUtils::ReadDefaultConfigFile(Name, args, false))
	{
		cerr << "missing config file of " << Name << endl;
		return false;
	}
	try
	{

		algo->Load(args);
	} catch (ParameterException& e)
	{
		cerr << "problem in loading parameters of " << Name << ":" << endl;
		cerr << e.what() << endl;
		return false;
	}
	return true;
}

void LoadParameters(const map<String, BaseAlgorithm*> algos)
{
	map<String, BaseAlgorithm*>::const_iterator it;
	for (it = algos.begin(); it != algos.end(); ++it)
	{
		if (!LoadParameters(it->first, it->second))
		{
			_enabledAlgorithms[it->first] = false;
			cerr << it->first
					<< " not working because failure in loading parameters"
					<< endl;
			exit(-1);
		}
		else
			_enabledAlgorithms[it->first] = true;
	}
}

void LoadParameters()
{
	LoadParameters(_frontAlgos);
	LoadParameters(_bottomAlgos);
}
void InitAlgorithms()
{
	CreateAlgorithms();
	LoadParameters();
	map<string,BaseAlgorithm*>::iterator it;
	for (it=_frontAlgos.begin(); it!=_frontAlgos.end();++it)
	{
		it->second->SetOffset(_leftOffset);
	}
}

void Init()
{
	if (!fake)
	{
		cout << "waiting for cameras to work:" << endl;
		InitCameras();
	}
	_missionControl.InitCommunication();
	InitAlgorithms();
}

void RunAlgorithms(const map<string, BaseAlgorithm*>& algos, Mat& image,
		boost::thread_group& g)
{
	map<string, BaseAlgorithm*>::const_iterator it;
	for (it = algos.begin(); it != algos.end(); ++it)
	{
		if (_enabledAlgorithms[it->first])
		{
			g.create_thread(
					boost::bind(&BaseAlgorithm::MakeCopyAndRun, it->second,
							boost::ref(image)));
		}
	}
}
void RunAllAlgoritms(Mat* front, Mat* bottom)
{
//TODO maybe change this to thread pool so not create thread each time

	double t = (double) ((cvGetTickCount()));
	boost::thread_group g;
	if (front != NULL)
		RunAlgorithms(_frontAlgos, *front, g);
	if (bottom != NULL)
		RunAlgorithms(_bottomAlgos, *bottom, g);
	try
	{
		g.join_all();
	} catch (boost::thread_interrupted& e)
	{
		cerr << "some error occured" << endl;
	}

	t = (double) ((cvGetTickCount())) - t;
	double diffms = (t / ((double) ((cvGetTickFrequency())) * 1000));
	if (fake)
		cout << imageIndex << ".jpg:";
	//cout << "execution time: " << diffms << " milliseconds" << endl;
}

void GetImages(Mat*& left, Mat*& right, Mat*& bottom)
{
	if (fake)
	{
		stringstream str1;
		str1 << "images/640_480/front/" << imageIndex << ".jpg";
		string imagePath = str1.str();
		Mat imageLeft = imread(imagePath);
		if (imageLeft.data)
		{
			left = new Mat(imageLeft.size(), imageLeft.type());
			imageLeft.copyTo(*left);
		}
		else
		{
			left = NULL;
			cerr << "can't read " << imagePath << endl;
		}

		if (left != NULL)
		{
			right = new Mat(left->size(), left->type());
			left->copyTo(*right);
		}
		else
		{
			right = NULL;
			cerr << "can't read " << imagePath << endl;
		}
		stringstream str2;
		str2 << "images/640_480/bottom/" << imageIndex << ".jpg";
		imagePath = str2.str();
		Mat imageB = imread(imagePath, CV_LOAD_IMAGE_COLOR);
		if (imageB.data)
		{
			bottom = new Mat(imageB.size(), imageB.type());
			imageB.copyTo(*bottom);
		}
		else
		{
			bottom = NULL;
			cerr << "can't read " << imagePath << endl;
		}
		++imageIndex;
		if (imageIndex > 20)
		{
			imageIndex = 1;

		}
	}
	else
	{
		try
		{
			left = _cameraControl.Read(Left);
		} catch (CamerasException& ex)
		{
			left = NULL;
		}
		try
		{
			right = _cameraControl.Read(Right);
		} catch (CamerasException& ex)
		{
			right = NULL;
		}
		try
		{
			bottom = _cameraControl.Read(Bottom);
		} catch (CamerasException& ex)
		{
			bottom = NULL;
		}
	}
}

void SendResults(Mat* front, Mat* bottom)
{
	vector<MissionControlMessage> res;
	map<string, BaseAlgorithm*>::iterator it;
	if (front != NULL)
	{
		for (it = _frontAlgos.begin(); it != _frontAlgos.end(); ++it)
			it->second->ToMesseges(res);
	}
	if (bottom != NULL)
	{
		for (it = _bottomAlgos.begin(); it != _bottomAlgos.end(); ++it)
			it->second->ToMesseges(res);
	}
//send text results
	_missionControl.SendResults(res);

	if (front != NULL)
	{
		for (it = _frontAlgos.begin(); it != _frontAlgos.end(); ++it)
			if (_enabledAlgorithms[it->first])
				it->second->Draw(*front);
	}
	if (bottom != NULL)
	{
		for (it = _bottomAlgos.begin(); it != _bottomAlgos.end(); ++it)
			if (_enabledAlgorithms[it->first])
				it->second->Draw(*bottom);
	}
	_missionControl.SendDrawResults(front, bottom);
}

int main(int argc, char** argv)
{
	fake = false;
	if (argc > 1)
	{
		if (((string) argv[1]) == "true")
			fake = true;
	}
	if (fake)
		cout << "working with saved images" << endl;
	else
		cout << "working with cameras" << endl;
	imageIndex = 1;
	Init();
	while (true)
	{
		//check if the cameras are working
		if (!fake)
		{
			map<CameraType,CameraStatus> stat =_cameraControl.GetStatus();
			bool frontWorking = stat.at(Left) == Working &&
					stat.at(Right) == Working;
			bool bottomWorking =
					stat.at(Bottom) == Working;
			if (!frontWorking || !bottomWorking)
			{
				if (!frontWorking)
					notify("front camera not working");
				if (!bottomWorking)
					notify("bottom camera not working");
				HandleExit();
			}
		}

		Mat* left = NULL;
		Mat* right = NULL;
		Mat* bottom = NULL;
		GetImages(left, right, bottom);

		Mat* front = NULL;
		if (left != NULL)
			front = left;
		else
			front = right;

		if (!fake)
		{
			if (_missionControl.vi->isLeftEnabled())
				front = left;
			else
				front = right;
		}

		if (front == NULL && fake)
		{
			cerr << "front image not loaded !" << endl;
		}

		if (bottom == NULL && fake)
		{
			if (fake)
				cerr << "bottom image not loaded !" << endl;
		}
		//TODO: not sure what to do if both camera suddenly dont work
		if (!(front == NULL && bottom == NULL))
		{
			_missionControl.SendImages(left, right, bottom);
			Mat* frontCut = front;
			if (!fake && _missionControl.vi->isLeftEnabled())
			{
				frontCut = Utils::CopyImage(*front);
				Rect frame;
				frame.x = _leftOffset.x;
				frame.y = _leftOffset.y;
				frame.width  = front->cols -frame.x;
				frame.height = front->rows - frame.y;
				*frontCut = (*frontCut)(frame);
			}
			RunAllAlgoritms(frontCut, bottom);
			if (frontCut!= front)
				delete frontCut;
			SendResults(front, bottom);

		}
		delete left;
		delete right;
		delete bottom;
	}
}
