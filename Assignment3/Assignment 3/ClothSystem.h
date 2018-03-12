#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

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

class ClothSystem: public ParticleSystem
{
///ADD MORE FUNCTION AND FIELDS HERE
public:
	ClothSystem(int m, int n, float s);
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();
    
    void toggleWireframe();

private:
    int height;
    int width;
    float spacing;
    vector<vector<vector<int>>> springIndices;
};


#endif
