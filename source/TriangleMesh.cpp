#include "TriangleMesh.h"
#include "OBJReader.h"
#include "Material.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <iostream>

void TriangleMesh::AssembleTriangles(const std::vector<Elite::FPoint3>& vertices, std::vector<Face> faces)
{

    m_VertexIndexBuffer.clear();
    m_VertexBuffer.clear();
    m_VertexIndexBuffer.reserve(faces.size());
    m_VertexBuffer.reserve(vertices.size());
    for (Face& face : faces)
    {
        for (size_t i = 0; i < face.nrVerts; i++)
        {
            auto itVert{ std::find(m_VertexBuffer.begin(), m_VertexBuffer.end(), vertices[face.vertIndices[i]]) };
            if (itVert == m_VertexBuffer.end()) //vertex is not in the buffer yet
            {
                m_VertexBuffer.push_back(vertices[face.vertIndices[i]]);
                face.vertIndices[i] = int(m_VertexBuffer.size() - 1);
            }
            else
            {
                face.vertIndices[i] = int(itVert - m_VertexBuffer.begin()); //vertex already in the buffer so use that one instead of adding a new one
            }
        }
        face.normalTransformed = face.normal = Elite::GetNormalized(Elite::Cross(m_VertexBuffer[face.vertIndices[1]] - m_VertexBuffer[face.vertIndices[0]], m_VertexBuffer[face.vertIndices[2]] - m_VertexBuffer[face.vertIndices[0]]));
        m_VertexIndexBuffer.push_back(face);
    }
    m_VertexBuffer.shrink_to_fit();
}

void TriangleMesh::CreateBoundingBox()
{
    auto upperX = std::max_element(m_VertexBufferTransformed.begin(), m_VertexBufferTransformed.end(), [](const Elite::FPoint3& left, const Elite::FPoint3& right)
        {
            return left.x < right.x;
        });
    auto lowerX = std::max_element(m_VertexBufferTransformed.begin(), m_VertexBufferTransformed.end(), [](const Elite::FPoint3& left, const Elite::FPoint3& right)
        {
            return left.x > right.x;
        });
    auto upperY = std::max_element(m_VertexBufferTransformed.begin(), m_VertexBufferTransformed.end(), [](const Elite::FPoint3& left, const Elite::FPoint3& right)
        {
            return left.y < right.y;
        });
    auto lowerY = std::max_element(m_VertexBufferTransformed.begin(), m_VertexBufferTransformed.end(), [](const Elite::FPoint3& left, const Elite::FPoint3& right)
        {
            return left.y > right.y;
        });
    auto upperZ = std::max_element(m_VertexBufferTransformed.begin(), m_VertexBufferTransformed.end(), [](const Elite::FPoint3& left, const Elite::FPoint3& right)
        {
            return left.z < right.z;
        });
    auto lowerZ = std::max_element(m_VertexBufferTransformed.begin(), m_VertexBufferTransformed.end(), [](const Elite::FPoint3& left, const Elite::FPoint3& right)
        {
            return left.z > right.z;
        });

    m_BoundingBox.maxPoint.x = upperX->x;
    m_BoundingBox.maxPoint.y = upperY->y;
    m_BoundingBox.maxPoint.z = upperZ->z;

    m_BoundingBox.minPoint.x = lowerX->x;
    m_BoundingBox.minPoint.y = lowerY->y;
    m_BoundingBox.minPoint.z = lowerZ->z; //points defining a box around all vertices
}

