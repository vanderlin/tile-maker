//
//  Thumbnail.h
//  tile-maker
//
//  Created by Todd Vanderlin on 11/16/17.
//
//

#pragma once
#include "ofMain.h"
#include "Asset.h"


namespace TileMaker {
	
	class Thumbnail : public ofRectangle {
		public:
		
		Asset * ref;
		ofPoint dragPos;
		bool isOver, isPressed, didDrop;
		Thumbnail(Asset * _ref);
		
		void mouseMoved(int _x, int _y );
		void mouseDragged(int _x, int _y, int button);
		void mousePressed(int _x, int _y, int button);
		bool mouseReleased(int _x, int _y, int button);
		void draw();
	};
}
