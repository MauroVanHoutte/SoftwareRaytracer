#include "Sphere.h"

Sphere::Sphere(const Elite::FPoint3& origin, float radius, Material* pMaterial)
	: m_Origin{origin}
	, m_Radius{radius}
	, m_pMaterial{pMaterial}
{
}

Sphere::~Sphere()
{
	delete m_pMaterial;
}

bool Sphere::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	//Analytic intersection at² + bt + c = 0
	
	// (d.d)t² + (2d.(Oray - Osphere)) + ((Oray - Osphere).(Oray - Osphere)) - r² = 0


	float argumentA{ Elite::SqrMagnitude(ray.direction) }; // SqrMagnitude = dot product with itself
	float argumentB{ Elite::Dot(2 * ray.direction, Elite::FVector3(ray.origin - m_Origin)) };
	float argumentC{ Elite::SqrMagnitude(Elite::FVector3(ray.origin - m_Origin)) - m_Radius * m_Radius };

	float Discriminant{ argumentB * argumentB - 4 * argumentA * argumentC }; // D = b² -4ac
	if (Discriminant > 0)
	{
		float t = (-argumentB - sqrtf(Discriminant)) / (2 * argumentA);
		if (ray.tMin < t && t < hitRecord.t)
		{
			Elite::FPoint3 pIntersect{ ray.origin + t * ray.direction };
			hitRecord = HitRecord{ true, pIntersect, t, (pIntersect-m_Origin)/m_Radius, m_pMaterial };
			return true;
		}
		t = (-argumentB + sqrtf(Discriminant)) / (2 * argumentA);
		if (ray.tMin < t && t < hitRecord.t)
		{
			Elite::FPoint3 pIntersect{ ray.origin + t * ray.direction };
			hitRecord = HitRecord{ true, pIntersect, t, (pIntersect - m_Origin) / m_Radius, m_pMaterial };
			return true;
		}
	}
	return false;

}

bool Sphere::Hit(const Ray& ray) const
{
	float argumentA{ Elite::SqrMagnitude(ray.direction) }; // SqrMagnitude = dot product with itself
	float argumentB{ Elite::Dot(2 * ray.direction, Elite::FVector3(ray.origin - m_Origin)) };
	float argumentC{ Elite::SqrMagnitude(Elite::FVector3(ray.origin - m_Origin)) - m_Radius * m_Radius };

	float Discriminant{ argumentB * argumentB - 4 * argumentA * argumentC }; // D = b² -4ac
	if (Discriminant > 0)
	{
		float t = (-argumentB - sqrtf(Discriminant)) / (2 * argumentA);
		if (ray.tMin < t && t < ray.tMax)
		{
			return true;
		}
	}
	return false;
}

const Material* Sphere::GetMaterial() const
{
	return m_pMaterial;
}
