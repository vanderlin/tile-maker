//
//  Asset.cpp
//  tile-maker
//
//  Created by Todd Vanderlin on 11/16/17.
//
//

#include "Asset.h"
using namespace TileMaker;
vector <Asset*> Asset::items;

Asset * Asset::at(int i) {
	return Asset::items[i];
}
int Asset::size() {
	return Asset::items.size();
}
vector <Asset*> Asset::all() {
	return Asset::items;
}
Asset * Asset::getWithFilename(string filename) {
	for(auto asset : Asset::items) {
		if (asset->getFilename() == filename) {
			return asset;
		}
	}
	return nullptr;
}
bool Asset::load(string dirName) {
	ofDirectory dir;
	dir.listDir(dirName);
	Asset::items.clear();
	for (int i=0; i<dir.size(); i++) {
		Asset::items.push_back(new Asset(dir.getPath(i)));
	}
	return Asset::items.size();
}
