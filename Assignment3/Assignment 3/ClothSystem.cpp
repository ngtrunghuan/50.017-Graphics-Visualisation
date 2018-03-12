#include "ClothSystem.h"
#include <vector>
#include <math.h>

//TODO: Initialize here
ClothSystem::ClothSystem(int m, int n, float s)
{
	m_numParticles = m*n;
	height = m;
	width = n;
	spacing = s;
    // loop vertically
	for (int i = 0; i < m; i++) {
        // loop horizontally
		for (int j = 0; j < n; j++) {
			// for this system, we care about the position and the velocity
			m_vVecState.push_back(Vector3f(1 + spacing * j, -spacing * i, -2));
			m_vVecState.push_back(Vector3f(0, 0, 0));

            // set up all related points with struct, shear and flex springs
			vector<vector<int>> springIndex;
			vector<int> structIndex;
			vector<int> shearIndex;
			vector<int> flexIndex;

			/*
			0		1		2		...		n-1
			n		n+1		n+2		...		2n-1
			2n		2n+1	2n+2	...		3n-1
			:
			:
			(m-1)n	(m-1)n+1		...		mn-1
			*/

			bool leftSpring = j - 1 >= 0;
			bool rightSpring = j + 1 < n;
			bool upSpring = i - 1 >= 0;
			bool downSpring = i + 1 < m;

			//structural springs
			if (leftSpring) structIndex.push_back(i*n + j - 1);
			if (rightSpring) structIndex.push_back(i*n + j + 1);
			if (upSpring) structIndex.push_back((i - 1)*n + j);
			if (downSpring) structIndex.push_back((i + 1)*n + j);

			//shear springs
			if (leftSpring & upSpring) shearIndex.push_back((i - 1)*n + j - 1);
			if (rightSpring & upSpring) shearIndex.push_back((i - 1)*n + j + 1);
			if (leftSpring & downSpring) shearIndex.push_back((i + 1)*n + j - 1);
			if (rightSpring & downSpring) shearIndex.push_back((i + 1)*n + j + 1);

			//flex springs
			if (j - 2 >= 0) flexIndex.push_back(i*n + j - 2);
			if (j + 2 < n) flexIndex.push_back(i*n + j + 2);
			if (i - 2 >= 0) flexIndex.push_back((i - 2)*n + j);
			if (i + 2 < m) flexIndex.push_back((i + 2)*n + j);

			springIndex.push_back(structIndex);
			springIndex.push_back(shearIndex);
			springIndex.push_back(flexIndex);
			springIndices.push_back(springIndex);
		}
	}
}


// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	float mass = 0.5f;
	float gravity = 10.f;
	float viscocity = 2.f;

	for (unsigned i = 0; i < state.size(); i += 2) {
		if (i == 0 || i==(width-1)*2)
        {
			f.push_back(state[i + 1]);
			Vector3f d = state[i] - Vector3f(state[i].x(), 0, 2);
			Vector3f springF = -1 * d.abs()*d / d.abs();
			f.push_back(springF);
		}
		else
        {
			f.push_back(state[i + 1]);
			Vector3f netForce(0.f, 0.f, 0.f);

			netForce += Vector3f(0, -1, 0) * gravity * mass; //gravity
			netForce += state[i + 1] * -viscocity; //drag

			vector<vector<int>> springIndex = springIndices[i / 2];
			/*
			0		1		2		...		n-1
			n		n+1		n+2		...		2n-1
			2n		2n+1	2n+2	...		3n-1
			:
			:
			(m-1)n	(m-1)n+1		...		mn-1
			*/

			vector<int> structIndex = springIndex[0];
			vector<int> shearIndex = springIndex[1];
			vector<int> flexIndex = springIndex[2];
			for (int n = 0; n < structIndex.size(); n++) {
				float springLength = spacing;
				float springStiff = 200.f;
				Vector3f distance = state[i] - state[2*structIndex[n]];
				Vector3f springForce = -springStiff * (distance.abs() - springLength)*distance / distance.abs();
				netForce += springForce;
			}
			for (int n = 0; n < shearIndex.size(); n++) {
				float springLength = spacing/sqrt(2);
				float springStiff = 40.f;
				Vector3f distance = state[i] - state[2 * shearIndex[n]];
				Vector3f springForce = -springStiff * (distance.abs() - springLength)*distance / distance.abs();
				netForce += springForce;
			}
			for (int n = 0; n < flexIndex.size(); n++) {
				float springLength = spacing*2;
				float springStiff = 200.f;
				Vector3f distance = state[i] - state[2 * flexIndex[n]];
				Vector3f springForce = -springStiff * (distance.abs() - springLength)*distance / distance.abs();
				netForce += springForce;
			}

			f.push_back(netForce);
		}
	}

	return f;
}

