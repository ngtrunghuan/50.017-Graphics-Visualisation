#include "TimeStepper.hpp"

///TODO: implement Explicit Euler time integrator here
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{
    vector<Vector3f> newStates;
    vector<Vector3f> currStates = particleSystem->getState();
    vector<Vector3f> firstF = particleSystem->evalF(currStates);
    for (unsigned i = 0; i < currStates.size(); ++i)
    {
        newStates.push_back(currStates[i] + stepSize * firstF[i]);
    }
    particleSystem->setState(newStates);
}

///TODO: implement Trapzoidal rule here
void Trapzoidal::takeStep(ParticleSystem* particleSystem, float stepSize)
{
    vector<Vector3f> newStates;
    vector<Vector3f> currStates = particleSystem->getState();
    vector<Vector3f> firstF = particleSystem->evalF(currStates);
    
    vector<Vector3f> f0States;
    for (unsigned i = 0; i < currStates.size(); ++i)
    {
        f0States.push_back(currStates[i] + stepSize * firstF[i]);
    }
    vector<Vector3f> secondF = particleSystem->evalF(f0States);
    
    for (unsigned i = 0; i < currStates.size(); ++i)
    {
        newStates.push_back(currStates[i] + (stepSize / 2) * (firstF[i] + secondF[i]));
    }
    particleSystem->setState(newStates);
}

