#include "../headers/Texture.h"
#include <SOIL\SOIL.h>
#include <assert.h>
#include "../headers/TextureException.h"

const bool INVERT_Y = true;

TextureArray *TextureArray::instance = nullptr;


/******************************************************************************************************************************************************************************************/
/*Texture Class*/
Texture::Texture() {
	id = 0; 
	texture = ""; 
}
/************************************************************************************/
Texture::Texture(Texture &A){
	id = A.getTexture();
	texture = A.getTextureFile(); 
}

Texture::Texture(unsigned int idd, std::string name){
	id = idd; 
	texture = name; 
}


Texture::Texture(std::string& filename)
{

		glGenTextures(1, &id);
		int width = 0, height = 0; 
		unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		if (image == 0)
			throw TextureException(filename);
		else {
			texture = filename;
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -3);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	
}


Texture::~Texture()
{
	

}

void Texture::clean(){
	if (DEBUG_DESTRUCTOR)
		std::cout << "Texture Deleted" << std::endl;

	glDeleteTextures(1, &id);
}
/************************************************************************************/

GLuint Texture::getTexture() {
	
	return id; 
}
/************************************************************************************/

void Texture::Bind(Echeyde::TEX tex_index , Shader *shader) {
	assert(id != 0);
	assert(shader != nullptr); 
	shader->BindTextures(tex_index);
	
	glActiveTexture(GL_TEXTURE0 + tex_index);
	glBindTexture(GL_TEXTURE_2D, id);

}
/************************************************************************************/

void Texture::Unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

/************************************************************************************/
bool Texture::isInitialized() {
	return (id == 0 || texture.size()==0) ? false : true;
}

/************************************************************************************/
bool Texture::operator==(Texture& A) const{
	if (texture.compare(A.getTextureFile()) == 0)
		return true;
	else
		return false; 
}

bool Texture::operator<(const Texture &A) const{
	if (texture < A.getTextureFile())
		return true;
	else
		return false;
}

void Texture::operator=(Texture &A)  {
	id = A.getTexture(); 
	texture = A.getTextureFile(); 
}

/******************************************************************************************************************************************************************************************/
/*TextureGroup class*/
TextureGroup::TextureGroup() {
	diffuse_textures = std::shared_ptr<Texture>(new Texture());
	blend_textures = std::shared_ptr<Texture>(new Texture());
	normal_textures = std::shared_ptr<Texture>(new Texture());
	opacity_textures = std::shared_ptr<Texture>(new Texture());
	distortion_textures = std::shared_ptr<Texture>(new Texture());
	optional_textures = std::shared_ptr<Texture>(new Texture());
	shadowmap_textures = std::shared_ptr<Texture>(new Texture());
}

TextureGroup::TextureGroup(TextureGroup &A){
	diffuse_textures = std::shared_ptr<Texture>(new Texture(*A.getDiffuse_textures()));
	blend_textures = std::shared_ptr<Texture>(new Texture(*A.getBlend_textures()));
	normal_textures = std::shared_ptr<Texture>(new Texture(*A.getNormal_textures()));
	opacity_textures = std::shared_ptr<Texture>(new Texture(*A.getOpacity_textures()));
	distortion_textures = std::shared_ptr<Texture>(new Texture(*A.getDistortion_textures()));
	optional_textures = std::shared_ptr<Texture>(new Texture(*A.getOptional_textures()));
	shadowmap_textures = std::shared_ptr<Texture>(new Texture(*A.getShadowmap_textures()));
}



/************************************************************************************/
TextureGroup::TextureGroup(material_data& mat) {
	TextureArray *instance = TextureArray::getUniqueInstance(); 
	diffuse_textures = std::shared_ptr<Texture>(new Texture());
	normal_textures = std::shared_ptr<Texture>(new Texture());
	opacity_textures = std::shared_ptr<Texture>(new Texture());
	distortion_textures = std::shared_ptr<Texture>(new Texture());
	optional_textures = std::shared_ptr<Texture>(new Texture());
	blend_textures = std::shared_ptr<Texture>(new Texture());
	shadowmap_textures = std::shared_ptr<Texture>(new Texture()); 
	try {
		
			
		if (!mat.diffuse_empty())	diffuse_textures=instance->addOrReturn(mat.textures.diffuse,-1);
		if (!mat.normal_empty())	normal_textures= instance->addOrReturn(mat.textures.normal,-1);
		if (!mat.opacity_empty())	opacity_textures = instance->addOrReturn(mat.textures.opacity, -1);
		if (!mat.distortion_empty()) distortion_textures = instance->addOrReturn(mat.textures.distortion, -1);
		if (!mat.optional_empty())	optional_textures = instance->addOrReturn(mat.textures.optional, -1);
		if (!mat.blend_empty())	blend_textures = instance->addOrReturn(mat.textures.blend, -1);
		if (!mat.shadowmap_empty()) shadowmap_textures = instance->addOrReturn(mat.textures.shadowmap, -1);
		Mdata = mat; 
		
	}
	catch (TextureException &e) {
		std::cout << e.what() << std::endl;
	}
}



