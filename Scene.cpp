#include <iostream>
#include <iomanip>
#include <string.h>
#include <time.h>
#if defined _WIN32
	#include <Windows.h>
	#define CPSleep(a) Sleep(a)
#else
	#include <unistd.h>
	#define CPSleep(a) usleep(a*1000)
#endif
#include "ThirdParty/rapidxml/rapidxml.hpp"
#include "ThirdParty/tinythread/tinythread.h"

#include "Objects.h"
#include "Image.h"
#include "Instance.h"
#include "Scene.h"

void ThreadRender(void* params)
{
	((Scene::RenderParams_s*)params)->scene->Render(*(Scene::RenderParams_s*)params);
}

bool Scene::ParseInput(char* input, char* scratchPad, int spSize)
{
	using namespace rapidxml;
	xml_document<> doc;    // character type defaults to char
	
	try
	{
		doc.parse<0>(input);    // 0 means default parse flags
	}
	catch (const rapidxml::parse_error& e)
	{
		cout << "Problem with input: " << e.what() << endl;
		return false;
	}
	catch (...)
	{
		cout << "Unknown error parsing input file" << endl;
		return false;
	}

	xml_node<> *node = doc.first_node("objects");

	if (!node)
	{
		cout << "No objects found in input file." << endl;
		return false;
	}
	for (xml_node<> *nd = node->first_node(); nd; nd = nd->next_sibling())
	{
		xml_attribute<> *attr = nd->first_attribute("type");

		if (!attr)
		{
			cout << "Object missing a type!" << endl;
			return false;
		}
		
		if (strcmp(attr->value(), "simplemesh") == 0)
		{
			attr = nd->first_attribute("name");
			SimpleMesh* sm = new SimpleMesh(attr ? attr->value() : "unknown");
			if (!sm->Parse(nd, scratchPad, spSize))
			{
				delete sm;
			}
			else
			{
				m_objects.push_back(sm);
			}
		}
	
		else if (strcmp(attr->value(), "sphere") == 0)
		{
			attr = nd->first_attribute("name");
			Sphere* sphere = new Sphere(attr ? attr->value() : "unknown");
			if (!sphere->Parse(nd, scratchPad, spSize))
			{
				delete sphere;
			}
			else
			{
				m_objects.push_back(sphere);
			}
		}
		else
		{
			cout << "Unknown object type: " << attr->value() << endl;
			return false;
		}
	}

	node = doc.first_node("materials");

	if (!node)
	{
		cout << "No materials found in input file." << endl;
		return false;
	}
	for (xml_node<> *nd = node->first_node(); nd; nd = nd->next_sibling())
	{
		xml_attribute<> *attr = nd->first_attribute("name");
		Material* m = new Material(attr ? attr->value() : "unknown");
		m->Parse(nd, scratchPad, spSize);
		m_materials.push_back(m);
	}

	node = doc.first_node("scene");
	if (!node)
	{
		cout << "No scenes found in input file." << endl;
		return false;
	}

	for (xml_node<> *nd = node->first_node(); nd; nd = nd->next_sibling())
	{
		if (strcmp(nd->name(), "inst") == 0)
		{
			xml_attribute<> *attr = nd->first_attribute("type");

			if (!attr)
			{
				cout << "Instance missing a type!" << endl;
			}
			else	
			{
				if (strcmp(attr->value(), "simplemesh") == 0 || strcmp(attr->value(), "sphere") == 0)
				{
					attr = nd->first_attribute("obj");

					if (!attr)
					{
						cout << "Instance has no object ref!" << endl;
					}
					else
					{
						Object* o = GetObjectByName(attr->value());
						if (!o)
						{
							cout << "Could not find object: " << attr->value() << endl;
						}
						else
						{
							xml_attribute<>* ma = nd->first_attribute("material");
							Material* mat = ma ? GetMaterialByName(ma->value()) : NULL;
							Matrix4 m;
							m.Parse(nd->first_node("matrix"), scratchPad, spSize);
							ObjectInstance* oi = new ObjectInstance(m, o);
							oi->SetMaterial(mat);
							m_instances.push_back(oi);

							if (nd->first_attribute("light"))
							{
								m_lights.push_back(oi);
							}
							else
							{
								// not drawing lights for now (todo)
								//m_instances.push_back(oi);
							}
						}
					}
				}
				else if (strcmp(attr->value(), "camera") == 0)
				{
					attr = nd->first_attribute("fov");
					float fov = attr ? (float)atof(attr->value()) : 60.f;
					attr = nd->first_attribute("nearz");
					float nearz = attr ? (float)atof(attr->value()) : 0.1f;

					Vector3 eye, target;
					eye.Parse(nd->first_node("eye"), scratchPad, spSize);
					target.Parse(nd->first_node("target"), scratchPad, spSize);
					m_cameras.push_back(new CameraInstance(eye, target, fov, nearz));
				}
			}
		}
		else if (strcmp(nd->name(), "ambient") == 0)
		{
			m_ambientLight.Parse(nd->first_node("colour"), scratchPad, spSize);
		}
		else
		{
			cout << "Unknown scene entry: " << nd->name() << endl;
		}
	}
	return true;
}

Object* Scene::GetObjectByName(char* objName)
{
	for (unsigned int i=0;i<m_objects.size(); i++)
	{
		if (m_objects[i] && m_objects[i]->Is(objName))
		{
			return m_objects[i];
		}
	}
	return NULL;
}

