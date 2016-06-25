#include "TestUtils.h"
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <highgui.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include "Utils/ParamUtils.h"

void TestUtils::setParam(map<string, string> &args, string name, int& param)
{
	if (args.find(name) != args.end())
	{
		ParamUtils::setParam(args,name,param);
	}
}

void TestUtils::setParam(map<string, string> &args, string name, double& param)
{
	if (args.find(name) != args.end())
	{
		ParamUtils::setParam(args,name,param);
	}
}

string TestUtils::getString(int value)
{
	ostringstream tmp;
	tmp << value;
	return tmp.str();
}

void TestUtils::setParamPercent(map<string, string>& args, string name,
		double& param)
{
	if (args.find(name) != args.end())
	{
		ParamUtils::setParamPercent(args,name,param);
	}
}

string TestUtils::getString(double value)
{
	ostringstream tmp;
	tmp << value;
	return tmp.str();
}

string TestUtils::getFileName(const string& path)
{
	char tmp[200];
	strcpy(tmp, path.c_str());
	return basename(tmp);
}

Mat TestUtils::LoadImage(const string& imagePath)
{
	Mat image = imread(imagePath, CV_LOAD_IMAGE_COLOR);
	if (!image.data)
	   throw "can't find image " + imagePath;
	return image;
}

vector<string> TestUtils::getFilesInDirectory(string dir)
{
	vector<string> files;
	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(dir.c_str())) == NULL)
	{
		ostringstream tmp;
		tmp << "Error(" << errno << ") opening " << dir << endl;
		throw tmp.str();
	}

	while ((dirp = readdir(dp)) != NULL)
	{
		ostringstream filePathStream;
		filePathStream << dir << "/" << dirp->d_name;
		if (TestUtils::isFile(filePathStream.str()))
			files.push_back(string(dirp->d_name));
	}
	closedir(dp);
	return files;
}

bool TestUtils::isFile(string path)
{
	struct stat status;
	stat(path.c_str(), &status);
	return (S_ISREG(status.st_mode));
}

bool TestUtils::isDirectory(string path)
{
	struct stat status;
	stat(path.c_str(), &status);
	return (S_ISDIR(status.st_mode));
}

