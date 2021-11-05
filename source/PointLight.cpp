#include "PointLight.h"

PointLight::PointLight(const Elite::FPoint3& position, const Elite::RGBColor& color, float intensity)
	: m_Position{ position }
	, m_Color{ color }
	, m_Intensity{ intensity }	
{
}

Elite::RGBColor PointLight::CalculateRadiance(const HitRecord& hitRecord) const
{
	Elite::FVector3 lightToTarget { m_Position - hitRecord.pIntersect};
	return m_Color*(m_Intensity / Elite::SqrMagnitude(lightToTarget));
}

Elite::FVector3 PointLight::GetDirection(const Elite::FPoint3& target, float distance) const
{
	return {(m_Position - target)/distance};
}

LightType PointLight::GetLightType() const
{
	return m_Type;
}
