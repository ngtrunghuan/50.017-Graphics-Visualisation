#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include "vecmath.h"
#include <vector>


#ifdef __APPLE__
#include <GLUT/glut.h>
#elif _WIN32
#include <windows.h>
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif


#include "particleSystem.h"

class PendulumSystem: public ParticleSystem
{
public:
	PendulumSystem(int numParticles);
	
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();
	
    void toggleWireframe();
};

#endif
