//
//  Canvas.cpp
//  tile-maker
//
//  Created by Todd Vanderlin on 11/16/17.
//
//

#include "Canvas.h"
#include "ofApp.h"

using namespace TileMaker;

bool Canvas::shouldRemoveShape(Shape &shape) {
	return shape.shouldRemove;
}

//--------------------------------------------------------------
Canvas::Canvas() {
	historyIndex = -1;
	selectedShape = nullptr;
	pressedShapeInSelection = false;
	pressedInsideShapes = false;
	pressedInsideArtboard = false;
	editArtboard = false;
	shapeCount = 0;
	enablePan = false;
	
	selectionEnabled = true;
	AppSettings::worldScale = 1;
	minZoom = 0.01; maxZoom = 50;
	
	commandKeyPressed = false;
	shiftKeyPressed = false;
	optionKeyPressed = false;
}

#pragma mark - adding shapes
//--------------------------------------------------------------
Shape * Canvas::addShape(Shape shape) {
	shapes.push_back(shape);
	Shape * ptr = &shapes.back();
	ptr->canvasRef = this;
	ptr->orderIndex = shapeCount;
	shapeCount ++;
	return &shapes.back();
}

//--------------------------------------------------------------
void Canvas::setSize(int w, int h) {
	artboard.setSize(w, h);
}

//--------------------------------------------------------------
bool Canvas::isPanning() {
	return enablePan;
}

#pragma mark - static canvas mouse positions
//--------------------------------------------------------------
ofPoint Canvas::getScaledMouse() {
	float scale = AppSettings::worldScale;
	ofApp * app = (ofApp*)ofGetAppPtr();
	float mx = ((ofGetMouseX() - app->canvas.x) / scale);
	float my = ((ofGetMouseY() - app->canvas.y) / scale);
	return ofPoint(mx, my);
}

//--------------------------------------------------------------
ofPoint Canvas::getPreviousScaledMouse() {
	float scale = AppSettings::worldScale;
	ofApp * app = (ofApp*)ofGetAppPtr();
	float mx = ((ofGetPreviousMouseX() - app->canvas.x) / scale);
	float my = ((ofGetPreviousMouseY() - app->canvas.y) / scale);
	return ofPoint(mx, my);
}
//--------------------------------------------------------------
ofPoint Canvas::getDiffScaledMouse() {
	ofPoint a = Canvas::getScaledMouse();
	ofPoint b = Canvas::getPreviousScaledMouse();
	ofPoint c = b - a;
	return c;
}
#pragma mark - canvas helpers

//--------------------------------------------------------------
ofPoint Canvas::toCanvas(ofPoint pt) {
	return toCanvas(pt.x, pt.y);
}
ofPoint Canvas::toCanvas(float _x, float _y) {
	float scale = AppSettings::worldScale;
	float mx = (_x - x) / scale;
	float my = (_y - y) / scale;
	return ofPoint(mx, my);
}

#pragma mark - History
//--------------------------------------------------------------
void Canvas::saveState() {
	
	if (history.size() > MAX_HISTORY) {
		history.erase(history.begin());
	}
	History state;
	state.setShapes(shapes);
	
	history.push_back(state);
	historyIndex = history.size()-1;
}

//--------------------------------------------------------------
void Canvas::setSate(int index) {
	History state = history[index];
	shapes.clear();
	for (auto shape : state.shapes) {
		shapes.push_back(shape);
	}
}

//--------------------------------------------------------------
void Canvas::undo() {
	if(historyIndex < history.size()-1) {
		historyIndex ++;
	}
	setSate(historyIndex);
}

//--------------------------------------------------------------
void Canvas::redo() {
	if(historyIndex > 0) {
		historyIndex --;
	}
	setSate(historyIndex);
}

