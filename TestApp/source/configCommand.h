/*
 * configCommand.h
 *
 *  Created on: Apr 3, 2013
 *      Author: shani
 */

#ifndef CONFIGCOMMAND_H_
#define CONFIGCOMMAND_H_
#include <string>
#include "BaseTestAlgorithm.h"
#include "CamerasController.h"
void createTrackbarWindow(string& imagePath, BaseTestAlgorithm* algo);
void createLiveTrackbarWindow(CameraType type, BaseTestAlgorithm* algo);
void createMultiWindowConfig(string dirPath, BaseTestAlgorithm* algo);


#endif /* CONFIGCOMMAND_H_ */