///TODO: render the system (ie draw the particles)
void ClothSystem::draw()
{
	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos = m_vVecState[i * 2];//  position of particle i. YOUR CODE HERE
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2]);
		//glutSolidSphere(0.075f, 10.0f, 10.0f);
		glPopMatrix();
	}
    if (!isWireframe)
    {
        for (int i = 0; i < height-1; i++)
        {
            for (int j = 0; j < width-1; j++)
            {

                Vector3f v11 = m_vVecState[2 * (i * width + j)];
                Vector3f v12 = m_vVecState[2 * ((i + 1) * width + j)];
                Vector3f v13 = m_vVecState[2 * ((i + 1) * width + j + 1 )];
                Vector3f n1 = Vector3f::cross(v12 - v11, v13 - v11).normalized();
                Vector3f n2 = n1*-1;
                glBegin(GL_TRIANGLES);
                glNormal3f(n1[0], n1[1], n1[2]);
                glVertex3f(v11[0],v11[1],v11[2]);
                glVertex3f(v12[0], v12[1], v12[2]);
                glVertex3f(v13[0], v13[1], v13[2]);
                glEnd();
                glBegin(GL_TRIANGLES);
                glNormal3f(n2[0], n2[1], n2[2]);
                glVertex3f(v13[0], v13[1], v13[2]);
                glVertex3f(v12[0], v12[1], v12[2]);
                glVertex3f(v11[0], v11[1], v11[2]);
                glEnd();

                Vector3f v21 = m_vVecState[2 * (i * width + j)];
                Vector3f v22 = m_vVecState[2 * ((i + 1) * width + j + 1)];
                Vector3f v23 = m_vVecState[2 * (i * width + j + 1)];
                glBegin(GL_TRIANGLES);
                glNormal3f(n1[0], n1[1], n1[2]);
                glVertex3f(v21[0], v21[1], v21[2]);
                glVertex3f(v22[0], v22[1], v22[2]);
                glVertex3f(v23[0], v23[1], v23[2]);
                glEnd();
                glBegin(GL_TRIANGLES);
                glNormal3f(n2[0], n2[1], n2[2]);
                glVertex3f(v23[0], v23[1], v23[2]);
                glVertex3f(v22[0], v22[1], v22[2]);
                glVertex3f(v21[0], v21[1], v21[2]);
                glEnd();


            }
        }
    }
    else
    {
        // draw vertical wireframe lines
        for (int i = 0; i < height - 1; i++)
        {
            for (int j = 0; j < width; j++)
            {
                Vector3f point1 = m_vVecState[2 * (i * width + j)];
                Vector3f point2 = m_vVecState[2 * ((i + 1) * width + j)];
                glLineWidth(3.0f);
                glBegin(GL_LINES);
                glVertex3f(point1.x(), point1.y(), point1.z());
                glVertex3f(point2.x(), point2.y(), point2.z());
                glEnd();
            }
        }
        
        // draw horizontal wireframe lines
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width - 1; j++)
            {
                Vector3f point1 = m_vVecState[2 * (i * width + j)];
                Vector3f point2 = m_vVecState[2 * (i * width + j + 1)];
                glLineWidth(3.0f);
                glBegin(GL_LINES);
                glVertex3f(point1.x(), point1.y(), point1.z());
                glVertex3f(point2.x(), point2.y(), point2.z());
                glEnd();
            }
        }
    }
}

void ClothSystem::toggleWireframe()
{
    isWireframe = !isWireframe;
}