#pragma mark - key input events
//--------------------------------------------------------------
void Canvas::keyPressed(int key) {
	
	if(key == OF_KEY_COMMAND) {
		commandKeyPressed = true;
	}
	if(key == OF_KEY_SHIFT) {
		shiftKeyPressed = true;
	}
	if(key == OF_KEY_ALT) {
		optionKeyPressed = true;
	}
	
	// delete
	if(key == OF_KEY_DEL || key == OF_KEY_BACKSPACE) {
		saveState();
		
		if(selectedShape) {
			selectedShape->shouldRemove = true;
			selectedShape = nullptr;
		}
		
		for(auto shape : selectionGroup) {
			shape->shouldRemove = true;
		}
		selectionGroup.clear();
		
		// call house keeper
		ofRemove(shapes, shouldRemoveShape);
		
		ofLogNotice() << "delete shape";
	}
	
	if (selectedShape) {
		float amt = shiftKeyPressed ? 10 : 1;
		if(key == OF_KEY_UP) {
			selectedShape->move(0, -amt);
		}
		if(key == OF_KEY_DOWN) {
			selectedShape->move(0, amt);
		}
		if(key == OF_KEY_RIGHT) {
			selectedShape->move(amt, 0);
		}
		if(key == OF_KEY_LEFT) {
			selectedShape->move(-amt, 0);
		}
	}
	
	if (selectionGroup.size()) {
		float amt = shiftKeyPressed ? 10 : 1;
		if(key == OF_KEY_UP) {
			for(auto& shape : selectionGroup) {
				shape->move(0, -amt);
			}
		}
		if(key == OF_KEY_DOWN) {
			for(auto& shape : selectionGroup) {
				shape->move(0, amt);
			}
		}
		if(key == OF_KEY_RIGHT) {
			for(auto& shape : selectionGroup) {
				shape->move(amt, 0);
			}
		}
		if(key == OF_KEY_LEFT) {
			for(auto& shape : selectionGroup) {
				shape->move(-amt, 0);
			}
		}
	}
		
	if (editArtboard) {
		if(key == OF_KEY_RETURN) {
			editArtboard = false;
		}
		if(key == 'z') {
			artboard.setPosition(0, 0);
		}
	}
	
	if (key == ' ') {
		enablePan = true;
		Cursor::setMode(ofGetMousePressed() ? Cursor::CURSOR_GRAB : Cursor::CURSOR_HAND);
	}
	
	if (key == OF_KEY_SHIFT) {
		for(auto& shape : shapes) {
			shape.canRotate = true;
		}
	}
	
	if (key == '`') {
		for(auto& shape : shapes) {
			shape.setRotation(0);
		}
	}
	
	
	// shift + command
	if (shiftKeyPressed && commandKeyPressed) {
		if(key == 'a') {
			editArtboard = !editArtboard;
		}
		if(key == '1') {
			setZoom(1);
		}
		if(key == '1') {
			setZoom(1);
		}
		if(key == 'z') {
			redo();
		}
		if(key == 'd') {
			deselectAll();
		}
	}
	
	// command
	if(!shiftKeyPressed && commandKeyPressed) {
		if(key == 'a') {
			selectAll();
		}
		if(key == '1') {
			fitToScreen();
		}
		if(key == 'z') {
			undo();
		}
		if(key == 'e') {
			exportSVG();
		}
		if(key == 's') {
			save();
		}
	}
	
}

//--------------------------------------------------------------
void Canvas::keyReleased(int key) {
	enablePan = false;
	if(key == OF_KEY_COMMAND) {
		commandKeyPressed = false;
	}
	if(key == OF_KEY_SHIFT) {
		shiftKeyPressed = false;
	}
	if(key == OF_KEY_ALT) {
		optionKeyPressed = false;
	}
	for(auto shape : shapes) {
		shape.canRotate = false;
	}
}

