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
	void sendTexture(const char* name, uint texture) const;
	void sendVec2(const char* name, float2 value) const;
	void sendUint(const char* name, uint value) const;

private:

	std::string getShaderCode(const char* shaderPath);
	uint compileShader(const char* shaderCode, GLenum shaderType);

public:

	uint programID;
};

#endif // !__SHADER_H__