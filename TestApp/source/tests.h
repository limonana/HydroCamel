/*
 * tests.h
 *
 *  Created on: Apr 4, 2013
 *      Author: shani
 */

#ifndef TESTS_H_
#define TESTS_H_
#include "BaseTestAlgorithm.h"
using namespace cv;
using namespace std;

void RunAlgo(BaseTestAlgorithm* algo, Mat& image, Mat& draw,
		const map<string, string>& args, const string& imageName, const string& windowName,
		bool showSteps, const vector<string>& categories,bool oneAlgo = false);

void RunOneImageFile(string imagePath, vector<BaseTestAlgorithm*> &algos,
		map<string, map<string, string> > Algo2Args, bool showSteps, vector<string> &categories);

void RunAlgoSimple(BaseTestAlgorithm* algo, Mat& image,
		map<string, string> args, Mat& draw);

void printParameters(const map<string, string>& data, char delimiter);



#endif /* TESTS_H_ */
