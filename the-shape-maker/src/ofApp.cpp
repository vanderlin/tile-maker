#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	
	ofDisableAntiAliasing();
	ofBackground(34);

	refImage.load("tree.jpg");
	
	hideGui = false;
	gui.setup("the wall", "gui.xml", 10, 10);
	
	params.add(mode.set("function mode", 5, 0, 10));
	
	params.add(tw.set("texture width", 100, 10, 1000));
	params.add(th.set("texture height", 100, 10, 1000));
	params.add(res.set("texture res", 1, 1, 100));
	params.add(invert.set("invert", false));
	params.add(numLayers.set("layers", 10, 1, 255));
	params.add(depthRange.set("depth", 10, 0, 100));
	
	params.add(scaleX.set("scale x", 1, 0, 10.0));
	params.add(scaleY.set("scale y", 1, 0, 10.0));
	params.add(theta.set("theta", 1, 0, 10.0));
	
	params.add(animate.set("animate", true));
	params.add(showLayers.set("show layers", false));
	params.add(drawMesh.set("mesh", true));

	//gui.add(new ofxRangeSlider(range, 0, 255));
	gui.add(params);
	
	allocate();
	generateData();
	
	// zoom in the camera
	camera.setDistance(100);
	
	// listen to any param changes
	ofAddListener(params.parameterChangedE(), this, &ofApp::onParamChanged);
	
	gui.loadFromFile("gui.xml");
}

//--------------------------------------------------------------
void ofApp::allocate() {
	
	int w = tw * res;
	int h = th * res;
	
	sourceWidth = w;
	sourceHeight = h;
	
	texture.allocate(w, h, OF_IMAGE_COLOR);
	rangeImage.allocate(w, h, OF_IMAGE_COLOR);
	grayImage.allocate(w, h);
	
	// allocate the layers
	layers.clear();
	for(int i=0; i<numLayers; i++) {
		layers.push_back(Layer());
		layers.back().image.allocate(w, h, OF_IMAGE_GRAYSCALE);
	}
	
	mesh.clear();
	
	// hmm ok make a mesh
	// mesh = ofMesh::plane(sw, sh, sw, sh);
	
	for (int y = 0; y < h; y++){
		for (int x = 0; x < w; x++){
			mesh.addVertex(ofPoint(x, y, 0));
		}
	}
	
	for (int y = 0; y<h-1; y++){
		for (int x=0; x<w-1; x++){
			mesh.addIndex(x+y*w);				// 0
			mesh.addIndex((x+1)+y*w);			// 1
			mesh.addIndex(x+(y+1)*w);			// 10
			
			mesh.addIndex((x+1)+y*w);			// 1
			mesh.addIndex((x+1)+(y+1)*w);		// 11
			mesh.addIndex(x+(y+1)*w);			// 10
		}
	}
	
}

