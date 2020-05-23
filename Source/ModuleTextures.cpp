#include "ModuleTextures.h"
#include "Application.h"
#include "Globals.h"

#include <filesystem>

#include "DevIL\include\il.h"
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"

#pragma comment( lib, "DevIL/libx86/DevIL.lib")
#pragma comment( lib, "DevIL/libx86/ILU.lib")
#pragma comment( lib, "DevIL/libx86/ILUT.lib")

namespace fs = std::experimental::filesystem;

ModuleTextures::ModuleTextures(bool start_enabled) : Module(start_enabled)
{
}

ModuleTextures::~ModuleTextures()
{
}

bool ModuleTextures::Init()
{
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return true;
}

bool ModuleTextures::Start()
{
	GenerateResources();
	CreateWhiteTexture();

	return true;
}

bool ModuleTextures::CleanUp()
{
	std::map<uint, Texture*>::iterator it_t;
	it_t = textures.begin();
	while (it_t != textures.end())
	{
		RELEASE((*it_t).second);
		it_t++;
	}
	textures.clear();

	ilShutDown();
	
	return true;
}

std::string ModuleTextures::GetFileName(const char* path, bool includeExtension)
{
	std::string str = NormalizePath(path);

	uint pos_slash = str.find_last_of('/');
	uint pos_dot = str.find_last_of('.');

	if (pos_slash < str.length())
		str = str.substr(pos_slash + 1, ((includeExtension)? str.length():pos_dot) - pos_slash - 1);
	else
		str = str.substr(0, ((includeExtension) ? str.length() : pos_dot));

	return str.c_str();
}

std::string ModuleTextures::NormalizePath(const char * path)
{
	std::string str = path;

	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == '\\')
			str[i] = '/';
	}

	return str.c_str();
}

bool ModuleTextures::ImportTexture(const char* path)
{
	ILuint ilImage;
	bool success;
	ILenum error;

	ilGenImages(1, &ilImage);
	ilBindImage(ilImage);

	success = ilLoadImage(path);

	if (success)
	{
		std::string newPath = ASSETS_FOLDER + GetFileName(path);		
		ilSaveImage(newPath.c_str());
	}
	else
	{
		error = ilGetError();
		LOG("Image load failed - IL error: %s", iluErrorString(error));
	}

	ilDeleteImages(1, &ilImage);

	Texture* tex = new Texture(path);
	textures.insert(std::pair<uint, Texture*>(tex->UID, tex));

	return success;
}

void ModuleTextures::ExportTexture(const char* path, Texture* texture) const
{
	ILuint ilImage;

	ilGenImages(1, &ilImage);
	ilBindImage(ilImage);
	ilTexImage(texture->width, texture->height, 0, 4, IL_RGBA, IL_UNSIGNED_BYTE, nullptr);

	uint* pixels = new uint[texture->width*texture->height];
	glBindTexture(GL_TEXTURE_2D, texture->GL_id);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	//ilSetPixels()
	ilSetData(pixels);

	ilSaveImage(path);

	ilDeleteImages(1, &ilImage);
	delete[] pixels;
}

void ModuleTextures::ExportTexture(const char* path, uint width, uint height, uint textureID) const
{
	ILuint ilImage;

	ilGenImages(1, &ilImage);
	ilBindImage(ilImage);
	ilTexImage(width, height, 0, 4, IL_RGBA, IL_UNSIGNED_BYTE, nullptr);

	uint* pixels = new uint[width*height];
	glBindTexture(GL_TEXTURE_2D, textureID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	ilSetData(pixels);

	ilSaveImage(path);

	ilDeleteImages(1, &ilImage);
	delete[] pixels;
}

void ModuleTextures::ExportImage(const char * path, uint imageID) const
{
	ilBindImage(imageID);
	ilSaveImage(path);
}

void ModuleTextures::GenerateResources()
{
	for (auto& f : fs::recursive_directory_iterator(ASSETS_FOLDER))
	{
		std::string extension = f.path().extension().string();
		std::string file = f.path().string();
		file = NormalizePath(file.c_str());
		if (extension == ".png" || extension == ".jpeg" || extension == ".jpg" || extension == ".dds")
		{
			Texture* tex = new Texture(file.c_str());
			textures.insert(std::pair<uint, Texture*>(tex->UID, tex));
		}
		else
		{
			LOG("Unsuported file format: %s", file.c_str())
		}
	}
}

void ModuleTextures::LoadTexture(Texture* texture)
{
	ILuint ilImage;
	bool success;
	ILenum error;

	ilGenImages(1, &ilImage);
	ilBindImage(ilImage);

	success = ilLoadImage(texture->path.c_str());

	if (success)
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);

		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			iluFlipImage();

		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (!success)
		{
			error = ilGetError();
			LOG("Image conversion failed - IL error: %s", iluErrorString(error));
		}

		glGenTextures(1, &texture->GL_id);
		glBindTexture(GL_TEXTURE_2D, texture->GL_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageInfo.Width, ImageInfo.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
		texture->width = ImageInfo.Width;
		texture->height = ImageInfo.Height;

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		LOG("Error loading texture: %s", texture->path.c_str());
	}

	ilDeleteImages(1, &ilImage);
}

