#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Loader.h"
class MainModel
{
public:
	MainModel(GLuint shader, std::string path);
	~MainModel();
	void drawModel(glm::mat4 view_matrix, glm::mat4 perspective);
	void createModel();
	void loadModel();
	GLint mTexture_slot;
	GLint mView_uniform;
	GLint mPerspective_uniform;
	GLuint mVao;
	std::vector<GLfloat> mVertices_count;
	std::vector<GLfloat> mStarting_vertex;
	std::vector<GLuint> mTextures;
private:
	GLuint mShader;

	std::string mMeshPath;

	
};

