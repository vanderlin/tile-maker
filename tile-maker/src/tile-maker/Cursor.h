//
//  Cursor.h
//  tile-maker
//
//  Created by Todd Vanderlin on 11/16/17.
//
//

#pragma once
#include "ofMain.h"
namespace TileMaker {
	
	class Cursor {
	private:
		static bool didSetup;
		static void setup();
		Cursor();
	public:
		enum {
			CURSOR_DEFAULT,
			CURSOR_POINTER,
			CURSOR_HAND,
			CURSOR_GRAB,
			CURSOR_ADD,
			CURSOR_ROTATE_BL,
			CURSOR_ROTATE_BR,
			CURSOR_ROTATE_TL,
			CURSOR_ROTATE_TR,
			
			CURSOR_SCALE_BL,
			CURSOR_SCALE_BR,
			CURSOR_SCALE_TL,
			CURSOR_SCALE_TR,
			
			CURSOR_MOVE_COPY
			
		};
		static int mode;
		static map <int, ofImage> types;
		static void setMode(int _mode);
		static int getMode();
		static void draw();
		
	};
};
