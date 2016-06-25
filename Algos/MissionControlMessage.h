#ifndef MissionControlMessage_H
#define MissionControlMessage_H
#include "stdio.h"
using namespace std;
enum MissionCodeEnum {Torpedo=0 , Path=1 , GreenGate=2, Traffics=3,BlackGate=4 , Shadow = 5};
struct MissionControlMessage
{
	// the code of the mission. can use the enum MissionCode
	int MissionCode;
	//code used for additional information. for example: color of object
	int additionalInformation;
	//points of bounds of the object. the order can be important.
	std::vector<std::pair<int,int> > bounds;
	//additional points that are important. the order can be important.
	std::vector<std::pair<int,int> > intrestPoints;
};
#endif
