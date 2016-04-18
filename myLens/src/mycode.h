//
//  mycode.h
//  myLens
//
//  Created by 山口 周悟 on 2016/04/18.
//
//

#ifndef mycode_h
#define mycode_h

#include "ofxOpenCv.h"

using namespace std;
using namespace cv;


ofImage grabImage(ofVideoGrabber& grabber);
void savePhoto(string filename, ofImage& photo);

void detectRectangle(cv::Mat src, std::vector<cv::Point>& dstcorners);
void cleanCorners(std::vector<cv::Point> corners, std::vector<cv::Point>& dstcorners, int width, int height);
void myTransform(cv::Mat src, cv::Mat& dst, const int DST_WIDTH, const int DST_HEIGHT, vector<cv::Point>& corners);

#endif /* mycode_h */
