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
	
	class Canvas : public ofRectangle {
	public:
		ofParameter<bool> editArtboard;
		Artboard artboard;
		ofPolyline corners;
		bool pressedInsideArtboard;
		bool enablePan;
		ofPoint downPos;
		ofRectangle worldRect;
		float maxZoom, minZoom;
		vector <Shape*> shapes;
		int shapeCount;
		Canvas() {
			pressedInsideArtboard = false;
			editArtboard = false;
			shapeCount = 0;
			enablePan = false;
			AppSettings::worldScale = 1;
			minZoom = 0.2; maxZoom = 50;
			corners.resize(4);
		}
		Shape * addShape(Shape * shape) {
			shape->canvasRef = this;
			shape->orderIndex = shapeCount;
			shapes.push_back(shape);
			shapeCount ++;
			return shapes.back();
		}
		void setSize(int w, int h) {
			ofRectangle::setSize(w, h);
			artboard.setSize(w, h);
			updateCorners();
		}
		void updateCorners() {
			corners[CORNER_TL] = getTopLeft();
			corners[CORNER_TR] = getTopRight();
			corners[CORNER_BR] = getBottomRight();
			corners[CORNER_BL] = getBottomLeft();
		}
		bool isPanning() {
			return enablePan;
		}
		void keyPressed(int key) {
			if (key == ' ') {
				enablePan = true;
				if (enablePan) {
					Cursor::setMode(ofGetMousePressed() ? Cursor::CURSOR_GRAB : Cursor::CURSOR_HAND);
				}
			}
			if (key == OF_KEY_SHIFT) {
				for(auto shape : shapes) {
					shape->canRotate = true;
				}
			}
			if (key == 'x') {
				fitToScreen();
			}
			if (key == '`') {
				for(auto shape : shapes) {
					shape->setRotation(0);
				}
			}
			if (key == 'e') {
				exportSVG();
			}
			if (key == 'a') {
				editArtboard = !editArtboard;
			}
		}
		void keyReleased(int key) {
			enablePan = false;
			for(auto shape : shapes) {
				shape->canRotate = false;
			}
		}
		ofPoint getScaledMouse() {
			float scale = AppSettings::worldScale;
			float mx = ((ofGetMouseX() - x) / scale);
			float my = ((ofGetMouseY() - y) / scale);
			return ofPoint(mx, my);
		}
		ofPoint getPreviousScaledMouse() {
			float scale = AppSettings::worldScale;
			float mx = ((ofGetPreviousMouseX() - x) / scale);
			float my = ((ofGetPreviousMouseY() - y) / scale);
			return ofPoint(mx, my);
		}
		
		ofPoint toCanvas(ofPoint pt) {
			return toCanvas(pt.x, pt.y);
		}
		ofPoint toCanvas(float _x, float _y) {
			float scale = AppSettings::worldScale;
			float mx = (_x - x) / scale;
			float my = (_y - y) / scale;
			return ofPoint(mx, my);
		}
		
		void mouseMoved(int _x, int _y ) {
			ofPoint mouse = getScaledMouse();

			if(editArtboard) {
				artboard.mouseMoved(mouse.x, mouse.y);
			}
			else if(enablePan) {
			
			}
			else {
				bool isInsideShape = false;
				for(auto shape : shapes) {
					shape->mouse = getScaledMouse();
					shape->prevMouse = getPreviousScaledMouse();
					shape->isOver = false;
				}
				
				for(int i=shapes.size()-1; i>=0; i--) {
					Shape * shape = shapes[i];
					if(shape->mouseMoved(mouse.x, mouse.y)) {
						break;
					}
				}
			}
		}
		void mouseDragged(int _x, int _y, int button) {
			float scale = AppSettings::worldScale;

			ofPoint mouse = getScaledMouse();
			ofPoint prevMouse = getPreviousScaledMouse();
			bool insideArtboard = artboard.inside(mouse.x, mouse.y);
			
			if (!enablePan && editArtboard) {
				artboard.mouseDragged(mouse.x, mouse.y, prevMouse.x, prevMouse.y, button);
			}
			
			else if(enablePan) {
				x += (mouse.x - prevMouse.x) * scale;
				y += (mouse.y - prevMouse.y) * scale;
			}
			
			else {
				for(auto shape : shapes) {
					shape->mouse = getScaledMouse();
					shape->prevMouse = getPreviousScaledMouse();
					shape->worldOffset.set(x, y);
					shape->mouseDragged(mouse.x, mouse.y, button);
				}
			}
		}
		void mousePressed(int _x, int _y, int button) {
			ofPoint mouse = getScaledMouse();
			if(editArtboard) {
				artboard.mousePressed(mouse.x, mouse.y, button);
			}
			else if(enablePan) {
				downPos = ofPoint(_x, _y) - getPosition();
			}
			else {
				ofPoint mouse = getScaledMouse();
				for(auto shape : shapes) {
					shape->mouse = getScaledMouse();
					shape->prevMouse = getPreviousScaledMouse();
					shape->isPressed = false;
				}
				
				for(int i=shapes.size()-1; i>=0; i--) {
					Shape * shape = shapes[i];
					if(shape->mousePressed(mouse.x, mouse.y, button)) {
						break;
					}
				}
			}
		}
		
		void mouseReleased(int _x, int _y, int button) {
			ofPoint mouse = getScaledMouse();
			artboard.mouseReleased(mouse.x, mouse.y, button);
			for(auto shape : shapes) {
				shape->mouse = getScaledMouse();
				shape->prevMouse = getPreviousScaledMouse();
				shape->mouseReleased(_x, _y, button);
			}
		}
		
		// set the zoom / scale of the canvas
		void setZoom(float z) {
			AppSettings::worldScale = ofClamp(z, minZoom, maxZoom);
		}
		void setZoom(ofParameter<float> z) {
			minZoom = z.getMin(); maxZoom = z.getMax();
			AppSettings::worldScale = ofClamp(z, minZoom, maxZoom);
		}
		
		void fitToScreen() {
			float g = 10;
			float m = MIN(worldRect.width - g, worldRect.height - g);
			float s = m / artboard.width;
			float tileSize = artboard.width * s;
			ofLogNotice() << "scale " << s << "\n" << "size " << tileSize << "x" << tileSize;
			setZoom(s);
			x = worldRect.x + (worldRect.width-tileSize)/2;
			y = worldRect.y + (worldRect.height-tileSize)/2;
		}
		
		// push and pop canvas
		void begin() {
			float scale = AppSettings::worldScale;
			
			ofSetColor(0);
			ofFill();
			ofDrawCircle(x, y, 2);
			
			ofPoint pos(x, y);
			ofPushMatrix();
			ofTranslate(pos);
			ofScale(scale, scale);
		}
		void end() {
			ofPopMatrix();
		}
		
		void exportSVG() {
			ofBeginSaveScreenAsSVG("test.svg");
			for(auto shape : shapes) {
				shape->drawRaw();
			}
			ofEndSaveScreenAsSVG();
		}
		ofRectangle getScaledRect() {
			float scale = AppSettings::worldScale;
			return ofRectangle(x, y, width * scale, height * scale);
		}
		
		void drawMiniMap() {
//			return;
//			float sr = getRatio();
//			float mapW = 100;
//			float mapH = mapW * sr;
//
//			float sx = mapW / width;
//			float sy = mapH / height;
//
//			//			float sy = sh / (float)ofGetHeight();
//			ofPoint pos(x, y);
//			//			pos.x *= sx * scale;
//			//			pos.y *= sy * scale;
//			pos.x *= sx;
//			pos.y *= sy;
//
//			float canvasW = (width * sx) / scale;
//			float canvasH = (height * sy) / scale;
//			ofPushMatrix();
//			ofTranslate(ofGetWidth()-(mapW+10), ofGetHeight()-(mapH+10));
//			ofSetColor(255, 200);
//			ofFill();
//			ofDrawRectangle(0, 0, mapW, mapH);
//
//			drawAtScale(sx / 3, sy / 3);
//
//			ofSetColor(0, 200);
//			ofFill();
//			ofDrawRectangle(pos, canvasW, canvasH);
//
//			ofPopMatrix();
		}
		void drawAtScale(float sx, float sy) {
			ofPoint pos(x, y);
			pos.x *= sx;
			pos.y *= sy;
			ofPushMatrix();
			ofTranslate(pos);
			ofScale(sx, sy);

			ofPopMatrix();
		}
		
		void update(ofRectangle bounds=ofGetCurrentViewport()) {
			worldRect = bounds;
			updateCorners();
			for(auto shape : shapes) {
				shape->mouse = getScaledMouse();
				shape->prevMouse = getPreviousScaledMouse();
				shape->worldOffset.set(x, y);
			}
		}
		
		void drawUI() {
			float scale = AppSettings::worldScale;
			ofRectangle uiRect(0, worldRect.y, worldRect.x, worldRect.height-0);
			ofFill();
			ofSetColor(COLOR_UI_BACKGROUND);
			ofDrawRectangle(uiRect);
			ofSetColor(COLOR_UI_BORDER);
			ofDrawLine(uiRect.getRight(), uiRect.getTop(), uiRect.getRight(), uiRect.getBottom());
			
			float scalePct = scale * 100;
			ofDrawBitmapStringHighlight("Scale X " + ofToString(scalePct, 1)+"%", 20, ofGetHeight()-15);
		}
		
		void drawBackground() {
			
			ofFill();
			ofSetColor(95);
			ofDrawRectangle(worldRect);
			/*
			ofSetColor(195);
			if(editArtboard) {
				if(getScaledRect().inside(ofGetMouse())) {
					ofSetColor(150);
				}
			}
			ofFill();
			ofDrawRectangle(x, y, width * scale, height * scale);
			*/
			begin();
			artboard.editable = editArtboard;
			artboard.draw();
			end();
			
		}
	};
}
