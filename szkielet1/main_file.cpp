#include "constants.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <FreeImage.h>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Loader.h"
#include "MainModel.h"
//******************************************************************************
GLFWwindow *window_handle = nullptr;
int window_width = 0;
int window_height = 0;
double actual_time;
double previous_time;
Loader loader;

float P1 = 0.1f;
float P2 = 200.0f;
float FOV = 1.15f;
float aspect;
float camera_horizontal_angle = 0;
float camera_vertical_angle = 0;
glm::vec3 camera_position(-5.0, 8.0, -2.0);
glm::vec3 camera_direction(1.0, 0.0, 0.0);
glm::vec3 camera_right(1.0, 0.0, 0.0);
glm::vec3 camera_up;
glm::mat4 view_matrix;
glm::mat4 perspective;

void updateTimer()
{
	previous_time = actual_time;
	actual_time = glfwGetTime();
}
//******************************************************************************
void lockCursor(GLFWwindow *window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, window_width / 2.0, window_height / 2.0);
}
//******************************************************************************

void setCameraAngles(float horizontal, float vertical)
{
	camera_direction = glm::vec3(cos(vertical) * sin(horizontal),
		sin(vertical),
		cos(vertical) * cos(horizontal));

	camera_right = glm::vec3(-cos(horizontal), 0, sin(horizontal));

	camera_up = glm::cross(camera_right, camera_direction);
	view_matrix = glm::lookAt(camera_position, camera_position +
		camera_direction, camera_up);
}


void recalculateCamera()
{
	camera_up = glm::cross(camera_right, camera_direction);
	view_matrix = glm::lookAt(camera_position, camera_position +
		camera_direction, camera_up);

	view_matrix = glm::lookAt(camera_position, camera_position +
		camera_direction, camera_up);

	perspective = glm::perspective(FOV, aspect, P1, P2);
}
//******************************************************************************
void cursorPositionCallback(GLFWwindow *window, double x_cursor_pos,
	double y_cursor_pos)
{
	lockCursor(window);

	float mouse_speed = 0.5f;
	float time_delta = static_cast<float>(actual_time - previous_time);

	double x_delta = x_cursor_pos - window_width / 2.0;
	double y_delta = y_cursor_pos - window_height / 2.0;

	camera_horizontal_angle += mouse_speed * static_cast<float>(time_delta) *
		static_cast<float>(-x_delta);
	camera_vertical_angle += mouse_speed * static_cast<float>(time_delta) *
		static_cast<float>(-y_delta);

	if (camera_horizontal_angle > 2 * glm::pi<float>() ||
		camera_horizontal_angle < -2 * glm::pi<float>())
		camera_horizontal_angle = 0.0f;

	if (camera_vertical_angle >= glm::half_pi<float>())
		camera_vertical_angle = glm::half_pi<float>();
	if (camera_vertical_angle <= -glm::half_pi<float>())
		camera_vertical_angle = -glm::half_pi<float>();

	setCameraAngles(camera_horizontal_angle, camera_vertical_angle);
}
//******************************************************************************
void KeyCallback(GLFWwindow *window, int key, int /*scancode*/, int action,
	int /*mods*/)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}

	printf("pressed key!  %d", camera_position);
	float move_speed = 55.0f;
	float time_delta = static_cast<float>(actual_time - previous_time);

	if (key == GLFW_KEY_W)
		camera_position += camera_direction * time_delta * move_speed;
	if (key == GLFW_KEY_S)
		camera_position -= camera_direction * time_delta * move_speed;
	if (key == GLFW_KEY_A)
		camera_position -= camera_right * time_delta * move_speed;
	if (key == GLFW_KEY_D)
		camera_position += camera_right * time_delta * move_speed;

	recalculateCamera();
}
//******************************************************************************
void windowSizeCallback(GLFWwindow* /*window*/, int width, int height)
{
	window_width = width;
	window_height = height;

	aspect = float(window_width) / float(window_height);
	recalculateCamera();
}
//******************************************************************************
int createWindow(int width, int height, std::string name, int samples,
	bool fullscreen)
{
	if (name.empty())
		name = "Unnamed Window";

	if (width == 0 || height == 0)
	{
		std::cout << "Wrong window's \"" << name << "\" size." << std::endl;
		return -1;
	}

	if (!glfwInit())
	{
		std::cout << "GLFW initialization error." << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, samples);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window_handle = glfwCreateWindow(width, height, name.c_str(),
		fullscreen ? glfwGetPrimaryMonitor() :
		nullptr, nullptr);

	if (!window_handle)
	{
		std::cout << "Error creating window \"" << name << "\"." << std::endl;
		glfwTerminate();
		return -1;
	}

	window_width = width;
	window_height = height;

	glfwMakeContextCurrent(window_handle);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialization error." << std::endl;
		return -1;
	}

	glfwSetCursorPos(window_handle, window_width / 2.0, window_height / 2.0);
	glfwSetKeyCallback(window_handle, KeyCallback);
	glfwSetCursorPosCallback(window_handle, cursorPositionCallback);
	glfwSetWindowSizeCallback(window_handle, windowSizeCallback);

	return 0;
}
//******************************************************************************

