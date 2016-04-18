#include "ofApp.h"
//#ifdef __cplusplus
//#include <opencv2/opencv.hpp>
//#endif

//--------------------------------------------------------------
void ofApp::setup(){	
	//ofSetOrientation(OF_ORIENTATION_90_RIGHT);//Set iOS to Orientation Landscape Right

	ofSetFrameRate(30);
    ofBackground(0, 0, 0);
    ofSetCircleResolution(150);
    //ofEnableAntiAliasing();
    ofEnableSmoothing();
    
    font.loadFont("Arial.ttf", 36);
    
    _state = CAMERA;
    
    button_pt = ofPoint(320,1048);
    text_pt = ofPoint(455, 1048);

	//grabber.setup(640, 960, OF_PIXELS_BGRA);
    //grabber.setup(1080, 1920, OF_PIXELS_BGRA);
    grabber.setup(1080,1920);

    //grabber.
    //grabber.draw(<#const ofRectangle &rect#>)
    
    colorImg.allocate(grabber.getWidth(), grabber.getHeight());
    capImg.allocate(650, 1075);
    photo.allocate(grabber.getWidth(), grabber.getHeight(), GL_RGB);
	tex.allocate(grabber.getWidth(), grabber.getHeight(), GL_RGB);
	
	pix = new unsigned char[ (int)( grabber.getWidth() * grabber.getHeight() * 3.0) ];
    
    //img.load("snake.png");
}

//--------------------------------------------------------------
void ofApp::update(){
	//ofBackground(255,255,255);
	
    if (_state == CAMERA) {
        grabber.update();
        bool bNewFrame = grabber.isFrameNew();
        if (bNewFrame == false) return;
        if( grabber.getPixels().getData() == NULL ) return;
        
        colorImg.setFromPixels(grabber.getPixels().getData(), grabber.getWidth(), grabber.getHeight());
        
        cvImg = colorImg.getCvImage();
        detectRectangle(cvImg, corners0);
        cleanCorners(corners0, corners, grabber.getWidth(), grabber.getHeight());
        
    }
	
//    if(grabber.isFrameNew() == true) {
//        ofPixels & pixels = grabber.getPixels();
//        unsigned char * src = pixels.getData();
//        int totalPix = grabber.getWidth() * grabber.getHeight() * 3;
//        
//        for(int k = 0; k < totalPix; k+= 3){
//            pix[k  ] = 255 - src[k];
//            pix[k+1] = 255 - src[k+1];
//            pix[k+2] = 255 - src[k+2];		
//        }
//        
//        tex.loadData(pix, grabber.getWidth(), grabber.getHeight(), GL_RGB);
//    }
}

//--------------------------------------------------------------
void ofApp::draw(){	
	
    if (_state == CAMERA) {
        //ofSetColor(0);
        //ofRect(0,0,90,360);
        //ofEnableAntiAliasing();
        ofSetColor(193, 39, 45);
        ofFill();
        ofCircle(320, 1048, 50);
        ofNoFill();
        ofSetLineWidth(7);
        ofCircle(320, 1048, 60);
        
        //ofDisableAntiAliasing();
        ofSetColor(255);
        //ofScale(2, 2);
        float r = 1.8;
        //grabber.draw(0, 0, grabber.getWidth()*2, grabber.getHeight()*2);
        grabber.draw(0, 0, grabber.getWidth()*r, grabber.getHeight()*r);
        
        int s = corners.size();
        //if (s == 4) {
        for (int i=0; i<s; i++) {
            //ofLine(corners[i%s].x*2, corners[i%s].y*2, corners[(i+1)%s].x*2, corners[(i+1)%s].y*2);
            ofLine(corners[i%s].x*r, corners[i%s].y*r, corners[(i+1)%s].x*r, corners[(i+1)%s].y*r);
        }
        //}
        
        //tex.draw(0, 0, tex.getWidth() / 4, tex.getHeight() / 4);
    }
    else if (_state == PHOTO) {
//        ofSetColor(96, 20, 22);
//        ofFill();
//        ofCircle(button_pt, 50);
//        ofNoFill();
//        ofSetLineWidth(7);
//        ofCircle(button_pt, 60);
        
        
        
        ofSetColor(255);
        //ofScale(2, 2);
        //img.draw(80, 0);
        //photo.draw(0, 0, photo.getWidth()*2, photo.getHeight()*2);
        capImg.draw(0, 0);//, photo.getWidth(), photo.getHeight());
        
        ofSetColor(193, 39, 45);
        //ofRotate(90);
        font.drawString("DONE", text_pt.x, text_pt.y);
        
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
    
}

//--------------------------------------------------------------

//void savePhoto(string filename, ofImage& photo){
//    ofFile file;
//    ofBuffer imageBuffer;
//    ofSaveImage(photo.getPixelsRef(), imageBuffer);
//    
//    file.open(ofxiPhoneGetDocumentsDirectory() + filename, ofFile::WriteOnly);
//    file << imageBuffer;
//    file.close();
//}

void ofApp::touchDown(ofTouchEventArgs & touch){

    if (_state == CAMERA)
    {
        float xx = touch.x - button_pt.x;
        float yy = touch.y - button_pt.y;
        if (xx*xx + yy*yy < 50*50)
        {
            if (corners.size() != 4) {
                return;
            }
            _state = PHOTO;
            //photo = grabber.getTexture();
            cv::Mat dst;
            myTransform(cvImg, dst, capImg.getWidth(), capImg.getHeight(), corners);
            //*capImg.getCvImage() = dst;
//            IplImage iplimg = dst;//cv::Mat to IplImage
//            capImg.getCvImage() = &iplimg;
            IplImage iplImage(dst);
            capImg = &iplImage;
            
            //ofImage image = grabber.getPixels();
            
//            // get time
//            int y = ofGetYear();
//            int mt = ofGetMonth();
//            int d = ofGetDay();
//            int h = ofGetHours();
//            int m = ofGetMinutes();
//            int s = ofGetSeconds();
//            string time = ofToString(y, 0) +
//                          ofToString(mt, 0) +
//                          ofToString(d, 0) +
//                          ofToString(h, 0) +
//                          ofToString(m, 0) +
//                          ofToString(s, 0);
//            ofImage colorImg = grabImage(grabber);
//            savePhoto(time+".png", colorImg);
//            //colorImg.saveImage(time+".png");
        }
    }
    else
    {
        float xx = touch.x - text_pt.x;
        float yy = touch.y - text_pt.y;
        if (xx > 0 && xx < 128 && yy < 0 && yy > -58) {
            _state = CAMERA;
        }
    }
}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::lostFocus(){
    
}

//--------------------------------------------------------------
void ofApp::gotFocus(){
    
}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){
    
}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){
    
}


