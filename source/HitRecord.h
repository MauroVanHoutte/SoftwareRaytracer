#pragma once
#include "EMath.h"
#include "EMathUtilities.h"
#include "ERGBColor.h"

class Material;

struct HitRecord
{
	HitRecord() = default;
	HitRecord(bool hit, const Elite::FPoint3& pIntersect, float t, const Elite::FVector3& normal, const Material* pMaterial);

	//Datamembers

	bool hit;
	Elite::FPoint3 pIntersect;
	float t;
	Elite::FVector3 normal;
	const Material* pMaterial;
};


