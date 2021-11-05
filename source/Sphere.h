#pragma once
#include "RenderObject.h"

class Material;

class Sphere : public RenderObject
{
public:
	Sphere(const Elite::FPoint3& origin, float radius, Material* pMaterial);
	~Sphere();

	bool Hit(const Ray& ray, HitRecord& hitRecord) const override;
	bool Hit(const Ray& ray) const override;

	const Material* GetMaterial() const override;

private:
	//Datamembers
	const Elite::FPoint3 m_Origin;
	const float m_Radius;
	Material* m_pMaterial;
};