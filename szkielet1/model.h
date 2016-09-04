#pragma once
#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Model {
public:
	GLuint vao = 0;
	std::vector<GLfloat> vertices_count;
	std::vector<GLfloat> starting_vertex;
	std::vector<GLuint> textures;

	GLuint shader;
	std::string path;


	Model::Model(std::string path);
	Model::Model();
	void Model::sendMatrix(glm::mat4 view_matrix, glm::mat4 perspective_matrix);
	void Model::drawModel();


	GLint texture_slot;
	GLint view_uniform;
	GLint perspective_uniform;
	GLint trans_uniform;

	void Model::setPath(std::string path);
	void Model::getUniform();




};
