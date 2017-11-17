#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Utils.h"
#include "ofxGuiExtensions.h"

#include "AppGlobals.h"
#include "Cursor.h"
#include "Canvas.h"
#include "Thumbnail.h"
#include "Asset.h"
#include "Shape.h"

#define MIN_HANDLE_DIS 25

using namespace TileMaker;

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
	void mouseScrolled(int x, int y, float scrollX, float scrollY);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void load();
	void save();
	
	// on param change
	void onParamChanged(ofAbstractParameter &value);
	Asset* getAssetFromFilename(string filename);
	
	vector<Asset*> assets;
	vector<Thumbnail*> thumbnails;
	Canvas canvas;
	
	// thumbnail slider
	ofRectangle thumbnailBounds;
	int thumbnailIndex;
	
	// the gui
	bool hideGui;
	ofXml settings;
	ofParameterGroup params;
	ofxPanel gui;
	
};
