#include "Plane.h"

Plane::Plane(const Elite::FPoint3& origin, const Elite::FVector3& normal, Material* pMaterial)
	: m_Origin{origin}
	, m_Normal{normal}
	, m_pMaterial{pMaterial}
{
}

Plane::~Plane()
{
	delete m_pMaterial;
}

bool Plane::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	float t{ Elite::Dot(Elite::FVector3{m_Origin - ray.origin}, m_Normal) / Elite::Dot(ray.direction, m_Normal) }; //distance to intersection point
	if (t > ray.tMin && t < hitRecord.t)
	{
		Elite::FPoint3 pIntersect{ ray.origin + t * ray.direction };
		hitRecord = HitRecord{ true, pIntersect, t, m_Normal, m_pMaterial };
		return true;
	}
	return false;
}

bool Plane::Hit(const Ray& ray) const
{
	float t{ Elite::Dot(Elite::FVector3{m_Origin - ray.origin}, m_Normal) / Elite::Dot(ray.direction, m_Normal) };
	if (t > ray.tMin && t < ray.tMax)
	{
		return true;
	}
	return false;
}

const Material* Plane::GetMaterial() const
{
	return m_pMaterial;
}

