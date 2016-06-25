/*
 * ParamUtils.h
 *
 *  Created on: Apr 18, 2013
 *      Author: shani
 */

#ifndef PARAMUTILS_H_
#define PARAMUTILS_H_
#include <map>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

class ParamUtils
{
public:
	ParamUtils();
	static void setParam(map<string, string> &args, string name, double& param);
	static void setParam(map<string, string> &args, string name, int& param);
	static void setParamPercent(map<string, string> &args, string name, double& param);
	static bool ReadConfigFile(const string& fileName, map<string, string>& params);
	static bool ReadDefaultConfigFile(const string& AlgoName, map<string, string>& params,bool vision);
	static string GetDefaultConfigFile(const string& AlgoName,bool test);
	static string GetConfigFilePath(const string& AlgoName,const string& configFile);
	virtual ~ParamUtils();
};

class ParameterException: exception
{
public:
	string _param;
	string _problem;
	ParameterException(string param, string problem) :
			exception()
	{
		_param = param;
		_problem = problem;
	}

	virtual const char* what() const throw()
	{
		stringstream stream;
		stream << _param << ": " << _problem;
		return stream.str().c_str();
	}

	~ParameterException() throw ()
	{
	}
};
#endif /* PARAMUTILS_H_ */
