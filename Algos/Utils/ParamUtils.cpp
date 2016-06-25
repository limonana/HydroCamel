/*
 * ParamUtils.cpp
 *
 *  Created on: Apr 18, 2013
 *      Author: shani
 */

#include "ParamUtils.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <math.h>
ParamUtils::ParamUtils() {
	// TODO Auto-generated constructor stub

}

void ParamUtils::setParam(map<string, string>& args, string name,
		double& param) {
	try {
		stringstream a(args.at(name));
		a >> param;
		if (a.fail()) {
			stringstream err;
			err << "value of " << name << " is: " << args.at(name) << ". "
					<< "and is not a  double!" << endl;
			throw ParameterException(name, err.str());
		}
	} catch (...) {
		throw ParameterException(name, "don't exists in args");
	}
}

void ParamUtils::setParam(map<string, string>& args, string name, int& param) {
	try {
		stringstream a(args.at(name));
		a >> param;
		if (a.fail()) {
			stringstream err;
			err << "value of " << name << " is: " << args.at(name) << ". "
					<< "and is not an int!" << endl;
			throw ParameterException(name, err.str());
		}
	} catch (...) {
		throw ParameterException(name, "don't exists in args");
	}
}

void ParamUtils::setParamPercent(map<string, string>& args, string name,
		double& param) {
	try {
		int tmp;
		stringstream a(args.at(name));
		a >> tmp;
		if (a.fail()) {
			stringstream err;
			err << "value of " << name << " is: " << args.at(name) << ". "
					<< "and is not an int!" << endl;
			throw ParameterException(name, err.str());
		}
		param = (double)tmp / 100;

	} catch (...) {
		throw ParameterException(name, "don't exists in args");
	}
}

string ParamUtils::GetDefaultConfigFile(const string& AlgoName,bool test) {
	if (!test)
		return AlgoName+".config";
	else
		return GetConfigFilePath(AlgoName,AlgoName);
}

string ParamUtils::GetConfigFilePath(const string& AlgoName,const string& configFile) {
	stringstream stream;
	stream <<"config/" <<AlgoName <<"/" <<configFile << ".config";
	return stream.str();
}

ParamUtils::~ParamUtils() {
	// TODO Auto-generated destructor stub
}
bool ParamUtils::ReadDefaultConfigFile(const string& AlgoName,
		map<string, string>& params, bool test) {
	string filePath =  ParamUtils::GetDefaultConfigFile(AlgoName,test);
	return ReadConfigFile(filePath, params);
}

bool ParamUtils::ReadConfigFile(const string& filePath,
		map<string, string>& params) {
	ifstream file;

	file.open(filePath.c_str(), ios::in);

	if (!file.is_open() || !file.good())
		return false;

	while (file.good()) {
		string paramName, paramValue;
		getline(file, paramName, ' ');
		getline(file, paramValue);
		if (paramName == "")
			break;
		params[paramName] = paramValue;
	}
	file.close();
	return true;
}
