/*
 * MissionControl.cpp
 *
 *  Created on: Apr 17, 2013
 *      Author: shani
 */

#include "MissionControl.h"
#include "VisionComm.h"

MissionControl::MissionControl()
{
	// TODO Auto-generated constructor stub

}

void MissionControl::InitCommunication()
{
	vi = new VisionComm(0, 0);
}

void MissionControl::SendImages(Mat* left, Mat* right, Mat* bottom)
{
	if (left != NULL)
		vi->publishImageLeft(*left);
	if (right != NULL)
		vi->publishImageRight(*right);
	if (bottom != NULL)
		vi->publishImageBottom(*bottom);
}

void MissionControl::SendResults(vector<MissionControlMessage>& mesaages)
{
	for (uint i=0; i<mesaages.size(); ++i)
	{
		MissionControlMessage& msg = mesaages[i];
		vi->publishVisionMsg(msg.MissionCode,
				msg.additionalInformation, msg.bounds , msg.intrestPoints);
	}
}

void MissionControl::SendDrawResults(Mat* front, Mat* bottom)
{
	//TODO:send the results through function of vi
	if (bottom != NULL)
		vi->publishImageBottomAnswer(*bottom);
	if (front != NULL)
		vi->publishImageLeftAnswer(*front);
}

MissionControl::~MissionControl()
{
	// TODO Auto-generated destructor stub
}

