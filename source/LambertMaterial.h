#pragma once
#include "Material.h"

class LambertMaterial : public Material
{
public:
	LambertMaterial(const Elite::RGBColor& diffuseColor, const Elite::RGBColor& diffuseReflectance);
	~LambertMaterial() = default;

	Elite::RGBColor Shade(const HitRecord& hitRecord, const Elite::FVector3& incoming, const Elite::FVector3& outgoing) const override;

private:
	const Elite::RGBColor m_Color;
	const Elite::RGBColor m_DiffuseReflectance;
};