#include "BaseTestAlgorithm.h"
#include "TestUtils.h"
#include "Utils/ParamUtils.h"

void BaseTestAlgorithm::Run(Mat& image, map<string, string> args)
{
	try
	{
		BaseAlgorithm* algo = getAlgorithm();
		algo->ClearProcessData();
		algo->SetDefaultParams();
		if (args.size() > 0)
		{
			//fill the args with missing parameters (if using test command)
			map<string, string> newArgs = fillMissingParameters(args);
			algo->Load(newArgs);
		}

		algo->Run(image);
	}
	catch (ParameterException& e)
	{
		throw e.what();
	}
}

map<string, string> BaseTestAlgorithm::getArgsAndValues() const
{
	vector<ConfigParam> configParams = getConfigParams();
	return getArgsAndValues(configParams);
}

map<string, string> BaseTestAlgorithm::getArgsAndValues(vector<ConfigParam> configParams) const
{
	//TODO: maybe add filed to ConfigParam so it print some unit.
	map<string, string> res;
	for (uint i = 0; i < configParams.size(); ++i)
	{
		const ConfigParam& p = configParams[i];
		res[p.Name] = TestUtils::getString(p.Value);
	}
	return res;

}

void BaseTestAlgorithm::Load(const string& configFile)
{
	map<string, string> params;
	if (ParamUtils::ReadConfigFile(configFile, params))
		getAlgorithm()->Load(params);
	else
		throw configFile+" not found";
}

map<string, string> BaseTestAlgorithm::fillMissingParameters(map<string, string> args)
{
	map<string, string> orgArgs = getArgsAndValues();
	map<string, string>::iterator it;
	for (it = orgArgs.begin(); it != orgArgs.end(); ++it)
	{
		if (args.find(it->first) == args.end()) args[it->first] = it->second;
	}
	return args;
}

BaseTestAlgorithm::~BaseTestAlgorithm()
{

}
