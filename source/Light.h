#pragma once
#include "EMath.h"
#include "EMathUtilities.h"
#include "ERGBColor.h"
#include "HitRecord.h"

enum class LightType
{
	PointLightType,
	DirectionalLightType
};

class Light
{
public:
	Light() = default;
	~Light() = default;

	Elite::RGBColor virtual CalculateRadiance(const HitRecord& hitRecord) const = 0;

	Elite::FVector3 virtual GetDirection(const Elite::FPoint3& target, float distance) const = 0;

	LightType virtual GetLightType() const = 0;
	void ToggleLight();
	bool GetLightStatus() const;

private:
	bool m_IsEnabled = true;
};