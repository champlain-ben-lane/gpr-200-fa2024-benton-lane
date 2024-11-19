#pragma once

#ifndef MESH_H
#define MESH_H

#include "../ew/external/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../bl_library/shader.h"

#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4

using namespace std;

struct Vertex {
	// Position
	glm::vec3 Position;
	// Normal
	glm::vec3 Normal;
	// Texture coordinates
	glm::vec2 TexCoords;
	// Tangent
	glm::vec3 Tangent;
	// Bitangent
	glm::vec3 Bitangent;
		// Bone indexes which have influence on the vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

    // render the mesh
    void Draw(Shader& shader);

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
	void setupMesh();
};
#endif