void ModuleTextures::UnloadTexture(Texture* texture)
{
	glDeleteTextures(1, &texture->GL_id);
}

void ModuleTextures::CreateWhiteTexture()
{
	whiteTexture = new Texture("white");

	glGenTextures(1, &whiteTexture->GL_id);

	GLubyte data[] = { 255, 255, 255, 255};

	glBindTexture(GL_TEXTURE_2D, whiteTexture->GL_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	whiteTexture->width = 1;
	whiteTexture->height = 1;
}

uint ModuleTextures::UseTexture(uint textureUID)
{
	if (textures.find(textureUID) != textures.end())
	{
		Texture* tex = textures[textureUID];
		if (tex->timesUsed++ == 0)
			LoadTexture(tex);
		return tex->GL_id;
	}

	return 0;
}

void ModuleTextures::UnuseTexture(uint textureUID)
{
	if (textures.find(textureUID) != textures.end())
	{
		Texture* tex = textures[textureUID];
		if (--tex->timesUsed == 0)
			UnloadTexture(tex);
	}
}

uint ModuleTextures::UseWhiteTexture()
{
	return whiteTexture->GL_id;
}

uint ModuleTextures::GetTextureUID(const char* name)
{
	uint ret = 0;
	for (std::map<uint, Texture*>::iterator it_t = textures.begin(); it_t != textures.end(); ++it_t)
	{
		if ((*it_t).second->name == name)
		{
			ret = (*it_t).first;
			break;
		}
	}

	return ret;
}

std::string ModuleTextures::GetTextureName(uint textureUID)
{
	if (textures.find(textureUID) != textures.end())
	{
		Texture* tex = textures[textureUID];
		return tex->name.c_str();
	}

	return std::string();
}

uint ModuleTextures::CreateTextureImage(uint ID, int width, int height)
{
	ILuint ilImage;

	ilGenImages(1, &ilImage);
	ilBindImage(ilImage);
	ilTexImage(width, height, 0, 4, IL_RGBA, IL_UNSIGNED_BYTE, nullptr);

	uint* pixels = new uint[width*height];
	glBindTexture(GL_TEXTURE_2D, ID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	ilSetData(pixels);

	delete[] pixels;

	return ilImage;
}

uint ModuleTextures::GenerateImage(int width, int height)
{
	ILuint ilImage;

	ilGenImages(1, &ilImage);
	ilBindImage(ilImage);
	ilTexImage(width, height, 0, 4, IL_RGBA, IL_UNSIGNED_BYTE, nullptr);

	return ilImage;
}

void ModuleTextures::InsertImage(uint target, uint source, int offsetX, int offsetY, int width, int height)
{
	//Retrieve source image pixel data
	ilBindImage(source);
	uint* pixels = (uint*)ilGetData();

	//Insert pixel data into target image
	ilBindImage(target);
	ilSetPixels(offsetX, offsetY, 0, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void ModuleTextures::DeleteImage(uint imageID)
{
	ilDeleteImage(imageID);
}

Texture::Texture(const char* path): path(path)
{
	UID = GENERATE_UID();
	name = App->textures->GetFileName(path, false);
}

Texture::~Texture()
{
	glDeleteTextures(1, &GL_id);
}
