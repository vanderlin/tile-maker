//
//  Shape.cpp
//  tile-maker
//
//  Created by Todd Vanderlin on 11/16/17.
//
//

#include "Shape.h"
#include "Canvas.h"

using namespace TileMaker;

Shape::Shape(Asset * _ref, float _x, float _y, float _angle) {
	canvasRef = NULL;
	ref = _ref;
	x = _x; y = _y;
	angle = _angle;
	width = ref->getWidth();
	height = ref->getHeight();
	init();
}

Shape Shape::clone() {
	Shape c = Shape(ref, x, y, angle);
	c.width = width;
	c.height = height;
	c.canvasRef = canvasRef;
	c.init();
	return c;
}

void Shape::init() {
	isOver = false;
	isPressed = false;
	canRotate = false;
	isScaling = false;
	isSelected = false;
	cornerIndex = -1;
	overCornerIndex = -1;
	
	ofRectangle rect = getRectangle();
	
	corners.resize(4);
	corners[CORNER_TL] = rect.getTopLeft();
	corners[CORNER_TR] = rect.getTopRight();
	corners[CORNER_BR] = rect.getBottomRight();
	corners[CORNER_BL] = rect.getBottomLeft();
	updateCorners();
	
	ranColor = ofRandomColor();
}
//--------------------------------------------------------------
ofVec2f Shape::getPosition() {
	return getRectangle().getCenter();
}

//--------------------------------------------------------------
ofRectangle Shape::getRectangle() {
	return ofRectangle(x - width/2, y - height/2, width, height);
}

//--------------------------------------------------------------
ofXml Shape::toXML() {
	ofXml shapeXML;
	shapeXML.addChild("shape");
	shapeXML.setTo("shape");
	shapeXML.addValue("filename", ref->getFilename());
	shapeXML.addValue("x", getPosition().x);
	shapeXML.addValue("y", getPosition().y);
	shapeXML.addValue("width", getWidth());
	shapeXML.addValue("height", getHeight());
	shapeXML.addValue("rotation", getRotation());
	shapeXML.addValue("ratio", ref->getRatio());
	
	shapeXML.addValue("org_width", ref->getWidth());
	shapeXML.addValue("org_height", ref->getHeight());
	
	return shapeXML;
}

//--------------------------------------------------------------
bool Shape::inside(float _x, float _y) {
	return corners.inside(_x, _y);
}

//--------------------------------------------------------------
void Shape::updateCorners() {
	ofRectangle rect = getRectangle();
	ofVec2f center = getPosition();
	corners[CORNER_TL] = rect.getTopLeft();
	corners[CORNER_TR] = rect.getTopRight();
	corners[CORNER_BR] = rect.getBottomRight();
	corners[CORNER_BL] = rect.getBottomLeft();
	for(int i=0; i<4; i++) {
		ofVec2f p = corners[i] - center;
		p.rotate(angle);
		p += center;
		corners[i] = p;
	}
}

//--------------------------------------------------------------
void Shape::updateCursor(int index) {
	if (index != -1) {
		if (canRotate) {
			if (index == CORNER_TL) Cursor::setMode(Cursor::CURSOR_ROTATE_TL);
			if (index == CORNER_TR) Cursor::setMode(Cursor::CURSOR_ROTATE_TR);
			if (index == CORNER_BL) Cursor::setMode(Cursor::CURSOR_ROTATE_BL);
			if (index == CORNER_BR) Cursor::setMode(Cursor::CURSOR_ROTATE_BR);
		}
		else {
			if (index == CORNER_TL) Cursor::setMode(Cursor::CURSOR_SCALE_TL);
			if (index == CORNER_TR) Cursor::setMode(Cursor::CURSOR_SCALE_TR);
			if (index == CORNER_BL) Cursor::setMode(Cursor::CURSOR_SCALE_BL);
			if (index == CORNER_BR) Cursor::setMode(Cursor::CURSOR_SCALE_BR);
		}
	} else {
		if (!canvasRef->isPanning()) {
			Cursor::setMode(Cursor::CURSOR_DEFAULT);
		}
	}
}

