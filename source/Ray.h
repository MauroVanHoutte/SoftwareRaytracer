#pragma once
#include "EMath.h"
#include "EMathUtilities.h"

struct Ray
{
	Ray() = default;
	Ray(Elite::FPoint3 origin, Elite::FVector3 direction);


	//Datamembers
	Elite::FPoint3 origin = {};
	Elite::FVector3 direction = {};
	float tMin = 0.0001f;
	float tMax = FLT_MAX;
};

