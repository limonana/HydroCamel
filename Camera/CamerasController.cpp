/*
 * CamerasController.cpp
 *
 *  Created on: Apr 17, 2013
 *      Author: shani
 */

#include "CamerasController.h"
#include <unistd.h>

CamerasController::CamerasController()
{
	// TODO Auto-generated constructor stub
	_maxNotWorkingTime = 5;
}

void CamerasController::InitFront()
{
	FrontCamera::InitType statusInit;
	try
	{
		statusInit = _frontCamera.Init();
	} catch (char* str)
	{
		cerr << str << endl;
		statusInit = FrontCamera::FAILURE;
	} catch (string& str)
	{
		cerr << str << endl;
		statusInit = FrontCamera::FAILURE;
	} catch (...)
	{
		statusInit = FrontCamera::FAILURE;
	}
	switch (statusInit)
	{
	case FrontCamera::SUCSSES:
		_status[Left] = Working;
		_status[Right] = Working;
		break;
	case FrontCamera::FAILURE:
		_status[Left] = NotWorking;
		_status[Right] = NotWorking;
		break;
	case FrontCamera::RIGHT:
		_status[Left] = NotWorking;
		_status[Right] = Working;
		break;
	case FrontCamera::LEFT:
		_status[Left] = Working;
		_status[Right] = NotWorking;
		break;
	}
}

void CamerasController::InitBottom()
{
	_bottomCamera.open(0); // open default camera
	if (_bottomCamera.isOpened())
	{
		_bottomCamera.set(CV_CAP_PROP_FRAME_WIDTH, 300);
		_bottomCamera.set(CV_CAP_PROP_FRAME_HEIGHT, 200);
		_bottomCamera.set(CV_CAP_PROP_FPS, 15);
		_status[Bottom] = Working;
	}
	else
		_status[Bottom] = NotWorking;
}
void CamerasController::Init()
{
	InitFront();
	InitBottom();
}

Mat* CamerasController::Read(CameraType camera)
{
	if (_status[camera] == NotWorking)
		return NULL;

	bool res;
	Mat* image = NULL;
	try
	{
		switch (camera)
		{
		case Left:
			image = _frontCamera.ReadLeft();
			break;
		case Right:
			image = _frontCamera.ReadRight();
			break;
		case Bottom:
			image = new Mat();
			res = _bottomCamera.read(*image);
			if (!res)
			{
				delete image;
				image = NULL;
			}
			break;
		}
		_lastTime[camera] = getTickCount();
		return image;
	}
	catch (string& ex)
	{
		throw CamerasException(ex);
	} catch (char* ex)
	{
		throw CamerasException(ex);
	} catch (...)
	{
		stringstream str;
		str << "Unknown problem with " << cameraString(camera) << endl;
		throw CamerasException(str.str());
	}
}

string cameraString(CameraType camera_type)
{
	string camera;
	switch (camera_type)
	{
	case Left:
		camera = "left";
		break;
	case Right:
		camera = "right";
		break;
	case Bottom:
		camera = "bottom";
		break;
	}
	camera += " camera";
	return camera;
}

map<CameraType, Mat*> CamerasController::ReadAll()
{
	map<CameraType, Mat*> res;
	res[Bottom] = Read(Bottom);
	res[Left] = Read(Left);
	res[Right] = Read(Right);
	return res;
}

map<CameraType, CameraStatus> CamerasController::GetStatus()
{
	updateStatus(Left);
	updateStatus(Right);
	updateStatus(Bottom);
	return _status;
}

CameraStatus CamerasController::GetStatus(CameraType camera)
{
	updateStatus(camera);
	return _status[camera];
}

void CamerasController::updateStatus(CameraType camera)
{
	//the camera not working yet
	if (_lastTime.find(camera) ==_lastTime.end())
		return;

	double curr = getTickCount();
	double tickspersecond=cvGetTickFrequency() * 1.0e6;
	double elapsed_seconds = (curr - _lastTime.at(camera))/tickspersecond;
	if (elapsed_seconds > _maxNotWorkingTime)
	{
		_status[camera] = NotWorking;
	}
}

void CamerasController::Init(CameraType camera)
{
	if (camera == Left || camera == Right)
		InitFront();
	if (camera == Bottom)
		InitBottom();

}

CamerasController::~CamerasController()
{

}

void CamerasController::Close(bool waiting)
{
	Close(Left,waiting);
	Close(Bottom,waiting);
}

void CamerasController::Close(CameraType enumCameraType,bool waiting)
{
	if (enumCameraType == Left || enumCameraType == Right)
	{
		bool res = _frontCamera.Close();
		while (!res && waiting)
		{
			res = _frontCamera.Close();
			sleep(1);
		}
	}
	else
		_bottomCamera.release();
}

