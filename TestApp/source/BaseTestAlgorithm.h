/*
 * BaseTestAlgorithm.h
 * interface each algorithm have to implement in order
 * to work with the test application
 */
#ifndef BaseTestAlgorithm_H
#define BaseTestAlgorithm_H
#include <cv.h>
#include <highgui.h>
#include <vector>
#include <map>
#include <string>
#include "BaseAlgorithm.h"
using namespace cv;
using namespace std;

struct ConfigParam
{
	string Name;
	int Value;
	int MaxValue;
};
class BaseTestAlgorithm
{
	/* IMPORTANT: all the arguments / parameters appearing at these functions
	 * must have the same name.
	 * (Run,getConfigParams,getParams)
	 */

	/* TO OVERRIDE*/
public:
	//return the name of the algorithm
	virtual string Name() const=0;

	//return the categories of process images
	//defined by each person.
	//use for help (not implemented yet)
	virtual vector<string> getImagesCategories() const =0;

	/*
	 * return the images of the process by categories.
		you create you own categories.
		used for see images of the process of the algorithm.
	 */
	virtual map<string, Mat> getProcessImagesByCategories(vector<string> categries) const =0;

	//return the images of the process by their names		
	virtual map<string, Mat> getAllProcessImages() const =0;

	/*
	 * return the name ,value and max value for each argument.
	   the value and max value are ints,
	   so if the algorithm use another type of argument, make a conversion.
	   this arguments will shown as trackbars when you use the config command.
	 */
	virtual vector<ConfigParam> getConfigParams() const =0;

	/*
	 * return the parameters of the results.
	 * return pairs of name and value.
	 * the values must be string so if it isn't convert it to string
	 */
	virtual map<string, string> getResults() const =0;

	//draw the results on the image	
	virtual void DrawResults(Mat& draw)=0;

	/*
	 * return process data .
	 * return pair of name and value.
	 */
	virtual map<string, string> getProcessData() const =0;
protected:
	//return the algorithm this test class use.
	virtual BaseAlgorithm* getAlgorithm()=0;

public:
	void Run(Mat& image, map<string, string> args);
	map<string, string> getArgsAndValues() const;
	map<string, string> getArgsAndValues(vector<ConfigParam> configParams) const;
	virtual ~BaseTestAlgorithm();
	void Load(const string& configFile);

private:
	map<string, string> fillMissingParameters(map<string, string> args);
};

#endif
