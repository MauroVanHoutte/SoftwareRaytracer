#include "Triangle.h"
#include "TriangleMesh.h"

Triangle::Triangle(Elite::FPoint3* pVertex0, Elite::FPoint3* pVertex1, Elite::FPoint3* pVertex2, Material* material, CullingMode cullingMode, const TriangleMesh* mesh = nullptr)
	: m_pVertex0{pVertex0}
	, m_pVertex1{pVertex1}
	, m_pVertex2{pVertex2}
	, m_pMaterial{material}
	, m_CullingMode{cullingMode}
	, m_pMesh{mesh}
	, m_Normal{ Elite::GetNormalized(Elite::Cross(*m_pVertex1 - *m_pVertex0, *m_pVertex2 - *m_pVertex0)) }
{
}

bool Triangle::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	Elite::FVector3 vertex0{*m_pVertex0};
	Elite::FVector3 vertex1{*m_pVertex1};
	Elite::FVector3 vertex2{*m_pVertex2};
	Elite::FVector3 normal{ m_Normal };
	
	float dotNormalView{ Elite::Dot(normal, ray.direction) };

	switch (m_CullingMode)
	{
	case CullingMode::Front:
		if (dotNormalView < 0)
			return false;
		break;
	case CullingMode::Back:
		if (dotNormalView > 0)
			return false;
		break;
	}
	if (dotNormalView == 0)
		return false;

	Elite::FPoint3 center;
	center.x = (vertex0.x + vertex1.x + vertex2.x) / 3.f;
	center.y= (vertex0.y + vertex1.y + vertex2.y) / 3.f;
	center.z = (vertex0.z + vertex1.z + vertex2.z) / 3.f;
	float t = Elite::Dot({center-ray.origin}, normal) / Elite::Dot(ray.direction, normal);
	if ( t < ray.tMin || t > hitRecord.t )
		return false;

	Elite::FPoint3 pIntersect = ray.origin + t * ray.direction;

	Elite::FVector3 edge{ vertex1 - vertex0 };
	Elite::FVector3 vertex0ToIntersect{ pIntersect - vertex0 };
	if (Elite::Dot(normal, Elite::Cross(edge, vertex0ToIntersect)) < 0)
		return false;

	Elite::FVector3 vertex1ToIntersect{ pIntersect - vertex1 };
	edge = vertex2 - vertex1;
	if (Elite::Dot(normal, Elite::Cross(edge, vertex1ToIntersect)) < 0)
		return false;

	Elite::FVector3 vertex2ToIntersect{ pIntersect - vertex2 };
	edge = vertex0 - vertex2;
	if (Elite::Dot(normal, Elite::Cross(edge, vertex2ToIntersect)) < 0)
		return false;

	hitRecord = HitRecord{ true, pIntersect, t, normal, m_pMaterial };
	if (dotNormalView > 0) //making sure the normal is going towards the camera, otherwise the triangle will be black due to the lambert cosine law
	{
		hitRecord.normal *= -1;
	}
	return true;
}

bool Triangle::Hit(const Ray& ray) const
{
	Elite::FVector3 vertex0{ *m_pVertex0 };
	Elite::FVector3 vertex1{ *m_pVertex1 };
	Elite::FVector3 vertex2{ *m_pVertex2 };
	Elite::FVector3 normal{ m_Normal };
	

	float dotNormalView{ Elite::Dot(normal, ray.direction) };
	switch (m_CullingMode)
	{
	case CullingMode::Front:
		if (dotNormalView > 0)
			return false;
		break;
	case CullingMode::Back:
		if (dotNormalView < 0)
			return false;
		break;
	}
	if (dotNormalView == 0)
		return false;

	Elite::FPoint3 center;
	center.x = (vertex0.x + vertex1.x + vertex2.x) / 3.f;
	center.y = (vertex0.y + vertex1.y + vertex2.y) / 3.f;
	center.z = (vertex0.z + vertex1.z + vertex2.z) / 3.f;
	float t = Elite::Dot({ center - ray.origin }, normal) / Elite::Dot(ray.direction, normal);
	if (t < ray.tMin || t > ray.tMax)
		return false;

	Elite::FPoint3 pIntersect = ray.origin + t * ray.direction;

	Elite::FVector3 edge{ vertex1 - vertex0 };
	Elite::FVector3 vertex0ToIntersect{ pIntersect - vertex0 };
	if (Elite::Dot(normal, Elite::Cross(edge, vertex0ToIntersect)) < 0)
		return false;

	Elite::FVector3 vertex1ToIntersect{ pIntersect - vertex1 };
	edge = vertex2 - vertex1;
	if (Elite::Dot(normal, Elite::Cross(edge, vertex1ToIntersect)) < 0)
		return false;

	Elite::FVector3 vertex2ToIntersect{ pIntersect - vertex2 };
	edge = vertex0 - vertex2;
	if (Elite::Dot(normal, Elite::Cross(edge, vertex2ToIntersect)) < 0)
		return false;
	
	return true;
}

const Material* Triangle::GetMaterial() const
{
	return m_pMaterial;
}
