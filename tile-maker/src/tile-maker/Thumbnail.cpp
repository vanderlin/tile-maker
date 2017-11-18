//
//  Thumbnail.cpp
//  tile-maker
//
//  Created by Todd Vanderlin on 11/16/17.
//
//

#include "Thumbnail.h"
#include "Utils.h"

using namespace TileMaker;

Thumbnail::Thumbnail(Asset * _ref) {
	ref = _ref;
	didDrop = false;
	isOver = false;
	isPressed = false;
	width = MIN(THUMBNAIL_WIDTH, ref->getWidth());
	height = width * ref->getRatio();
	
	if (height > MAX_THUMBNAIL_HEIGHT) {
		height = MAX_THUMBNAIL_HEIGHT;
		width = height * ref->getRatioVertical();
	}
}

void Thumbnail::mouseMoved(int _x, int _y ) {
	if(inside(_x, _y)) {
		isOver = true;
	}
	else {
		isOver = false;
	}
}
void Thumbnail::mouseDragged(int _x, int _y, int button) {
	if(isPressed) {
		//ofPoint pos(_x - downPos.x, _y - downPos.y);
		ofPoint diff = ofGetMouseDiff();
		x += diff.x;
		y += diff.y;
		
		dragPos += diff;
		
		didDrop = true;
	}
}

void Thumbnail::mousePressed(int _x, int _y, int button) {
	if(inside(_x, _y)) {
		dragPos.set(x, y);
		isPressed = true;
	}
}

bool Thumbnail::mouseReleased(int _x, int _y, int button) {
	bool didDragAndDrop = false;
	if(isPressed && didDrop) {
		didDragAndDrop = true;
	}
	didDrop = false;
	isPressed = false;
	return didDragAndDrop;
}

void Thumbnail::draw() {
	ofPushMatrix();
	ofTranslate(x, y);
	if(isOver) {
		ofSetColor(255, isPressed?200:100);
		ofFill();
		ofDrawRectangle(0, 0, width, height);
	}
	ofSetColor(255);
	if(AppSettings::drawImages) {
		ref->image.draw(0, 0, width, height);
	}
	ofNoFill();
	ofDrawRectangle(0, 0, width, height);
	ofPopMatrix();
	if(isPressed) {
		ofSetColor(255, 100);
		ref->image.draw(dragPos.x, dragPos.y, width, height);
	}
}

