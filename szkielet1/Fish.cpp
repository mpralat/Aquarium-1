#include"Fish.h"


Fish::Fish(std::string path, float radius, float offset_x, float offset_y, float offset_z, bool R_diretion, int max_deviation) {
	setPath(path);
	this->radius = radius;
	this->offset_x = offset_x;
	this->offset_y = offset_y;
	this->offset_z = offset_z;
	this->R_diretion = R_diretion;
	this->max_deviation = max_deviation;
	y_trans = 3.7; 
}


Fish::Fish() {

}


void Fish::sendMatrix(glm::mat4 view_matrix, glm::mat4 perspective_matrix) {
	getUniform();
	glUseProgram(shader);
	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(perspective_uniform, 1, GL_FALSE, glm::value_ptr(perspective_matrix));
	glUniformMatrix4fv(trans_uniform, 1, GL_FALSE, glm::value_ptr(rotate_matrix));
}

void Fish::getUniform() {
	texture_slot = glGetUniformLocation(shader, "basic_texture");
	if (texture_slot == -1)
		std::cout << "Variable 'basic_texture' not found." << std::endl;
	glUniform1i(texture_slot, 0);

	view_uniform = glGetUniformLocation(shader, "view_matrix");
	if (view_uniform == -1)
		std::cout << "Variable 'view_matrix' not found." << std::endl;

	perspective_uniform = glGetUniformLocation(shader, "perspective_matrix");
	if (perspective_uniform == -1)
		std::cout << "Variable 'perspective_matrix' not found." << std::endl;

	trans_uniform = glGetUniformLocation(shader, "trans_matrix");
	if (trans_uniform == -1)
		std::cout << "Variable 'trans_matrix' not found." << std::endl;

}
void Fish::updateMatrix(int angle) {

	x_trans = radius * cos(angle * PI / 180.0);
	z_trans = radius * sin(angle * PI / 180.0);
	int phi_temp = angle % 360;
	float phis = -phi_temp * PI / 180.0;

	float phi = swim();
	
	if (R_diretion) {
		translate_matrix = glm::translate(glm::translate(glm::mat4(1.0),
			glm::vec3(-x_trans + offset_x, y_trans, -z_trans + offset_z)), glm::vec3(0.0, offset_y, 0.0));
		rotate_matrix = glm::rotate(glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0)), -phi, glm::vec3(1, 0, 0));
	}
	else {
		translate_matrix = glm::translate(glm::translate(glm::mat4(1.0),
			glm::vec3(x_trans + offset_x, y_trans, z_trans + offset_z)), glm::vec3(0.0, offset_y, 0.0));
		rotate_matrix = glm::rotate(glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0)), phi, glm::vec3(1, 0, 0));
	}	
}

int Fish::swim() {

	int phi = 0;
	if (current_deviation < max_deviation/2) {
		y_trans = y_trans - 0.005; 
		phi = 145;
	}
	else {
		y_trans = y_trans + 0.005;
		phi = -145;
	}
	if (current_deviation >= max_deviation)
		current_deviation = 0;
	current_deviation++;
	return phi;
}