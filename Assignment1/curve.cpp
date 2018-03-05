#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }

    
}
    
float calcWeightBezier(unsigned index, float t, bool isTangent = false)
{
	if (!isTangent) {
		switch (index)
		{
		case 1:
			return powf(1 - t, 3.0f);
		case 2:
			return 3 * t * powf(1 - t, 2.0f);
		case 3:
			return 3 * powf(t, 2.0f) * (1 - t);
		case 4:
			return powf(t, 3.0f);
		default:
			return 0.0f;
		}
	}
	else
	{
		switch (index)
		{
		case 1:
			return -3.0f + 6.0f * t - 3.0f * powf(t, 2.0f);
		case 2:
			return 3.0f - 12.0f * t + 9.0f * powf(t, 2.0f);
		case 3:
			return 6.0f * t - 9.0f * powf(t, 2.0f);
		case 4:
			return 3 * powf(t, 2.0f);
		default:
			return 0.0f;
		}
	}
}

float calcWeightSpline(unsigned index, float t, bool isTangent = false)
{
	if (!isTangent) {
		switch (index)
		{
		case 1:
			return 1.0f / 6.0f * powf(1 - t, 3.0f);
		case 2:
			return 1.0f / 6.0f * (3.0f * powf(t, 3.0f) - 6.0f * powf(t, 2.0f) + 4.0f);
		case 3:
			return 1.0f / 6.0f * (-3.0f * powf(t, 3.0f) + 3.0f * powf(t, 2.0f) + 3.0f * t + 1);
		case 4:
			return 1.0f / 6.0f * powf(t, 3.0f);
		default:
			return 0.0f;
		}
	}
	else
	{
		switch (index)
		{
		case 1:
			return -0.5f * powf(1 - t, 2.0f);
		case 2:
			return 1.5f * powf(t, 2.0f) - 2.0f * t;
		case 3:
			return -1.5f * powf(t, 2.0f) + t + 0.5f;
		case 4:
			return 0.5f * powf(t, 2.0f);
		default:
			return 0.0f;
		}
	}
}

//vector<Vector3f> convertBSplinePointsToBezier(vector<Vector3f> P)
//{
//	vector<Vector3f> newP = {};
//	
//}

Vector3f calcVector(float t, vector<Vector3f> P, bool isSpline = false, bool isTangent = false)
{
	Vector3f vect = Vector3f();
	if (!isSpline)
	{
		for (unsigned i = 0; i < P.size(); ++i)
		{
			vect = vect + calcWeightBezier(i + 1, t, isTangent) * P[i];
		}
	}
	else
	{
		for (unsigned i = 0; i < P.size(); ++i)
		{
			vect = vect + calcWeightSpline(i + 1, t, isTangent) * P[i];
		}
	}

	return vect;
}

Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit( 0 );
    }

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;


    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
	Curve R(steps);

	for (unsigned step = 0; step < steps; ++step)
	{
		float t = 1.0f / float(steps - 1) * float(step);

		R[step].V = calcVector(t, P, false, false);
		R[step].T = calcVector(t, P, false, true).normalized();
		if (step == 0) R[step].N.cross(R[step].T + Vector3f(0, 1, 0), R[step].T).normalized();
		else R[step].N.cross(R[step - 1].B, R[step].T).normalized();
		R[step].B.cross(R[step].T, R[step].N).normalized();

	}
    // Right now this will just return this empty curve.
    return R;
}

Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }

    // TODO:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.

    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
	Curve R(steps * (P.size() - 3));
    for( unsigned i = 0; i < P.size() - 3; ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
		vector<Vector3f> splineSegment(P.begin() + i, P.begin() + i + 4);
		//cerr << "\t\tChecking first spline point is correct = " << approx(P[i], splineSegment[0]) << endl;
		cerr << "\t\tLength of splineSlice = " << splineSegment.size() << endl;
		for (unsigned j = 0; j < splineSegment.size(); ++j)
		{
			cerr << "\t>>> " << *splineSegment[j] << endl;
		}

		for (unsigned step = 0; step < steps; ++step)
		{
			float t = 1.0f / float(steps - 1) * float(step);
			unsigned stepInCurve = i * steps + step;
			cerr << "\t\tDrawing for StepInCurve = " << stepInCurve << endl;

			R[stepInCurve].V = calcVector(t, splineSegment, true, false);
			cerr << "\t\tV at " << stepInCurve << " = " << *R[stepInCurve].V << endl;

			R[stepInCurve].T = calcVector(t, splineSegment, true, true).normalized();
			if (stepInCurve == 0) R[stepInCurve].N.cross(R[stepInCurve].T + Vector3f(0, 8, 0), R[stepInCurve].T).normalized();
			else R[stepInCurve].N.cross(R[stepInCurve].B, R[stepInCurve].T).normalized();
			R[stepInCurve].B.cross(R[stepInCurve].T, R[stepInCurve].N).normalized();
		}
    }

    cerr << "\t>>> Steps (type steps): " << steps * (P.size() - 3) + 1 << endl;
    //cerr << "\t>>> Returning empty curve." << endl;

    // Return an empty curve right now.
    return R;
}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

