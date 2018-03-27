#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include "vecmath.h"
#include <cmath>
using namespace std;
///TODO: Implement Plane representing an infinite plane
///choose your representation , add more fields and fill in the functions
class Plane: public Object3D
{
public:
	Plane() {}
	Plane( const Vector3f& normal , float d , Material* m):Object3D(m) {
        this->normal = normal.normalized();
        this->d = d;
	}
	~Plane() {}
	virtual bool intersect( const Ray& r , Hit& h , float tmin) {
        Vector3f ro = r.getOrigin();
        Vector3f rd = r.getDirection().normalized();
        float nDotRd = Vector3f::dot(this->normal, rd);
        
        if (nDotRd == 0)
        {
            return false;
        }
        
        float nDotRo = Vector3f::dot(this->normal, ro);
        float t = -1.0 * (-1.0 * this->d + nDotRo) / (nDotRd);
        
        if (t > tmin && t < h.getT())
        {
            h.set(t, this->material, this->normal);
            return true;
        }
        else
        {
            return false;
        }
	}
	
protected:
    Vector3f normal;
    float d;
};
#endif //PLANE_H


