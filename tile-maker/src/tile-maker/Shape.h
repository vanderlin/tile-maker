//
//  Shape.h
//  tile-maker
//
//  Created by Todd Vanderlin on 11/16/17.
//
//

#pragma once
#include "ofMain.h"
#include "Asset.h"
#include "Cursor.h"

#define HANDLE_SIZE 20
#define DRAW_HANDLE_SIZE 10

namespace TileMaker {
	class Canvas;
	
	// --------------------------------
	class Shape {
	private:
	public:
		Canvas * canvasRef;
		Asset * ref;
		bool isOver, isPressed;
		bool canRotate;
		ofPoint prevMouse, mouse;
		ofPoint downPos, downMouse;
		ofPoint downRotatePos;
		ofPoint worldOffset;
		int cornerIndex;
		int overCornerIndex;
		float angle, downAngle;
		int orderIndex;
		ofRectangle downRect;
		
		// location and size of the shape
		float x, y, width, height;
		
		ofPolyline corners;
		
		enum {
			CORNER_TL,
			CORNER_TR,
			CORNER_BR,
			CORNER_BL
		};
		
		Shape(Asset * _ref, float _x=0, float _y=0);
		ofVec2f getPosition();
		ofRectangle getRectangle();
		float getAngleFromPoints(ofVec2f a, ofVec2f b);
		float getWidth() {return width;}
		float getHeight() {return height;}
		float getRotation() {return angle;}
		void updateCorners();
		void updateCursor(int index);
		
		void setRotation(float a);
		void setPosition(float _x, float _y);
		void setSize(float _w, float _h);
		void move(float _x, float _y);
		void rotate(float _angle);
		
		array<ofRectangle, 4> getCornerRects();
		bool inside(float _x, float _y);
		int insideCorner(float _x, float _y);
		
		bool mouseMoved(int _x, int _y );
		void mouseDragged(int _x, int _y, int button);
		bool mousePressed(int _x, int _y, int button);
		void mouseReleased(int _x, int _y, int button);
		void draw();
		void drawRaw();
	};
}












