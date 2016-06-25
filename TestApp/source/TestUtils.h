#ifndef TEST_UTILS
#define TEST_UTILS
#include <map>
#include <vector>
#include <string>
#include <cv.h>
using namespace cv;
using namespace std;
class TestUtils
{
public:
	static void setParam( map<string,string> &args,string name,double& param );
	static void setParam( map<string,string> &args,string name,int& param );
	static void setParamPercent( map<string,string> &args,string name,double& param );
	static string getString(int value);
	static string getString(double value);
	static string getFileName(const string& path);
	static Mat LoadImage(const string& imagePath);
	static vector<string> getFilesInDirectory(string dir);
	static bool isFile(string path);
	static bool isDirectory(string path);
};
#endif
