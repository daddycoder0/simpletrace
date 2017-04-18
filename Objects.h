#ifndef __OBJECTS_H_
#define __OBJECTS_H_

#include <vector>
#include <stdio.h>
#include <string.h>
#include "Matrix.h"
#include "Vertex.h"
#include "ThirdParty/rapidxml/rapidxml.hpp"

using namespace rapidxml;
using namespace std;

#define OBJECT_MAX_NAMELENGTH 32

class Instance;
class Material;
class Scene;

class Object
{
	public:
			Object(const char* name)
			{
				snprintf(m_objectName, OBJECT_MAX_NAMELENGTH, "%s", name);
			}
			virtual ~Object				()										{}

			virtual bool Parse			(xml_node<>* node, char* sp, int spSize){return true;}
			virtual void CalcNormals	()										{};
			bool Is						(char* name)							{return strcmp(m_objectName, name) == 0;}
			virtual float RayIntersects	(Vector3& start, Vector3& dir, unsigned int& triangleIndex, bool bfcull = true) = 0;
			virtual void GetColourForIntersection(	Vector3& start, Vector3& dir, float t, Instance* inst, unsigned int triangleIndex, Vector3& colOut, 
													Vector3& ambientLight, vector<Instance*>* lights, Material* material, Matrix4& transform, Scene& scene)=0;


	protected:
		char	m_objectName[OBJECT_MAX_NAMELENGTH];
};

class SimpleMesh : public Object
{
	public:
			
		SimpleMesh(const char* name) : Object(name)
		{
			m_vertexList	= NULL;
			m_triangleList	= NULL;
			m_faceNormals	= NULL;
			m_vertNormals	= NULL;
			m_numVerts		= 0;
			m_numTriangles	= 0;
		}

		virtual ~SimpleMesh()
		{
			delete [] m_vertexList;
			delete [] m_triangleList;
			delete [] m_faceNormals;
			delete [] m_vertNormals;
		}

		bool	Parse			(xml_node<>* node, char* sp, int spSize);
		float	RayIntersects	(Vector3& start, Vector3& dir, unsigned int& triangleIndex, bool bfcull = true);
		void	GetColourForIntersection(	Vector3& start, Vector3& dir, float t, Instance* inst, unsigned int triangleIndex, Vector3& colOut, 
											Vector3& ambientLight, vector<Instance*>* lights, Material* material, Matrix4& transform, Scene& scene);

	private:

		void SetNumVerts(unsigned int num)
		{
			m_vertexList = new Vertex[num];
			m_vertNormals = new Vector3[num];
			m_numVerts = num;
		}

		bool SetVert(unsigned int index, Vertex* vert)
		{
			if (index >= 0 && index < m_numVerts)
			{
				m_vertexList[index] = *vert;
				return true;
			}
			return false;
		}

		void SetNumTris(unsigned int num)
		{
			m_triangleList = new unsigned int[num * 3];
			m_faceNormals = new Vector3[num];
			m_numTriangles = num;
		}

		bool SetTri(unsigned int index, int v0, int v1, int v2)
		{
			if (index >= 0 && index < m_numTriangles)
			{
				unsigned int i = index*3;
				m_triangleList[i+0] = v0;
				m_triangleList[i+1] = v1;
				m_triangleList[i+2] = v2;
				return true;
			}
			return false;
		}
			
	
		void			CalcNormals();

		Vertex*			m_vertexList;
		unsigned int*	m_triangleList;
		Vector3*		m_faceNormals;
		Vector3*		m_vertNormals;
		unsigned int	m_numVerts;
		unsigned int	m_numTriangles;

};
#endif
