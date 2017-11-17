#include "ofApp.h"

ofImage image;
ofRectangle bounds;

Shape * testShape;

//--------------------------------------------------------------
void ofApp::setup() {
	
	ofBackground(54);

	gui.setup("App", "gui.xml", 10, 10);
	params.setName("tile maker");
	params.add(zoom.set("zoom", 1, 0.1, 50));

	gui.add(params);
	
	// listen to any param changes
	ofAddListener(params.parameterChangedE(), this, &ofApp::onParamChanged);
	
	
	image.load("octopus-forever.jpg");
	
	
	ofDirectory dir;
	dir.listDir("crops");
	for (int i=0; i<dir.size(); i++) {
		assets.push_back(new Asset(dir.getPath(i)));
	}
	for(auto asset : assets) {
		thumbnails.push_back(new Thumbnail(asset));
	}
	
	int w = 1000; // in pixels
	canvas.setSize(w, w);
	
	testShape = NULL;
	//testShape = assets[(int)ofRandom(assets.size()-1)];
	//testShape->setPosition(100, 100);
	
	//canvas.addShape(new Shape(thumbnails[4]->ref, 300, 210));
	
	// load the gui from a file
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
	for(auto thumbnail : thumbnails) {
		thumbnail->setPosition(x, y);
		x += thumbnail->getWidth();
	}
	
}

//--------------------------------------------------------------
void ofApp::draw() {
	canvas.update();
	for(auto thumbnail : thumbnails) {
		thumbnail->draw();
	}
	
	canvas.begin();
	for(auto shape : canvas.shapes) {
		shape->draw();
	}
	canvas.end();
	
	canvas.setZoom(zoom);
	canvas.draw();
	canvas.drawMiniMap();
	
	ofSetColor(255, 0, 52);
	ofDrawRectangle(canvas);
	
	if(!hideGui) gui.draw();
	
	Cursor::draw();
}

//--------------------------------------------------------------
void ofApp::load() {
	gui.loadFromFile("gui.xml");
	if(settings.load("settings.xml")) {
		ofLogNotice() << "settings loaded";
		
		settings.setTo("canvas");
		float canvasX = ofToFloat(settings.getAttribute("x"));
		float canvasY = ofToFloat(settings.getAttribute("y"));
		canvas.setPosition(canvasX, canvasY);
		
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
		/*for (auto shape : canvas.shapes) {
			
			ofXml shapeXML;
			shapeXML.addChild("shape");
			shapeXML.setTo("shape");
			shapeXML.addValue("filename", shape->ref->getFilename());
			shapeXML.addValue("x", shape->getPosition().x);
			shapeXML.addValue("y", shape->getPosition().y);
			shapeXML.addValue("width", shape->getWidth());
			shapeXML.addValue("height", shape->getHeight());
			shapeXML.addValue("ratio", shape->ref->getRatio());
			
			settings.addXml(shapeXML);
		}*/
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
	settings.setToParent();
	
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
	canvas.keyPressed(key);
	if (key == 't') {
		hideGui = !hideGui;
	}
	if (key == 's') {
		save();
	}
	if (key == 'z') {
		zoom = 1.0;
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
		float value = zoom + ofMap(scrollY,-1, 1, -range, range);
		value = ofClamp(value, zoom.getMin(), zoom.getMax());

		zoom = value;
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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
