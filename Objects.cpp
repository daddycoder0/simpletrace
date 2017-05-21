#include <iostream>
#include <string.h>


#include "Instance.h"
#include "Material.h"
#include "Objects.h"

#include "Scene.h"

using namespace std;


bool Sphere::Parse(xml_node<>* node, char* spad, int spSize)
{
	xml_node<>* n = node->first_node("radius");
	if (n)
	{
		snprintf(spad, spSize, n->value());
		m_radius = (float)atof(spad);
	}

	return true;
}

float Sphere::RayIntersects	(Vector3& start, Vector3& dir, unsigned int& triangleIndex, bool bfcull)
{
	// http://www.ccs.neu.edu/home/fell/CSU540/programs/RayTracingFormulas.htm
	float a = dir.m_x*dir.m_x + dir.m_y*dir.m_y + dir.m_z*dir.m_z;
	float b = 2*dir.m_x*(start.m_x) +  2*dir.m_y*(start.m_y) +  2*dir.m_z*(start.m_z);
	float c = (start.m_x*start.m_x + start.m_y*start.m_y + start.m_z*start.m_z) - m_radius*m_radius;
	
	float delta = (b*b)-4*a*c;
	if (delta > 0)
	{
		float d1=(-b-sqrtf(delta))/(2*a);
		//float d2=(-b+sqrtf(delta))/(2*a);
		
		return d1;// < d2 ? d1 : d2;
	}
	return -1.0f;
}

void Sphere::GetIntersectionNormal(Vector3& start, Vector3& dir, Vector3& normal, unsigned int triangleIndex, float t, Matrix4& transform)
{
	Vector3 scaledDir;
	scaledDir.Mul(dir, t);
	Vector3 intPos = start + scaledDir;
	Vector3 pos = transform.Pos();
	normal.Sub(intPos, pos);
	normal.Normalize();
}


bool SimpleMesh::Parse(xml_node<>* node, char* spad, int spSize)
{
	xml_node<>* n = node->first_node("verts");
	unsigned int numElements = 0;
	char* context = NULL;

	if (n)
	{
		if ((int)strlen(n->value()) >= spSize)
		{
			cout << "Object vert list too big for scratch pad: " << m_objectName << endl;
			return false;	
		}
		snprintf(spad, spSize, n->value());
		char* c = strtk(spad, &context);

		while (c != NULL)
		{
			numElements++;
			c = strtk(NULL, &context);
		}

		if (numElements%3 != 0)
		{
			cout << "Bad verts list for object: " << m_objectName << endl;
			return false;
		}

		SetNumVerts(numElements/3);

		numElements = 0;
		snprintf(spad, spSize, n->value());
		context = NULL;
		char* x = strtk(spad, &context);
		char* y = strtk(NULL, &context);
		char* z = strtk(NULL, &context);
		Vector3 v(atof(x), atof(y), atof(z));
		Vertex vert(v);
		SetVert(numElements++, &vert);

		while (x != NULL)
		{
			x = strtk(NULL, &context);
			y = strtk(NULL, &context);
			z = strtk(NULL, &context);
			if (x)
			{
				v.Set(atof(x), atof(y), atof(z));
				vert.SetPos(v);
				SetVert(numElements++, &vert);
			}
		}

	}
	context = NULL;
	n = node->first_node("tris");
	if (n)
	{
		if ((int)strlen(n->value()) >= spSize)
		{
			cout << "Object triangle list too big for scratch pad: " << m_objectName << endl;
			return false;	
		}
		numElements = 0;
		snprintf(spad, spSize, n->value());
		char* c = strtk(spad, &context);

		while (c != NULL)
		{
			numElements++;
			c = strtk(NULL, &context);
		}

		if (numElements%3 != 0)
		{
			cout << "Bad triangle list for object: " << m_objectName << endl;
			return false;
		}

		SetNumTris(numElements/3);
		context = NULL;
		numElements = 0;
		snprintf(spad, spSize, n->value());
		char* aa = strtk(spad, &context);
		char* bb = strtk(NULL, &context);
		char* cc = strtk(NULL, &context);
		
		SetTri(numElements++, atoi(aa), atoi(bb), atoi(cc));

		while (aa != NULL)
		{
			aa = strtk(NULL, &context);
			bb = strtk(NULL, &context);
			cc = strtk(NULL, &context);
			if (aa)
			{
				SetTri(numElements++, atoi(aa), atoi(bb), atoi(cc));
			}
		}

	}

	CalcNormals();
	return true;
}

void SimpleMesh::CalcNormals()
{
	for (unsigned int i=0;i<m_numTriangles; i++)
	{
		int ti = i * 3;
		Vector3 e1 = e1.Sub(m_vertexList[m_triangleList[ti+1]].Pos(),m_vertexList[m_triangleList[ti]].Pos());
		Vector3 e2 = e2.Sub(m_vertexList[m_triangleList[ti+2]].Pos(),m_vertexList[m_triangleList[ti]].Pos());
		m_faceNormals[i] = CrossProduct(e1, e2);
		m_faceNormals[i].Normalize();
	}
}

