#pragma once
#include "Material.h"

class LambertPhong : public Material
{
public:
	LambertPhong(const Elite::RGBColor& diffuseReflectance, const Elite::RGBColor& diffuseColor, float specularReflectance, float phongExponent);
	~LambertPhong() = default;

	Elite::RGBColor Shade(const HitRecord& hitRecord, const Elite::FVector3& incoming, const Elite::FVector3& outgoing) const override;

private:
	const Elite::RGBColor m_DiffuseReflectance;
	const Elite::RGBColor m_DiffuseColor;
	const float m_SpecularReflectance;
	const float m_PhongExponent;
};