//#include <cvaux.h>
//#include <highgui.h>
//#include <photo_c.h>
//#include <cxcore.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <assert.h>
//#include <math.h>
//#include <float.h>
//#include <limits.h>
//#include <time.h>
//#include <ctype.h>
//#include "read.h"
//class Mouse
//{
//public:
//static Mat _img;
//static void onMouse(int event, int x, int y, int flags, void* param)
//{
//    char text[20];
//    Vec3b p;
//    Mat img2, img3;
//    img2 = Mouse::_img;
//    if (event == CV_EVENT_LBUTTONDOWN)
//    {
//    p = img2.at<Vec3b>(y,x);
//      //  std::cout << img2.at<cv::Vec3b>(x,y)[0] << " " << img2.at<cv::Vec3b>(x,y)[1] << " " << img2.at<cv::Vec3b>(x,y)[2] << std::endl;
//        sprintf(text, "R=%d, G=%d, B=%d", p[2], p[1], p[0]);
//        putText(img2, text, Point(5,15), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,255,0));
//        imshow("image", img2);
//       // putText(img2, text, textOrg, fontFace, fontScale,Scalar::all(255), thickness, 8);
//    }
//    else if (event == CV_EVENT_RBUTTONDOWN)
//    {
//        cvtColor(img2, img3, CV_BGR2HSV);
//        Vec3b p2 = img3.at<Vec3b>(y,x);
//        sprintf(text, "H=%d, S=%d, V=%d", p2[0], p2[1], p2[2]);
//        putText(img3, text, Point(5,15), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,255,0));
//        imshow("image", img3);
//    }
//    else
//        sprintf(text, "x=%d, y=%d", x, y);
////    CvScalar jkl;
//   // cvPutText(frame, text, cvPoint(5, 15), CV_FONT_HERSHEY_SIMPLEX, cvScalar(1.0));
//}
//};
