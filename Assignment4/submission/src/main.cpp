#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
    // Fill in your implementation here.
    bool isDepthOn = false;
    bool isNormalOn = false;
    
    // basic
    char* input;
    char* output;
    int width = 0;
    int height = 0;
    
    // depth
    float depthMin = 0;
    float depthMax = 0;
    char* depthOutput;
    // normal
    char* normalOutput;
    
    // This loop loops over each of the input arguments.
    // argNum is initialized to 1 because the first
    // "argument" provided to the program is actually the
    // name of the executable (in our case, "a4").
    // Read the arguments
    for( int argNum = 1; argNum < argc; ++argNum )
    {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
        if (strcmp(argv[argNum], "-input") == 0)
        {
            input = argv[argNum + 1];
        }
        else if (strcmp(argv[argNum], "-size") == 0)
        {
            width = atoi(argv[argNum + 1]);
            height = atoi(argv[argNum + 2]);
        }
        else if (strcmp(argv[argNum], "-output") == 0)
        {
            output = argv[argNum + 1];
        }
        else if (strcmp(argv[argNum], "-depth") == 0)
        {
            isDepthOn = true;
            depthMin = atof(argv[argNum + 1]);
            depthMax = atof(argv[argNum + 2]);
            depthOutput = argv[argNum + 3];
        }
        else if (strcmp(argv[argNum], "-normal") == 0)
        {
            isNormalOn = true;
            normalOutput = argv[argNum + 1];
        }
    }


    // First, parse the scene using SceneParser.
    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    SceneParser scene(input);
    Image img(width, height);
    img.SetAllPixels(scene.getBackgroundColor());
    
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            // make sure coordinates are from [-1,1]
            Vector2f coord = Vector2f((2 * float(i) / (width - 1)) - 1 , (2 * float(j) / (height - 1)) - 1);
            Ray ray = scene.getCamera()->generateRay(coord);
            Hit hit(MAXFLOAT, NULL, Vector3f::ZERO);
            
            if (scene.getGroup()->intersect(ray, hit, scene.getCamera()->getTMin())){
//                cout << (hit.getT()) << endl;
                Vector3f pixCol = Vector3f::ZERO;
                
                // diffuse color
                for (int indL = 0; indL < scene.getNumLights(); indL++){
                    Light* light = scene.getLight(indL);
                    Vector3f lightDirection;
                    Vector3f lightColor;
                    float distanceToLight;
                    
                    light->getIllumination(ray.pointAtParameter(hit.getT()), lightDirection, lightColor, distanceToLight);
                    
                    Vector3f shadingCol = hit.getMaterial()->Shade(ray, hit, lightDirection, lightColor);
                    pixCol = pixCol + shadingCol;
                }
                // ambient color
                pixCol = pixCol + hit.getMaterial()->getDiffuseColor() * scene.getAmbientLight();
                
                // set pixel
                img.SetPixel(j, i, pixCol);
            }
        }
    }
    img.SaveBMP(output);
    
    // produce depth image
    if (isDepthOn)
    {
        Image imgDepth(width, height);
        imgDepth.SetAllPixels(Vector3f::ZERO);
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                // make sure coordinates are from [-1,1]
                Vector2f coord = Vector2f( (2*float(i) / (width-1)) -1 , (2*float(j) / (height-1)) -1 );
                Ray ray = scene.getCamera()->generateRay(coord);
                Hit hit(MAXFLOAT, NULL, Vector3f::ZERO);
                if(scene.getGroup()->intersect(ray, hit, scene.getCamera()->getTMin()))
                {
                    if (hit.getT() < depthMin)
                    {
                        imgDepth.SetPixel(j, i, Vector3f(1.0f));
                    }
                    else if (hit.getT() > depthMax)
                    {
                        imgDepth.SetPixel(j, i, Vector3f::ZERO);
                    }
                    else
                    {
                        float grayScale = (depthMax - hit.getT()) / (depthMax - depthMin);
                        imgDepth.SetPixel(j, i, Vector3f(grayScale));
                    }
                }
            }
        }
        imgDepth.SaveBMP(depthOutput);
    }
            
    // produce normal image
    if (isNormalOn)
    {
        Image imgNormals(width, height);
        imgNormals.SetAllPixels(Vector3f::ZERO);
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                // make sure coordinates are from [-1,1]
                Vector2f coord = Vector2f( (2*float(i) / (width-1)) -1 , (2*float(j) / (height-1)) -1 );
                Ray ray = scene.getCamera()->generateRay(coord);
                Hit hit(MAXFLOAT, NULL, Vector3f::ZERO);
                if(scene.getGroup()->intersect(ray, hit, 0.0f))
                {
                    Vector3f pixColNorm = hit.getNormal().normalized();
                    for (int x = 0; x < 3; x++){
                        if (pixColNorm[x] < 0){
                            pixColNorm[x] = pixColNorm[x] * -1.0f;
                        }
                    }
                    imgNormals.SetPixel(j, i, pixColNorm);
                }
            }
        }
        imgNormals.SaveBMP(normalOutput);
    }



 
    ///TODO: below demonstrates how to use the provided Image class
    ///Should be removed when you start
    return 0;
}

