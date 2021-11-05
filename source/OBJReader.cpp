#include "OBJReader.h"
#include <fstream>
#include <sstream>
#include <regex>
#include "EMath.h"
#include "EMathUtilities.h"

OBJReader* OBJReader::m_Instance = nullptr;

OBJReader* OBJReader::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new OBJReader();
	}
    return m_Instance;
}

void OBJReader::ReadOBJ(std::vector<Elite::FPoint3>& vertices, std::vector<Face>& faces, const std::string& path) const
{
    vertices.clear();
    faces.clear();
    std::ifstream file;
    file.open(path);
    Elite::FPoint3 vertex;


    if (file.is_open())
    {
        vertices.reserve(1000);
        faces.reserve(1000);

        std::string line;
        while (!file.eof())
        {
            std::getline(file, line);
            std::stringstream lineStream{ line };
            std::string temp;
            std::regex vertexRegex("(-*\\d+\\.\\d*)");
            std::regex faceRegex("(\\d+)");
            std::smatch matches;
            switch (line[0])
            {
            case 'v':
                std::regex_search(line, matches, vertexRegex);
                vertex.x = std::stof(matches.str(1));
                line = matches.suffix();
                std::regex_search(line, matches, vertexRegex);
                vertex.y = std::stof(matches.str(1));
                line = matches.suffix();
                std::regex_search(line, matches, vertexRegex);
                vertex.z = std::stof(matches.str(1));
                vertices.push_back(vertex);
                break;
            case 'f':
                Face face;
                std::regex_search(line, matches, faceRegex);
                face.vertIndices[0] = std::stoi(matches.str(1)) - 1;
                line = matches.suffix();
                std::regex_search(line, matches, faceRegex);
                face.vertIndices[1] = std::stoi(matches.str(1)) - 1;
                line = matches.suffix();
                std::regex_search(line, matches, faceRegex);
                face.vertIndices[2] = std::stoi(matches.str(1)) - 1;
                faces.push_back(face);
                break;
            }
        }
        file.close();

        vertices.shrink_to_fit();
        faces.shrink_to_fit();
    }
}

void OBJReader::Delete()
{
	if (m_Instance != nullptr)
	{
		delete m_Instance;
	}
}

OBJReader::OBJReader()
{
}
OBJReader::~OBJReader()
{
}
