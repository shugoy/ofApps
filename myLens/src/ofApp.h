#pragma once

#include "ofxiOS.h"
#include "ofxOpenCv.h"
#include "mycode.h"
//#include "cutRect.h"

enum storystate{CAMERA, PHOTO};

class ofApp : public ofxiOSApp{
    
private:
    ofTrueTypeFont font;
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void touchDown(ofTouchEventArgs & touch);
    void touchMoved(ofTouchEventArgs & touch);
    void touchUp(ofTouchEventArgs & touch);
    void touchDoubleTap(ofTouchEventArgs & touch);
    void touchCancelled(ofTouchEventArgs & touch);
    
    void lostFocus();
    void gotFocus();
    void gotMemoryWarning();
    void deviceOrientationChanged(int newOrientation);
    
    ofVideoGrabber grabber;
    ofTexture tex;
    unsigned char * pix;
    
    storystate _state;
    
    ofImage img;
    ofTexture photo;
    
    ofPoint button_pt;
    ofPoint text_pt;
    
    vector<cv::Point> corners0;
    vector<cv::Point> corners;
    ofxCvColorImage colorImg;
    cv::Mat cvImg;
    ofxCvColorImage capImg;
};