//--------------------------------------------------------------
float Shape::getAngleFromPoints(ofVec2f a, ofVec2f b) {
	ofVec2f fromCenterTo = a - b;
	float actualAngle = -1.0 * atan2f(fromCenterTo.x,fromCenterTo.y) + HALF_PI;
	return ofRadToDeg(actualAngle);
}

//--------------------------------------------------------------
void Shape::setRotation(float a) {
	angle = a;
	ofLogNotice() << "rotate" << a;
	updateCorners();
}
//--------------------------------------------------------------
void Shape::rotate(float _angle) {
	angle += _angle;
	updateCorners();
}
//--------------------------------------------------------------
void Shape::setPosition(float _x, float _y) {
	x = _x;
	y = _y;
	updateCorners();
}

//--------------------------------------------------------------
void Shape::setSize(float _w, float _h) {
	width = _w;
	height = _w * ref->getRatio();
	
#pragma warning fix this later!
	float r = _h / _w;
	double diff = abs(ref->getRatio() - r);
	if (r != ref->getRatio()) {
		ofLogError() << "setting shape to a bad ratio " << diff << "\n" << r << " " << ref->getRatio();
	}
	updateCorners();
}

//--------------------------------------------------------------
void Shape::move(float _x, float _y) {
	x += _x;
	y += _y;
	updateCorners();
}

//--------------------------------------------------------------
array<ofRectangle, 4> Shape::getCornerRects() {
	array<ofRectangle, 4> rects;
	for (int i=0; i<4; i++) {
		rects[i].set(corners[i].x-HANDLE_SIZE/2, corners[i].y-HANDLE_SIZE/2, HANDLE_SIZE, HANDLE_SIZE);
	}
	return rects;
}

//--------------------------------------------------------------
int Shape::insideCorner(float _x, float _y) {
	// first are we inside a corner?
	int index = -1;
	auto rects = getCornerRects();
	for (int i=0; i<4; i++) {
		float dis = corners[i].distance(ofVec2f(_x, _y));
//		rects[i].inside(_x, _y) ||
		if(dis < 25) {
			index = i;
		}
	}
	return index;
}

//--------------------------------------------------------------
bool Shape::mouseMoved(int _x, int _y ) {
	
	// first are we inside a corner?
	overCornerIndex = insideCorner(_x, _y);
	updateCursor(overCornerIndex);
	
	if (overCornerIndex == -1) {
		isOver = inside(_x, _y);
	} else {
		isOver = false;
	}
	return isOver || overCornerIndex != -1;
}

//--------------------------------------------------------------
void Shape::mouseDragged(int _x, int _y, int button) {
	ofPoint mouse = Canvas::getScaledMouse();
	ofPoint prevMouse = Canvas::getPreviousScaledMouse();
	
	if (isPressed) {
		ofPoint diff = Canvas::getDiffScaledMouse();
		move(-diff.x, -diff.y);
	}
	else if (cornerIndex != -1) {
		
		if (canRotate) {
			float prevAngle = getAngleFromPoints(prevMouse, getPosition());
			float currentAngle = getAngleFromPoints(mouse, getPosition());
			float diff = currentAngle - prevAngle;
			rotate(diff);
		}
		else {
			isScaling = true;
			float ratio = ref->getRatio();
			float dx;
			ofPoint anchor;
			if(cornerIndex == CORNER_TR || cornerIndex == CORNER_BR) {
				dx = _x - downMouse.x;
			} else {
				dx = downMouse.x - _x;
			}
			
			
			// we need to work on this
			/*if (cornerIndex == CORNER_TR) {
				anchor = downRect.getBottomLeft();
				anchor.x += width/2;
				anchor.y -= height/2;
			}
			if (cornerIndex == CORNER_BR) {
				anchor = downRect.getBottomLeft();
				anchor.x += width/2;
				anchor.y += height/2;
			}*/
			
			float w = round(downRect.width + dx);
			
			if (w < MIN_SHAPE_SIZE) {
				w = MIN_SHAPE_SIZE;
			}
			
			if (w > ref->getWidth()) {
				w = ref->getWidth();
			}
			
			ofLogNotice() << w;
			float nw = w;
			float nh = nw * ratio;
			
			width = nw;
			height = nh;
			
		}
	}

	// now update the corners
	updateCorners();

}

