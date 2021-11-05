#pragma once
#include "RenderObject.h"
#include "EMath.h"
#include "EMathUtilities.h"

class TriangleMesh;

enum class CullingMode 
{
	Front,
	Back,
	None
};

class Triangle : public RenderObject
{
public:
	Triangle(Elite::FPoint3* pVertex0, Elite::FPoint3* pVertex1, Elite::FPoint3* pVertex2, Material* material, CullingMode cullingMode, const TriangleMesh* mesh);

	bool Hit(const Ray& ray, HitRecord& hitRecord) const override;
	bool Hit(const Ray& ray) const override;
	const Material* GetMaterial() const override;

	const Elite::FPoint3* m_pVertex0;
	const Elite::FPoint3* m_pVertex1;
	const Elite::FPoint3* m_pVertex2;
private:
	const TriangleMesh* m_pMesh;
	const Elite::FVector3 m_Normal;
	Material* m_pMaterial;
	const CullingMode m_CullingMode;
};