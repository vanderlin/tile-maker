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
	ofDirectory dir;
	dir.listDir("assets/crops");
	for (int i=0; i<dir.size(); i++) {
		assets.push_back(new Asset(dir.getPath(i)));
	}
	
	// create thumbnail refs
	for(auto asset : assets) {
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
Asset* ofApp::getAssetFromFilename(string filename) {
	for(auto asset : assets) {
		if (asset->getFilename() == filename) {
			return asset;
		}
	}
	return nullptr;
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
	for(auto shape : canvas.shapes) {
		shape->draw();
	}
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
	if(settings.load("settings.xml")) {
		
		settings.setTo("canvas");
		float scale = ofToFloat(settings.getAttribute("scale"));
		AppSettings::worldScale = scale <= 0 ? 1 : scale;
		canvas.setPosition(ofToFloat(settings.getAttribute("x")), ofToFloat(settings.getAttribute("y")));
		
		settings.setTo("artboard");
		float ax = settings.getValue<float>("x");
		float ay = settings.getValue<float>("y");
		float aw = MAX(settings.getValue<float>("width"), 1);
		float ah = MAX(settings.getValue<float>("height"), 1);
		
		canvas.artboard.set(ax, ay, aw, ah);
		settings.setToParent();

		if (settings.exists("//shapes")) {
			settings.setTo("shapes");
			int nShapes = settings.getNumChildren("shape");
			if (nShapes) {
				settings.setTo("shape[0]");
				for(int i=0; i<nShapes; i++) {
					
					Asset * asset = getAssetFromFilename(settings.getValue<string>("filename"));
					if (asset) {
						ofLogNotice() << "shapes " << asset;
						Shape * shape = canvas.addShape(new Shape(asset, 0, 0));
						shape->setPosition(settings.getValue<float>("x"), settings.getValue<float>("y"));
						shape->setSize(settings.getValue<float>("width"), settings.getValue<float>("height"));
						shape->setRotation(settings.getValue<float>("rotation"));

					}
					settings.setToSibling();
				}
			}
		}
		settings.setToParent();
		ofLogNotice() << "settings loaded";
	}
}

//--------------------------------------------------------------
void ofApp::save() {
	
	gui.saveToFile("gui.xml");
	
	settings.clear();
	settings.addChild("app");
	settings.addChild("canvas");
	settings.setTo("canvas");
	settings.setAttribute("x", ofToString(canvas.x));
	settings.setAttribute("y", ofToString(canvas.y));
	settings.setAttribute("scale", ofToString(AppSettings::worldScale));
	
	ofXml artboardXML;
	artboardXML.addChild("artboard");
	artboardXML.addValue("x", canvas.artboard.x);
	artboardXML.addValue("y", canvas.artboard.y);
	artboardXML.addValue("width", canvas.artboard.width);
	artboardXML.addValue("height", canvas.artboard.height);
	settings.addXml(artboardXML);
	
	settings.addChild("shapes");
	settings.setTo("shapes");
	
	for (auto shape : canvas.shapes) {
		
		ofXml shapeXML;
		shapeXML.addChild("shape");
		shapeXML.setTo("shape");
		shapeXML.addValue("filename", shape->ref->getFilename());
		shapeXML.addValue("x", shape->getPosition().x);
		shapeXML.addValue("y", shape->getPosition().y);
		shapeXML.addValue("width", shape->getWidth());
		shapeXML.addValue("height", shape->getHeight());
		shapeXML.addValue("rotation", shape->getRotation());
		shapeXML.addValue("ratio", shape->ref->getRatio());
		
		shapeXML.addValue("org_width", shape->ref->getWidth());
		shapeXML.addValue("org_height", shape->ref->getHeight());
		
		settings.addXml(shapeXML);
	}
	
	settings.setToParent();
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
		if (key == 's') {
			save();
		}
		if (key == 'z') {
			canvas.setZoom(1);
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	canvas.keyReleased(key);
	Cursor::setMode(Cursor::CURSOR_DEFAULT);
}

//--------------------------------------------------------------
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
	
	for(auto thumbnail : thumbnails) {
		thumbnail->mousePressed(x, y, button);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	
	canvas.mouseReleased(x, y, button);

	for(auto thumbnail : thumbnails) {
		if(thumbnail->mouseReleased(x, y, button)) {
			ofPoint dropPoint = canvas.toCanvas(x, y);
			canvas.addShape(new Shape(thumbnail->ref, dropPoint.x, dropPoint.y));
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
