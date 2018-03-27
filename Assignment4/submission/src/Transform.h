#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "vecmath.h"
#include "Object3D.h"
///TODO implement this class
///So that the intersect function first transforms the ray
///Add more fields as necessary
class Transform: public Object3D
{
public: 
    Transform(){}
    Transform( const Matrix4f& m, Object3D* obj ):o(obj)
    {
        this->matrix = m;
        this->o = obj;
    }
    ~Transform(){}
    virtual bool intersect( const Ray& r , Hit& h , float tmin)
    {
        Vector4f ro4 = Vector4f(r.getOrigin(), 1.0f);
        Vector4f rd4 = Vector4f(r.getDirection(), 0.0f);
        Vector4f ro4Transform = this->matrix.inverse() * ro4;
        Vector4f rd4Transform = this->matrix.inverse() * rd4;
        Vector3f ro3Transform = ro4Transform.xyz();
        Vector3f rd3Transform = rd4Transform.xyz();
        Ray rayNew = Ray(ro3Transform, rd3Transform);
        if (this->o->intersect(rayNew, h, tmin)){
            Vector4f normalTrans4 = Vector4f(h.getNormal(), 0.0f);
            Vector4f normal4 = (this->matrix.inverse().transposed() * normalTrans4).normalized();
            Vector3f normal = normal4.xyz();
            h.set(h.getT(), h.getMaterial(), normal);
            return true;
        }
        return false;
    }

protected:
    Object3D* o; //un-transformed object
    Matrix4f matrix;
};

#endif //TRANSFORM_H
