//
//  Artboard.h
//  tile-maker
//
//  Created by Todd Vanderlin on 11/18/17.
//
//

#pragma once
#include "ofMain.h"
#include "Asset.h"
#include "Shape.h"
#include "Cursor.h"

namespace TileMaker {
	
	class Artboard {
	private:
		float cornerSize;
		int overCornerIndex;
		int pressedCornerIndex;
	public:
		ofPolyline corners;
		float x, y, width, height;
		bool editable;
		bool isOver, isPressed;
		Artboard();
		void update();
		void mouseMoved(int _x, int _y );
		void mouseDragged(int _x, int _y, float _px, float _py, int button);
		void mousePressed(int _x, int _y, int button);
		void mouseReleased(int _x, int _y, int button);
		
		void set(float _x, float _y, float _width, float _height);
		void setPosition(float _x, float _y);
		void setSize(float _width, float _height);
		
		bool inside(float _x, float _y);
		float getRatio();
		ofRectangle getRect();
		ofPolyline getCornersScaled();
		array<ofRectangle, 4> getCornerRects();
		ofPath getShape();
		void draw();
		
	};
}
