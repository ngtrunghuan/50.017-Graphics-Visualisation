#ifndef GROUP_H
#define GROUP_H


#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>

using  namespace std;

///TODO:
///Implement Group
///Add data structure to store a list of Object*
class Group:public Object3D
{
public:

    Group():Object3D(NULL){}

    Group( int num_objects ){}

    ~Group(){}

    virtual bool intersect( const Ray& r , Hit& h , float tmin )
    {
        bool intersect = false;
        for (int i = 0; i < this->getGroupSize(); i++)
        {
            Object3D* object = objects[i];
            if (object->intersect(r, h, tmin))
            {
                intersect = true;
            }
        }
        return intersect;
    }

    void addObject( int index , Object3D* obj )
    {
        this->objects.push_back(obj);
    }

    int getGroupSize()
    {
        return this->objects.size();
    }

private:
    vector<Object3D*> objects;
};

#endif
