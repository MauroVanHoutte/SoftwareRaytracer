#include "DirectionalLight.h"
#include "EMath.h"
#include "EMathUtilities.h"

DirectionalLight::DirectionalLight(const Elite::FVector3& direction, const Elite::RGBColor& color, float intensity)
    : m_Color{color}
    , m_Intensity{intensity}
	, m_Direction{Elite::GetNormalized(direction)}

{
}

Elite::RGBColor DirectionalLight::CalculateRadiance(const HitRecord& hitRecord) const
{
	return m_Color * m_Intensity;
}

Elite::FVector3 DirectionalLight::GetDirection(const Elite::FPoint3& target, float distance) const
{
	return -m_Direction;
}

LightType DirectionalLight::GetLightType() const
{
	return m_Type;
}
