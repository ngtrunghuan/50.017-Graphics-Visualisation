#include "surf.h"
#include "extra.h"
using namespace std;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.

	// Interate through each rotated replicated curve
	for (unsigned int step = 0; step < steps; step++){

		// calculate rotated angle
		float angle = 2.0f * M_PI * step / steps;

		// calculate transformation matrix for vertex and norm
		Matrix4f vertexRotationMatrix = Matrix4f::rotateY(angle);
		Matrix3f normRotationMatrix = vertexRotationMatrix.getSubmatrix3x3(0,0).inverse().transposed();

		// Interate through each point, add surfaces to global vector
		for (unsigned int i = 0; i < profile.size(); i++){
			CurvePoint curr_point = profile[i];

			// Find vertex and normal for current point
			Vector3f curr_vertex = (vertexRotationMatrix * Vector4f(curr_point.V,1)).xyz();
			Vector3f curr_norm = (normRotationMatrix * curr_point.N * (-1)).normalized();
			surface.VV.push_back(curr_vertex);
			surface.VN.push_back(curr_norm);

			// Construct face given vertex and normal
			if (i < profile.size() - 1) {
				unsigned int start_index = i + profile.size() * step;
				unsigned int next_index = (start_index + profile.size()) % (steps * profile.size());
				Tup3u triangle1(start_index, start_index + 1, next_index);
				Tup3u triangle2(start_index + 1, next_index + 1, next_index);
				surface.VF.push_back(triangle1);
				surface.VF.push_back(triangle2);			
			}
		}
	}
    return surface;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.

    // cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." <<endl;

	for (unsigned int i = 0; i < profile.size(); i++) 
	{
		for (unsigned int j = 0; j < sweep.size(); j++) 
		{
			// Define transformation matrix for vertex of profile
			Matrix4f coordMatrix = Matrix4f(sweep[j].N[0], sweep[j].B[0], sweep[j].T[0], sweep[j].V[0], 
											sweep[j].N[1], sweep[j].B[1], sweep[j].T[1], sweep[j].V[1], 
											sweep[j].N[2], sweep[j].B[2], sweep[j].T[2], sweep[j].V[2], 
											0.0f, 0.0f, 0.0f, 1.0f);

			// Define transformation matrix for normal of profile
			Matrix3f normMatrix = coordMatrix.getSubmatrix3x3(0, 0).transposed().inverse();

			// Calculate transformed profile vertex
			Vector3f surfaceVector = (coordMatrix * (Vector4f(profile[i].V.xyz(), 1.0f))).xyz();
		
			// Calculate transformed profile normal
			Vector3f normalVector = (normMatrix * profile[i].N.xyz() * (-1)).normalized();

			// Push into surface vector
			surface.VV.push_back(surfaceVector);
			surface.VN.push_back(normalVector);
		}
	}

	for (unsigned int k = 0; k < surface.VV.size() - (sweep.size()); k++){

		// Declaration of triangles
		Tup3u firstTriangle;	
		Tup3u secondTriangle;

		if ((k + 1) % (sweep.size()) != 0)
		{
			// Draw triangle in counter-clockwise manner
			firstTriangle = Tup3u(k + 1, k, k + sweep.size());
			secondTriangle = Tup3u(k + 1, k + sweep.size(), k + 1 + sweep.size());   
			}

		surface.VF.push_back(firstTriangle);
		surface.VF.push_back(secondTriangle);

	}

    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);

    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
