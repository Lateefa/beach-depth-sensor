


#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetWindowShape(1280, 960);
	debug = true;
		realSense = RSDevice::createUniquePtr();

		realSense->checkConnectedDialog();
		realSense->setDepthSize(424, 240);
		ofSetVerticalSync(true);
		//realSense->hardwareReset();

		ofLogNotice("Device detected..");
		//gui_post.setup("PostProcessing", "postprocessingSetup", 0, 0); // most of the time you don't need a name but don't forget to call setup
		//gui_post.add(realSense->param_usePostProcessing);
		//gui_post.add(realSense->param_filterDecimation);
		//gui_post.add(realSense->param_filterDecimation_mag);
		//gui_post.add(realSense->param_filterDisparities);
		//gui_post.add(realSense->param_filterSpatial);
		//gui_post.add(realSense->param_filterSpatial_smoothAlpha);
		//gui_post.add(realSense->param_filterSpatial_smoothDelta);
		//gui_post.add(realSense->param_filterSpatial_mag);
		//gui_post.add(realSense->param_filterTemporal);
		//gui_post.add(realSense->param_filterTemporal_smoothAlpha);
		//gui_post.add(realSense->param_filterTemporal_smoothDelta);
		//gui_post.add(realSense->param_filterTemporal_persistency);

		//realSense->enablePointCloud(CloudRes::FULL_RES);
		realSense->setPointCloudRange(100.0f, 1000.0f);

		gui_device.setup("Device", "deviceSettings", 200, 0);

		if (realSense->capture()) {
			// the device settings should be loaded/set after the start()
		/*	gui_device.add(realSense->param_deviceLaser);
			gui_device.add(realSense->param_deviceLaser_mag);
			gui_device.add(realSense->param_deviceAutoExposure);
			gui_device.add(realSense->param_deviceExposure_mag);
			gui_device.add(realSense->param_deviceGain_mag);
			gui_device.add(realSense->param_deviceFrameQueSize_mag);
			gui_device.add(realSense->param_deviceAsicTemparature);
			gui_device.add(realSense->param_deviceProjectorTemparature);*/
			gui_device.add(farThreshold.set("Far Threshold", .425, 0, 5));
		
	
	}

	realSense->printDeviceInfo();
	ofEnableAlphaBlending();

player1.load("videos/diirty.mp4");
player1.setLoopState(OF_LOOP_NORMAL);
player1.play();

player2.load("videos/clean.mp4");
player2.setLoopState(OF_LOOP_NORMAL);
player2.play();

play1 = false;
play2 = false;


}

//--------------------------------------------------------------
void ofApp::update()
{
	player1.update();
	player2.update();

	if (realSense->update(ofxRSSDK::PointCloud::INFRALEFT)) {
		// if a frame has been updated, the code continues in here,
		// in case you need to do something special...
		amountPresent = 0;
		remove_background(realSense->mDepthFrame, farThreshold);//this number is in meters

	}
	int size = realSense->getDepthWidth() * realSense->getDepthHeight();

	float fade = ofMap(amountPresent, 0, size, 0, 255); // floatvalue, input min, input max, output min, output max

	cout << fade << "" << endl;


	if (fade > 33) {
		play1 = true;
		play2 = false;

		cout << "dirty" << endl;
		}
		else {

			if (fade < 25) {
				play1 = false;
				play2 = true;
	
				cout << "clean" << endl;

			}
		}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	int size = realSense->getDepthWidth() * realSense->getDepthHeight();

	float fade = ofMap(amountPresent, 0, size, 0, 255); // floatvalue, input min, input max, output min, output max



	ofClear(ofColor::black);
	ofSetColor(ofColor::white);

	if (debug) {
		//realSense->drawVideoStream(ofRectangle(0, 0, ofGetWidth(), ofGetHeight()));
		realSense->drawVideoStream(ofRectangle(0, 0, ofGetWidth() / 2., ofGetHeight() / 2.));
		realSense->drawDepthStream(ofRectangle(ofGetWidth() / 2., 0, ofGetWidth() / 2, ofGetHeight() / 2.));
		ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), ofGetWidth() - 200, 10);
		gui_post.draw();
		gui_device.draw();
	}
	else {

		ofEnableBlendMode(OF_BLENDMODE_ALPHA); //enables alpha mode

		if (play1 == true) {

			ofSetColor(255, 255, 255, 255); // 100 is the alpha value; Here is where u set the alpha value
			player2.draw(0, 0, ofGetWidth(), ofGetHeight());
			ofSetColor(255, 255, 255, fade);
			player1.draw(0, 0, ofGetWidth(), ofGetHeight());
		}
		else {

			if (play2 == true) {
				{
					ofSetColor(255, 255, 255, fade); // 100 is the alpha value; Here is where u set the alpha value
					player1.draw(0, 0, ofGetWidth(), ofGetHeight());
					ofSetColor(255, 255, 255, 255);
					player2.draw(0, 0, ofGetWidth(), ofGetHeight());
				}
			}
		}
	}
}
	








//--------------------------------------------------------------

void ofApp::remove_background(ofPixelsRef threshold_frame, float clipping_dist)

{

#pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop



	rs2::depth_frame df = realSense->rs2Depth;

	const uint16_t* depth_frame = reinterpret_cast<const uint16_t*>(df.get_data());

	float depth_scale = .001; //sets the depth to be in meters

	for (int y = 0; y < realSense->getDepthHeight(); y++) {

		for (int x = 0; x < realSense->getDepthWidth(); x++) {

			glm::vec2 col = glm::vec2(x, y);

			float distance = depth_scale * depth_frame[x + y * realSense->getDepthWidth()];
			if (distance > clipping_dist){ // || x > realSense->getDepthWidth() / 2 ) {// || distance < someCloseMeterNumeber  // thats where you draw edges?

				threshold_frame.setColor(x, y, ofColor(0));
			
			}
			else {
				amountPresent += 1;  //if something isn't within the threshold, increase by 1

				}
			
			}
		}
}
	






void ofApp::exit()
{
	realSense->stop();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	if (key == 'd') {
		debug = !debug;
	}
	if (key == 'f') {
		ofToggleFullscreen();
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}