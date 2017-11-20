//
//  Artboard.cpp
//  tile-maker
//
//  Created by Todd Vanderlin on 11/18/17.
//
//

#include "Artboard.h"

using namespace TileMaker;
	
Artboard::Artboard() {
	cornerSize = 15;
	corners.resize(4);
	editable = false;
	isPressed = false;
	overCornerIndex = -1;
	pressedCornerIndex = -1;
}

void Artboard::update() {
	corners[CORNER_TL].set(x, y);
	corners[CORNER_TR].set(x + width, y);
	corners[CORNER_BR].set(x + width, y + height);
	corners[CORNER_BL].set(x, y + height);
}

void Artboard::mouseMoved(int _x, int _y ) {
	auto cornerRects = getCornerRects();
	overCornerIndex = -1;
	isOver = false;
	for (int i=0; i<4; i++) {
		if(cornerRects[i].inside(_x, _y)) {
			overCornerIndex = i;
			break;
		}
	}
	if(overCornerIndex == -1) {
		isOver = corners.inside(_x, _y);
	}
}

void Artboard::mouseDragged(int _x, int _y, float _px, float _py, int button) {
	float dx = _x - _px;
	float dy = _y - _py;
	
	if(pressedCornerIndex != -1) {
		if (pressedCornerIndex == CORNER_TR) {
			width += dx;
			height = width;
			y -= dx;
		}
		else if (pressedCornerIndex == CORNER_BR) {
			height += dx;
			width = height;
		}
		else if (pressedCornerIndex == CORNER_TL) {
			width -= dx;
			height = width;
			x += dx;
			y += dx;
		}
		else if (pressedCornerIndex == CORNER_BL) {
			width -= dx;
			height = width;
			x += dx;
		}
		ofLogNotice() << width << "x" << height;
		update();
	}
	else if(isPressed) {
		
		x += dx;
		y += dy;
		update();
	}
}

void Artboard::mousePressed(int _x, int _y, int button) {
	// first check if clicked a handle
	pressedCornerIndex = -1;
	auto cornerRects = getCornerRects();
	for (int i=0; i<4; i++) {
		if(cornerRects[i].inside(_x, _y)) {
			pressedCornerIndex = i;
			break;
		}
	}
	if(pressedCornerIndex== -1 && corners.inside(_x, _y)) {
		isPressed = true;
	}
}

void Artboard::mouseReleased(int _x, int _y, int button) {
	isPressed = false;
}

bool Artboard::inside(float _x, float _y) {
	return corners.inside(_x, _y);
}

void Artboard::setPosition(float _x, float _y) {
	set(_x, _y, width, height);
}

void Artboard::set(float _x, float _y, float _width, float _height) {
	x = _x; y = _y;
	width = _width; height = _height;
	update();
}

void Artboard::setSize(float _width, float _height) {
	set(x, y, _width, _height);
}

float Artboard::getRatio() {
	return height / width;
}

ofRectangle Artboard::getRect() {
	ofRectangle rect(x, y , width, height);
	return rect;
}

ofPolyline Artboard::getCornersScaled() {
	ofPolyline p;
	p.resize(4);
	for (int i=0; i<4; i++) {
		p[i] = corners[i] * AppSettings::worldScale;
	}
	return p;
}

ofPath Artboard::getShape() {
	ofPath shape;
	shape.setFilled(true);
	shape.setFillColor(ofColor((editable && isOver) ? 170 : 195));
	for (int i=0; i<4; i++) {
		i==0 ? shape.moveTo(corners[i]) : shape.lineTo(corners[i]);
	}
	shape.close();
	return shape;
}

array<ofRectangle, 4> Artboard::getCornerRects() {
	array<ofRectangle, 4> rects;
	for (int i=0; i<4; i++) {
		float cw = cornerSize / AppSettings::worldScale;
		rects[i].set(corners[i].x-cw/2, corners[i].y-cw/2, cw, cw);
	}
	return rects;
}

void Artboard::draw() {
	getShape().draw();
	auto cornerRects = getCornerRects();
	corners.draw();
	
	if(editable) {
		for (int i=0; i<4; i++) {
			ofFill();
			ofSetColor(overCornerIndex == i ? 0 : 10);
			ofDrawRectangle(cornerRects[i]);
		}
	}
	
	ofNoFill();
	ofSetColor(10);
	float pad = 1;
	ofDrawRectangle(x-pad, y-pad, (width)+(pad*2), (height)+(pad*2));
	
	ofSetColor(120);
	ofDrawBitmapString(ofToString(width,0)+"x"+ofToString(height,0), x, y + (height) + 15);
}

