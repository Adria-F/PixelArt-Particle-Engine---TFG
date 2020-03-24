#ifndef __SHADER_H__
#define __SHADER_H__

#include "Globals.h"

class Shader
{
public:
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	~Shader();

	void Use();

private:

	const char* getShaderCode(const char* shaderPath);
	uint compileShader(const char* shaderCode, GLenum shaderType);

public:

	uint programID;
};

#endif // !__SHADER_H__