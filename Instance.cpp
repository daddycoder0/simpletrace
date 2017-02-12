#include "Objects.h"
#include "Material.h"
#include "Instance.h"

float ObjectInstance::RayIntersects(Vector3& start, Vector3& dir, unsigned int& triangleIndex, bool bfcull)
{
	if (m_object)
	{
		Vector3 localOrigin = m_posrot.InverseTransform(start);
		Vector3 localDir = m_posrot.InverseTransform(start+dir);
		localDir.Sub(localDir, localOrigin);

		return m_object->RayIntersects(localOrigin, localDir, triangleIndex, bfcull);
	}
	return -1.f;
}

void ObjectInstance::GetColourForIntersection(Vector3& start, Vector3& dir, float t, Instance* inst, unsigned int triangleIndex, Vector3& colOut, Vector3& ambientLight, vector<Instance*>* lights, Scene& scene)
{
	if (m_object)
	{
		m_object->GetColourForIntersection(start, dir, t, inst, triangleIndex, colOut, ambientLight, lights, m_material, m_posrot, scene);
	}
}


CameraInstance::CameraInstance(Vector3& eyePos, Vector3& targetPos, float fov, float nearz)
{
	Vector3 camVec = targetPos.Sub(targetPos, eyePos);
	camVec.Normalize();
	Vector3 up(0.f, 1.f, 0.f);
	Vector3 camRight = CrossProduct(camVec, up);
	Vector3 camUp = CrossProduct(camVec, camRight);

	Vector4 x, y, z, w;
	x.Set(camRight, 0.f);
	y.Set(camUp, 0.f);
	z.Set(camVec, 0.f);
	w.Set(eyePos, 1.f);

	m_posrot.Set(x, y, z, w);

	m_fov = fov;
	m_nearZ = nearz;
	m_aspectRatio = 0.75f;
}

void CameraInstance::Prime(float aspectRatio)
{
	m_aspectRatio = aspectRatio;

	Vector3 cameraDir, camRight, camUp, camPos, midPoint;
	m_posrot.GetVectors(camRight, camUp, cameraDir, camPos);

	float fovInRad = m_fov*DEG2RAD;
	
	//yRot.SetRotateY((-(m_fov*DEG2RAD)/2.f)+(m_fov*DEG2RAD*xRatio));
	//zRot.SetRotateZ(((m_fov*m_aspectRatio*DEG2RAD)/2.f)-(m_fov*DEG2RAD*yRatio*m_aspectRatio));

	Matrix4 yRot, zRot;
	yRot.SetRotateY((-(fovInRad)/2.f));
	zRot.SetRotateZ(((fovInRad*m_aspectRatio)/2.f));
	m_topLeft = yRot.Transform(cameraDir);
	m_topLeft = zRot.Transform(m_topLeft);

	yRot.SetRotateY((-(fovInRad)/2.f)+fovInRad);
	m_topRight = yRot.Transform(cameraDir);
	m_topRight = zRot.Transform(m_topRight);

	zRot.SetRotateZ(((fovInRad*m_aspectRatio)/2.f)-(fovInRad*m_aspectRatio));
	m_bottomRight = yRot.Transform(cameraDir);
	m_bottomRight = zRot.Transform(m_bottomRight);

	yRot.SetRotateY((-(fovInRad)/2.f));
	m_bottomLeft = yRot.Transform(cameraDir);
	m_bottomLeft = zRot.Transform(m_bottomLeft);
	
}

void CameraInstance::GetRayForPixel(float xRatio, float yRatio, Vector3& start, Vector3& dir)
{
	Vector3 camPos = m_posrot.Pos();
	Vector3 top = InterpolateVectors(m_topLeft, m_topRight, xRatio);
	Vector3 bottom = InterpolateVectors(m_bottomLeft, m_bottomRight, xRatio);
	Vector3 target = InterpolateVectors(top, bottom, 1.f-yRatio);
	dir = target;
	dir.Normalize();
	Vector3 camDir(dir);
	start = camDir.Mul(camDir, m_nearZ);
	start = start + camPos;
}
