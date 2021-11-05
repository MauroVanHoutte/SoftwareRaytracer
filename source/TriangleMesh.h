#pragma once
#include "RenderObject.h"
#include "Triangle.h"
#include <vector>
#include <string>
#include "EMath.h"
#include "EMathUtilities.h"
#include "OBJReader.h"

struct BoundingBox
{
	Elite::FPoint3 maxPoint;
	Elite::FPoint3 minPoint;
};

class TriangleMesh : public RenderObject
{
public:
	TriangleMesh( const std::string& path, Material* pMaterial, const Elite::FPoint3& pos, const Elite::FVector3& forward, CullingMode cullingMode );
	~TriangleMesh();

	bool Hit(const Ray& ray, HitRecord& hitRecord) const override;
	bool Hit(const Ray& ray) const override;

	bool TriangleHit(const Ray& ray, HitRecord& hitRecord, const Face& face) const;
	bool TriangleHit(const Ray& ray, const Face& face) const;

	void Rotate(float elapsedSec);
	void RecalculateTranformation();
	const Material* GetMaterial() const override;
	const Elite::FMatrix4& GetTranformationMatrix() const;

private:
	void AssembleTriangles(const std::vector<Elite::FPoint3>& vertices, const std::vector<Face> faces);
	void CreateBoundingBox();
	bool BoundingBoxIntersect(const Ray& ray) const;
	Elite::FVector3 m_Forward;
	Elite::FPoint3 m_Position;
	Elite::FMatrix4 m_TranformationMatrix;
	Material* m_pMaterial;
	std::vector<Elite::FPoint3> m_VertexBuffer;
	std::vector<Elite::FPoint3> m_VertexBufferTransformed;
	std::vector<Face> m_VertexIndexBuffer;
	const CullingMode m_CullingMode;
	BoundingBox m_BoundingBox;
};