bool TriangleMesh::BoundingBoxIntersect(const Ray& ray) const //box intersect formula from scratchapixel.com
{
    float tx0 = (m_BoundingBox.minPoint.x - ray.origin.x) / ray.direction.x; //distance to intersect on x axis
    float tx1 = (m_BoundingBox.maxPoint.x - ray.origin.x) / ray.direction.x;

    if (tx0 > tx1) //swap when the close intersect is further then the far intersect
    {
        tx0 += tx1;
        tx1 = tx0 - tx1;
        tx0 -= tx1;
    }

    float ty0 = (m_BoundingBox.minPoint.y - ray.origin.y) / ray.direction.y;
    float ty1 = (m_BoundingBox.maxPoint.y - ray.origin.y) / ray.direction.y;

    if (ty0 > ty1)
    {
        ty0 += ty1;
        ty1 = ty0 - ty1;
        ty0 -= ty1;
    }

    if (tx0 > ty1 || ty0 > tx1) //ray cant intersect with the box
        return false;

    float tFirstIntersect{ (tx0 > ty0) ? tx0 : ty0 }; //for the first intersection, the biggest tvalue will be the actual intersect point 
    float tSecondIntersect{ (tx1 > ty1) ? ty1 : tx1 };//for the second intersection the smallest tvalue will be the actual intersect point

    float tz0 = (m_BoundingBox.minPoint.z - ray.origin.z) / ray.direction.z;
    float tz1 = (m_BoundingBox.maxPoint.z - ray.origin.z) / ray.direction.z;

    if (tz0 > tz1)
    {
        tz0 += tz1;
        tz1 = tz0 - tz1;
        tz0 -= tz1;
    }

    if (tz0 > tSecondIntersect || tFirstIntersect > tz1) //ray cant intersect with the box
        return false;

    return true;
}


TriangleMesh::TriangleMesh(const std::string& path, Material* pMaterial, const Elite::FPoint3& pos, const Elite::FVector3& forward, CullingMode cullingMode)
    : m_pMaterial{pMaterial}
    , m_Forward{forward}
    , m_Position{pos}
    , m_CullingMode{cullingMode}
{
    std::vector<Elite::FPoint3> vertices;
    std::vector<Face> faces;
    OBJReader::GetInstance()->ReadOBJ(vertices, faces, path);
    AssembleTriangles(vertices, faces);
    m_VertexBufferTransformed.resize(m_VertexBuffer.size());
    RecalculateTranformation();
}

TriangleMesh::~TriangleMesh()
{
    delete m_pMaterial;
}

bool TriangleMesh::Hit(const Ray& ray, HitRecord& hitRecord) const
{
    if (BoundingBoxIntersect(ray))
    {
        for (const Face& face : m_VertexIndexBuffer)
        {
            TriangleHit(ray, hitRecord, face);
        }
    }
    return hitRecord.hit;
}

bool TriangleMesh::Hit(const Ray& ray) const
{
    if (BoundingBoxIntersect(ray))
    {
        for (const Face& face : m_VertexIndexBuffer)
        {
            if (TriangleHit(ray, face))
                return true;
        }
    }
    return false;
}

bool TriangleMesh::TriangleHit(const Ray& ray, HitRecord& hitRecord, const Face& face) const
{
    Elite::FVector3 vertex0{ m_VertexBufferTransformed[face.vertIndices[0]] };
    Elite::FVector3 vertex1{ m_VertexBufferTransformed[face.vertIndices[1]] };
    Elite::FVector3 vertex2{ m_VertexBufferTransformed[face.vertIndices[2]] };
    Elite::FVector3 normal{ face.normalTransformed };
    
    float dotNormalView{ Elite::Dot(normal, ray.direction) };

    switch (m_CullingMode)
    {
    case CullingMode::Front:
        if (dotNormalView < 0)
            return false;
        break;
    case CullingMode::Back:
        if (dotNormalView > 0)
            return false;
        break;
    }
    if (dotNormalView == 0)
        return false;

    Elite::FPoint3 center;
    center.x = (vertex0.x + vertex1.x + vertex2.x) / 3.f;
    center.y = (vertex0.y + vertex1.y + vertex2.y) / 3.f;
    center.z = (vertex0.z + vertex1.z + vertex2.z) / 3.f;
    float t = Elite::Dot({ center - ray.origin }, normal) / Elite::Dot(ray.direction, normal);
    if (t < ray.tMin || t > hitRecord.t)
        return false;

    Elite::FPoint3 pIntersect = ray.origin + t * ray.direction;

    Elite::FVector3 edge{ vertex1 - vertex0 };
    Elite::FVector3 vertex0ToIntersect{ pIntersect - vertex0 };
    if (Elite::Dot(normal, Elite::Cross(edge, vertex0ToIntersect)) < 0)
        return false;

    Elite::FVector3 vertex1ToIntersect{ pIntersect - vertex1 };
    edge = vertex2 - vertex1;
    if (Elite::Dot(normal, Elite::Cross(edge, vertex1ToIntersect)) < 0)
        return false;

    Elite::FVector3 vertex2ToIntersect{ pIntersect - vertex2 };
    edge = vertex0 - vertex2;
    if (Elite::Dot(normal, Elite::Cross(edge, vertex2ToIntersect)) < 0)
        return false;

    hitRecord = HitRecord{ true, pIntersect, t, normal, m_pMaterial };
    if (dotNormalView > 0) //making sure the normal is going towards the camera, otherwise the triangle will be black due to the lambert cosine law
    {
        hitRecord.normal *= -1;
    }
    return true;
}

