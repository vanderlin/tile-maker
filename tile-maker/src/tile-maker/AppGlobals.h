//
//  AppGlobals.h
//  tile-maker
//
//  Created by Todd Vanderlin on 11/17/17.
//
//

#pragma once
#include "ofMain.h"
#include "Colors.h"

#define UI_WIDTH 200
#define MIN_SHAPE_SIZE 10
#define HANDLE_SIZE 20
#define DRAW_HANDLE_SIZE 10
#define THUMBNAIL_WIDTH 80
#define MAX_THUMBNAIL_HEIGHT 120

enum {
	CORNER_TL,
	CORNER_TR,
	CORNER_BR,
	CORNER_BL
};

class AppSettings {
public:
	static ofParameter<bool> drawImages;
	static ofParameter<float> worldScale;
};

