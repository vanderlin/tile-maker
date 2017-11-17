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

Shape::Shape(Asset * _ref, float _x, float _y) {
	canvasRef = NULL;
	ref = _ref;
	isOver = false;
	isPressed = false;
	canRotate = false;
	isScaling = false;
	x = _x; y = _y;
	width = ref->getWidth();
	height = ref->getHeight();
	cornerIndex = -1;
	overCornerIndex = -1;
	angle = 0;
	ofRectangle rect = getRectangle();
	
	corners.resize(4);
	corners[CORNER_TL] = rect.getTopLeft();
	corners[CORNER_TR] = rect.getTopRight();
	corners[CORNER_BR] = rect.getBottomRight();
	corners[CORNER_BL] = rect.getBottomLeft();
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
	
	if (isPressed) {
		//setPosition(_x - downPos.x, _y - downPos.y);
		ofPoint diff = mouse - prevMouse;
		move(diff.x, diff.y);
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
	
	ofPoint pos = getPosition();
	
	ofPushMatrix();
	ofTranslate(pos);
	ofRotate(angle);
	ofFill();
	ofSetColor(isOver?200:255);
	ofDrawRectangle(-width/2, -height/2, width, height);
	ofSetColor(20, 100, 255);
	ofDrawBitmapString(ofToString(orderIndex), 0, 0);

	ofPopMatrix();

	ofSetColor(0, isOver?255:220);
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
		
		ofSetRectMode(OF_RECTMODE_CENTER);
		ofDrawRectangle(pnt, 10, 10);
		ofSetRectMode(OF_RECTMODE_CORNER);
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
