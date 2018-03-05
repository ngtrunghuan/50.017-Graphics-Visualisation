#include "SkeletalModel.h"

#include <FL/Fl.H>

using namespace std;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton( const char* filename )
{
	// Load the skeleton from file here.
    ifstream stream (filename);
    if (!stream.good())
    {
        cout << "Unable to load file " << filename << endl;
        return;
    }
    
    vector<vector<string>> lines;
    string line;
    while (std::getline(stream, line))
    {
//        cout << line << endl;
        stringstream lineStream(line);
        float x, y, z;
        int parent;
        lineStream >> x >> y >> z >> parent;
        
        Joint *joint = new Joint;
        m_joints.push_back(joint);
        
        // Register transform matrix for joint
        joint->transform = Matrix4f::translation(x,y,z);
        
        // Register as a child for parent joint
        // and compute world-to-joint transform
        // for non-root joints
        if (parent > -1)
        {
            m_joints[parent]->children.push_back(joint);
//            joint->bindWorldToJointTransform = m_joints[parent]->bindWorldToJointTransform * joint->transform;
        }
        else
        {
            m_rootJoint = joint;
//            m_rootJoint->bindWorldToJointTransform = joint->transform;
        }
    }
}

void SkeletalModel::drawJoints( )
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.
    cout << "Root = " << m_rootJoint->transform.getCol(3).abs() << endl;
    traverseJoints(m_rootJoint, m_matrixStack);
}

void SkeletalModel::traverseJoints(Joint *parent, MatrixStack stack)
{
    // Push joint onto stack
    stack.push(parent->transform);
    
    // Once a geometric node is found, draw it
    glLoadMatrixf(stack.top());
    glutSolidSphere(.025f, 12, 12);
    
    // Traverse down if there are children
    for (int i = 0; i < parent->children.size(); ++i)
    {
        traverseJoints(parent->children[i], stack);
    }
    
    // If there is no child and pop the current joint
    stack.pop();
}

void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy.
    traverseBones(m_rootJoint, m_matrixStack);
}

void SkeletalModel::traverseBones(Joint *joint, MatrixStack stack)
{
//    cout << "draw bone : " << joint << endl;
    
    if (joint != m_rootJoint)
    {
        // Calculate the translation matrix
        Matrix4f translate = Matrix4f::translation(0, 0, .5);
        
        // Calculate the scaling matrix
        Matrix4f currentTransform = joint->transform;
        Vector3f parentOffset (currentTransform[12],currentTransform[13],currentTransform[14]);
        float distance = parentOffset.abs();
        Matrix4f scale = Matrix4f::scaling(.05, .05, distance);
        
        // Calculate the z-rotation matrix
        Vector3f rnd (0, 0, 1);
        Vector3f z = parentOffset.normalized();
        Vector3f y = Vector3f::cross(z, rnd).normalized();
        Vector3f x = Vector3f::cross(y, z).normalized();
        Matrix3f tempRotation (x, y, z);
        Matrix4f rotation = Matrix4f::identity();
        rotation.setSubmatrix3x3(0, 0, tempRotation);
        
        // Calculate the transform
        Matrix4f transform = rotation * scale * translate;

        // Draw
        stack.push(transform);
        glLoadMatrixf(stack.top());
        glutSolidCube(1.0f);
        stack.pop();
    }
    
    stack.push(joint->transform);
    for (int i = 0; i < joint->children.size(); ++i)
    {
        traverseBones(joint->children[i], stack);
    }
    stack.pop();
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
    Matrix4f rotation = Matrix4f::rotateX(rX) * Matrix4f::rotateY(rY) * Matrix4f::rotateZ(rZ);
    m_joints[jointIndex]->transform.setSubmatrix3x3(0, 0, rotation.getSubmatrix3x3(0, 0));
    
}

void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
    traverseBindWorldToJoint(m_rootJoint);
}

void SkeletalModel::traverseBindWorldToJoint(Joint *joint)
{
    m_matrixStack.push(joint->transform);
    
    joint->bindWorldToJointTransform = m_matrixStack.top().inverse();
    for (unsigned i = 0; i < joint->children.size(); i++) {
        traverseBindWorldToJoint(joint->children[i]);
    }
    
    m_matrixStack.pop();
}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
    traverseJointToWorld(m_rootJoint);
}

void SkeletalModel::traverseJointToWorld(Joint *joint)
{
    m_matrixStack.push(joint->transform);
    
    joint->currentJointToWorldTransform = m_matrixStack.top();
    for (unsigned i = 0; i<(joint->children.size()); i++) {
        traverseJointToWorld(joint->children[i]);
    }
    
    m_matrixStack.pop();
}
void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.
    vector<Vector3f> newVertices;
    for (unsigned i = 0; i < m_mesh.currentVertices.size(); ++i)
    {
        Vector4f vertex = Vector4f();
        for (unsigned j = 0; j < m_mesh.attachments[i].size(); ++j)
        {
            float weight = m_mesh.attachments[i][j];
            if (weight != 0)
            {
                vertex = vertex + weight * (m_joints[j + 1]->currentJointToWorldTransform *
                                            m_joints[j + 1]->bindWorldToJointTransform *
                                            Vector4f(m_mesh.bindVertices[i][0], m_mesh.bindVertices[i][1],
                                                     m_mesh.bindVertices[i][2], 1.0f));
            }
        }
        newVertices.push_back(vertex.xyz());
    }
    m_mesh.currentVertices = newVertices;
}

