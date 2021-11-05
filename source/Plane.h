#pragma once
#include "RenderObject.h"

class Material;

class Plane : public RenderObject
{
public:
	Plane(const Elite::FPoint3& origin, const Elite::FVector3& normal, Material* pMaterial);
	~Plane();

	bool Hit(const Ray& ray, HitRecord& hitRecord) const override;
	bool Hit(const Ray& ray) const override;

	const Material* GetMaterial() const override;

private:
	const Elite::FPoint3 m_Origin;
	const Elite::FVector3 m_Normal;
	const Elite::RGBColor m_Color;
	Material* m_pMaterial;
};