#pragma mark - Selection
//--------------------------------------------------------------
bool Canvas::isShapeInSelection(Shape &shape) {
	vector <ofPoint> pts;
	pts.push_back(selectionRect.getTopLeft());
	pts.push_back(selectionRect.getTopRight());
	pts.push_back(selectionRect.getBottomRight());
	pts.push_back(selectionRect.getBottomLeft());
	pts.push_back(selectionRect.getTopLeft());
	ofPolyline poly(pts);
	for (auto pt : shape.corners) {
		if(poly.inside(pt)) {
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------
void Canvas::selectAll() {
	selectionGroup.clear();
	for(auto& shape : shapes) {
		shape.isSelected = true;
		selectionGroup.push_back(&shape);
	}
}
//--------------------------------------------------------------
void Canvas::deselectAll() {
	selectionGroup.clear();
	for(auto& shape : shapes) {
		shape.isSelected = false;
	}
}
#pragma mark - Mouse events
//--------------------------------------------------------------
void Canvas::mouseMoved(int _x, int _y ) {
	ofPoint mouse = getScaledMouse();
	
	if(editArtboard) {
		artboard.mouseMoved(mouse.x, mouse.y);
	}
	else if(enablePan) {
		
	}
	else {
		bool isInsideShape = false;
		for(auto& shape : shapes) {
			shape.isOver = false;
		}
		for(int i=shapes.size()-1; i>=0; i--) {
			Shape& shape = shapes[i];
			if(shape.mouseMoved(mouse.x, mouse.y)) {
				break;
			}
		}
	}
}

//--------------------------------------------------------------
void Canvas::mouseDragged(int _x, int _y, int button) {
	
	float scale = AppSettings::worldScale;
	ofPoint mouse = getScaledMouse();
	ofPoint prevMouse = getPreviousScaledMouse();
	ofPoint diffMouse = mouse - prevMouse;
	bool insideArtboard = artboard.inside(mouse.x, mouse.y);
	
	// if we are not panning and we pressed inside a selection
	// group - we then just
	if(!enablePan && pressedShapeInSelection) {
		for(auto shape : selectionGroup) {
			shape->move(diffMouse.x, diffMouse.y);
		}
		return;
	}
	
	if (!enablePan && editArtboard) {
		artboard.mouseDragged(mouse.x, mouse.y, prevMouse.x, prevMouse.y, button);
	}
	
	// pan the world!
	else if(enablePan) {
		move(diffMouse.x * scale, diffMouse.y * scale);
	}
	
	else {
		
		// group the selection rectangle
		if (selectionGroup.size() == 0 && !pressedInsideShapes && selectionEnabled) {
			selectionRect.width += diffMouse.x;
			selectionRect.height += diffMouse.y;
			
			for(auto& shape : shapes) {
				if(isShapeInSelection(shape)) {
					shape.isSelected = true;
				}
				else {
					shape.isSelected = false;
				}
			}
		}
		
		// we are draggin a single selected shape
		else {
			if(selectedShape) {
				selectedShape->worldOffset.set(x, y);
				selectedShape->mouseDragged(mouse.x, mouse.y, button);
			}
			/*for(auto& shape : shapes) {
				shape.mouse = getScaledMouse();
				shape.prevMouse = getPreviousScaledMouse();
				shape.worldOffset.set(x, y);
				shape.mouseDragged(mouse.x, mouse.y, button);
			}*/
		}
	}
}

//--------------------------------------------------------------
void Canvas::mousePressed(int _x, int _y, int button) {
	ofPoint mouse = getScaledMouse();
	float scale = AppSettings::worldScale;
	
	// save the cuurent state of shapes
	saveState();
	
	// are we in a selection group
	if(selectionGroup.size() > 0) {
		pressedShapeInSelection = false;
		for (auto shape : selectionGroup) {
			if(shape->inside(mouse.x, mouse.y)) {
				pressedShapeInSelection = true;
			}
		}
	}
	
	// we pressed inside the selection group
	if (pressedShapeInSelection) {
		
		// we are holding down option and want to copy the whole group
		if(optionKeyPressed) {
			Cursor::setMode(Cursor::CURSOR_MOVE_COPY);
			vector<Shape*> tempCopy;
			for (auto shape : selectionGroup) {
				Shape * clone = addShape(shape->clone());
				clone->isSelected = true;
				
				tempCopy.push_back(clone);
			}
			for (auto shape : selectionGroup) {
				shape->isSelected = false;
			}
			selectionGroup.clear();
			for (auto shape : tempCopy) {
				selectionGroup.push_back(shape);
			}
		}
		return;
	}
	
	// we did not click the selection group clear it only
	// if we are not panning - then we want the selection still
	if (!enablePan) {
		selectionGroup.clear();
	}
	
	// edit the artboard
	if(editArtboard) {
		artboard.mousePressed(mouse.x, mouse.y, button);
	}
	
	// panning the artboard
	else if(enablePan) {
		// just return we do nothing
		return;
	}
	
	// just a press
	else {
		
		// update the shapes
		for(auto& shape : shapes) {
			shape.isSelected = false;
			shape.isPressed = false;
		}
		
		// get the first shape that we press - by going
		// backward untill we return true that we pressed shape
		pressedInsideShapes = false;
		
		for(int i=shapes.size()-1; i>=0; i--) {
			Shape& shape = shapes[i];
			if(shape.mousePressed(mouse.x, mouse.y, button)) {
				pressedInsideShapes = true;
				shape.isSelected = true;
				selectedShape = &shape;
				break;
			}
		}
		
		// is the option key pressed and if so copy the shape
		// and swap selected shape with new copy
		if(optionKeyPressed && selectedShape) {
			Cursor::setMode(Cursor::CURSOR_MOVE_COPY);
			
			selectedShape->isSelected = false;
			Shape clone = selectedShape->clone();
			selectedShape = addShape(clone);
			selectedShape->mousePressed(mouse.x, mouse.y, button);
			selectedShape->isSelected = true;

		}
		
		if (!pressedInsideShapes && selectionEnabled) {
			selectionRect.setPosition(mouse);
			selectedShape = NULL;
		}
	}
}

//--------------------------------------------------------------
void Canvas::mouseReleased(int _x, int _y, int button) {
	
	// we have made a selection - now look and see
	// if a shape is inside the selection rectangle
	if(selectionRect.getArea() > 0) {
		for(auto& shape : shapes) {
			if(isShapeInSelection(shape)) {
				shape.isSelected = true;
				selectionGroup.push_back(&shape);
			}
		}
	}
	
	pressedInsideShapes = false;
	ofPoint mouse = getScaledMouse();
	artboard.mouseReleased(mouse.x, mouse.y, button);
	
	for(auto& shape : shapes) {
		shape.mouseReleased(mouse.x, mouse.y, button);
	}
	
	selectionRect.set(0, 0, 0, 0);
}

// set the zoom / scale of the canvas
#pragma mark - zoom and fitting
//--------------------------------------------------------------
float Canvas::setZoom(float z) {
	AppSettings::worldScale = ofClamp(z, minZoom, maxZoom);
	ofLogNotice() << "zoom " << AppSettings::worldScale;
	return AppSettings::worldScale;
}

//--------------------------------------------------------------
void Canvas::move(float _x, float _y) {
	x += _x;
	y += _y;
}
//--------------------------------------------------------------
void Canvas::fitToScreen() {
	float g = 100;
	float m = MIN(worldRect.width - g, worldRect.height - g);
	float s = m / artboard.width;
	float scale = setZoom(s);

	ofLogNotice() << "desired tile size = " << m;
	ofLogNotice() << "artboard size = " << artboard.width;
	ofLogNotice() << "world size = " << worldRect.width;
	ofLogNotice() << "sacle size = " << s;

	ofRectangle scaledArtboard = artboard.getRect();
	scaledArtboard.x *= scale; scaledArtboard.width *= scale;
	scaledArtboard.y *= scale; scaledArtboard.height *= scale;
	 
	x = (worldRect.x - scaledArtboard.x) + ((worldRect.width - scaledArtboard.width) * 0.5);
	y = (worldRect.y - scaledArtboard.y) + ((worldRect.height - scaledArtboard.height) * 0.5);
}

// push and pop canvas
#pragma mark - push and pop canvas
//--------------------------------------------------------------
void Canvas::begin() {
	float scale = AppSettings::worldScale;
	
	ofSetColor(0);
	ofFill();
	ofDrawCircle(x, y, 2);
	
	ofPoint pos(x, y);
	ofPushMatrix();
	ofTranslate(pos);
	ofScale(scale, scale);
}

//--------------------------------------------------------------
void Canvas::end() {
	ofSetColor(COLOR_HIGHLIGHT);
	ofNoFill();
	ofDrawRectangle(selectionRect);
	ofPopMatrix();
}

#pragma mark - save & load
//--------------------------------------------------------------
void Canvas::save(string filename) {
	
	ofXml xml;
	xml.addChild("canvas");
	xml.setTo("canvas");
	xml.setAttribute("x", ofToString(x));
	xml.setAttribute("y", ofToString(y));
	xml.setAttribute("scale", ofToString(AppSettings::worldScale));
	
	ofXml artboardXML;
	artboardXML.addChild("artboard");
	artboardXML.addValue("x", artboard.x);
	artboardXML.addValue("y", artboard.y);
	artboardXML.addValue("width", artboard.width);
	artboardXML.addValue("height", artboard.height);
	
	xml.addXml(artboardXML);
	
	xml.addChild("shapes");
	xml.setTo("shapes");
	
	for (auto& shape : shapes) {
		ofXml shapeXML = shape.toXML();
		xml.addXml(shapeXML);
	}
	
	xml.save(filename);
	
	ofLogNotice() << filename << " saved";
}

//--------------------------------------------------------------
void Canvas::load(string filename) {
	ofXml xml;
	if(xml.load(filename)) {
		
		xml.setTo("canvas");
		float scale = ofToFloat(xml.getAttribute("scale"));
		AppSettings::worldScale = scale <= 0 ? 1 : scale;
		x = ofToFloat(xml.getAttribute("x"));
		y = ofToFloat(xml.getAttribute("y"));
		
		xml.setTo("artboard");
		float ax = xml.getValue<float>("x");
		float ay = xml.getValue<float>("y");
		float aw = MAX(xml.getValue<float>("width"), 1);
		float ah = MAX(xml.getValue<float>("height"), 1);
		
		artboard.set(ax, ay, aw, ah);
		xml.setToParent();
		
		if (xml.exists("//shapes")) {
			xml.setTo("shapes");
			int nShapes = xml.getNumChildren("shape");
			if (nShapes) {
				xml.setTo("shape[0]");
				for(int i=0; i<nShapes; i++) {
					Asset * asset = Asset::getWithFilename(xml.getValue<string>("filename"));
					if (asset) {
						Shape * shape = addShape(Shape(asset, 0, 0));
						ofLogNotice() << "shapes " << asset;
						shape->setPosition(xml.getValue<float>("x"), xml.getValue<float>("y"));
						shape->setSize(xml.getValue<float>("width"), xml.getValue<float>("height"));
						shape->setRotation(xml.getValue<float>("rotation"));
					}
					
					xml.setToSibling();
				}
			}
		}
		ofLogNotice() << filename << " loaded";
	}
}

#pragma mark - exporting
//--------------------------------------------------------------
void Canvas::exportSVG() {
	ofBeginSaveScreenAsSVG("test.svg");
	for(auto& shape : shapes) {
		shape.drawRaw();
	}
	ofEndSaveScreenAsSVG();
}

#pragma mark - drawing
//--------------------------------------------------------------
void Canvas::drawMiniMap() {
	//			return;
	//			float sr = getRatio();
	//			float mapW = 100;
	//			float mapH = mapW * sr;
	//
	//			float sx = mapW / width;
	//			float sy = mapH / height;
	//
	//			//			float sy = sh / (float)ofGetHeight();
	//			ofPoint pos(x, y);
	//			//			pos.x *= sx * scale;
	//			//			pos.y *= sy * scale;
	//			pos.x *= sx;
	//			pos.y *= sy;
	//
	//			float canvasW = (width * sx) / scale;
	//			float canvasH = (height * sy) / scale;
	//			ofPushMatrix();
	//			ofTranslate(ofGetWidth()-(mapW+10), ofGetHeight()-(mapH+10));
	//			ofSetColor(255, 200);
	//			ofFill();
	//			ofDrawRectangle(0, 0, mapW, mapH);
	//
	//			drawAtScale(sx / 3, sy / 3);
	//
	//			ofSetColor(0, 200);
	//			ofFill();
	//			ofDrawRectangle(pos, canvasW, canvasH);
	//
	//			ofPopMatrix();
}

//--------------------------------------------------------------
void Canvas::drawAtScale(float sx, float sy) {
	ofPoint pos(x, y);
	pos.x *= sx;
	pos.y *= sy;
	ofPushMatrix();
	ofTranslate(pos);
	ofScale(sx, sy);
	
	ofPopMatrix();
}

//--------------------------------------------------------------
void Canvas::update(ofRectangle bounds) {
	worldRect = bounds;
	for(auto& shape : shapes) {
		shape.worldOffset.set(x, y);
	}
}

//--------------------------------------------------------------
void Canvas::drawUI() {
	float scale = AppSettings::worldScale;
	ofRectangle uiRect(0, worldRect.y, worldRect.x, worldRect.height-0);
	ofFill();
	ofSetColor(COLOR_UI_BACKGROUND);
	ofDrawRectangle(uiRect);
	ofSetColor(COLOR_UI_BORDER);
	ofDrawLine(uiRect.getRight(), uiRect.getTop(), uiRect.getRight(), uiRect.getBottom());
	
	bool drawHistory = false;
	if (drawHistory) {
		for(int i=0; i<history.size(); i++) {
			if (i==historyIndex) {
				ofSetColor(255, 255, 0);
			}
			else {
				ofSetColor(10);
			}
			ofFill();
			
			ofPushMatrix();
			ofTranslate(worldRect.x + 10, worldRect.y + 10 + (i * 22));
			ofDrawRectangle(0, 0, 100, 20);
			ofSetColor(255);
			ofDrawBitmapStringHighlight(ofToString(history[i].shapes.size()), 10, 14);
			ofPopMatrix();
		}
	}
	float scalePct = scale * 100;
	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate(),0)+"fps\nScale X " + ofToString(scalePct, 1)+"%\nx "+ofToString(x)+" y"+ofToString(y), 20, ofGetHeight()-35);
}

//--------------------------------------------------------------
void Canvas::drawBackground() {
	
	ofFill();
	ofSetColor(95);
	ofDrawRectangle(worldRect);
	ofNoFill();
	ofSetColor(10);
	ofDrawRectangle(worldRect);
	/*
	 ofSetColor(195);
	 if(editArtboard) {
	 if(getScaledRect().inside(ofGetMouse())) {
	 ofSetColor(150);
	 }
	 }
	 ofFill();
	 ofDrawRectangle(x, y, width * scale, height * scale);
	 */
	
	begin();
	artboard.editable = editArtboard;
	artboard.draw();
	end();
	
	
}

