/*
 * MissionControl.h
 *
 *  Created on: Apr 17, 2013
 *      Author: shani
 */

#ifndef MISSIONCONTROL_H_
#define MISSIONCONTROL_H_
#include <cv.h>
#include <VisionComm.h>
#include "MissionControlMessage.h"
using namespace cv;



class MissionControl
{
public:
	MissionControl();
	void InitCommunication();
	void SendImages(Mat* left,Mat* right, Mat* bottom);
	void SendResults(vector<MissionControlMessage>& messages);
	void SendDrawResults(Mat* front,Mat* bottom);
	virtual ~MissionControl();

//private:
	VisionComm* vi;
};

#endif /* MISSIONCONTROL_H_ */
