#include "ofMain.h"
#include "ofApp.h"

int main() {
	
	ofSetupOpenGL(1800, 800, OF_WINDOW);
	
	if(!ofGLCheckExtension("GL_ARB_geometry_shader4") && !ofGLCheckExtension("GL_EXT_geometry_shader4") && !ofIsGLProgrammableRenderer()){
		ofLogFatalError() << "geometry shaders not supported on this graphics card";
		return 1;
	}

	ofRunApp(new ofApp());
}
