
#include "pendulumSystem.h"

PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles)
{
	m_numParticles = numParticles;
	
	// fill in code for initializing the state based on the number of particles
	for (int i = 0; i < m_numParticles; i++) {
		
		// for this system, we care about the position and the velocity
		//pos
		m_vVecState.push_back(Vector3f(1+0.5*i, -0.5*i, 0));
		//vel
		m_vVecState.push_back(Vector3f(0, 0, 0));

	}
}


// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	//float mass = 1.f;
	float gravity = 10.f;
	float viscocity = 2.f;
	float springStiff = 200.f;
	float springLength = 0.8f;

	// YOUR CODE HERE
	for (unsigned i = 0; i < state.size(); i += 2) {
		if (i < 2)
        {
			f.push_back(Vector3f());
			f.push_back(Vector3f());
		}
		else
        {
			f.push_back(state[i + 1]);
			Vector3f netForce(0.f, 0.f, 0.f);

			netForce += Vector3f(0, -1, 0)*gravity;
			netForce += state[i + 1] * -viscocity;

			Vector3f distance = state[i] - state[i - 2];
			Vector3f springForce = -springStiff * (distance.abs() - springLength)*distance / distance.abs();
			state[i - 1] -= springForce; //Newton's 3rd law
			netForce += springForce;

			f.push_back(netForce);
		}
	}

	return f;
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos = m_vVecState[i * 2];//  position of particle i. YOUR CODE HERE
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.075f,10.0f,10.0f);
		glPopMatrix();
	}
}

void PendulumSystem::toggleWireframe(){}
