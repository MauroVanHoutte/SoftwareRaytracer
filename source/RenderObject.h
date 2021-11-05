#pragma once
#include "Ray.h"
#include "HitRecord.h"

class RenderObject
{
public:
	RenderObject();
	virtual ~RenderObject();

	virtual bool Hit(const Ray& ray, HitRecord& hitRecord) const = 0;
	virtual bool Hit(const Ray& ray) const = 0;
	virtual const Material* GetMaterial() const = 0;
};

