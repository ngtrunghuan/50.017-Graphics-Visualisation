#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include "vecmath.h"
#include <cmath>
#include <iostream>

using namespace std;
///TODO: implement this class.
///Add more fields as necessary,
///but do not remove hasTex, normals or texCoords
///they are filled in by other components
class Triangle: public Object3D
{
protected:
    Vector3f a;
    Vector3f b;
    Vector3f c;
    
public:
    bool hasTexture;
    Vector3f normals[3];
    Vector2f textureCoordinates[3];
    
	Triangle();
        ///@param a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m):Object3D(m){
        this->a = a;
        this->b = b;
        this->c = c;
        this->hasTexture = false;
	}

	virtual bool intersect( const Ray& r,  Hit& h , float tmin)
    {
        Vector3f ro = r.getOrigin();
        Vector3f rd = r.getDirection();

        Matrix3f A( this->a.x()-this->b.x() , this->a.x()-this->c.x() , rd.x() ,
                   this->a.y()-this->b.y() , this->a.y()-this->c.y() , rd.y() ,
                   this->a.z()-this->b.z() , this->a.z()-this->c.z() , rd.z()  );

        Matrix3f BetaM( this->a.x()-ro.x() , this->a.x()-this->c.x() , rd.x() ,
                       this->a.y()-ro.y() , this->a.y()-this->c.y() , rd.y() ,
                       this->a.z()-ro.z() , this->a.z()-this->c.z() , rd.z()  );

        float beta = BetaM.determinant() / A.determinant();

        Matrix3f GammaM( this->a.x()-this->b.x() , this->a.x()-ro.x() , rd.x() ,
                        this->a.y()-this->b.y() , this->a.y()-ro.y() , rd.y() ,
                        this->a.z()-this->b.z() , this->a.z()-ro.z() , rd.z()  );

        float gamma = GammaM.determinant() / A.determinant();

        float alpha = 1.0f - beta - gamma;

        Matrix3f tM( this->a.x()-this->b.x() , this->a.x()-this->c.x() , this->a.x()-ro.x() ,
                    this->a.y()-this->b.y() , this->a.y()-this->c.y() , this->a.y()-ro.y() ,
                    this->a.z()-this->b.z() , this->a.z()-this->c.z() , this->a.z()-ro.z()  );

        float t = tM.determinant() / A.determinant();

        if (beta + gamma > 1 || beta < 0 || gamma < 0)
        {
            return false;
        }

        if (t > tmin && t < h.getT())
        {

            Vector3f normal = (alpha*this->normals[0] + beta*this->normals[1] + gamma*this->normals[2]).normalized();
            h.set(t, this->material, normal);

            Vector2f textureCoordinate = (alpha*this->textureCoordinates[0] + beta*this->textureCoordinates[1] + gamma*this->textureCoordinates[2]);
            h.setTexCoord(textureCoordinate);

            return true;
        }
        return false;
        
    }
};

//    bool hasTex;
//    Vector3f normals[3];
//    Vector2f texCoords[3];

#endif //TRIANGLE_H
