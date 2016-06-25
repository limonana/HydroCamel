#include <iostream>
#include <fstream>
#include "export.h"
#include <stdio.h>
#include "TestUtils.h"
#include "Utils/ParamUtils.h"
#include <fstream>
#include <iostream>
using namespace std;

string ExportAlgorithm(const BaseTestAlgorithm& algo)
{
	string fileName;
	string filePath;
	int i=1;
	while (true)
	{
		stringstream stream;
		//check which name exsists
		stream << "config/" << algo.Name() <<"/";
		stringstream fileNamestream;
		fileNamestream <<  algo.Name();
		if (i >1)
			fileNamestream<<i;
		fileNamestream<<".config";
		fileName = fileNamestream.str();
		stream << fileName;
		filePath = stream.str();
		ifstream fileExists(filePath.c_str());
		if (!fileExists.is_open()) break; //if found name that don't exist dtop searching
		fileExists.close();
		++i;
	}

	ofstream file;
	file.open(filePath.c_str(), ios::out);
	map<string, string> args = algo.getArgsAndValues();
	map<string, string>::iterator it;
	for (it = args.begin(); it != args.end(); ++it)
	{
		file << it->first << " " << it->second << endl;
	}
	file.close();

	return fileName;
}
