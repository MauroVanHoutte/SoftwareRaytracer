#pragma once
#include "Light.h"

class PointLight : public Light
{
public: 
	PointLight(const Elite::FPoint3& position, const Elite::RGBColor& color, float intensity);
	~PointLight() = default;

	Elite::RGBColor CalculateRadiance(const HitRecord& hitRecord) const override;

	Elite::FVector3 virtual GetDirection(const Elite::FPoint3& target, float distance) const;

	LightType virtual GetLightType() const;

private:
	const Elite::FPoint3 m_Position;
	const Elite::RGBColor m_Color;
	const float m_Intensity;
	const LightType m_Type = LightType::PointLightType;
};