/************************************************************************************/


TextureGroup::~TextureGroup() {
	
}

void TextureGroup::clean(){
	if (DEBUG_DESTRUCTOR)
		std::cout << "Texture group deleted" << std::endl;
	if (diffuse_textures != nullptr) diffuse_textures->clean();
	if (normal_textures != nullptr) normal_textures->clean();
	if (opacity_textures != nullptr) opacity_textures->clean();
	if (blend_textures != nullptr) blend_textures->clean();
	if (distortion_textures != nullptr) distortion_textures->clean();
	if (optional_textures != nullptr) optional_textures->clean();
	if (shadowmap_textures != nullptr) shadowmap_textures->clean();
}

/************************************************************************************/
bool TextureGroup::isInitialized() {/*check null*/
	if (diffuse_textures == nullptr && shadowmap_textures == nullptr && normal_textures == nullptr && opacity_textures == nullptr && distortion_textures == nullptr && optional_textures == nullptr && blend_textures == nullptr)
		return false;
	else {
		if (diffuse_textures != nullptr && diffuse_textures->isInitialized() || normal_textures != nullptr && normal_textures->isInitialized()
			|| opacity_textures != nullptr && opacity_textures->isInitialized() || distortion_textures != nullptr && distortion_textures->isInitialized()
			|| optional_textures != nullptr && optional_textures->isInitialized() || blend_textures != nullptr && blend_textures->isInitialized() || shadowmap_textures != nullptr && shadowmap_textures->isInitialized())
			return true;
		else
			return false;
		
	}
}

bool TextureGroup::operator<(const TextureGroup &A) const{
	bool diffuse = diffuse_textures < A.getDiffuse_textures();
	bool normal = normal_textures < A.getNormal_textures(); 
	bool opacity = opacity_textures < A.getOpacity_textures(); 
	bool optional = optional_textures < A.getOptional_textures(); 
	bool blend = blend_textures < A.getBlend_textures(); 
	bool disto = distortion_textures < A.getDistortion_textures();
	bool shadowmap = shadowmap_textures < A.getShadowmap_textures(); 
	return diffuse && normal && opacity && optional && blend && disto && shadowmap; 

}
/************************************************************************************/

bool TextureGroup::operator==(TextureGroup &A) const{
	if (*blend_textures == *A.getBlend_textures() 
		&& *diffuse_textures == *A.getDiffuse_textures() 
		&& *normal_textures == *A.getNormal_textures()
		&& *distortion_textures == *A.getDistortion_textures() 
		&& *opacity_textures == *A.getOpacity_textures() 
		&& *optional_textures == *A.getOptional_textures()
		&& *shadowmap_textures == *A.getShadowmap_textures()
		)
		return true;
	else
		return false; 
}

/************************************************************************************/

void TextureGroup::bindFirst(Shader* shader) {
	if (blend_textures != nullptr && blend_textures->isInitialized()) 
		blend_textures->Bind(Echeyde::BLENDMAP0, shader);

	if (diffuse_textures != nullptr && diffuse_textures->isInitialized()) 
		diffuse_textures->Bind(Echeyde::DIFFUSE0, shader);
	
	if (normal_textures != nullptr && normal_textures->isInitialized()) 
		normal_textures->Bind(Echeyde::NORMAL0, shader);
	
	if (opacity_textures != nullptr && opacity_textures->isInitialized())
		opacity_textures->Bind(Echeyde::OPACITY0, shader);

	if (distortion_textures != nullptr && distortion_textures->isInitialized()) 
		distortion_textures->Bind(Echeyde::DUDV0, shader);

	if (optional_textures != nullptr && optional_textures->isInitialized()) 
		optional_textures->Bind(Echeyde::OPTIONAL0, shader);

	if (shadowmap_textures != nullptr && shadowmap_textures->isInitialized()) 
		shadowmap_textures->Bind(Echeyde::SHADOWMAP0, shader); 
	

}
	




