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
//******************************************************************************
GLFWwindow *window_handle = nullptr;
int window_width = 0;
int window_height = 0;
double actual_time;
double previous_time;

const float PI = 3.14;
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
struct Texture
{
	BYTE *bits;
	FIBITMAP *image_ptr;
	int width;
	int height;
};

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
int loadTexture(std::string file_name, Texture &texture)
{
	FREE_IMAGE_FORMAT image_format = FIF_UNKNOWN;
	FIBITMAP *image_ptr = nullptr;
	BYTE *bits = nullptr;

	image_format = FreeImage_GetFileType(file_name.c_str(), 0);
	if (image_format == FIF_UNKNOWN)
		image_format = FreeImage_GetFIFFromFilename(file_name.c_str());

	if (image_format == FIF_UNKNOWN)
	{
		std::cout << "Texture \"" << file_name << "\" has unknown file format." <<
			std::endl;
		return -1;
	}

	if (FreeImage_FIFSupportsReading(image_format))
		image_ptr = FreeImage_Load(image_format, file_name.c_str());

	if (!image_ptr)
	{
		std::cout << "Unable to load texture \"" << file_name << "\"." << std::endl;
		return -1;
	}

	bits = FreeImage_GetBits(image_ptr);

	unsigned int image_width = 0;
	unsigned int image_height = 0;
	image_width = FreeImage_GetWidth(image_ptr);
	image_height = FreeImage_GetHeight(image_ptr);

	if ((bits == 0) || (image_width == 0) || (image_height == 0))
	{
		std::cout << "Texture \"" << file_name << "\" format error." << std::endl;
		return -1;
	}

	std::cout << "Texture \"" << file_name << "\" loaded." << std::endl;

	texture.bits = bits;
	texture.image_ptr = image_ptr;
	texture.width = image_width;
	texture.height = image_height;

	return 0;
}


int LoadTexture(std::string file_name, GLuint& texture_handle)
{
	FREE_IMAGE_FORMAT image_format = FIF_UNKNOWN;
	FIBITMAP* image_ptr = 0;
	BYTE* bits = 0;

	image_format = FreeImage_GetFileType(file_name.c_str(), 0);
	if (image_format == FIF_UNKNOWN)
		image_format = FreeImage_GetFIFFromFilename(file_name.c_str());

	if (FreeImage_FIFSupportsReading(image_format))
		image_ptr = FreeImage_Load(image_format, file_name.c_str());

	bits = FreeImage_GetBits(image_ptr);

	int image_width = 0;
	int image_height = 0;
	image_width = FreeImage_GetWidth(image_ptr);
	image_height = FreeImage_GetHeight(image_ptr);
	if ((bits == 0) || (image_width == 0) || (image_height == 0))
		return -1;

	glGenTextures(1, &texture_handle);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	unsigned int colours = FreeImage_GetBPP(image_ptr);
	if (colours == 24)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0,
			GL_BGR, GL_UNSIGNED_BYTE, bits);
	else if (colours == 32)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height,
			0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
	glGenerateMipmap(GL_TEXTURE_2D);

	return 0;
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

