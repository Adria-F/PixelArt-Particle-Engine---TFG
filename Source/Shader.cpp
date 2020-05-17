#include "Application.h"
#include "Shader.h"

//TODO: Replace with own file system
#include <string>
#include <fstream> //ifstream
#include <sstream> //stringstream

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	//Read vertex and fragment shaders files
	std::string vShaderCode = getShaderCode(vertexShaderPath);
	std::string fShaderCode = getShaderCode(fragmentShaderPath);

	//Compile both shaders
	uint vertexShader = compileShader(vShaderCode.c_str(), GL_VERTEX_SHADER);
	uint fragmentShader = compileShader(fShaderCode.c_str(), GL_FRAGMENT_SHADER);

	//Create program
	programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);

	//Check for errors
	int success;
	char infoLog[512];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programID, 512, NULL, infoLog);
		LOG("Error linking program: %s", infoLog);
	}

	//Delete shaders (already linked into program)
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
	glDeleteProgram(programID);
}

void Shader::Use()
{
	glUseProgram(programID);
}

void Shader::sendColor(const char * name, float4 value) const
{
	glUniform4fv(glGetUniformLocation(programID, name), 1, value.ptr());
}

void Shader::sendMat4(const char* name, float* value) const
{
	glUniformMatrix4fv(glGetUniformLocation(programID, name), 1, GL_FALSE, value);
}

void Shader::sendTexture(const char* name, uint texture) const
{
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	//glUniform1ui(glGetUniformLocation(programID, name), 0);
}

std::string Shader::getShaderCode(const char* shaderPath)
{
	std::string shaderCode;
	std::ifstream shaderFile;

	shaderFile.open(shaderPath);
	if (!shaderFile)
	{
		LOG("Error opening shader file: %s", shaderPath);
	}
	else
	{
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();

		shaderFile.close();

		shaderCode = shaderStream.str();
	}

	return shaderCode.c_str();
}

uint Shader::compileShader(const char* shaderCode, GLenum shaderType)
{
	uint shader = 0;

	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);

	//Check for errors
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		LOG("Error compiling shader: %s", infoLog);
	}

	return shader;
}
