#pragma once

#include <string>

class GLShader
{
public:
	GLShader();
	~GLShader();

	GLShader& Attach(const std::string& filepath);
	GLShader& Link();

	void Bind() const;
	void Unbind() const;

private:
	unsigned int CreateShader(const std::string& filepath);
	unsigned int id;
};

