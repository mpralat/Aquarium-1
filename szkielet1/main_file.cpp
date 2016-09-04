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
#include "Model.h"
#include "Fish.h"
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
	aspect = float(window_width) / float(window_height);
	recalculateCamera();

	// SKYBOX
	GLuint skyboxShaders = loader.LoadShaders("vertex_shader_skybox.txt", "fragment_shader_skybox.txt");

	GLint view_uniform_sky = findUniform(skyboxShaders, "view_matrix");
	GLint perspective_uniform_sky = findUniform(skyboxShaders, "perspective_matrix");

	

	//Aquarium Base and Plants

	Model aquariumBase("Meshes/aquarium.obj");
	loader.LoadSceneFromFile(aquariumBase.path, aquariumBase.vao, aquariumBase.vertices_count, aquariumBase.starting_vertex, aquariumBase.textures);
	aquariumBase.shader = loader.LoadShaders("aquarium_vertex_shader.glsl", "aquarium_fragment_shader.glsl");

	
	//Aquarium Glass

	Model aquariumGlass("Meshes/aquarium-glass.obj");
	loader.LoadSceneFromFile(aquariumGlass.path, aquariumGlass.vao, aquariumGlass.vertices_count, aquariumGlass.starting_vertex, aquariumGlass.textures);
	aquariumGlass.shader = loader.LoadShaders("aquariumGlassvertex_shader.glsl", "aquariumGlassfragment_shader.glsl");


	Fish fish01("Meshes/TropicalFish01.obj",1.7);
	loader.LoadSceneFromFile(fish01.path, fish01.vao, fish01.vertices_count, fish01.starting_vertex, fish01.textures);
	fish01.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");

	Fish fish02("Meshes/TropicalFish02.obj",1.1);
	loader.LoadSceneFromFile(fish02.path, fish02.vao, fish02.vertices_count, fish02.starting_vertex, fish02.textures);
	fish02.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");

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
	glDepthFunc(GL_LESS);



	//Fishes

	float x_trans = 0.0;
	float y_trans = 0.0;
	int angle = 0;
	float h = 3.7;
	int count = 0;

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

	
		
		// Fish swimming
		static double prev_time = glfwGetTime();
		double act_time = glfwGetTime();
		double elapsed_time = actual_time - prev_time;

		if (elapsed_time > 0.01)
		{
			prev_time = act_time;
			angle += 1;

			x_trans = fish01.radius * cos(angle * PI / 180.0);
			y_trans = fish01.radius * sin(angle * PI / 180.0);

			//LEMNISKATA 
			//x_trans = (radiusFish01 * sqrt(2) * cos(angle * PI / 180.0))/ (1 + sin(angle * PI / 180.0)* sin(angle * PI / 180.0));
			//y_trans = (radiusFish01 * sqrt(2) * cos(angle * PI / 180.0) * sin(angle * PI / 180.0)) / (1 + sin(angle * PI / 180.0)* sin(angle * PI / 180.0));
		}
		int phi = angle % 360;
		float phis = -phi * PI / 180.0;

		fish02.translate_matrix = glm::translate(glm::mat4(1.0), glm::vec3(fish02.radius * cos(angle*PI/180.0), 0.0, fish02.radius * sin(angle*PI / 180.0)));
		fish02.rotate_matrix = glm::rotate(fish02.translate_matrix, phis, glm::vec3(0, 1, 0));
		// PIERWSZA RYBKA:
		// UP AND DOWN
		fish01.translate_matrix = glm::translate(glm::translate(glm::mat4(1.0), glm::vec3(x_trans, 0.0, y_trans)), glm::vec3(0.0, h, 0.0));
		if (count < 500) {
			h = h - 0.005;
			fish01.rotate_matrix = glm::rotate(glm::rotate(fish01.translate_matrix, phis, glm::vec3(0, 1, 0)), 145.f, glm::vec3(1, 0, 0));
		}
		else {
			h = h + 0.005;
			fish01.rotate_matrix = glm::rotate(glm::rotate(fish01.translate_matrix, phis, glm::vec3(0, 1, 0)), -145.f, glm::vec3(1, 0, 0));
		}
		if (count >= 1000)
			count = 0;
		//h = sin(count);
		count++;


		//Draw aquarium
		aquariumGlass.sendMatrix(view_matrix, perspective);
		aquariumGlass.drawModel();

		aquariumBase.sendMatrix(view_matrix, perspective);
		aquariumBase.drawModel();

		//Draw Fish
		fish01.sendMatrix(view_matrix,perspective);
		fish01.drawModel();
		
	    fish02.sendMatrix(view_matrix, perspective);
		fish02.drawModel();

		processWindowEvents();
	}

	terminate();
	return 0;
}
