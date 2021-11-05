#pragma once
#include "Light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight(const Elite::FVector3& direction, const Elite::RGBColor& color, float intensity);
	~DirectionalLight() = default;

	Elite::RGBColor CalculateRadiance(const HitRecord& hitRecord) const override;

	Elite::FVector3 virtual GetDirection(const Elite::FPoint3& target, float distance) const;

	LightType virtual GetLightType() const;

private:
	const Elite::FVector3 m_Direction;
	const Elite::RGBColor m_Color;
	const float m_Intensity;
	const LightType m_Type = LightType::DirectionalLightType;
};