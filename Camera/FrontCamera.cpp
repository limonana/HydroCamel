/*
 * FrontCamera.cpp
 *
 *  Created on: Feb 27, 2013
 *      Author: robosub
 */

#include "FrontCamera.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#ifdef __linux__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

//OpenCV includes
#include <highgui.h>
//#include <dr_lib.h>
#include <cv.h>
#include <cvaux.h>
#include <cxcore.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//VRMagic includes
#include  <vrmusbcam2.h>
#define VRMUSBCAMDEMO_H

FrontCamera::FrontCamera() {
	this->Left = 1;
	this->Right = 0;
}

FrontCamera::InitType FrontCamera::Init() {
	// uncomment one of the following lines to enable logging features of VRmUsbCam (for customer support)
	//VRmUsbCamEnableLogging(); // save logfile to default location
	//VRmUsbCamEnableLoggingEx("mylogfile.log"); //save logfile to user defined location
	// check for connected devices
	VRmDWORD size = 0;
	if (!VRmUsbCamGetDeviceKeyListSize(&size)) {
		VRmUsbCamCleanup();
		Error();
	}
	// open first usable device
	_device = 0;
	VRmDeviceKey* p_device_key = 0;
	for (VRmDWORD i = 0; i < size && !_device; ++i) {
		if (!VRmUsbCamGetDeviceKeyListEntry(i, &p_device_key))
		{
			VRmUsbCamCleanup();
			Error();
		}
		//open the camera (2 both)
		if (!p_device_key->m_busy) {
			if (!VRmUsbCamOpenDevice(p_device_key, &_device))
			{
				VRmUsbCamCleanup();
				Error();
			}
		}
		if (!VRmUsbCamFreeDeviceKey(&p_device_key))
		{
			VRmUsbCamCleanup();
			Error();
		}
	}
	// display error when no camera has been found
	if (!_device) {
		throw "No suitable VRmagic device found!";
	}
	if (!VRmUsbCamResetFrameCounter(_device))
	{
		VRmUsbCamCleanup();
		Error();
	}

	// start grabber at first
	if (!VRmUsbCamStart(_device))
	{
		VRmUsbCamCleanup();
		Error();
	}

	bool resLeft = Init(Left, _left_port);
	bool resRight = Init(Right, _right_port);
	if (resLeft && resRight)
		return SUCSSES;
	if (!resLeft && !resRight)
		return FAILURE;
	if (resLeft)
		return LEFT;
	if (resRight)
		return RIGHT;
}

bool FrontCamera::Close() {

	bool res = VRmUsbCamStop(_device) == VRM_SUCCESS;
	VRmUsbCamCleanup();
	return res;
}

Mat* FrontCamera::ReadLeft() {
	return Read(_left_port);
}

Mat* FrontCamera::ReadRight() {
	return Read(_right_port);
}

vector<Mat*> FrontCamera::ReadBoth() {
	vector<Mat*> res;
	res.push_back(ReadLeft());
	res.push_back(ReadRight());
	return res;
}

FrontCamera::~FrontCamera() {
	// TODO Auto-generated destructor stub
}

Mat* FrontCamera::Read(VRmDWORD port) {
	VRmImage* p_source_img = 0;
	VRmDWORD frames_dropped = 0;
	if (!VRmUsbCamLockNextImageEx(_device, port, &p_source_img,
			&frames_dropped)) {
		// in case of an error, check for trigger timeouts and trigger stalls.
		// both can be recovered, so continue. otherwise exit the app
		if (VRmUsbCamLastErrorWasTriggerTimeout())
			throw "trigger timeout_1";
		else if (VRmUsbCamLastErrorWasTriggerStall())
			throw "trigger stall_1";
		else {
			Error();
		}
	}
	// note: p_source_img may be null in case a recoverable error
	// (like a trigger timeout) occured.
	// in this case, we just pump GUI events and then continue with the loop
	int temp;
	Mat* pImage = new Mat(p_source_img->m_image_format.m_height,
			p_source_img->m_image_format.m_width, CV_8UC1,
			(void*) p_source_img->mp_buffer);

	cvtColor(*pImage, *pImage, COLOR_BayerBG2RGB);
	// free the buffer to lock NextImage
	if (!VRmUsbCamUnlockNextImage(_device, &p_source_img))
		Error();
	return pImage;
}

//TODO: change so we can't see error
bool FrontCamera::Init(int camera, VRmDWORD& port) {
	try {
		// SDL keystate array
		Uint8 *keystate;
		// initialize viewer
		VRmDeviceKey* p_key;

		if (!VRmUsbCamGetDeviceKey(_device, &p_key))
		{
			VRmUsbCamCleanup();
			Error();
		}
		VRmSTRING l_serial;
		if (!VRmUsbCamGetSerialString(p_key, &l_serial))
		{
			VRmUsbCamCleanup();
			Error();
		}

		if (!VRmUsbCamFreeDeviceKey(&p_key))
		{
			VRmUsbCamCleanup();
			Error();
		}

		// and enter the loop
		VRmBOOL supported;
		int i = 1; //initialize number of images - Duplex

		if (!VRmUsbCamGetSensorPortListEntry(_device, camera, &port))
		{
			VRmUsbCamCleanup();
			Error();
		}

		// on single sensor devices this property does not exist
		VRmPropId sensor_enable = (VRmPropId) (VRM_PROPID_GRAB_SENSOR_ENABLE_1_B
				- 1 + port);
		if (!VRmUsbCamGetPropertySupported(_device, sensor_enable, &supported))
		{
			VRmUsbCamCleanup();
			Error();
		}

		if (supported) {
			//enable first sensor in port list
			VRmBOOL enable = 1;
			//enable = 1;
			if (!VRmUsbCamSetPropertyValueB(_device, sensor_enable, &enable))
			{
				VRmUsbCamCleanup();
				Error();
			}

		}
		//now get the first sensor port
		//if(!VRmUsbCamGetSensorPortListEntry(device,0,&port))
		//	Error();
		if (!VRmUsbCamGetSensorPortListEntry(_device, camera, &port))
		{
			VRmUsbCamCleanup();
			Error();
		}
		VRmPropInfo p;
		/*int bufferSize = 1;
		VRmUsbCamSetPropertyValueI(_device,
				VRM_PROPID_GRAB_HOST_RINGBUFFER_SIZE_I, &bufferSize);*/
	} catch (...) {
		return false;
	}
	return true;
}

void FrontCamera::Error() {
	ostringstream errorstrem;
	errorstrem << "VRmUsbCam Error: " << VRmUsbCamGetLastError()
			<< "\nApplication exit" << endl;
	string error = errorstrem.str();
	throw error;
}
