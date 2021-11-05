#include "Ray.h"

Ray::Ray(Elite::FPoint3 origin, Elite::FVector3 direction)
	: origin{ origin }
	, direction{ GetNormalized(direction) }
{
}