Material* Scene::GetMaterialByName(char* matName)
{
	for (unsigned int i=0;i<m_materials.size(); i++)
	{
		if (m_materials[i] && m_materials[i]->Is(matName))
		{
			return m_materials[i];
		}
	}
	return NULL;
}

char* Scene::LogInfo()
{
	static char sceneInfo[512];
	snprintf(sceneInfo, 512, "\tNum objects:\t%d\n\tNum instances:\t%d\n\tNum cameras:\t%d\n\tNum materials:\t%d\n", m_objects.size(), m_instances.size(), m_cameras.size(), m_materials.size());
	return sceneInfo;
}

bool Scene::Render(int width, int height, int superSample, int startX, int endX, int startY, int endY)
{
	if (endX < 0)
	{
		endX = width;	
	}
	if (endY < 0)
	{
		endY = height;	
	}
	if (endX > width || endY > height)
	{
		cout << "Bad params passed to render" << endl;
		return false;
	}
	
	int viewW = endX-startX;
	int viewH = endY-startY;
	const int bpp = 3;

	int numThreads = tthread::thread::hardware_concurrency() <= 32 ? tthread::thread::hardware_concurrency() : 32;

	Image outImage(viewW, viewH, bpp);

	for (unsigned int i=0;i<m_cameras.size();i++)
	{
		time_t start,end;
		time(&start);
		m_cameras[i]->Prime((float)height/float(width));
		float xStep = 1.f/(float)width/((float)superSample+1.f);
		float yStep = 1.f/(float)height/((float)superSample+1.f);


		int xOut = 0;
		int yOut = 0;

		RenderParams_s params;
		params.xOut = 0;
		params.yOut = 0;
		params.xStep = xStep;
		params.yStep = yStep;
		params.height = height;
		params.width = width;
		params.startX = startX;
		params.endX = endX;
		params.startY = startY;
		params.endY = endY;
		params.superSample = superSample;
		params.outImage = &outImage;
		params.scene = this;
		params.cameraIndex = i;
		

		tthread::thread* threads[32];
		RenderParams_s paramList[32];
		
		for (int t=0; t<numThreads; t++)
		{
			paramList[t] = params;
			int tHeight = paramList[t].height / numThreads;
			paramList[t].startY = params.startY + (t * tHeight);
			paramList[t].endY = paramList[t].startY + tHeight;
			paramList[t].yOut = t * tHeight;
		}
		
		int activeThreads = 0;

		for (int t=0; t<numThreads; t++)
		{
			m_numActiveThreads++;
			threads[activeThreads] = new tthread::thread(ThreadRender, (void*)&paramList[activeThreads]);
			activeThreads++;
		}

		while (m_numActiveThreads > 0)
		{
			CPSleep(10);
		}

		for (int t=0;t<activeThreads;t++)
		{
			if (threads[t]->joinable())
			{
				threads[t]->join();
			}
			delete threads[t];
		}

		char fileName[64];
		snprintf(fileName, 64, "camera%03d.bmp", i);
		outImage.SaveImage(fileName);

		time (&end);
		double dif = difftime (end,start);
		cout << "View " << i << " took " <<  setprecision(2) << dif << " second(s)." << endl;
	
	}
	
	return true;
}

bool Scene::Render(RenderParams_s& params)
{
	float ss = (float)params.superSample * (float)params.superSample;

	for (int y=params.startY;y<params.endY;y++)
	{
		for (int x=params.startX;x<params.endX;x++)
		{
			Vector3 colTotal(0.f, 0.f, 0.f);
			for (int sx=0;sx<params.superSample;sx++)
			{
				for (int sy=0;sy<params.superSample;sy++)
				{
					Vector3 start, dir;
					m_cameras[params.cameraIndex]->GetRayForPixel(((float)x/(float)params.width)+(sx*params.xStep), ((float)y/(float)params.height)+(sy*params.yStep), start, dir);

					Vector3 colour(0.f, 0.f, 0.f);

					Instance* inst = NULL;
					unsigned int triangleIndex = 0;
					float t = GetNearestIntersection(start, dir, inst, triangleIndex); 
					if (t >= 0.f)
					{
						GetColourForIntersection(start, dir, t, inst, triangleIndex, colour);
						colTotal = colTotal.Add(colTotal, colour);
					}
				}
			}
				
			params.outImage->SetPixelColour(params.xOut++, params.yOut, colTotal.m_x/ss, colTotal.m_y/ss, colTotal.m_z/ss);
		}

		params.xOut = 0;
		params.yOut++;
	}

	m_numActiveThreads--;
	return true;
}


float Scene::GetNearestIntersection(Vector3& start, Vector3& dir, Instance*& instance, unsigned int& triangleIndex, bool bfcull)
{
	float nearestT = -1.f;
	instance = NULL;

	for (unsigned int i=0;i<m_instances.size();i++)
	{
		float t = m_instances[i]->RayIntersects(start, dir, triangleIndex, bfcull);
	
		if (t > 0.f && (nearestT < 0.f || t < nearestT))
		{
			instance = m_instances[i];
			nearestT = t;
		}
	}

	return nearestT;
}

void Scene::GetColourForIntersection(Vector3& start, Vector3& dir, float t, Instance* instance, unsigned int triangleIndex, Vector3& colOut)
{
	if (instance)
	{
		instance->GetColourForIntersection(start, dir, t, instance, triangleIndex, colOut, m_ambientLight, &m_lights, *this);
	}
}
