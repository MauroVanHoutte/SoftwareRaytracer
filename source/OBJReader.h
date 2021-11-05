#pragma once
#include <vector>
#include <string>
#include "EMath.h"
#include "EMathUtilities.h"

struct Face
{
	static const int nrVerts{3};
	int vertIndices[nrVerts];
	Elite::FVector3 normal;
	Elite::FVector3 normalTransformed;
};

class OBJReader final
{
public:
	static OBJReader* GetInstance();

	void ReadOBJ(std::vector<Elite::FPoint3>& vertices, std::vector<Face>& faces, const std::string& path) const;

	static void Delete();
private:
	OBJReader();
	~OBJReader();
	
	static OBJReader* m_Instance;
};