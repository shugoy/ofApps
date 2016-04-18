//
//  mycode.cpp
//  myLens
//
//  Created by 山口 周悟 on 2016/04/19.
//
//

//#include "ofxiOS.h"
#include "mycode.h"


//ofImage grabImage(ofVideoGrabber& grabber)
//{
//    ofImage image;
//    int w = grabber.getWidth();
//    int h = grabber.getHeight();
//    image.allocate(w, h, OF_IMAGE_COLOR);
//    unsigned char *cdata = grabber.getPixels(),
//    *idata = image.getPixels();
//    //  image = -camera;
//    for (int k = 0; k < w * h * 3; k++) {
//        idata[k] = cdata[k];
//    }
//    return image;
//};


void mymin(vector<int>& vec, int* index) {
    double value = vec[0];
    for (size_t i=1; i<vec.size(); i++) {
        if (value > vec[i]) {
            value = vec[i];
            *index = (int)i;
        }
    }
}

void mymin(vector<cv::Point>& vec, cv::Point point,int* index) {
    double value = (vec[0].x - point.x) * (vec[0].x - point.x) + (vec[0].y - point.y) * (vec[0].y - point.y);
    for (size_t i=1; i<vec.size(); i++) {
        //double value1 = abs((vec[i].x - point.x) * (vec[i].y - point.y));
        double value1 = (vec[i].x - point.x) * (vec[i].x - point.x) + (vec[i].y - point.y) * (vec[i].y - point.y);
        if (value > value1) {
            value = value1;
            *index = (int)i;
        }
    }
}

//int preview() {
//    int count = 0;
//    int duration = (int)framevec.size();
//    if (duration == 0) {
//        return -1;
//    }
//    while (1) {
//        imshow("quickcheck", framevec[count]);
//        waitKey(30);
//        count = (count + 1) % duration;
//    }
//    return 0;
//}


void detectRectangle(cv::Mat src, std::vector<cv::Point>& corners) {
    
    cv::Mat channels[3];
    split(src, channels);
    Mat canny_r, canny_g, canny_b, canny_img;
    Canny(channels[0], canny_b, 50, 200);
    Canny(channels[1], canny_g, 50, 200);
    Canny(channels[2], canny_r, 50, 200);
    
    bitwise_or(canny_b, canny_g, canny_img);
    bitwise_or(canny_img, canny_r, canny_img);
    cv::Mat dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC3);
    
    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;
    cv::Mat dst2 = cv::Mat::zeros(src.rows, src.cols, CV_8UC3);
    
    // approximate
    cv::findContours( canny_img, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    
    vector<vector<cv::Point>> cnt;
    cnt.resize(contours.size());
    for( size_t k = 0; k < contours.size(); k++ ) {
        approxPolyDP(Mat(contours[k]), cnt[k], 3, true);
    }
    
    // get max length
    double length = 0;
    int index = 0;
    for(int idx = 0; idx < cnt.size(); idx++ )
    {
        double length1 = contourArea(cnt[idx], true);
        if (length < length1 ) {
            length = length1;
            index = idx;
        }
    }
    
    corners.clear();
    if (cnt.size()>0) {
        copy(cnt[index].begin(), cnt[index].end(), back_inserter(corners) );
    }

}