//--------------------------------------------------------------
void ofApp::generateData() {
	
	int w = tw * res;
	int h = th * res;
	
	bins.clear();
	for(int i=0; i<numLayers; i++) {
		int v = ofMap(i, 0, numLayers-1, 10, 255);
		bins.push_back(ofColor(v));
	}
	
	float t = animate ? (ofGetElapsedTimeMillis()*0.0002) : ofRandomf();
	
	for(int i=0; i<w; i++) {
		for(int j=0; j<h; j++) {
			float nx = ofMap(i, 0, w-1, 0.0, 1.0, true);
			float ny = ofMap(j, 0, h-1, 0.0, 1.0, true);
			
			float value = 0;
			
			if(mode == 0) {
				value = ofNoise(nx * scaleX, (ny + t) * scaleY) * 255;
			}
			else if(mode == 1) {
				value = ofNoise(cos(nx * TWO_PI) * scaleX, (ny + t) * scaleY) * 255;
			}
			else if(mode == 2) {
				float r = 10;
				float dx = r - (nx * r);
				float dy = r - (ny * r);
				float d = sqrt(dx*dx + dy*dy) - r;
				float z = sin(d * PI);
				value = ofMap(z, -1, 1, 0, 255, true);
			}
			else if(mode == 3) {
				float cx = 100;
				float cy = 100;
				float radius = 70;
				
				float waveLength = 30 * theta;
				
				float tw = 3.4;
				float phase = 1.5 + t;

				float x = nx * 200 * scaleX;
				float y = ny * 200 * scaleY;
				float dx = x - cx ;
				float dy = y - cy ;
				float d = sqrt(dx*dx + dy*dy);
				double r = (d - radius) / waveLength ;
				double k = r - (1-phase) * radius / waveLength ;
				double a = 1.0 / (1.0 + (r/tw) * (r/tw));
				
				float n = ofNoise(cos(nx * TWO_PI) * scaleX, (ny + t) * scaleY);
				float z = n * sin(k * TWO_PI);

				value = ofMap(z, -1, 1, 0, 255, true);
			}
			else if(mode == 4) {
				
				float x = cos(nx * TWO_PI * scaleX);
				float y = sin(ny * TWO_PI * scaleY);
				ofColor refPx = refImage.getColor(x*refImage.getWidth(), y*refImage.getHeight());
				
				float refGray = (refPx.r + refPx.g + refPx.b) / 3.0;
				float rn = ofMap(refGray, 0, 255, 0, 1);
				float z = ofNoise(rn * x, rn * y);
				//value = ofMap(z, -1, 1, 0, 255, true);
				value = ofMap(z, 0, 1, 0, 255, true);
				
			}
			else if(mode == 5) {
				float r = 10;
				float dx = cos(nx * TWO_PI * scaleX);
				float dy = cos(ny * TWO_PI * scaleY);
				float d = sqrt(dx*dx + dy*dy) - r;
				float z = sin(dx*dy * theta * t);
				value = ofMap(z, -1, 1, 0, 255, true);
			}
			else if(mode == 6) {
				float ratio = (float)h/(float)w;
				float r = cos(nx * ny) * scaleX;
				float cx = 0.5 * r;
				float cy = 0.5 * ratio;
				float x = nx * r;// * scaleX;
				float y = ny * r;// * scaleY;
				float dx = x - cx;
				float dy = y - cy;
				float d = sqrt(dx*dx + dy*dy);
				
				float z = sin(ofNoise(cos(nx * TWO_PI), ny, theta) * TWO_PI);
				
				value = ofMap(z, -1, 1, 0, 255, true);

			}
			int rangeIndex = ofMap(value, 0, 255, 0, bins.size(), true);
			ofColor rangeValue = bins[rangeIndex];
			
			texture.setColor(i, j, ofColor(value));
			rangeImage.setColor(i, j, rangeValue);
		}
	}
	
	if(invert) {
		ofxCv::invert(rangeImage);
	}
	
	for(int i=0; i<numLayers; i++) {
		
		int value = bins[i].r; // we are gray scale its ok
		
		for(int x=0; x<w; x++) {
			for(int y=0; y<h; y++) {
				int px = rangeImage.getColor(x, y).r;
				
				if (px >= value) {
					px = 255;
				} else {
					px = 0;
				}
				
				layers[i].image.setColor(x, y, ofColor(px));
			}
		}
		
		layers[i].image.update();
		
	}
	
	
	for(int i=0; i<layers.size(); i++) {
		
		// set the gray image from the layer pixels
		grayImage.setFromPixels(layers[i].image.getPixels());
		
		// create contours of the gray image
		contourFinder.findContours(grayImage, 0, w*h, 30, true);
		
		// parse contour
		layers[i].contours.clear();
		layers[i].path.clear();
		
		for(int j=0; j<contourFinder.blobs.size(); j++) {
			ofPolyline contour;
			contour.addVertices(contourFinder.blobs[j].pts);
			contour.close();
			layers[i].contours.push_back(contour);
			
			
			//cout << "adding shape" << endl;
		}
	}
	
	// scale the contours down ( clean up )
	float r = (float)h/(float)w;
	float sw = 100;
	float sh = sw * r;
	for(int i=0; i<layers.size(); i++) {
		
		layers[i].path.clear();
		layers[i].path.setFillColor(ofColor(255, 200));
		layers[i].path.setStrokeWidth(1);
		layers[i].path.setStrokeColor(ofColor(0));
		
		for(int j=0; j<layers[i].contours.size(); j++) {
			//shapes[i][j] = shapes[i][j].getSmoothed(2);
			ofPolyline & poly = layers[i].contours[j];
			
			for(int k=0; k<poly.size(); k++) {
				poly[k].x -= w/2;
				poly[k].y -= h/2;
				
				poly[k].x /= w;
				poly[k].y /= h;
				
				poly[k].x *= sw;
				poly[k].y *= sh;
			
				if (k == 0) {
					layers[i].path.moveTo(poly[k]);
				} else {
					layers[i].path.lineTo(poly[k]);
				}
			}
			layers[i].path.close();
			
		}
	}
	
	for(int i=0; i<layers.size(); i++) {
		layers[i].create();
	}
	
	
	/*for(int i=0; i<w*h; i++) {
	
		ofColor color = texture.getColor(i * 3 + 0);
	
		ofVec3f v = mesh.getVertex(i);
		
		v.z = ofMap(color.r, 0, 255, -depthRange, 0, true);
		
		mesh.setVertex(i, v);
	
	}*/
	for (int y = 0; y < h; y++){
		for (int x = 0; x < w; x++){
			
			ofColor color = texture.getColor(x, y);
			
			ofVec3f v = mesh.getVertex(y * w + x);
			
			v.z = ofMap(color.r, 0, 255, 0, depthRange, true);
			if ( y == 0 || y == h-1 || x == 0 || x == w-1) {
				//v.z = 0;
			}
			mesh.setVertex(y * w + x, v);
		}
	}
	rangeImage.update();
	texture.update();
}

