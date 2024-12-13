#include <iostream>
#include <string.h>
#include <math.h>

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
		snprintf(spad, spSize, "%s", n->value());
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

bool Sphere::GetRandomPointOnSurface(Vector3& position)
{
	Vector3 randomVector((float)rand(), (float)rand(), (float)rand());
	randomVector.Normalize();
	position.Mul(randomVector, m_radius);
	return true;
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
		snprintf(spad, spSize, "%s", n->value());
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
		snprintf(spad, spSize, "%s", n->value());
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
		snprintf(spad, spSize, "%s", n->value());
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
		snprintf(spad, spSize, "%s", n->value());
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
}

bool SimpleMesh::GetRandomPointOnSurface(Vector3& position)
{
	// not accounting for different size polygons, not too important for now.
	// select a poly at random
	int t = 3 * (rand() % m_numTriangles);
	Vector3 A = m_vertexList[m_triangleList[t]].Pos();
	Vector3 B = m_vertexList[m_triangleList[t+1]].Pos();
	Vector3 C = m_vertexList[m_triangleList[t+2]].Pos();

	// find area in triangle (http://www.cs.princeton.edu/~funk/tog02.pdf)
	real r1 = real(rand()) / real(RAND_MAX);
	real r2 = real(rand()) / real(RAND_MAX);

	real a = (1.f-sqrtf(r1));
	real b = (sqrtf(r1)*(1.f - r2));
	real c = (r2 * sqrtf(r1));
	A.Mul(A, a);
	B.Mul(B, b);
	C.Mul(C, c);
	
	position.Add(A, B);
	position.Add(position, C);
	return true;
}


void Object::GetColourForIntersection(Vector3& start, Vector3& dir, float t, Instance* inst, unsigned int triangleIndex, Vector3& colOut, Vector3& ambientLight, vector<LightInstance*>* lights, Material* material, Matrix4& transform, Scene& scene)
{
	Vector3 matCol(1.f, 0.f, 1.f);
	Vector3 finalCol;
	Vector3 intersect = dir * t;
	Vector3 intersection;
	intersect.Add(intersect, start);
	float reflectivity = 0.f;
	if (material)
	{
		material->GetColour(matCol);
		reflectivity = material->GetReflectivity();
	}

	finalCol = matCol * ambientLight;
	Vector3 normal;
	GetIntersectionNormal(start, dir, normal, triangleIndex, t, transform);
	
	for (unsigned int i=0;lights && i<lights->size(); i++)
	{
		Vector3 lightPos, vecToLight;
		LightInstance* li = lights->at(i);
		li->GetPos(lightPos);

		Instance* shadowInstance = NULL;
		unsigned int tIndex = 0;

		// count how many samples are valid (lots of scope for optimization here)
		unsigned int numHits = 0;
		for (unsigned int s = 0;s < li->GetNumSamples();s++)
		{
			Vector3 samplePos;
			li->GetSamplePoint(s, samplePos);
			vecToLight = vecToLight.Sub(samplePos, intersect);
			float distToLight = vecToLight.Length();
			vecToLight.Normalize();
			intersection = intersection.Add(intersect, vecToLight * -0.0001f);	// make sure we don't hit ourselves.

			float shadowT = inst == li ? -1.f : scene.GetNearestIntersection(intersection, vecToLight, shadowInstance, tIndex, true);
			if (shadowT < 0.f || shadowT > distToLight || shadowInstance == li)
			{
				numHits++;
			}
		}

		float lightScale = (float)numHits / (float)li->GetNumSamples();

		vecToLight = vecToLight.Sub(lightPos, intersect);
		float distToLight = vecToLight.Length();
		vecToLight.Normalize();

		if (lightScale > 0.f)
		{
			float lightDot = inst == li ? 2.f : DotProduct(normal, vecToLight);
			if (lightDot > 0.f)
			{
				lightDot *= lightScale;	// factor for soft shadows
				Vector3 lightCol(1.f, 1.f, 1.f);
				Material* lightMat = li->GetMaterial();
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

	if (reflectivity > 0)
	{
		Vector3 rVec = dir;
		Vector3 rColour(0.f, 0.f, 0.f);
		rVec.Reflect(normal);
		Instance* inst;
		unsigned int tIndex;
		intersection = intersection.Add(intersect, rVec * 0.0001f);	// make sure we don't hit ourselves.

		float t = scene.GetNearestIntersection(intersection, rVec, inst, tIndex);

		if (t >= 0.f)
		{
			scene.GetColourForIntersection(intersection, rVec, t, inst, tIndex, rColour);
			finalCol = finalCol.Add(finalCol * (1.0f - reflectivity), rColour * reflectivity);
		}
		else
		{
			finalCol = finalCol * (1.0f - reflectivity);
		}

	}
	
	colOut.m_x = finalCol.m_x < 1.f ? finalCol.m_x : 1.f;
	colOut.m_y = finalCol.m_y < 1.f ? finalCol.m_y : 1.f;
	colOut.m_z = finalCol.m_z < 1.f ? finalCol.m_z : 1.f;

}
