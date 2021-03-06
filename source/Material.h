#pragma once
#include "EMath.h"
#include "EMathUtilities.h"
#include "ERGBColor.h"
#include "HitRecord.h"

class Material
{
public:
	Material() = default;
	~Material() = default;

	Elite::RGBColor virtual Shade(const HitRecord& hitRecord, const Elite::FVector3& incoming, const Elite::FVector3& outgoing) const = 0;
	bool m_Metal{ false };
	bool m_Reflects{ false };
	bool m_Refracts{ false };
	float m_RefractionIndex{ 1.0f };
};