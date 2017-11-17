#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "Utils.h"
#include "ofxGuiExtensions.h"

class Layer {

public:
	ofImage image;
	ofPath path;
	vector <ofPolyline> contours;
	
	void create() {
		
	}
	
	void draw() {
		
	}
};
class ofApp : public ofBaseApp {

	public:
	
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	// on param change
	void onParamChanged(ofAbstractParameter &value);
	
	// allocate the texture base on the w,h
	void allocate();
	
	// generate texture data
	void generateData();
	
	// the texture for rendering
	ofImage texture;
	ofImage rangeImage;
	
	// 3d camera
	ofEasyCam camera;
	
	// params for gui
	ofParameterGroup params;
	
	// texture width and height
	ofParameter<int> tw, th, res;
	
	// source width and height
	int sourceWidth, sourceHeight;
	
	// what function are we using
	ofParameter<int> mode;
	
	// explode view height
	ofParameter<float> depthRange;
	
	// animate the generation
	ofParameter<bool> animate, showLayers, drawMesh;
	
	// invert the map
	ofParameter<bool> invert;
	
	// scale the generation function
	ofParameter<float> scaleX, scaleY, theta;
	
	// how many layers in the rendering
	ofParameter<int> numLayers;
	
	// image that we want to sample from
	ofImage refImage;
	
	// 3d version of map
	ofMesh mesh;
	
	// layer that we are hovering
	int hoverIndex;
	
	// color bins
	vector <ofColor> bins;
	
	// hold all layers
	vector <Layer> layers;
	
	// hide the gui
	bool hideGui;
	
	// the gui
	ofxPanel gui;
	
	// contour finder
	ofxCvContourFinder contourFinder;
	ofxCvGrayscaleImage grayImage;
	
};
