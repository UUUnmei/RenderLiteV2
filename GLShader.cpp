#include <fstream>
#include <iostream>

#include "GLShader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


static void glClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static bool glCheckError(const char* func, const char* file, int line) {
	while (GLenum err = glGetError()) {
		std::cout << "[OpenGL ERROR] " << err
			<< " : [" << file << "] [" << line << "] [" << func << "]"
			<< '\n';
		return false;
	}
	return true;
}

#define ASSERT(x) if(!(x)) __debugbreak(); 
#define GLCall(x) glClearError(); \
    x; \
    ASSERT(glCheckError(#x, __FILE__, __LINE__))


GLShader::GLShader()
{
	id = glCreateProgram();
}

GLShader::~GLShader()
{
	GLCall(glDeleteProgram(id));
}

GLShader& GLShader::Link()
{
	GLCall(glLinkProgram(id));
	GLCall(glValidateProgram(id));

	return *this;
}

GLShader& GLShader::Attach(const std::string & filepath)
{
	std::ifstream fd(filepath);
	auto src = std::string(
		std::istreambuf_iterator<char>(fd),
		std::istreambuf_iterator<char>()
	);
	ASSERT(!src.empty())
	const char* source = src.c_str();
	unsigned int shader = CreateShader(filepath);
	GLCall(glShaderSource(shader, 1, &source, nullptr));
	GLCall(glCompileShader(shader));

	int result = 1;
	static char infoLog[1024];
	GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
	if (!result)
	{
		GLCall(glGetShaderInfoLog(shader, 1024, NULL, infoLog));
		std::cout << "ERROR::SHADER_COMPILATION_ERROR: "
			<< filepath << "\n"
			<< infoLog << "\n ------------------------------------------------------- " << std::endl;
		ASSERT(false);
	}


	GLCall(glAttachShader(id, shader));
	//GLCall( glDetachShader(id, shader) );
	GLCall(glDeleteShader(shader));

	return *this;
}

void GLShader::Bind() const
{
	GLCall(glUseProgram(id));
}

void GLShader::Unbind() const
{
	GLCall(glUseProgram(0));
}

unsigned int GLShader::CreateShader(const std::string & filepath)
{
	size_t index = filepath.rfind(".");
	std::string ext = filepath.substr(index + 1);
	unsigned int ShaderType;
	if (ext == "vert") ShaderType = GL_VERTEX_SHADER;
	else if (ext == "frag") ShaderType = GL_FRAGMENT_SHADER;
	else {
		ASSERT(false);
	}
	unsigned int shader = glCreateShader(ShaderType);
	ASSERT(shader != 0);
	return shader;
}
