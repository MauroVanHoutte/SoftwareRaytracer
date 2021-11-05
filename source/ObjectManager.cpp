#include "ObjectManager.h"
#include "RenderObject.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "TriangleMesh.h"

ObjectManager* ObjectManager::m_Instance = nullptr;

ObjectManager::ObjectManager()
{
}

ObjectManager* ObjectManager::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new ObjectManager();
	}
	return m_Instance;
}

const std::vector<RenderObject*>& ObjectManager::GetObjectVector()
{
	return m_ObjectVector;
}

void ObjectManager::AddSphere( Sphere* sphere)
{
	m_ObjectVector.push_back(sphere);
}

void ObjectManager::AddPlane( Plane* plane)
{
	m_ObjectVector.push_back(plane);
}

void ObjectManager::AddTriangle(Triangle* triangle)
{
	m_ObjectVector.push_back(triangle);
}

void ObjectManager::AddTriangleMesh(TriangleMesh* triangleMesh)
{
	m_ObjectVector.push_back(triangleMesh);
}

void ObjectManager::Clear()
{
	if (m_Instance != nullptr)
	{
		m_Instance->DeleteObjects();
	}
	delete m_Instance;
}

void ObjectManager::DeleteObjects()
{
	for (RenderObject* object : m_ObjectVector)
	{
		delete object;
	}
}

ObjectManager::~ObjectManager()
{
}
