#include"Fish.h"


Fish::Fish (std::string path, float radius, float height, int angle) {
	setPath(path);
	this->radius = radius;
	this->height = height;
	this->angle = angle;
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

int Fish::swim(int count, bool updown) {
	float x_trans = radius * cos(angle * PI / 180.0);
	float y_trans = radius * sin(angle * PI / 180.0);
	int phi = angle % 360;
	float phis = -phi * PI / 180.0;
	translate_matrix = glm::translate(glm::translate(glm::mat4(1.0), glm::vec3(x_trans, 0.0, y_trans)), glm::vec3(0.0, height, 0.0));
	if (updown) {
		if (count < 500) {
			height = height - 0.005;
			rotate_matrix = glm::rotate(glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0)), 145.f, glm::vec3(1, 0, 0));
		}
		else {
			height = height + 0.005;
			rotate_matrix = glm::rotate(glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0)), -145.f, glm::vec3(1, 0, 0));
		}
		if (count >= 1000)
			count = 0;
		//h = sin(count);
		count++;
	}
	else
		rotate_matrix = glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0));

	return count;
}