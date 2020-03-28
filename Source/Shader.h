#ifndef __SHADER_H__
#define __SHADER_H__

#include "Globals.h"

class Shader
{
public:
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	~Shader();

	void Use();

	void sendColor(const char* name, float4 value) const;
	void sendMat4(const char* name, float* value) const;

private:

	const char* getShaderCode(const char* shaderPath);
	uint compileShader(const char* shaderCode, GLenum shaderType);

public:

	uint programID;
};

#endif // !__SHADER_H__