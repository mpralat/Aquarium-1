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
glm::vec3 camera_position(1.5, 3.0, -5.0);
glm::vec3 camera_direction(1.0, 0.0, 0.0);
glm::vec3 camera_right(1.0, 0.0, 0.0);
glm::vec3 camera_up;
glm::mat4 view_matrix;
glm::mat4 perspective;
float cam_height = 3.0;
glm::vec3 future_move;
const int FISH_NUMBER = 12;

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
	future_move = camera_position;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
	float move_speed = 55.0f;
	float time_delta = static_cast<float>(actual_time - previous_time);

	if (key == GLFW_KEY_W)
		future_move += camera_direction * time_delta * move_speed;
	if (key == GLFW_KEY_S) 
		future_move -= camera_direction * time_delta * move_speed;
	
	if (key == GLFW_KEY_A)
		future_move -= camera_right * time_delta * move_speed;
	if (key == GLFW_KEY_D)
		future_move += camera_right * time_delta * move_speed;

	if (!(future_move.x > -7.5 && future_move.x < 10.0 && future_move.z > -3.19 && future_move.z < 4.22))
		camera_position = future_move;

	camera_position.y = 3.0;
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

	

	// Fish
	std::string path = "Meshes/TropicalFish";
	std::vector<std::string> namesVector;
	for (int i = 1; i < FISH_NUMBER + 1; i++)
		namesVector.push_back(path + std::to_string(i) + ".obj");

	float radius[FISH_NUMBER] = { 1.1, 2.1, 2.8, 2.9, 1.5, 2.0, 1.1, 2.1,2.8,2.9,1.5,1.0};
	float offset_x[FISH_NUMBER] = { 3.0, -4.5, 3.0, -3.0, 4.0, -4.6, 5.0 -0.5, 5.0, 1.0, 4.0, 5.0 };
	float offset_y[FISH_NUMBER] = { -2.5,-4.0,-2.0,0,-2.7,0,-1.5,0,-2.0,0,-2.7,-1.0};
	float offset_z[FISH_NUMBER] = { 0.9,0,0.8,0.6,-0.7,0,2.6,0,0.8,0.4,-0.7,0};
	bool r_direction[FISH_NUMBER] = { false,false,true,false,false,true,true,true,true,false,false,false};
	int max_deviation[FISH_NUMBER] = { 300,400,300,600,350,500,400,700,500,1000,400,800 };
	std::vector<Fish> fishVector;
	for (int i = 0; i < FISH_NUMBER; i++) {
		Fish newFish(namesVector[i], radius[i], offset_x[i],offset_y[i], offset_z[i], r_direction[i],max_deviation[i]);
		loader.LoadSceneFromFile(newFish.path, newFish.vao, newFish.vertices_count, newFish.starting_vertex, newFish.textures);
		newFish.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");
		fishVector.push_back(newFish);
	}
	
	
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
	loader.loadSkybox("skybox/wall.jpg", "skybox/wall.jpg", "skybox/wall.jpg", "skybox/wall.jpg",
	"skybox/wall.jpg", "skybox/floor.jpg", skybox_texture);
	
	enableFaceCulling();
	glDepthFunc(GL_LESS);


	//Fishes
	float angle = 0;

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
		double elapsed_time = act_time - prev_time;

		
		if (elapsed_time > 0.01)
		{
			prev_time = act_time;
			angle += 0.8;
		}
	
		
		//Draw Fish
		for (int i = 0; i < FISH_NUMBER; i++) {
			fishVector[i].updateMatrix(angle);  // update translate matrix based on time
			fishVector[i].sendMatrix(view_matrix, perspective);
			fishVector[i].drawModel();
		}

		//Draw aquarium
		aquariumBase.sendMatrix(view_matrix, perspective);
		aquariumBase.drawModel();
		aquariumGlass.sendMatrix(view_matrix, perspective);
		aquariumGlass.drawModel();

		processWindowEvents();
	}

	terminate();
	return 0;
}
