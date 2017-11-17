//
//  Canvas.h
//  tile-maker
//
//  Created by Todd Vanderlin on 11/16/17.
//
//

#pragma once
#include "ofMain.h"
#include "Shape.h"
#include "Cursor.h"

namespace TileMaker {
	class Shape;
	class Canvas : public ofRectangle {
	public:
		bool enablePan;
		ofPoint downPos;
		float scale;
		float maxZoom, minZoom;
		vector <ofPoint> temp;
		vector <Shape*> shapes;
		int shapeCount;
		Canvas() {
			shapeCount = 0;
			enablePan = false;
			scale = 1;
			minZoom = 0.2; maxZoom = 50;
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
			int n = 100;
			temp.clear();
			for(int i=0; i<n; i++) {
				temp.push_back(ofPoint(ofRandom(0, w), ofRandom(0, h)));
			}
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
			if (key == 'r') {
				for(auto shape : shapes) {
					shape->canRotate = true;
				}
			}
			if (key == '`') {
				for(auto shape : shapes) {
					shape->setRotation(0);
				}
			}
			if (key == 'e') {
				exportSVG();
			}
		}
		void keyReleased(int key) {
			enablePan = false;
			for(auto shape : shapes) {
				shape->canRotate = false;
			}
		}
		ofPoint getScaledMouse() {
			float mx = ((ofGetMouseX() - x) / scale);
			float my = ((ofGetMouseY() - y) / scale);
			return ofPoint(mx, my);
		}
		ofPoint getPreviousScaledMouse() {
			float mx = ((ofGetPreviousMouseX() - x) / scale);
			float my = ((ofGetPreviousMouseY() - y) / scale);
			return ofPoint(mx, my);
		}
		ofPoint toCanvas(ofPoint pt) {
			return toCanvas(pt.x, pt.y);
		}
		ofPoint toCanvas(float _x, float _y) {
			float mx = (_x - x) / scale;
			float my = (_y - y) / scale;
			return ofPoint(mx, my);
		}
		
		void mouseMoved(int _x, int _y ) {
			if(enablePan) {
			} else {
				ofPoint mouse = getScaledMouse();
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
			if(enablePan) {
				setPosition(_x - downPos.x, _y - downPos.y);
			} else {
				ofPoint mouse = getScaledMouse();
				for(auto shape : shapes) {
					shape->mouse = getScaledMouse();
					shape->prevMouse = getPreviousScaledMouse();
					shape->worldOffset.set(x, y);
					shape->mouseDragged(mouse.x, mouse.y, button);
				}
			}
			
		}
		void mousePressed(int _x, int _y, int button) {
			if(enablePan) {
				downPos = ofPoint(_x, _y) - getPosition();
			} else {
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
			for(auto shape : shapes) {
				shape->mouse = getScaledMouse();
				shape->prevMouse = getPreviousScaledMouse();
				shape->mouseReleased(_x, _y, button);
			}
		}
		float getRatio() {
			return height / width;
		}
		void setZoom(float z) {
			scale = ofClamp(z, minZoom, maxZoom);
		}
		void setZoom(ofParameter<float> z) {
			minZoom = z.getMin(); maxZoom = z.getMax();
			scale = ofClamp(z, minZoom, maxZoom);
		}
		void begin() {
			ofPoint pos = getCanvasPosition();
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
		
		ofPoint getCanvasPosition() {
			float cx = ((ofGetWidth()-width*scale) * 0.5) - x;
			float cy = ((ofGetHeight()-width*scale) * 0.5) - y;
			cx = x;
			cy = y;
			return ofPoint(cx, cy);
		}
		void drawMiniMap() {
			float sr = getRatio();
			float mapW = 100;
			float mapH = mapW * sr;
			
			float sx = mapW / width;
			float sy = mapH / height;
			
			//			float sy = sh / (float)ofGetHeight();
			ofPoint pos = getCanvasPosition();
			//			pos.x *= sx * scale;
			//			pos.y *= sy * scale;
			pos.x *= sx;
			pos.y *= sy;
			
			float canvasW = (width * sx) / scale;
			float canvasH = (height * sy) / scale;
			ofPushMatrix();
			ofTranslate(ofGetWidth()-(mapW+10), ofGetHeight()-(mapH+10));
			ofSetColor(255, 200);
			ofFill();
			ofDrawRectangle(0, 0, mapW, mapH);
			
			drawAtScale(sx / 3, sy / 3);
			//ofSetColor(0, 200);
			//ofFill();
			//ofDrawRectangle(pos, canvasW, canvasH);
			
			ofPopMatrix();
		}
		void drawAtScale(float sx, float sy) {
			ofPoint pos = getCanvasPosition();
			pos.x *= sx;
			pos.y *= sy;
			ofPushMatrix();
			ofTranslate(pos);
			ofScale(sx, sy);
			ofNoFill();
			ofSetColor(255);
			ofDrawRectangle(0, 0, width, height);
			for(auto p : temp) {
				ofDrawCircle(p, 10);
			}
			ofPopMatrix();
			
		}
		void update() {
			for(auto shape : shapes) {
				shape->mouse = getScaledMouse();
				shape->prevMouse = getPreviousScaledMouse();
				shape->worldOffset.set(x, y);
			}
		}
		void draw() {
			ofPoint pos = getCanvasPosition();
			ofPushMatrix();
			ofTranslate(pos);
			ofScale(scale, scale);
			ofNoFill();
			ofSetColor(255);
			ofDrawRectangle(0, 0, width, height);
			for(auto p : temp) {
				ofDrawCircle(p, 10);
			}
			ofPopMatrix();
			
		}
	};
}