void TextureGroup::setTexture(Texture B, Echeyde::TEX type){
	TextureArray* texArray = TextureArray::getUniqueInstance(); 
	Texture A = *texArray->addOrReturn(const_cast<std::string&>(B.getTextureFile()), B.getTexture());
	switch (type){
	case Echeyde::DIFFUSE0 : 
		setDiffuse_textures(A);
		break;
	case Echeyde::NORMAL0 : 
		setNormal_textures(A); 
		break; 
	case Echeyde::OPACITY0:
		setOpacity_textures(A); 
		break; 
	case Echeyde::DUDV0:
		setDistortion_textures(A); 
		break;
	case Echeyde::OPTIONAL0:
		setOptional_textures(A);
		break;
	case Echeyde::SHADOWMAP0:
		setShadowmap_textures(A);
		break;
	case Echeyde::BLENDMAP0:
		setBlend_textures(A);
		break;
	}
}


void TextureGroup::setDiffuse_textures(Texture A)   { 
	
	*diffuse_textures = A; 
}
void TextureGroup::setNormal_textures(Texture A)  { 

	*normal_textures = A; 
}
void TextureGroup::setOpacity_textures(Texture A)  { 

	*opacity_textures = A; 
}
void TextureGroup::setDistortion_textures(Texture A)  {

	*distortion_textures = A; 
}
void TextureGroup::setOptional_textures(Texture A)  {

	*optional_textures = A; 
}
void TextureGroup::setBlend_textures(Texture A)  {

	*blend_textures = A; 
}
void TextureGroup::setShadowmap_textures(Texture A)  {

	*shadowmap_textures = A; 
}

void TextureGroup::unbind() {
	for (unsigned int i = 0; i < Echeyde::TEX_SIZE; i++){
		glActiveTexture(GL_TEXTURE0 + i); 
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

/******************************************************************************************************************************************************************************************/
/*TextureArray class*/


TextureArray* TextureArray::getUniqueInstance() {
	if (instance == nullptr)
		instance = new TextureArray(); 
	return instance;
}

/************************************************************************************/

void TextureArray::clean() {
	delete instance;
}

/************************************************************************************/

TextureArray::TextureArray() {

}
/************************************************************************************/

TextureArray::~TextureArray() {

}
/************************************************************************************/

std::shared_ptr<Texture> TextureArray::addOrReturn(std::string& file , GLuint id) {
	for (const std::pair<unsigned int, std::shared_ptr<Texture>> &P : texture_array) {
		if (P.second->getTextureFile().compare(file) == 0)
			return P.second;
	}
	std::shared_ptr<Texture> tex;
	try {
		if (file.find(std::string(".png")) != std::string::npos)
			tex = std::shared_ptr<Texture>(new Texture(file));
		else{
			tex = std::shared_ptr<Texture>(new Texture());
			assert(id != -1); 
			tex->setID(id); 
			tex->setName(file); 
		}

		}
	catch (const TextureException& e) {
		std::cout << e.what() << std::endl; 
		return std::shared_ptr<Texture>(); 
	}
	std::pair<unsigned int, std::shared_ptr<Texture>> pair(tex->getTexture(), tex); 
	texture_array.insert(pair); 
	return tex; 
}
/************************************************************************************/

bool TextureArray::idUsed(unsigned int id) {
	auto it = texture_array.find(id);
	if (it != texture_array.end())
	{
		if (glIsTexture(id))
			return true;
		else {
			std::cout << "texture " + (*it).second->getTextureFile() + " TextureArray::idUsed() problem !" << "\n";
			return false;
		}
	}
	else
		return false; 
}
/************************************************************************************/

bool TextureArray::isLoadedTexture(std::string& filename) {
	for (const std::pair<unsigned int, std::shared_ptr<Texture>> &P : texture_array) {
		if (P.second->getTextureFile().compare(filename) == 0)
			return true;
	}
	
	return false; 
}

/************************************************************************************/


bool TextureArray::erase(GLuint tex) {
	auto it = texture_array.find(tex);
	if (it != texture_array.end()) {
		texture_array.erase(it); 
		return true;
	}
	return false;
}

/************************************************************************************/
