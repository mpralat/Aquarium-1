#include "Model.h"

Model::Model(std::string path) {
	setPath(path);
}


void Model::setPath(std::string path) {
	this->path = path;
}

Model::Model() {

}



void Model::getUniform() {
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

}


void Model::sendMatrix(glm::mat4 view_matrix, glm::mat4 perspective_matrix) {
	getUniform();
	glUseProgram(shader);
	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(perspective_uniform, 1, GL_FALSE, glm::value_ptr(perspective_matrix));
}


void Model::drawModel() {

	glUseProgram(shader);

	glBindVertexArray(vao);
	for (int i = 0; i < starting_vertex.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glUniform1i(texture_slot, 0);
		glDrawArrays(GL_TRIANGLES, starting_vertex[i], vertices_count[i]);
	}
}