//******************************************************************************
int LoadSceneFromFile(std::string file_name, GLuint& vao,
	std::vector<GLfloat>& mesh_vertices_count,
	std::vector<GLfloat>& mesh_starting_vertex_index,
	std::vector<GLuint>& textures)
{
	const aiScene* scene = aiImportFile(file_name.c_str(), aiProcess_Triangulate);
	if (!scene)
	{
		std::cout << "Mesh not found." << std::endl;
		return -1;
	}

	int total_vertices_count = 0;

	std::vector<GLfloat> buffer_vbo_data;

	for (int i = 0; i != scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

		int mesh_vertices = 0;

		for (int j = 0; j != mesh->mNumFaces; j++)
		{
			const aiFace* face = &mesh->mFaces[j];

			for (int k = 0; k != 3; k++)
			{
				aiVector3D vertex_position{ 0, 0, 0 };
				aiVector3D vertex_normal{ 0, 0, 0 };
				aiVector3D vertex_texture_coord{ 0, 0, 0 };

				if (mesh->HasPositions())
					vertex_position = mesh->mVertices[face->mIndices[k]];

				if (mesh->HasNormals())
					vertex_normal = mesh->mNormals[face->mIndices[k]];

				if (mesh->HasTextureCoords(0))
					vertex_texture_coord = mesh->mTextureCoords[0][face->mIndices[k]];

				buffer_vbo_data.push_back(vertex_position.x);
				buffer_vbo_data.push_back(vertex_position.y);
				buffer_vbo_data.push_back(vertex_position.z);

				buffer_vbo_data.push_back(vertex_normal.x);
				buffer_vbo_data.push_back(vertex_normal.y);
				buffer_vbo_data.push_back(vertex_normal.z);

				buffer_vbo_data.push_back(vertex_texture_coord.x);
				buffer_vbo_data.push_back(vertex_texture_coord.y);

				mesh_vertices++;
			}
		}

		mesh_vertices_count.push_back(mesh_vertices);
		mesh_starting_vertex_index.push_back(total_vertices_count);
		total_vertices_count += mesh_vertices;

		if (scene->mNumMaterials != 0)
		{
			const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			aiString texture_path;

			GLuint tex = 0;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path) ==
				AI_SUCCESS)
			{
				unsigned int found_pos = file_name.find_last_of("/\\");
				std::string path = file_name.substr(0, found_pos);
				std::string name(texture_path.C_Str());
				if (name[0] == '/')
					name.erase(0, 1);

				std::string file_path = path + "/" + name;


				if (LoadTexture(file_path, tex))
					std::cout << "Texture " << file_path << " not found." <<
					std::endl;
				else
					std::cout << "Texture " << file_path << " loaded." <<
					std::endl;
			}

			textures.push_back(tex);
		}
	}

	GLuint vbo_buffer = 0;
	glGenBuffers(1, &vbo_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_buffer);
	glBufferData(GL_ARRAY_BUFFER, buffer_vbo_data.size() * sizeof(GLfloat),
		buffer_vbo_data.data(), GL_STATIC_DRAW);

	int single_vertex_size = 2 * 3 * sizeof(GLfloat) + 2 * sizeof(GLfloat);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, single_vertex_size, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, single_vertex_size,
		reinterpret_cast<void*>(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, single_vertex_size,
		reinterpret_cast<void*>(2 * 3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	return 0;
}

//******************************************************************************
int findUniform(GLuint shader_program, std::string uniform_name)
{
	GLint result = glGetUniformLocation(shader_program, uniform_name.c_str());

	if (result == -1)
		std::cout << "Uniform \"" << uniform_name << "\" not found." << std::endl;

	return result;
}
//******************************************************************************

//******************************************************************************


GLint LoadShaders(std::string vertex_shader, std::string fragment_shader)
{
	GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertex_shader_data;
	std::ifstream vertex_shader_file(vertex_shader.c_str(), std::ios::in);
	if (vertex_shader_file.is_open())
	{
		std::string line;
		while (std::getline(vertex_shader_file, line))
			vertex_shader_data += "\n" + line;

		vertex_shader_file.close();
	}

	std::string fragment_shader_data;
	std::ifstream fragment_shader_file(fragment_shader.c_str(), std::ios::in);
	if (fragment_shader_file.is_open())
	{
		std::string line;
		while (std::getline(fragment_shader_file, line))
			fragment_shader_data += "\n" + line;

		fragment_shader_file.close();
	}

	const char* vertex_ptr = vertex_shader_data.c_str();
	const char* fragment_ptr = fragment_shader_data.c_str();
	glShaderSource(vertex_shader_id, 1, &vertex_ptr, NULL);
	glShaderSource(fragment_shader_id, 1, &fragment_ptr, NULL);

	glCompileShader(vertex_shader_id);
	glCompileShader(fragment_shader_id);

	GLuint shader_programme = glCreateProgram();
	glAttachShader(shader_programme, vertex_shader_id);
	glAttachShader(shader_programme, fragment_shader_id);
	glLinkProgram(shader_programme);
	int link_status = -1; 
	glGetProgramiv(shader_programme, GL_LINK_STATUS, &link_status); 
	if (link_status != GL_TRUE) {
		std::cout << "Shader programme link error!" << std::endl;
		return -1;
	}
	const int max_length = 2048; 
	int length = 0; char log_text[max_length]; 
	glGetShaderInfoLog(vertex_shader_id, max_length, &length, log_text); 
	std::cout << log_text;

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	return shader_programme;
}
void loadSkybox(std::string front, std::string back, std::string left,
	std::string right, std::string up, std::string down,
	GLuint &texture_handle)
{
	glGenTextures(1, &texture_handle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_handle);

	std::string textures[] = { right, left, down, up, back, front };

	for (int i = 0; i < 6; i++)
	{
		Texture texture;
		loadTexture(textures[i], texture);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, texture.width,
			texture.height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture.bits);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

//******************************************************************************
int main()
{
	int result = createWindow(800, 600, "GL Window", 4, false);
	if (result)
		return -1;
	
	// SKYBOX
	GLuint skyboxShaders = LoadShaders("vertex_shader_skybox.txt", "fragment_shader_skybox.txt");

	GLint view_uniform_sky = findUniform(skyboxShaders, "view_matrix");
	GLint perspective_uniform_sky = findUniform(skyboxShaders, "perspective_matrix");

	aspect = float(window_width) / float(window_height);
	recalculateCamera();



	//Aquarium Glass

	GLuint vaoAquariumGlass = 0;
	std::vector<GLfloat> vertices_countAquariumGlass;
	std::vector<GLfloat> starting_vertexAquariumGlass;
	std::vector<GLuint> texturesAquariumGlass;

	LoadSceneFromFile("Meshes/aquarium-glass.obj", vaoAquariumGlass, vertices_countAquariumGlass,
		starting_vertexAquariumGlass, texturesAquariumGlass);

	// Zaladowanie shaderow i pobranie lokalizacji zmiennych uniform
	GLuint aquariumGlassShaders = LoadShaders("aquariumGlassvertex_shader.glsl", "aquariumGlassfragment_shader.glsl");


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

	GLuint vaoAquarium = 0;
	std::vector<GLfloat> vertices_countAquarium;
	std::vector<GLfloat> starting_vertexAquarium;
	std::vector<GLuint> texturesAquarium;

	glfwSetCursorPos(window_handle, window_width / 2.0, window_height / 2.0);

	LoadSceneFromFile("Meshes/aquarium.obj", vaoAquarium, vertices_countAquarium,
		starting_vertexAquarium, texturesAquarium);

	// Zaladowanie shaderow i pobranie lokalizacji zmiennych uniform
	GLuint aquariumShaders = LoadShaders("aquarium_vertex_shader.glsl", "aquarium_fragment_shader.glsl");


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

	LoadSceneFromFile("Meshes/TropicalFish01.obj", vaoFish01, vertices_countFish01,
		starting_vertexFish01, texturesFish01);

	// Zaladowanie shaderow i pobranie lokalizacji zmiennych uniform
	GLuint fishShaders = LoadShaders("fish_vertex_shader.glsl", "fish_fragment_shader.glsl");


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
	loadSkybox("skybox/sea_ft.JPG", "skybox/sea_bk.JPG", "skybox/sea_lf.JPG", "skybox/sea_rt.JPG",
	"skybox/sea_up.JPG", "skybox/sea_dn.JPG", skybox_texture);
	
	enableFaceCulling();


	glUseProgram(aquariumGlassShaders);
	// Wyslanie perspektywy i pozycji kamery do programu shadera
	glUniformMatrix4fv(view_uniform_aq_glass, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(perspective_uniform_aq_glass, 1, GL_FALSE,
		glm::value_ptr(perspective));

	glUseProgram(aquariumShaders);
	// Wyslanie perspektywy i kamery do programu shadera
	glUniformMatrix4fv(view_uniform_aq, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(perspective_uniform_aq, 1, GL_FALSE, glm::value_ptr(perspective));


	glUseProgram(fishShaders);
	// Wyslanie perspektywy i kamery do programu shadera
	glUniformMatrix4fv(view_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(perspective_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(perspective));
	glUniformMatrix4fv(trans_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(rotate_matrix));


	glDepthFunc(GL_LESS);

	while (renderingEnabled())
	{

		std::string title = "Aquarium";
		glfwSetWindowTitle(window_handle, title.c_str());

		updateTimer();

		clearColor(0.5, 0.5, 0.5);
		glm::mat4 view_static = view_matrix;
		glm::vec3 pos(0.0, 0.0, 0.0);
		view_static = glm::lookAt(pos, pos + camera_direction, camera_up);

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
		}
		int phi = angle % 360;
		float phis = -phi * PI / 180.0;

		translate_matrix = glm::translate(glm::mat4(1.0), glm::vec3(x_trans, 0.0, y_trans));
		rotate_matrix = glm::rotate(translate_matrix, phis, glm::vec3(0, 1, 0));


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

		glUseProgram(aquariumShaders);
		// Wyslanie perspektywy i kamery do programu shadera
		glUniformMatrix4fv(view_uniform_aq, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(perspective_uniform_aq, 1, GL_FALSE, glm::value_ptr(perspective));

		glUseProgram(fishShaders);
		// Wyslanie perspektywy i kamery do programu shadera
		glUniformMatrix4fv(view_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(perspective_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(perspective));
		glUniformMatrix4fv(trans_uniform_Fish01, 1, GL_FALSE, glm::value_ptr(rotate_matrix));


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
		glBindVertexArray(vaoAquarium);
		for (int i = 0; i < starting_vertexAquarium.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, texturesAquarium[i]);
			glUniform1i(texture_slot_aq, 0);
			glDrawArrays(GL_TRIANGLES, starting_vertexAquarium[i], vertices_countAquarium[i]);
		}
		glUseProgram(fishShaders);
		glBindVertexArray(vaoFish01);
		for (int i = 0; i < starting_vertexFish01.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, texturesFish01[i]);
			glUniform1i(texture_slot_aq, 0);
			glDrawArrays(GL_TRIANGLES, starting_vertexFish01[i], vertices_countFish01[i]);
		}


		processWindowEvents();
	}

	terminate();
	return 0;
}
