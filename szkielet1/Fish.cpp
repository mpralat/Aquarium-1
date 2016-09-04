#include"Fish.h"


Fish::Fish (std::string path, float radius) {
	setPath(path);
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