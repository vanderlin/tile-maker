#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	
	ofBackground(96);

	gui.setup("App", "gui.xml");
	gui.setSize(UI_WIDTH-1, ofGetHeight()-UI_WIDTH-1);
	gui.setDefaultWidth(UI_WIDTH-1);
	gui.setPosition(1, 130);
	params.setName("tile maker");
	params.add(AppSettings::worldScale.set("scale", 1, 0.1, 50));
	params.add(canvas.editArtboard.set("edit artboard", true));
	params.add(AppSettings::drawImages.set("draw images", true));

	gui.add(params);
	
	// listen to any param changes
	ofAddListener(params.parameterChangedE(), this, &ofApp::onParamChanged);
	
	// for now this is hard coded
	// we can add a open files option
	Asset::load("assets/crops");
	
	// create thumbnail refs
	for(auto asset : Asset::all()) {
		thumbnails.push_back(new Thumbnail(asset));
	}
	thumbnailBounds.set(0, 0, ofGetWidth(), 130);
	thumbnailIndex = thumbnails.size() ? 0 : -1;
	
	
	// not sure about this right now
	// we are making a 1000x1000px file
	int w = 1000; // in pixels
	canvas.setSize(w, w);
	
	// load the gui from a file
	// add and shapes in the canavas
	load();
}

//--------------------------------------------------------------
void ofApp::onParamChanged(ofAbstractParameter &value) {
	string name = value.getName();
}

//--------------------------------------------------------------
void ofApp::update() {
	float x = 0;
	float y = 0;
	float offsetX = 0;
	for (int i=0; i<thumbnailIndex; i++) {
		offsetX += thumbnails[i]->getWidth();
	}
	for(auto thumbnail : thumbnails) {
		thumbnail->setPosition(x - offsetX, y);
		x += thumbnail->getWidth();
	}
	
	canvas.update(ofRectangle(UI_WIDTH, thumbnailBounds.height, ofGetWidth()-UI_WIDTH, ofGetHeight()-thumbnailBounds.height));
}

//--------------------------------------------------------------
void ofApp::draw() {
	
	canvas.drawBackground();
	
	canvas.begin();
	for(auto& shape : canvas.shapes) {
		shape.draw();
	}
	
	float scale = AppSettings::worldScale;
	ofPoint mouse = Canvas::getScaledMouse();

	ofPoint pos = Canvas::getScaledMouse();
	pos.y -= 100;
	ofDrawBitmapString(ofToString(mouse.x)+"\n"+ofToString(mouse.y), pos);
	
	
	canvas.end();
	
	canvas.drawUI();
	canvas.drawMiniMap();
	
	/*ofSetColor(255, 0, 52);
	ofNoFill();
	ofDrawRectangle(canvas);*/
	
	if(!hideGui) gui.draw();
	
	// render the thumbnails back
	ofSetColor(COLOR_UI_BACKGROUND);
	ofFill();
	ofDrawRectangle(thumbnailBounds);
	ofSetColor(COLOR_UI_BORDER);
	ofDrawLine(0, thumbnailBounds.getBottom(), ofGetWidth(), thumbnailBounds.getBottom());
	
	// render the thumbnails
	for(auto thumbnail : thumbnails) {
		thumbnail->draw();
	}
	
	// app cursor
	Cursor::draw();
}

//--------------------------------------------------------------
void ofApp::load() {
	gui.loadFromFile("gui.xml");
	canvas.load();
	ofXml settings;
	if(settings.load("settings.xml")) {
		settings.setTo("app");
		ofSetWindowShape(getValueFromXml(settings, "window_width", DEFAULT_WINDOW_WIDTH), getValueFromXml(settings, "window_height", DEFAULT_WINDOW_HEIGHT));
	}
}
void ofApp::save() {
	gui.saveToFile("gui.xml");
	ofXml settings;
	settings.addChild("app");
	settings.setTo("app");
	settings.addValue("window_width", (int)ofGetWindowWidth());
	settings.addValue("window_height", (int)ofGetWindowHeight());
	settings.save("settings.xml");
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
	// scroll if we are inside the thumbnails bounds
	if(thumbnailBounds.inside(ofGetMouse())) {
		if(thumbnailIndex < thumbnails.size()-1 && key == '2') {
			thumbnailIndex ++;
		}
		else if(thumbnailIndex > 0 && key == '1') {
			thumbnailIndex --;
		}
	}
	else {
	
		canvas.keyPressed(key);
		
		if (key == 't') {
			hideGui = !hideGui;
		}
		if (canvas.commandKeyPressed && key == 's') {
			save();
		}
		
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	canvas.keyReleased(key);
	Cursor::setMode(Cursor::CURSOR_DEFAULT);
}

//--------------------------------------------------------------
ofPoint prevPt;
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
	if(scrollY > 0 || scrollY < 0) {
		double range = 0.01;
		float value = AppSettings::worldScale + ofMap(scrollY,-1, 1, -range, range);
		value = ofClamp(value, AppSettings::worldScale.getMin(), AppSettings::worldScale.getMax());
		
		AppSettings::worldScale = value;

	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {
	
	canvas.mouseMoved(x, y);
	
	for(auto thumbnail : thumbnails) {
		thumbnail->mouseMoved(x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	
	canvas.mouseDragged(x, y, button);
	for(auto thumbnail : thumbnails) {
		thumbnail->mouseDragged(x, y, button);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	
	canvas.mousePressed(x, y, button);
	if (thumbnailBounds.inside(x, y)) {
		canvas.selectionEnabled = false;
	}
	for(auto thumbnail : thumbnails) {
		thumbnail->mousePressed(x, y, button);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	
	canvas.mouseReleased(x, y, button);
	canvas.selectionEnabled = true;
	
	for(auto thumbnail : thumbnails) {
		if(thumbnail->mouseReleased(x, y, button) && thumbnail->canDrop()) {
			ofPoint dropPoint = canvas.toCanvas(x, y);
			canvas.addShape(Shape(thumbnail->ref, dropPoint.x, dropPoint.y));
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	thumbnailBounds.set(0, 0, ofGetWidth(), 130);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
