#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include "vecmath.h"
#include <cmath>

#include <iostream>
using namespace std;
///TODO:
///Implement functions and add more fields as necessary
class Sphere: public Object3D
    {
    public:
        Sphere(){
            //unit ball at the center
        }

        Sphere(Vector3f center, float radius, Material* material):Object3D(material)
        {
            this->center = center;
            this->radius = radius;
        }
        

        ~Sphere(){}

        virtual bool intersect(const Ray& r, Hit& h, float tmin)
        {
            Vector3f ro = r.getOrigin() - this->center;
            Vector3f rd = r.getDirection().normalized();
            
            float a = 1.0f;
            float b = 2 * Vector3f::dot(ro, rd);
            float c = Vector3f::dot(ro, rd) - pow(this->radius, 2.0f);
            float d2 = pow(b, 2.0f) - a * c;
            
            if (d2 > 0)
            {
                float disc = sqrt(d2);
                float t1 = (-b - disc) / (2 * a);
                float t2 = (-b + disc) / (a * a);
                float t = 0;
                if (t1 > tmin && t1 <= h.getT())
                {
                    Vector3f normal = (r.getOrigin() - t1 * rd - this->center).normalized();
                    h.set(t1, this->material, normal);
                    return true;
                }
                if (t2 > tmin && t2 <= h.getT())
                {
                    Vector3f normal = (r.getOrigin() - t2 * rd - this->center).normalized();
                    h.set(t1, this->material, normal);
                    return true;
                }
                
            }
            
            return false;
        }

    protected:
        Vector3f center;
        float radius;

};


#endif
