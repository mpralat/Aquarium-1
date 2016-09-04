#include "MainModel.h"




MainModel::MainModel(GLuint shader, std::string path)
{
	mShader = shader;
	mMeshPath = path;
	//MainModel::createModel;
}


MainModel::~MainModel()
{
}

void MainModel::createModel() {
	Loader mLoader;
	mVao = 0;

	mLoader.LoadSceneFromFile(mMeshPath, mVao, mVertices_count,
		mStarting_vertex, mTextures);

	mTexture_slot = glGetUniformLocation(mShader, "basic_texture");
	if (mTexture_slot == -1)
		std::cout << "Variable 'basic_texture' not found." << std::endl;
	glUniform1i(mTexture_slot, 0);

	mView_uniform = glGetUniformLocation(mShader, "view_matrix");
	if (mView_uniform == -1)
		std::cout << "Variable 'view_matrix' not found." << std::endl;

	mPerspective_uniform = glGetUniformLocation(mShader, "perspective_matrix");
	if (mPerspective_uniform == -1)
		std::cout << "Variable 'perspective_matrix' not found." << std::endl;

}

void MainModel::drawModel(glm::mat4 view_matrix, glm::mat4 perspective) {
	glUseProgram(mShader);
	// Wyslanie perspektywy i kamery do programu shadera
	glUniformMatrix4fv(mView_uniform, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(mPerspective_uniform, 1, GL_FALSE, glm::value_ptr(perspective));
	glUseProgram(mShader);
	glBindVertexArray(mVao);
	for (int i = 0; i < mStarting_vertex.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, mTextures[i]);
		glUniform1i(mTexture_slot, 0);
		glDrawArrays(GL_TRIANGLES, mStarting_vertex[i], mVertices_count[i]);
	}

}