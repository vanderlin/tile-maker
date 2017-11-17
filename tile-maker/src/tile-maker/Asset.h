//
//  Asset.h
//  tile-maker
//
//  Created by Todd Vanderlin on 11/16/17.
//
//

#pragma once
#include "ofMain.h"
namespace TileMaker {
	
	class Asset {
	private:
		float width, height, ratio;
		string filename;
	public:
		
		ofImage image;
		Asset(string _filename) {
			filename = _filename;
			image.load(_filename);
			width = image.getWidth();
			height = image.getHeight();
			ratio = height / width;
		}
		float getRatio() {
			return ratio;
		}
		float getWidth() {
			return width;
		}
		float getHeight() {
			return height;
		}
		string getFilename() {
			return filename;
		}
	};
	
}
