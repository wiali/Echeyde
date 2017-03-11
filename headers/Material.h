#pragma once
#include "View.h"
#include "Shader.h"
#include "Texture.h"
class Material
{
public:
	Material();
	Material(BaseShader *shader);
	Material(BaseShader *shader, TextureGroup& textures);
	Material(BaseShader *shader, material_data& material_dat);
	Material(BaseShader *shader, TextureGroup& textures, bool transparency);
	Material(const Material &A); 
	virtual ~Material();
	virtual void clean();
	const GLuint getProgram() const;
	void Unbind(); 
	void Bind();
	bool isTransparent() { return transparency; }
	bool isTextured() { return textures->isInitialized(); }
	bool operator==( Material A); 
	bool operator<(const Material &A) const;
	Material& operator=(Material& A);
	const TextureGroup getTextureGroup() const { return *textures;  }
	BaseShader* getShader() const { return shader; }
	float getSpecularExponent(){ return specular_exponent; }
	float getReflectivity(){ return reflectivity;  }

private:
	
	void BindValues(); 
	
	
	BaseShader *shader; // pointer that
	glm::vec3 ambient_color;
	glm::vec3 specular_color;
	float specular_power;
	float specular_exponent;
	float reflectivity; 
	bool transparency; 
	std::shared_ptr<TextureGroup> textures;


};