//--------------------------------------------------------------
void ofApp::onParamChanged(ofAbstractParameter &value) {
	
	string name = value.getName();
	
	if(name == "texture width" || name == "texture height" || name == "texture res" || name == "layers") {
		allocate();
		ofLogNotice() << "allocate texture";
		generateData();
	}

}

//--------------------------------------------------------------
void ofApp::update() {
	if(animate) {
		generateData();
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofRectangle bounds(250, 200, 950, ofGetHeight()-200);

	float ratio = (float)th / (float)tw;
	float rw = 320;
	float rh = rw * ratio;
	
	ofSetColor(255);
	texture.draw(250, 10, rw, rh);
	rangeImage.draw(250 + rw + 10, 10, rw, rh);
	
	if (bins.size()) {
		int w = (float)rw / (float)bins.size();
		for(int i=0; i<bins.size(); i++) {
			ofSetColor(bins[i]);
			ofFill();
			ofDrawRectangle(250 + (i * w), 10 + th + 10, w, w);
			
			ofSetColor(20);
			ofNoFill();
			ofDrawRectangle(250 + (i * w), 10 + th + 10, w, w);
		}
	}
	
	// reset the hover index
	if (showLayers) {
		if(ofGetFrameNum()%3) hoverIndex += 1;
		if(hoverIndex > numLayers-1) {
			hoverIndex = 0;
		}
	} else {
		hoverIndex = -1;
	}
	float lw = 120;
	float lh = lw * ratio;
	ofPoint imagePos(bounds.getRight()+10, 10);
	for(int i=0; i<layers.size(); i++) {
		ofRectangle rect(imagePos, lw, lh);
		bool inside = rect.inside(ofGetMouseX(), ofGetMouseY());
		
		inside ? ofSetColor(247, 211, 108) : ofSetColor(255);
		
		if (inside && !showLayers) {
			hoverIndex = i;
		}
		
		ofPushMatrix();
		ofTranslate(imagePos);
		layers[i].image.draw(0, 0, lw, lh);
		ofPopMatrix();
		imagePos.y += (lh + 2);
		if(imagePos.y > (ofGetHeight() - lh+2)) {
			imagePos.y = 10;
			imagePos.x += lw + 2;
		}
	}
	
	gui.draw();
	
	float worldWidth = 100;
	float worldHeight = 50;
	
	// 3d world bounds
	ofNoFill();
	ofSetColor(20);
	ofDrawRectangle(bounds);
	
	ofEnableDepthTest();
	camera.begin(bounds);
	
	ofNoFill();
	ofSetColor(100);
	ofDrawBox(worldWidth, worldHeight, worldWidth);
	
	
	//ofSetColor(200);
	//ofNoFill();
	//ofDrawRectangle(-10, -10, 20, 20);
	
	
	if(drawMesh) {
		float sf = worldWidth / (float)sourceWidth;
		ofPushMatrix();
		ofTranslate(-(sourceWidth*sf)/2, 0, -(sourceHeight*sf)/2);
		ofRotate(90, 1, 0, 0);
		ofScale(sf, sf, sf);
		
		ofSetColor(255);
		mesh.drawFaces();
		ofSetColor(10);
		mesh.drawWireframe();
		
		ofPopMatrix();
		
	} else {
		ofPushMatrix();
		ofTranslate(0, 0, 0);
		for(int i=0; i<layers.size(); i++) {
			float depth = ofMap(i, 0, layers.size()-1, 0, depthRange) - (worldHeight/2);
			
			ofPushMatrix();
			ofTranslate(0, depth, 0);
			ofRotate(90, 1, 0, 0);
			
			if (hoverIndex == i) {
				layers[i].path.setFillColor(ofColor(247, 211, 108));
			} else {
				layers[i].path.setFillColor(ofColor(255, 200));
			}
			
			layers[i].path.draw();
			ofPopMatrix();
		
		
		}
		ofPopMatrix();
	}
	
	camera.end();
	ofDisableDepthTest();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
	if (key == 't') {
		hideGui = !hideGui;
	}
	else if(key == ' ') {
		generateData();
	}
	else if ('s') {
		
		mesh.save("mesh.ply");
		
	}
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
