#pragma once
#include "Model.h"
#include <string>
#include <vector>

class Fish :public Model {
public:
	Fish::Fish(std::string path, float radius,float height, int angle);
	Fish::Fish();
	float radius;
	float height;
	int angle;

	glm::mat4 translate_matrix;
	glm::mat4 rotate_matrix;

	void Fish::sendMatrix(glm::mat4 view_matrix, glm::mat4 perspective_matrix);
	void Fish::getUniform();
	void Fish::swim(int count, int angle);
};


