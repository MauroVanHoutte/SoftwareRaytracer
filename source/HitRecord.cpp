#include "HitRecord.h"



HitRecord::HitRecord(bool hit, const Elite::FPoint3& pIntersect, float t, const Elite::FVector3& normal, const Material* pMaterial)
	:hit{hit}
	, pIntersect{pIntersect}
	, t{t}
	, normal{ normal }
	, pMaterial{pMaterial}
{
}