bool TriangleMesh::TriangleHit(const Ray& ray, const Face& face) const
{
    Elite::FVector3 vertex0{ m_VertexBufferTransformed[face.vertIndices[0]] };
    Elite::FVector3 vertex1{ m_VertexBufferTransformed[face.vertIndices[1]] };
    Elite::FVector3 vertex2{ m_VertexBufferTransformed[face.vertIndices[2]] };
    Elite::FVector3 normal{ face.normalTransformed };

    float dotNormalView{ Elite::Dot(normal, ray.direction) };

    switch (m_CullingMode)
    {
    case CullingMode::Front:
        if (dotNormalView > 0)
            return false;
        break;
    case CullingMode::Back:
        if (dotNormalView < 0)
            return false;
        break;
    }
    if (dotNormalView == 0)
        return false;

    Elite::FPoint3 center;
    center.x = (vertex0.x + vertex1.x + vertex2.x) / 3.f;
    center.y = (vertex0.y + vertex1.y + vertex2.y) / 3.f;
    center.z = (vertex0.z + vertex1.z + vertex2.z) / 3.f;
    float t = Elite::Dot({ center - ray.origin }, normal) / Elite::Dot(ray.direction, normal);
    if (t < ray.tMin || t > ray.tMax)
        return false;

    Elite::FPoint3 pIntersect = ray.origin + t * ray.direction;

    Elite::FVector3 edge{ vertex1 - vertex0 };
    Elite::FVector3 vertex0ToIntersect{ pIntersect - vertex0 };
    if (Elite::Dot(normal, Elite::Cross(edge, vertex0ToIntersect)) < 0)
        return false;

    Elite::FVector3 vertex1ToIntersect{ pIntersect - vertex1 };
    edge = vertex2 - vertex1;
    if (Elite::Dot(normal, Elite::Cross(edge, vertex1ToIntersect)) < 0)
        return false;

    Elite::FVector3 vertex2ToIntersect{ pIntersect - vertex2 };
    edge = vertex0 - vertex2;
    if (Elite::Dot(normal, Elite::Cross(edge, vertex2ToIntersect)) < 0)
        return false;

    return true;
}

void TriangleMesh::Rotate(float elapsedSec)
{
    m_Forward = Elite::MakeRotationY(Elite::ToRadians(45.f*elapsedSec)) * m_Forward;
    RecalculateTranformation();
}

void TriangleMesh::RecalculateTranformation()
{
    Elite::FVector3 right{ Elite::Cross({0.f,1.f,0.f}, m_Forward) };
    Elite::FVector3 up{ Elite::Cross(m_Forward, right) };

    m_TranformationMatrix[0] = { right, 0 };
    m_TranformationMatrix[1] = { up, 0 };
    m_TranformationMatrix[2] = { m_Forward, 0 };
    m_TranformationMatrix[3] = Elite::FVector4{ Elite::FVector3{m_Position}, 1.f };

    for (size_t i = 0; i < m_VertexBuffer.size(); i++)
    {
        Elite::FPoint4 vert{ m_VertexBuffer[i].x, m_VertexBuffer[i].y, m_VertexBuffer[i].z, 1 }; //tranforming all vertices with new tranformation matrix
        vert = m_TranformationMatrix * vert;
        m_VertexBufferTransformed[i].x = vert.x;
        m_VertexBufferTransformed[i].y = vert.y;
        m_VertexBufferTransformed[i].z = vert.z;
    }
    for (size_t i = 0; i < m_VertexIndexBuffer.size(); i++)
    {
        m_VertexIndexBuffer[i].normalTransformed = Elite::FVector3{ m_TranformationMatrix * Elite::FVector4{ m_VertexIndexBuffer[i].normal, 0} };
    }
    CreateBoundingBox();
}

const Material* TriangleMesh::GetMaterial() const
{
    return m_pMaterial;
}

const Elite::FMatrix4& TriangleMesh::GetTranformationMatrix() const
{
    return m_TranformationMatrix;
}


