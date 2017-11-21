//
//  Cursor.cpp
//  tile-maker
//
//  Created by Todd Vanderlin on 11/16/17.
//
//

#include "Cursor.h"
using namespace TileMaker;

bool Cursor::didSetup = false;
int Cursor::mode;
map <int, ofImage> Cursor::types;
Cursor::Cursor(){}
void Cursor::setup() {
	didSetup = true;
	//grab-cursor.png
	//images.push_back(img);
	
	types[CURSOR_GRAB] = ofImage("cursors/Cur_Fist_11_11.png");
	types[CURSOR_HAND] = ofImage("cursors/Cur_Hand_11_11.png");
	types[CURSOR_POINTER] = ofImage("icons/pointer-cursor.png");
	types[CURSOR_ADD] = ofImage("icons/add-cursor.png");
	types[CURSOR_MOVE_COPY] = ofImage("cursors/Cur_MoveCopy_1_1.png");
	
	types[CURSOR_ROTATE_TR] = ofImage("cursors/Cur_RotateTopRightCorner_7_7.png");
	types[CURSOR_ROTATE_TL] = ofImage("cursors/Cur_RotateTopLeftCorner_7_7.png");
	types[CURSOR_ROTATE_BR] = ofImage("cursors/Cur_RotateBottomRightCorner_7_7.png");
	types[CURSOR_ROTATE_BL] = ofImage("cursors/Cur_RotateBottomLeftCorner_7_7.png");
	
	types[CURSOR_SCALE_TR] = ofImage("cursors/Cur_ScaleTRBL_9_9.png");
	types[CURSOR_SCALE_TL] = ofImage("cursors/Cur_ScaleTLBR_9_9.png");
	types[CURSOR_SCALE_BR] = ofImage("cursors/Cur_ScaleTLBR_9_9.png");
	types[CURSOR_SCALE_BL] = ofImage("cursors/Cur_ScaleTRBL_9_9.png");
}

void Cursor::setMode(int _mode) {
	if (!didSetup) {
		Cursor::setup();
	}
	mode = _mode;
}
int Cursor::getMode() {
	return mode;
}
void Cursor::draw() {
	float x = ofGetMouseX();
	float y = ofGetMouseY();
	if (mode == CURSOR_DEFAULT) {
		ofShowCursor();
	} else {
		ofHideCursor();
		ofSetColor(255);
		types[mode].draw(x, y);
	}
}

