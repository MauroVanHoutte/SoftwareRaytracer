#pragma once
#include <vector>

class RenderObject;
class Sphere;
class Plane;
class Triangle;
class TriangleMesh;

class ObjectManager final
{
public:
	static ObjectManager* GetInstance();

	const std::vector<RenderObject*>& GetObjectVector();
	void AddSphere( Sphere* sphere);
	void AddPlane( Plane* plane);
	void AddTriangle(Triangle* triangle);
	void AddTriangleMesh(TriangleMesh* triangleMesh);

	static void Clear();

private:
	ObjectManager();
	~ObjectManager();
	
	void DeleteObjects();

	static ObjectManager* m_Instance;
	std::vector<RenderObject*> m_ObjectVector;
};