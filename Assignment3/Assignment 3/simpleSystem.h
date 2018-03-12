#ifndef SIMPLESYSTEM_H
#define SIMPLESYSTEM_H


#ifdef __APPLE__
#include <GLUT/glut.h>
#elif _WIN32
#include <windows.h>
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif


#include "vecmath.h"
#include <vector>

#include "particleSystem.h"

using namespace std;

class SimpleSystem: public ParticleSystem
{
public:
	SimpleSystem();
	
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();
	
    void toggleWireframe();
};

#endif
