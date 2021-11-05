#pragma once
#include "Material.h"

class LambertCookTorrance : public Material
{
public:
	LambertCookTorrance(bool metalness, float roughness, const Elite::RGBColor& albedo = {0.04f, 0.04f, 0.04f});
	~LambertCookTorrance() = default;

	Elite::RGBColor Shade(const HitRecord& hitRecord, const Elite::FVector3& incoming, const Elite::FVector3& outgoing) const override;

private:
	const float m_Roughness;
	const Elite::RGBColor m_Albedo;
};