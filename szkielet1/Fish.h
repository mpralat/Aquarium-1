#pragma once
#include "Model.h"
#include "constants.h"
#include <string>
#include <vector>

class Fish :public Model {
public:
	Fish::Fish(std::string path, float radius, float offset_x, float offset_y , float offset_z, bool R_direction);
	Fish::Fish();

	float radius;
	bool R_diretion;

	float offset_x;
	float offset_y; 
	float offset_z;

	float x_trans;
	float z_trans;
	void Fish::updateMatrix(float y_trans, int angle, float phi);

	glm::mat4 translate_matrix;
	glm::mat4 rotate_matrix;

	void Fish::sendMatrix(glm::mat4 view_matrix, glm::mat4 perspective_matrix);
	void Fish::getUniform();

};


