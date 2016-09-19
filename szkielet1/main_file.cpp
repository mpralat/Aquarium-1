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

	Fish fish01("Meshes/TropicalFish01.obj",1.1,3.0,-2.5,0.9,false);
	loader.LoadSceneFromFile(fish01.path, fish01.vao, fish01.vertices_count, fish01.starting_vertex, fish01.textures);
	fish01.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");

	Fish fish02("Meshes/TropicalFish02.obj",2.1, -4.5, -4.0, 0, false);
	loader.LoadSceneFromFile(fish02.path, fish02.vao, fish02.vertices_count, fish02.starting_vertex, fish02.textures);
	fish02.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");
	
	Fish fish03("Meshes/TropicalFish03.obj", 2.8, 3.0, -2.0, 0.8, true);
	loader.LoadSceneFromFile(fish03.path, fish03.vao, fish03.vertices_count, fish03.starting_vertex, fish03.textures);
	fish03.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");

	Fish fish04("Meshes/TropicalFish04.obj", 2.9, -3.0, 0, 0.6, false);
	loader.LoadSceneFromFile(fish04.path, fish04.vao, fish04.vertices_count, fish04.starting_vertex, fish04.textures);
	fish04.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");

	Fish fish05("Meshes/TropicalFish05.obj", 1.5, 4.0, -2.7, -0.7, false);
	loader.LoadSceneFromFile(fish05.path, fish05.vao, fish05.vertices_count, fish05.starting_vertex, fish05.textures);
	fish05.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");

	Fish fish06("Meshes/TropicalFish06.obj", 2.0, -4.6, 0, 0, true);
	loader.LoadSceneFromFile(fish06.path, fish06.vao, fish06.vertices_count, fish06.starting_vertex, fish06.textures);
	fish06.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");


	Fish fish07("Meshes/TropicalFish15.obj",1.1,5.0,-1.5,2.6,true);
	loader.LoadSceneFromFile(fish07.path, fish07.vao, fish07.vertices_count, fish07.starting_vertex, fish07.textures);
	fish07.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");

	Fish fish08("Meshes/TropicalFish08.obj",2.1, -0.5, 0, 0, true);
	loader.LoadSceneFromFile(fish08.path, fish08.vao, fish08.vertices_count, fish08.starting_vertex, fish08.textures);
	fish08.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");
	
	Fish fish09("Meshes/TropicalFish09.obj", 2.8, 5.0, -2.0, 0.8, true);
	loader.LoadSceneFromFile(fish09.path, fish09.vao, fish09.vertices_count, fish09.starting_vertex, fish09.textures);
	fish09.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");

	Fish fish10("Meshes/TropicalFish10.obj", 2.9, 1.0, 0, 0.4, false);
	loader.LoadSceneFromFile(fish10.path, fish10.vao, fish10.vertices_count, fish10.starting_vertex, fish10.textures);
	fish10.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");

	Fish fish11("Meshes/TropicalFish11.obj", 1.5, 4.0, -2.7, -0.7, false);
	loader.LoadSceneFromFile(fish11.path, fish11.vao, fish11.vertices_count, fish11.starting_vertex, fish11.textures);
	fish11.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");

	Fish fish12("Meshes/TropicalFish12.obj", 1.0, 5.0, 0, 0, false);
	loader.LoadSceneFromFile(fish12.path, fish12.vao, fish12.vertices_count, fish12.starting_vertex, fish12.textures);
	fish12.shader = loader.LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");
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
	int angle = 0;
	float h = 3.7;
	int count = 400;

	float h2 = 1.7;
	int count2 = 0;

	float h3 = 1.7;
	int count3 = 100;

	float h4 = 1.7;
	int count4 = 200;


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
			angle += 1;
		}
	
		int phi;
		if (count < 500) {
			h = h - 0.005;
			phi = 145;
		}	
		else {
			h = h + 0.005;
			phi = -145;
		}		
		if (count >= 1000)
			count = 0;
		count++;
		fish01.updateMatrix(h, angle, phi);
		fish02.updateMatrix(h, angle, phi);
		fish03.updateMatrix(h, angle, phi);

		if (count2 < 300) {
			h2 = h2 - 0.005;
			phi = 145;
		}
		else {
			h2 = h2 + 0.005;
			phi = -145;
		}
		if (count2 >= 600)
			count2 = 0;
		count2++;

		fish04.updateMatrix(h2, angle, phi);
		fish05.updateMatrix(h2, angle, phi);
		fish06.updateMatrix(h2, angle, phi);

		if (count3 < 400) {
			h3 = h3 - 0.01;
			phi = 145;
		}
		else {
			h3 = h3 + 0.01;
			phi = -145;
		}
		if (count3 >= 800)
			count3 = 0;
		count3++;
		fish07.updateMatrix(h3, angle, phi);
		fish08.updateMatrix(h3, angle, phi);
		fish09.updateMatrix(h3, angle, phi);

		if (count4 < 300) {
			h4 = h4 - 0.01;
			phi = 145;
		}
		else {
			h4 = h4 + 0.01;
			phi = -145;
		}
		if (count4 >= 600)
			count4 = 0;
		count4++;

		fish10.updateMatrix(h4, angle, phi);
		fish11.updateMatrix(h4, angle, phi);
		fish12.updateMatrix(h4, angle, phi);




		//Draw Fish
		fish01.sendMatrix(view_matrix,perspective);
		fish01.drawModel();
		
	    fish02.sendMatrix(view_matrix, perspective);
		fish02.drawModel();

		fish03.sendMatrix(view_matrix, perspective);
		fish03.drawModel();

		fish04.sendMatrix(view_matrix, perspective);
		fish04.drawModel();

		fish05.sendMatrix(view_matrix, perspective);
		fish05.drawModel();

		fish06.sendMatrix(view_matrix, perspective);
		fish06.drawModel();

		fish07.sendMatrix(view_matrix, perspective);
		fish07.drawModel();

		fish08.sendMatrix(view_matrix, perspective);
		fish08.drawModel();

		fish09.sendMatrix(view_matrix, perspective);
		fish09.drawModel();

		fish10.sendMatrix(view_matrix, perspective);
		fish10.drawModel();

		fish11.sendMatrix(view_matrix, perspective);
		fish11.drawModel();

		fish12.sendMatrix(view_matrix, perspective);
		fish12.drawModel();

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
