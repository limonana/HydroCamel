/*
 * FrontCamera.h
 *
 *  Created on: Feb 27, 2013
 *      Author: robosub
 */

#ifndef FRONTCAMERA_H_
#define FRONTCAMERA_H_
#include <cv.h>
#include <stdio.h>
#include  <vrmusbcam2.h>
using namespace cv;
using namespace std;

class FrontCamera
{
public:
	enum InitType {SUCSSES, RIGHT,LEFT, FAILURE};
	FrontCamera();
	InitType Init();
	bool Close();
	Mat* ReadLeft();
	Mat* ReadRight();
	//TODO: check if it is really take 2 images of the same time
	vector<Mat*> ReadBoth();
	virtual ~FrontCamera();
private:
	int Left;
	int Right;
	VRmDWORD _right_port;
	VRmDWORD _left_port;
	Mat* Read(VRmDWORD port);
	bool  Init(int camera,VRmDWORD& port);
	VRmUsbCamDevice _device;
	void Error();
};

#endif /* FRONTCAMERA_H_ */
