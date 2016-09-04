#pragma once
#include "Model.h"
#include <string>
#include <vector>

class Fish :public Model {
public:
	Fish::Fish(std::string path, float radius);
	Fish::Fish();
	float radius;

	glm::mat4 translate_matrix;
	glm::mat4 rotate_matrix;

	void Fish::sendMatrix(glm::mat4 view_matrix, glm::mat4 perspective_matrix);
	void Fish::getUniform();

};