void cleanCorners(std::vector<cv::Point> corners, std::vector<cv::Point>& dstcorners, int width, int height) {
    if (corners.size() > 4) {
        
        //            for (size_t i=0; i<corners.size(); i++) {
        //                circle(dst, corners[i], 5, cv::Scalar(255,255,0));
        //                line(dst, corners[i], corners[(i+1)%4], cv::Scalar(255,255,0));
        //            }
        
        vector<cv::Scalar> colors(4);
        colors[0] = cv::Scalar(255,0,0);
        colors[1] = cv::Scalar(0,255,0);
        colors[2] = cv::Scalar(0,0,255);
        colors[3] = cv::Scalar(255,0,255);
        
        cv::Mat_<float> samples((int)corners.size(),2);
        cv::Mat_<float> centroids(4,2);
        cv::Mat_<int> labels((int)corners.size(), 1);
        cv::TermCriteria cri(cv::TermCriteria::MAX_ITER+cv::TermCriteria::EPS, 50, FLT_EPSILON);
        int clusterNum = 4;
        double compactness;
        
        /* samplesにデータを格納 */
        for (int i=0; i<(int)corners.size(); i++) {
            samples(i,0) = corners[i].x;
            samples(i,1) = corners[i].y;
        }
        
        /* k-means++を使用 */
        compactness = cv::kmeans(samples, clusterNum, labels, cri, 1, cv::KMEANS_PP_CENTERS, centroids);
        
        vector<vector<cv::Point>> points(4);
        for (int i=0; i<labels.rows; i++) {
            int index = labels(i,0);
            points[index].push_back(cv::Point(samples(i,0), samples(i,1)));
            //circle(dst, cv::Point(samples(i,0), samples(i,1)), 5, colors[index]);
        }
        
        vector<int> index0(4);
        vector<cv::Point> centvec(4);
        for (int j=0; j<4; j++) {
            centvec[j].x = centroids(j,0);
            centvec[j].y = centroids(j,1);
        }
        mymin(centvec, cv::Point(0,0), &index0[0]);
        mymin(centvec, cv::Point(width, 0), &index0[1]);
        mymin(centvec, cv::Point(width, height), &index0[2]);
        mymin(centvec, cv::Point(0,height), &index0[3]);
        
        //            for (size_t i=0; i<4; i++) {
        //                circle(dst, centvec[index0[i]], 5, colors[i]);
        //                line(dst, centvec[index0[i]], centvec[index0[(i+1)%4]], colors[i]);
        //            }
        
        float areasize = 0;
        vector<cv::Point> pointcandidate(4);
        vector<cv::Point> newcorners(4);
        //            for (int j=0; j<4; j++) {
        //                newcorners[j] = corners[j];
        //            }
        for (int i0=0; i0<points[index0[0]].size(); i0++) {
            pointcandidate[0].x = points[index0[0]][i0].x;
            pointcandidate[0].y = points[index0[0]][i0].y;
            
            for (int i1=0; i1<points[index0[1]].size(); i1++) {
                pointcandidate[1].x = points[index0[1]][i1].x;
                pointcandidate[1].y = points[index0[1]][i1].y;
                
                for (int i2=0; i2<points[index0[2]].size(); i2++) {
                    pointcandidate[2].x = points[index0[2]][i2].x;
                    pointcandidate[2].y = points[index0[2]][i2].y;
                    
                    for (int i3=0; i3<points[index0[3]].size(); i3++) {
                        pointcandidate[3].x = points[index0[3]][i3].x;
                        pointcandidate[3].y = points[index0[3]][i3].y;
                        
                        float areasize1 = cv::contourArea(pointcandidate);
                        //std::cout << areasize1 << std::endl;
                        if (areasize < areasize1) {
                            areasize = areasize1;
                            for (int j=0; j<4; j++) {
                                newcorners[j].x = pointcandidate[j].x;
                                newcorners[j].y = pointcandidate[j].y;
                            }
                        }
                    }
                }
            }
        }
        
        //            vector<int> index1(4);
        //            mymin(newcorners, cv::Point(0,0), &index1[0]);
        //            mymin(newcorners, cv::Point(0,480), &index1[1]);
        //            mymin(newcorners, cv::Point(640,480), &index1[2]);
        //            mymin(newcorners, cv::Point(640,0), &index1[3]);
        //            cv::Point2f src_pt[] = {
        //                newcorners[index[0]], newcorners[index[1]], newcorners[index[2]], newcorners[index[3]]
        //            };
        //vector<cv::Point> dstcorners(4);
        dstcorners.clear();
        dstcorners.resize(4);
        for (int i=0; i<4; i++) {
            dstcorners[i].x = newcorners[i].x;
            dstcorners[i].y = newcorners[i].y;
        }
        
        //        for (size_t i=0; i<4; i++) {
        //            circle(dst, dstcorners[i], 5, cv::Scalar(0,0,255));
        //            line(dst, dstcorners[i], dstcorners[(i+1)%4], cv::Scalar(0,0,255));
        //        }
    }
}

void myTransform(cv::Mat src, cv::Mat& dst, const int DST_WIDTH, const int DST_HEIGHT, vector<cv::Point>& corners) {
    vector<int> index(4);
    mymin(corners, cv::Point(0,0), &index[0]);
    mymin(corners, cv::Point(src.cols,0), &index[1]);
    mymin(corners, cv::Point(src.cols,src.rows), &index[2]);
    mymin(corners, cv::Point(0,src.rows), &index[3]);
    cv::Point2f src_pt[] = {
        corners[0], corners[1], corners[2], corners[3]
    };
    
    //    const int DST_WIDTH = dst.cols;
    //    const int DST_HEIGHT = dst.rows;
    
    const cv::Point2f dst_pt[]={
        cv::Point2f( 0.0, 0.0),
        cv::Point2f( DST_WIDTH - 1, 0),
        cv::Point2f( DST_WIDTH - 1 , DST_HEIGHT - 1),
        cv::Point2f( 0, DST_HEIGHT - 1)
    };
    
    const cv::Mat homography_matrix = cv::getPerspectiveTransform(src_pt,dst_pt);
    
    // transform
    dst = cv::Mat(DST_HEIGHT, DST_WIDTH, CV_8UC3);
    cv::warpPerspective( src, dst, homography_matrix, dst.size());
}

