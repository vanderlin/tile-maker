//
//  Canvas.h
//  tile-maker
//
//  Created by Todd Vanderlin on 11/16/17.
//
//

#pragma once
#include "ofMain.h"
#include "Asset.h"
#include "Shape.h"
#include "Cursor.h"
#include "Artboard.h"

namespace TileMaker {
	
	class Shape;	
	
	class Canvas {

	public:
		
		ofParameter<bool> editArtboard;
		float x, y;
		bool commandIsPressed;
		bool shiftIsPressed;
		bool pressedShapeInSelection;
		bool pressedInsideShapes;
		ofRectangle selectionRect;
		vector<Shape*> selectionGroup;
		Shape * selectedShape;
		
		Artboard artboard;
		bool pressedInsideArtboard;

		bool enablePan;
		ofRectangle worldRect;
		float maxZoom, minZoom;
		vector <Shape*> shapes;
		int shapeCount;
		
		Canvas();
		Shape * addShape(Shape * shape);
		void setSize(int w, int h);
		bool isPanning();
		
		void keyPressed(int key);
		void keyReleased(int key);
		
		static ofPoint getScaledMouse();
		static ofPoint getPreviousScaledMouse();
		static ofPoint getDiffScaledMouse();
		
		ofPoint toCanvas(ofPoint pt);
		ofPoint toCanvas(float _x, float _y);
		
		void save(string filename="canvas.xml");
		void load(string filename="canvas.xml");
		
		void mouseMoved(int _x, int _y );
		void mouseDragged(int _x, int _y, int button);
		void mousePressed(int _x, int _y, int button);
		void mouseReleased(int _x, int _y, int button);
		
		// set the zoom / scale of the canvas
		void setZoom(float z);
		void setZoom(ofParameter<float> z);
		void move(float _x, float _y);
		void fitToScreen();
		
		// push and pop canvas
		void begin();
		void end();
		
		void exportSVG();
		void drawMiniMap();
		void drawAtScale(float sx, float sy);
		void update(ofRectangle bounds=ofGetCurrentViewport());
		void drawUI();
		void drawBackground();
	};
}
