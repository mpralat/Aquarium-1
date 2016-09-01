#pragma once
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <FreeImage.h>
struct Texture
{
	BYTE *bits;
	FIBITMAP *image_ptr;
	int width;
	int height;
};
class Loader
{
public:
	Loader();
	~Loader();
	int LoadSceneFromFile(std::string file_name, GLuint& vao,
		std::vector<GLfloat>& mesh_vertices_count,
		std::vector<GLfloat>& mesh_starting_vertex_index,
		std::vector<GLuint>& textures);
	int loadTexture(std::string file_name, Texture &texture);
	int LoadTexture(std::string file_name, GLuint& texture_handle);
	void loadSkybox(std::string front, std::string back, std::string left,
		std::string right, std::string up, std::string down,
		GLuint &texture_handle);
	GLint LoadShaders(std::string vertex_shader, std::string fragment_shader);

};


