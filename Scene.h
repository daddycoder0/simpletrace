#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>
#include "Matrix.h"
#include "Instance.h"
#include "Material.h"

using namespace std;

class Object;
class Scene;
class Image;

class Scene
{
	public:
						Scene				()
						{
							m_ambientLight.Set(0.3f, 0.3f, 0.3f);
							m_numActiveThreads = 0;
						}
						virtual ~Scene		()
						{
							m_lights.clear();

							for ( int i = 0; i < m_objects.size(); i++ ) 
							{
								delete m_objects[i];
							}
							m_objects.clear();

							for ( int i = 0; i < m_instances.size(); i++ ) 
							{
								delete m_instances[i];
							}
							m_instances.clear();

							for ( int i = 0; i < m_cameras.size(); i++ ) 
							{
								delete m_cameras[i];
							}
							m_cameras.clear(); 

							for ( int i = 0; i < m_materials.size(); i++ ) 
							{
								delete m_materials[i];
							}

							m_materials.clear();
						}
	
		bool			ParseInput						(char* input, char* scratchPad, int spSize);

		Object*			GetObjectByName					(char* objName);
		Material*		GetMaterialByName				(char* matName);
		char*			LogInfo							();

		bool			Render							(int width = 400, int height = 300, int superSample = 1, int startX = 0, int endX = -1, int startY = 0, int endY = -1);
		float			GetNearestIntersection			(Vector3& start, Vector3& dir, Instance*& instance, unsigned int& triangleIndex, bool bfcull = true);
	
		struct RenderParams_s
		{
			RenderParams_s(){}
			int width;
			int height;
			int superSample;
			int startX;
			int endX;
			int startY;
			int endY;
			float xStep;
			float yStep;
			int xOut;
			int yOut;
			int cameraIndex;
			Image* outImage;
			Scene* scene;
		};

		bool			Render							(RenderParams_s& params);

	private:

		void			GetColourForIntersection		(Vector3& start, Vector3& dir, float t, Instance* instance, unsigned int triangleIndex, Vector3& colOut);

		vector<Object*>				m_objects;
		vector<Instance*>			m_lights;
		vector<Instance*>			m_instances;
		vector<CameraInstance*>		m_cameras;
		vector<Material*>			m_materials;

		Vector3						m_ambientLight;
		int							m_numActiveThreads;
};

void ThreadRender(Scene::RenderParams_s* params);

#endif
