//
//  Asset.h
//  tile-maker
//
//  Created by Todd Vanderlin on 11/16/17.
//
//

#pragma once
#include "ofMain.h"
#include "AppGlobals.h"
#include "Utils.h"
namespace TileMaker {
	
	class Asset {
	private:
		float width, height, ratio;
		string filename;
	public:
		
		static vector <Asset*> items;
		static Asset * at(int i);
		static vector <Asset*> all();
		static int size();
		static Asset* getWithFilename(string filename);
		static bool load(string dirName);

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
		float getRatioVertical() {
			return width / height;
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