//--------------------------------------------------------------
bool Shape::mousePressed(int _x, int _y, int button) {
	if (overCornerIndex == -1 && inside(_x, _y)) {
		isPressed = true;
	}
	else {
		cornerIndex = overCornerIndex;
		updateCursor(cornerIndex);
	}
	
	ofLogNotice() << cornerIndex;
	downRect = getRectangle();
	downPos = ofPoint(_x, _y) - getPosition();
	downMouse = ofPoint(_x, _y);
	//updateCorners();
	
	return isPressed || cornerIndex != -1;
}

//--------------------------------------------------------------
void Shape::mouseReleased(int _x, int _y, int button) {
	isPressed = false;
	isScaling = false;
	cornerIndex = -1;
	overCornerIndex = -1;
	updateCursor(-1);
}

//--------------------------------------------------------------
void Shape::drawRaw() {
	ofPoint pos = getPosition();
	ofPushMatrix();
	ofTranslate(pos);
	ofRotate(angle);
	ofSetColor(255);
	ref->image.draw(0, 0, width, height);
	ofPopMatrix();
}

//--------------------------------------------------------------
void Shape::draw() {
	
	ofPoint mouse = Canvas::getScaledMouse();
	ofPoint pos = getPosition();
	
	ofPushMatrix();
	ofTranslate(pos);
	ofRotate(angle);
	ofSetColor(255);
	if (AppSettings::drawImages && ref) {
		ref->image.draw(-width/2, -height/2, width, height);
	}
	if(AppSettings::debug) {
		ofFill();
		ofSetColor(ranColor, 100);
		ofDrawRectangle(-width/2, -height/2, width, height);
	}
	ofPopMatrix();

	ofSetColor(0, isOver?255:220);
	if(isSelected) {
		ofSetColor(COLOR_SELECTED);
	}
	corners.close();
	corners.draw();

	auto rects = getCornerRects();
	int closeIndex = -1;
	int minDis = 30;
	
	for (int i=0; i<4; i++) {
		ofPoint pnt(round(corners[i].x), round(corners[i].y));
		ofSetColor(0);
		//ofDrawBitmapString(ofToString(i), pnt);
		int overIndex = insideCorner(mouse.x, mouse.y);
		if (overIndex == i) {
			ofSetColor(0);
			closeIndex = i;
		}
		else {
			ofSetColor(20);
		}
		if(isSelected) {
			ofSetColor(COLOR_SELECTED);
		}
		ofSetRectMode(OF_RECTMODE_CENTER);
		ofDrawRectangle(pnt, 10, 10);
		ofSetRectMode(OF_RECTMODE_CORNER);
	}
	
	if(AppSettings::debug) {
		ofDrawBitmapStringHighlight(ofToString(orderIndex), x, y);
	}
	
	if (isScaling && cornerIndex != -1) {
		float scalePercent = getScaledPercent();
		scalePercent *= 100;
		auto corner = corners[cornerIndex];
		ofSetColor(10, 100);
		ofDrawLine(mouse, corner);
		ofSetColor(COLOR_HIGHLIGHT);
		ofDrawBitmapString(ofToString(scalePercent,1)+"%", mouse);

		string sizeString = ofToString(width,0)+"x"+ofToString(height,0);
		ofDrawBitmapString(sizeString, x - (sizeString.size()*11)/2, (y - height/2) - 11);

	}
}