void enableDepthTesting()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}
//******************************************************************************
void enableFaceCulling()
{
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
//******************************************************************************
void processWindowEvents()
{
	glfwSwapBuffers(window_handle);
	glfwPollEvents();
}
//******************************************************************************
void terminate()
{
	glfwDestroyWindow(window_handle);
	glfwTerminate();
}
//******************************************************************************
bool renderingEnabled()
{
	if (glfwWindowShouldClose(window_handle))
		return false;

	return true;
}
//******************************************************************************
void clearColor(float r, float g, float b)
{
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, window_width, window_height);
}

int findUniform(GLuint shader_program, std::string uniform_name)
{
	GLint result = glGetUniformLocation(shader_program, uniform_name.c_str());

	if (result == -1)
		std::cout << "Uniform \"" << uniform_name << "\" not found." << std::endl;

	return result;
}

int main()
{
	int result = createWindow(800, 600, "GL Window", 4, false);
	if (result)
		return -1;
	glfwSetCursorPos(window_handle, window_width / 2.0, window_height / 10.0);
	// SKYBOX
	GLuint skyboxShaders = loader.LoadShaders("vertex_shader_skybox.txt", "fragment_shader_skybox.txt");

	GLint view_uniform_sky = findUniform(skyboxShaders, "view_matrix");
	GLint perspective_uniform_sky = findUniform(skyboxShaders, "perspective_matrix");

	aspect = float(window_width) / float(window_height);
	recalculateCamera();

	//Shader
	GLuint aquariumGlassShaders = loader.LoadShaders("aquariumGlassvertex_shader.glsl", "aquariumGlassfragment_shader.glsl");

	//Aquarium Glass

	GLuint vaoAquariumGlass = 0;
	std::vector<GLfloat> vertices_countAquariumGlass;
	std::vector<GLfloat> starting_vertexAquariumGlass;
	std::vector<GLuint> texturesAquariumGlass;

	loader.LoadSceneFromFile("Meshes/aquarium-glass.obj", vaoAquariumGlass, vertices_countAquariumGlass,
		starting_vertexAquariumGlass, texturesAquariumGlass);


	// shaders + uniforms
	glm::mat4 trans(glm::translate(glm::mat4(1.0), glm::vec3(0.5, 0.3, 0.0)));
	GLint texture_slot_aq_glass = glGetUniformLocation(aquariumGlassShaders, "basic_texture");
	if (texture_slot_aq_glass == -1)
		std::cout << "Variable 'basic_texture' not found." << std::endl;
	glUniform1i(texture_slot_aq_glass, 0);

	GLint view_uniform_aq_glass = glGetUniformLocation(aquariumGlassShaders, "view_matrix");
	if (view_uniform_aq_glass == -1)
		std::cout << "Variable 'view_matrix' not found." << std::endl;

	GLint perspective_uniform_aq_glass = glGetUniformLocation(aquariumGlassShaders, "perspective_matrix");
	if (perspective_uniform_aq_glass == -1)
		std::cout << "Variable 'perspective_matrix' not found." << std::endl;

	//Aquarium Base and Plants

	GLuint aquariumShaders = loader.LoadShaders("aquarium_vertex_shader.glsl", "aquarium_fragment_shader.glsl");

	//create model

	GLuint vaoAquarium = 0;
	std::vector<GLfloat> vertices_countAquarium;
	std::vector<GLfloat> starting_vertexAquarium;
	std::vector<GLuint> texturesAquarium;
	loader.LoadSceneFromFile("Meshes/aquarium.obj", vaoAquarium, vertices_countAquarium,
		starting_vertexAquarium, texturesAquarium);


	GLint texture_slot_aq = glGetUniformLocation(aquariumShaders, "basic_texture");
	if (texture_slot_aq == -1)
		std::cout << "Variable 'basic_texture' not found." << std::endl;
	glUniform1i(texture_slot_aq, 0);

	GLint view_uniform_aq = glGetUniformLocation(aquariumShaders, "view_matrix");
	if (view_uniform_aq == -1)
		std::cout << "Variable 'view_matrix' not found." << std::endl;

	GLint perspective_uniform_aq = glGetUniformLocation(aquariumShaders, "perspective_matrix");
	if (perspective_uniform_aq == -1)
		std::cout << "Variable 'perspective_matrix' not found." << std::endl;
	
	//MainModel aquarium(aquariumShaders, "Meshes/aquarium.obj");
	
	//Fishes
	glm::mat4 translate_matrix;
	glm::mat4 rotate_matrix;

	const float radiusFish01 = 1.5;
	float x_trans = 0.0;
	float y_trans = 0.0;
	int angle = 0;


	GLuint vaoFish01 = 0;
	std::vector<GLfloat> vertices_countFish01;
	std::vector<GLfloat> starting_vertexFish01;
	std::vector<GLuint> texturesFish01;

	loader.LoadSceneFromFile("Meshes/TropicalFish01.obj", vaoFish01, vertices_countFish01,
		starting_vertexFish01, texturesFish01);


	// shaders + uniforms
	GLuint fishShaders = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");


	GLint texture_slot_Fish01 = glGetUniformLocation(fishShaders, "basic_texture");
	if (texture_slot_Fish01 == -1)
		std::cout << "Variable 'basic_texture' not found." << std::endl;
	glUniform1i(texture_slot_Fish01, 0);

	GLint view_uniform_Fish01 = glGetUniformLocation(fishShaders, "view_matrix");
	if (view_uniform_Fish01 == -1)
		std::cout << "Variable 'view_matrix' not found." << std::endl;

	GLint perspective_uniform_Fish01 = glGetUniformLocation(fishShaders, "perspective_matrix");
	if (perspective_uniform_Fish01 == -1)
		std::cout << "Variable 'perspective_matrix' not found." << std::endl;

	GLint trans_uniform_Fish01 = glGetUniformLocation(fishShaders, "trans_matrix");
	if (trans_uniform_Fish01 == -1)
		std::cout << "Variable 'trans_matrix' not found." << std::endl;

	GLuint vaofish2 = 0;
	loader.LoadSceneFromFile("Meshes/TropicalFish01.obj", vaofish2, vertices_countFish01,
		starting_vertexFish01, texturesFish01);

	// Skybox
	GLfloat skybox[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f,  1.0f, 1.0f,
		1.0f,  1.0f, 1.0f,
		1.0f,  1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f,  1.0f,
		1.0f, 1.0f,  1.0f,
		-1.0f, 1.0f,  1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	
	GLuint skybox_vbo = 0;
	glGenBuffers(1, &skybox_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox), &skybox, GL_STATIC_DRAW);

	GLuint skybox_vao = 0;
	glGenVertexArrays(1, &skybox_vao);
	glBindVertexArray(skybox_vao);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	GLuint skybox_texture = 0;
	loader.loadSkybox("skybox/sea_ft.JPG", "skybox/sea_bk.JPG", "skybox/sea_lf.JPG", "skybox/sea_rt.JPG",
	"skybox/sea_up.JPG", "skybox/sea_dn.JPG", skybox_texture);
	
	enableFaceCulling();


	glUseProgram(aquariumGlassShaders);
	//perspective
	glUniformMatrix4fv(view_uniform_aq_glass, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(perspective_uniform_aq_glass, 1, GL_FALSE,
		glm::value_ptr(perspective));

	glUseProgram(aquariumShaders);
	// perspective 
	glUniformMatrix4fv(view_uniform_aq, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(perspective_uniform_aq, 1, GL_FALSE, glm::value_ptr(perspective));
	//glUniformMatrix4fv(trans_uniform_aq, 1, GL_FALSE, glm::value_ptr(trans));


	glUseProgram(fishShaders);
	// perspective
	glUniformMatrix4fv(view_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(perspective_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(perspective));
	glUniformMatrix4fv(trans_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(rotate_matrix));


	glDepthFunc(GL_LESS);
	float h = 3.7;
	int count = 0;
	int swim_angle = 45.f;
	while (renderingEnabled())
	{

		std::string title = "Aquarium";
		glfwSetWindowTitle(window_handle, title.c_str());

		updateTimer();

		clearColor(0.5, 0.5, 0.5);
		glm::mat4 view_static = view_matrix;
		glm::vec3 pos(0.0, 0.0, 0.0);
		view_static = glm::lookAt(pos, pos + camera_direction, camera_up);

		


		

		//SKYBOX
		glUseProgram(skyboxShaders);
		glUniformMatrix4fv(view_uniform_sky, 1, GL_FALSE, glm::value_ptr(view_static));
		glUniformMatrix4fv(perspective_uniform_sky, 1, GL_FALSE, glm::value_ptr(perspective));

		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
		glBindVertexArray(skybox_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glEnable(GL_DEPTH_TEST);



		glUseProgram(aquariumGlassShaders);
		// Wyslanie perspektywy i pozycji kamery do programu shadera
		glUniformMatrix4fv(view_uniform_aq_glass, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(perspective_uniform_aq_glass, 1, GL_FALSE,
			glm::value_ptr(perspective));
		// Rysowanie
		glUseProgram(aquariumGlassShaders);
		glBindVertexArray(vaoAquariumGlass);
		for (int i = 0; i < starting_vertexAquariumGlass.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, texturesAquariumGlass[i]);
			glUniform1i(texture_slot_aq_glass, 0);
			glDrawArrays(GL_TRIANGLES, starting_vertexAquariumGlass[i], vertices_countAquariumGlass[i]);
		}

		
		glUseProgram(aquariumShaders);
		// Wyslanie perspektywy i kamery do programu shadera
		glUniformMatrix4fv(view_uniform_aq, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(perspective_uniform_aq, 1, GL_FALSE, glm::value_ptr(perspective));
		glUseProgram(aquariumShaders);
		glBindVertexArray(vaoAquarium);
		for (int i = 0; i < starting_vertexAquarium.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, texturesAquarium[i]);
			glUniform1i(texture_slot_aq, 0);
			glDrawArrays(GL_TRIANGLES, starting_vertexAquarium[i], vertices_countAquarium[i]);
		}
		
		// Fish swimming
		static double prev_time = glfwGetTime();
		double act_time = glfwGetTime();
		double elapsed_time = actual_time - prev_time;

		if (elapsed_time > 0.01)
		{
			prev_time = act_time;
			angle += 1;

			x_trans = radiusFish01 * cos(angle * PI / 180.0);
			y_trans = radiusFish01 * sin(angle * PI / 180.0);

			//LEMNISKATA 
			//x_trans = (radiusFish01 * sqrt(2) * cos(angle * PI / 180.0))/ (1 + sin(angle * PI / 180.0)* sin(angle * PI / 180.0));
			//y_trans = (radiusFish01 * sqrt(2) * cos(angle * PI / 180.0) * sin(angle * PI / 180.0)) / (1 + sin(angle * PI / 180.0)* sin(angle * PI / 180.0));
		}
		int phi = angle % 360;
		float phis = -phi * PI / 180.0;

		/*
		translate_matrix = glm::translate(glm::translate(glm::mat4(1.0), glm::vec3(x_trans, 0.0, y_trans)), glm::vec3(0.0, h, 0.0));
		//rotate_matrix = glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0));
		
		// UP AND DOWN
		if (count < 600) {
			h = h - 0.005;
			rotate_matrix = glm::rotate(glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0)), 145.f, glm::vec3(1, 0, 0));
		}
		else {
			h = h + 0.005;
			rotate_matrix = glm::rotate(glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0)), -145.f, glm::vec3(1, 0, 0));
		}

		if (count >= 1200)
			count = 0;

		//h = sin(count);
		count++;
		*/

		// PIERWSZA RYBKA:
		// UP AND DOWN
		if (count < 500) {
			h = h - 0.005;
			rotate_matrix = glm::rotate(glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0)), 145.f, glm::vec3(1, 0, 0));
		}
		else {
			h = h + 0.005;
			rotate_matrix = glm::rotate(glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0)), -145.f, glm::vec3(1, 0, 0));
		}

		if (count >= 1000)
			count = 0;

		//h = sin(count);
		count++;

		translate_matrix = glm::translate(glm::translate(glm::mat4(1.0), glm::vec3(x_trans, 0.0, y_trans)), glm::vec3(0.0, h, 0.0));
		rotate_matrix = glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0));


		glUseProgram(fishShaders);

		// Wyslanie perspektywy i kamery do programu shadera
		glUniformMatrix4fv(view_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(perspective_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(perspective));
		glUniformMatrix4fv(trans_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(rotate_matrix));
		// RYSUJEMY
		glUseProgram(fishShaders);
		glBindVertexArray(vaoFish01);
		for (int i = 0; i < starting_vertexFish01.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, texturesFish01[i]);
			glUniform1i(texture_slot_aq, 0);
			glDrawArrays(GL_TRIANGLES, starting_vertexFish01[i], vertices_countFish01[i]);
		}

		//ZMIENIAMY PRZESUNIeCIE ALE NA STAREJ MACIERZY
		translate_matrix = glm::translate(glm::translate(glm::mat4(1.0), glm::vec3(x_trans, 0.0, y_trans)), glm::vec3(0.0, 0.5, 0.0));
		rotate_matrix = glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0));

		//WYSYLAMY NOWE
		glUniformMatrix4fv(view_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(perspective_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(perspective));
		glUniformMatrix4fv(trans_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(rotate_matrix));
		glUseProgram(fishShaders);
		//RYSUJEMY DRUGa
		glBindVertexArray(vaofish2);
		for (int i = 0; i < starting_vertexFish01.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, texturesFish01[i]);
			glUniform1i(texture_slot_aq, 0);
			glDrawArrays(GL_TRIANGLES, starting_vertexFish01[i], vertices_countFish01[i]);
		}
	
		glUseProgram(fishShaders);
		// Wyslanie perspektywy i kamery do programu shadera
		glUniformMatrix4fv(view_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(perspective_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(perspective));
		glUniformMatrix4fv(trans_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(rotate_matrix));
		glUseProgram(fishShaders);
		glBindVertexArray(vaoFish01);
		for (int i = 0; i < starting_vertexFish01.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, texturesFish01[i]);
			glUniform1i(texture_slot_aq, 0);
			glDrawArrays(GL_TRIANGLES, starting_vertexFish01[i], vertices_countFish01[i]);
		}
		
		
		
		
		translate_matrix = glm::translate(glm::translate(glm::mat4(1.0), glm::vec3(x_trans, 0.0, y_trans)), glm::vec3(0.0, 1.0, 0.0));
		rotate_matrix = glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0));
		glUniformMatrix4fv(view_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(perspective_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(perspective));
		glUniformMatrix4fv(trans_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(rotate_matrix));
		glUseProgram(fishShaders);
		glBindVertexArray(vaofish2);
		for (int i = 0; i < starting_vertexFish01.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, texturesFish01[i]);
			glUniform1i(texture_slot_aq, 0);
			(GL_TRIANGLES, starting_vertexFish01[i], vertices_countFish01[i]);
		}

		processWindowEvents();
	}

	terminate();
	return 0;
}
