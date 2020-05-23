#ifndef __MODULETEXTURES_H__
#define __MODULETEXTURES_H__

#include "Module.h"
#include <string>

class ResourceMaterial;

struct Texture
{
	Texture(const char* path);
	~Texture();

	uint UID = 0;

	std::string path = "";
	std::string name = "";

	uint width = 0;
	uint height = 0;

	uint GL_id = 0;

	uint timesUsed = 0;
};

class ModuleTextures : public Module
{
public:
	ModuleTextures(bool start_enabled = true);
	~ModuleTextures();

	bool Init();
	bool Start();

	bool CleanUp();

	std::string GetFileName(const char* path, bool includeExtension = true);
	std::string NormalizePath(const char* path);
	
	bool ImportTexture(const char* path);
	void ExportTexture(const char* path, Texture* texture) const;
	void ExportTexture(const char* path, uint width, uint height, uint textureID) const;
	void ExportImage(const char* path, uint imageID) const;
	
	uint UseTexture(uint textureUID);
	void UnuseTexture(uint textureUID);
	uint UseWhiteTexture();

	uint GetTextureUID(const char* name);
	std::string GetTextureName(uint textureUID);

	uint CreateTextureImage(uint ID, int width, int height);
	uint GenerateImage(int width, int height);
	void InsertImage(uint target, uint source, int offsetX, int offsetY, int width, int height);
	void DeleteImage(uint imageID);

private:

	void GenerateResources();

	void LoadTexture(Texture* texture);
	void UnloadTexture(Texture* texture);

	void CreateWhiteTexture();

public:

	std::map<uint, Texture*> textures;

	Texture* whiteTexture = nullptr;
};

#endif // !__MODULETEXTURES_H__