/*
 * CamerasController.h
 *
 *  Created on: Apr 17, 2013
 *      Author: shani
 */

#ifndef CAMERASCONTROLLER_H_
#define CAMERASCONTROLLER_H_
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include "FrontCamera.h"
using namespace cv;
using namespace std;

class CamerasException:exception
{
	string _msg;
public:
	CamerasException(string msg){
		_msg = msg;
	}
	virtual const char* what() const throw()
	{
		return _msg.c_str();
	}

	~CamerasException() throw ()
	{
	}
};

enum CameraStatus {Working,NotWorking};
enum CameraType{ Left, Right , Bottom};
string cameraString(CameraType camera_type);
class CamerasController
{
private:
	FrontCamera _frontCamera;
	VideoCapture _bottomCamera;
	map<CameraType, CameraStatus > _status;
	map<CameraType, double> _lastTime;
	int _maxNotWorkingTime; //in seconds

	void updateStatus(CameraType camera);
	void InitFront();
	void InitBottom();
public:
	CamerasController();
	void Init();
	void Init(CameraType camera);
	Mat* Read( CameraType camera);
	map<CameraType,Mat*> ReadAll();
	map<CameraType, CameraStatus > GetStatus();
	CameraStatus GetStatus(CameraType);
	void Close(bool waiting);
	void Close(CameraType camera, bool waiting);
	virtual ~CamerasController();
};

#endif /* CAMERASCONTROLLER_H_ */
