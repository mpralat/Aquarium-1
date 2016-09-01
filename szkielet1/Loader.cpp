#include "Loader.h"



Loader::Loader()
{
}


Loader::~Loader()
{
}

int Loader::loadTexture(std::string file_name, Texture &texture)
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

void Loader::loadSkybox(std::string front, std::string back, std::string left,
	std::string right, std::string up, std::string down,
	GLuint &texture_handle)
{
	glGenTextures(1, &texture_handle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_handle);

	std::string textures[] = { right, left, down, up, back, front };

	for (int i = 0; i < 6; i++)
	{
		Texture texture;
		Loader::loadTexture(textures[i], texture);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, texture.width,
			texture.height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture.bits);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
	int Loader::LoadTexture(std::string file_name, GLuint& texture_handle)
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


int Loader::LoadSceneFromFile(std::string file_name, GLuint& vao,
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


				if (Loader::LoadTexture(file_path, tex))
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

GLint Loader::LoadShaders(std::string vertex_shader, std::string fragment_shader)
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




