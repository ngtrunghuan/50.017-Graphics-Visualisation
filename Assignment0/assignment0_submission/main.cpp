
// Please use this chunk of code
// to load the correct opengl and glut
// for your platform
#ifdef __APPLE__
#include <GLUT/glut.h>
#elif _WIN32
#include <windows.h>
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif
// --------------------------------

#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "vecmath.h"
#include <fstream>


// Included some helper macros and functions
// for those who miss the brevity of python.
#include "helpers.h"

using namespace std;

// Globals

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;


// You will need more global variables to implement color and position changes
int colorCount;
float vertLight = 1.0f;
float horzlight = 1.0f;

// These are convenience functions which allow us to call OpenGL
// methods on Vec3d objects
inline void glVertex(const Vector3f &a)
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a)
{ glNormal3fv(a); }

vector<string> splitValueString(string s, string delimiter);
void drawObject();

// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
	switch ( key )
	{
		case 27: // Escape key
			exit(0);
			break;
		case 'c':
			// add code to change color here
            colorCount = (colorCount + 1) % 4;
			break;
		default:
			cout << "Unhandled key press " << key << "." << endl;
	}
	
	// this will refresh the screen so that the user sees the color change
	glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
	switch ( key )
	{
		case GLUT_KEY_UP:
			// add code to change light position
            vertLight += .5f;
			break;
		case GLUT_KEY_DOWN:
			// add code to change light position
            vertLight -= .5f;
			break;
		case GLUT_KEY_LEFT:
			// add code to change light position
            horzlight -= .5f;
			break;
		case GLUT_KEY_RIGHT:
			// add code to change light position
            horzlight += .5f;
			break;
	}
	
	// this will refresh the screen so that the user sees the light position
	glutPostRedisplay();
}

vector<string> splitValueString(string s, string delimiter)
{
    vector<string> result = {};
    while (s.length() > 0)
    {
        result.push_back(s.substr(0, s.find(delimiter)));
        if (s.find(delimiter) == string::npos) break;
        s = s.substr(s.find(delimiter) + 1);
    }
    return result;
}

// This function is responsible for displaying the object.
void drawScene(void)
{
	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Rotate the image
	glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
	glLoadIdentity();              // Initialize to the identity
	
	// Position the camera at [0,0,5], looking at [0,0,0],
	// with [0,1,0] as the up direction.
	gluLookAt(0.0, 0.0, 5.0,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);
	
	// Set material properties of object
	
	// Here are some colors you might use - feel free to add more
	GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
		{0.9, 0.5, 0.5, 1.0},
		{0.5, 0.9, 0.3, 1.0},
		{0.3, 0.8, 0.9, 1.0} };
	
	// Here we use the first color entry as the diffuse color
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[colorCount]);
	
	// Define specular color and shininess
	GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
//    for (int i = 0; i < 4; i++){
//        specColor[i] = diffColors[colorCount][i];
//    }
	GLfloat shininess[] = {100.0};
	
	// Note that the specular color and shininess can stay constant
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	
	// Set light properties
	
	// Light color (RGBA)
	GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
	// Light position
	GLfloat Lt0pos[] = {horzlight, vertLight, 5.0f, 1.0f};
	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
	glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);
	
	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
//    glutSolidTeapot(1.0);
    drawObject();
	// Dump the image to the screen.
	glutSwapBuffers();
	
	
}

// Initialize OpenGL's rendering modes
void initRendering()
{
	glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
	glEnable(GL_LIGHTING);     // Enable lighting calculations
	glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
	// Always use the largest square viewport possible
	if (w > h) {
		glViewport((w - h) / 2, 0, h, h);
	} else {
		glViewport(0, (h - w) / 2, w, w);
	}
	
	// Set up a perspective view, with square aspect ratio
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// 50 degree fov, uniform aspect ratio, near = 1, far = 100
	gluPerspective(50.0, 1.0, 1.0, 100.0);
}
void loadInput()
{
    
}

void loadInput(string fileName)
{
	// load the OBJ file here
    ifstream stream (fileName);
    if (!stream.good()) return;
    
    std::string line;
    while (std::getline(stream, line))
    {
        int code;
        string codestr = line.substr(0, line.find(" "));
        string valuestr = line.substr(line.find(" ") + 1);
        
        // Translate code string into code int to use switch - case
        if (codestr == "v") code = 0;
        else if (codestr == "vn") code = 1;
        else if (codestr == "f") code = 2;
        else code = 3;

        // Load values into vecv, vecn and vecf
        vector<string> values = splitValueString(valuestr, " ");
        switch (code)
        {
            case 0:
            {
                vecv.push_back(Vector3f(stof(values[0]), stof(values[1]), stof(values[2])));
                break;
            }
            case 1:
            {
                vecn.push_back(Vector3f(stof(values[0]), stof(values[1]), stof(values[2])));
                break;
            }
            case 2:
            {
                vector<unsigned> face;
                for (int i = 0; i < values.size(); i++)
                {
                    vector<string> faces_str = splitValueString(values[i], "/");
                    for (int j = 0; j < faces_str.size(); j++)
                    {
                        face.push_back(stoul(faces_str[j]));
                    }
                }
                cout << "f ";
                for (int i = 0; i < face.size(); i++)
                {
                    cout << face[i] << " ";
                }
                cout << endl;
                vecf.push_back(face);
                break;
            }
            default:
                break;
        }
    }
}
// Draw object from vecv, vecn and vecf (limited to triangle vectors only)
void drawObject()
{
    for (int i = 0; i < vecf.size(); i++)
    {
        vector<unsigned> face = vecf[i];
        glBegin(GL_TRIANGLES);
        glNormal3d(vecn[face[2] - 1][0], vecn[face[2] - 1][1], vecn[face[2] - 1][2]);
        glVertex3d(vecv[face[0] - 1][0], vecv[face[0] - 1][1], vecv[face[0] - 1][2]);
        glNormal3d(vecn[face[5] - 1][0], vecn[face[5] - 1][1], vecn[face[5] - 1][2]);
        glVertex3d(vecv[face[3] - 1][0], vecv[face[3] - 1][1], vecv[face[3] - 1][2]);
        glNormal3d(vecn[face[8] - 1][0], vecn[face[8] - 1][1], vecn[face[8] - 1][2]);
        glVertex3d(vecv[face[6] - 1][0], vecv[face[6] - 1][1], vecv[face[6] - 1][2]);
        glEnd();
    }
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    if (argc != 2)
    {
        cout << "Invalid argument" << endl;
        return 0;
    }

    // Check if file is valid
    string fileName = argv[1];
    ifstream fstream (fileName);

    if (!fstream.good())
    {
        cout << "Invalid file" << endl;
        return 0;
    }
    
	loadInput(fileName);
	
	glutInit(&argc,argv);
	
	// We're going to animate it, so double buffer
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	
	// Initial parameters for window position and size
	glutInitWindowPosition( 60, 60 );
	glutInitWindowSize( 360, 360 );
	glutCreateWindow("Assignment 0");
	
	// Initialize OpenGL parameters.
	initRendering();
	
	// Set up callback functions for key presses
	glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
	glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys
	
	// Set up the callback function for resizing windows
	glutReshapeFunc( reshapeFunc );
	
	// Call this whenever window needs redrawing
	glutDisplayFunc( drawScene );
	
	// Start the main loop.  glutMainLoop never returns.
	glutMainLoop( );
	
	return 0;	// This line is never reached.
}