int triangle_intersection(	const Vector3&  V1,
							const Vector3&  V2,
							const Vector3&  V3,
							const Vector3&  O,
							const Vector3&  D,
							float& out )
{
	//Find vectors for two edges sharing V1
	Vector3 e1 = e1.Sub(V2,V1);
	Vector3 e2 = e2.Sub(V3,V1);
	//Begin calculating determinant - also used to calculate u parameter
	Vector3 P = CrossProduct(D, e2);
	//if determinant is near zero, ray lies in plane of triangle
	float det = DotProduct(e1, P);
 
	//NOT CULLING
	if(det > -EPSILON && det < EPSILON) return 0;
	
	float inv_det = 1.f / det;
 
	//calculate distance from V1 to ray origin
	Vector3 T = T.Sub(O, V1);
 
	//Calculate u parameter and test bound
	float u = DotProduct(T, P) * inv_det;
	//The intersection lies outside of the triangle
	if(u < 0.f || u > 1.f) return 0;
 
	//Prepare to test v parameter
	Vector3 Q = CrossProduct(T, e1);
 
	//Calculate V parameter and test bound
	float v = DotProduct(D, Q) * inv_det;
	//The intersection lies outside of the triangle
	if(v < 0.f || u + v  > 1.f) return 0;
 
	float t = DotProduct(e2, Q) * inv_det;
 
	if(t > EPSILON)
	{ //ray intersection
		out = t;
		return 1;
	}
	return 0;
}

float SimpleMesh::RayIntersects(Vector3& start, Vector3& dir, unsigned int& triangleIndex, bool bfcull)
{
	float t = -1.f;

	float nearestT = -1.f;

	for (unsigned int i=0;i<m_numTriangles;i++)
	{
		int ti = i*3;	
		if ((!bfcull || DotProduct(dir, m_faceNormals[i]) < 0.f) &&
			triangle_intersection(	m_vertexList[m_triangleList[ti+0]].Pos(), 
									m_vertexList[m_triangleList[ti+1]].Pos(),
									m_vertexList[m_triangleList[ti+2]].Pos(),
									start, dir, t))
		{
			if (nearestT < 0.f || t < nearestT)
			{
				triangleIndex = i;
				nearestT = t;
			}
		}
	}
	
	return t;
}

void SimpleMesh::GetIntersectionNormal(Vector3& start, Vector3& dir, Vector3& normal, unsigned int triangleIndex, float t, Matrix4& transform)
{
	Matrix4 trans = transform;
	trans.SetPos(0.f, 0.f, 0.f);
	normal = trans.Transform(m_faceNormals[triangleIndex]);
	normal.Normalize();
}

void Object::GetColourForIntersection(Vector3& start, Vector3& dir, float t, Instance* inst, unsigned int triangleIndex, Vector3& colOut, Vector3& ambientLight, vector<Instance*>* lights, Material* material, Matrix4& transform, Scene& scene)
{
	Vector3 matCol(1.f, 0.f, 1.f);
	Vector3 finalCol;
	Vector3 intersection = dir * t;
	intersection.Add(intersection, start);

	if (material)
	{
		material->GetColour(matCol);
	}

	finalCol = matCol * ambientLight;
	Vector3 normal;
	GetIntersectionNormal(start, dir, normal, triangleIndex, t, transform);

	for (unsigned int i=0;lights && i<lights->size(); i++)
	{
		Vector3 lightPos, vecToLight;
		lights->at(i)->GetPos(lightPos);

		vecToLight = vecToLight.Sub(lightPos, intersection);
		float distToLight = vecToLight.Length();
		vecToLight.Normalize();

		Instance* shadowInstance = NULL;
		unsigned int tIndex = 0;
		intersection = intersection.Add(intersection, vecToLight * 0.001f);	// make sure we don't hit ourselves.

		float shadowT = inst == lights->at(i) ? -1.f : scene.GetNearestIntersection(intersection, vecToLight, shadowInstance, tIndex, false);
		
		if (shadowT < 0.f || shadowT > distToLight || shadowInstance == lights->at(i))
		{
			float lightDot = inst == lights->at(i) ? 2.f : DotProduct(normal, vecToLight);
			if (lightDot > 0.f)
			{
				Vector3 lightCol(1.f, 1.f, 1.f);
				Material* lightMat = lights->at(i)->GetMaterial();
				if (lightMat)
				{
					lightMat->GetColour(lightCol);	
				}
				finalCol.m_x+=lightCol.m_x * lightDot * matCol.m_x;
				finalCol.m_y+=lightCol.m_y * lightDot * matCol.m_y;
				finalCol.m_z+=lightCol.m_z * lightDot * matCol.m_z;
			}
		}
	}

	
	colOut.m_x = finalCol.m_x < 1.f ? finalCol.m_x : 1.f;
	colOut.m_y = finalCol.m_y < 1.f ? finalCol.m_y : 1.f;
	colOut.m_z = finalCol.m_z < 1.f ? finalCol.m_z : 1.f;

}
