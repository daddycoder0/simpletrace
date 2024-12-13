#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#include <vector>
#include "Matrix.h"

class Object;
class Material;
class Scene;
class LightInstance;

using namespace std;

class Instance
{
	public:
							virtual ~Instance(){};

		virtual float		RayIntersects(Vector3& start, Vector3& dir, unsigned int& triangleIndex, bool bfcull = true){return -1;}
		virtual void		GetColourForIntersection(Vector3& start, Vector3& dir, float t, Instance* inst, unsigned int triangleIndex, Vector3& colOut, Vector3& ambientLight, vector<LightInstance*>* lights, Scene& scene){};
		virtual Material*	GetMaterial(){return NULL;}
		void				GetPos(Vector3& pos){pos = m_posrot.Pos();}

	protected:
						Instance(Matrix4& m)		{m_posrot = m;};
						Instance(){};

		Matrix4			m_posrot;
		bool			m_enabled;
};

class ObjectInstance : public Instance
{
	public:
					ObjectInstance(Matrix4 m, Object* obj) : Instance(m)
					{
						m_object = obj;
					}

		float		RayIntersects(Vector3& start, Vector3& dir, unsigned int& triangleIndex, bool bfcull = true);
		void		GetColourForIntersection(Vector3& start, Vector3& dir, float t, Instance* inst, unsigned int triangleIndex, Vector3& colOut, Vector3& ambientLight, vector<LightInstance*>* lights, Scene& scene);
		void		SetMaterial(Material* material){m_material = material;}
		Material*	GetMaterial(){return m_material;}


	protected:

		Object*		m_object;
		Material*	m_material;
};

class CameraInstance : public Instance
{
	public:
					CameraInstance			(Vector3& eyePos, Vector3& targetPos, float fov = 60.f, float nearz = 0.1f);
		void		Prime					(float aspectRatio = 0.75f);
		void		GetRayForPixel			(float xRatio, float yRatio, Vector3& start, Vector3& dir);

	private:

		float		m_fov;
		float		m_nearZ;
		float		m_aspectRatio;

		Vector3		m_topLeft;
		Vector3		m_topRight;
		Vector3		m_bottomLeft;
		Vector3		m_bottomRight;
};

class LightInstance : public ObjectInstance
{
	public:
							LightInstance(Matrix4 m, Object* obj) : ObjectInstance(m, obj)
							{
							}

		void				GenerateSamplePoints(int numSamples);

		bool				GetSamplePoint(unsigned int sample, Vector3& samplePosition);
		unsigned int		GetNumSamples() { return (unsigned int)m_samplePoints.size(); }
	
	private:

		vector<Vector3>		m_samplePoints;
};
#endif