#include "../headers/Window.h"
#include "../headers/Importer.h"
#include "../headers/Camera.h"
#include "../headers/Renderer.h"
#include "../headers/Light.h"
#include "../headers/GameObject.h"
#include "../headers/Terrain.h"
Window::Window(const std::string name , Uint16 width , Uint16 height,KInputs &inputs , Echeyde::Scene &sc)
{
	dimensions.height = height; 
	dimensions.width = width; 
	input = &inputs; 
	window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cout << "Window Creation error !" << "\n";
		SDL_Quit();
	}
	context = SDL_GL_CreateContext(window); 
	if (context == 0) {
		std::cout << "Context creation error !" << "\n"; 
		SDL_DestroyWindow(window); 
		SDL_Quit(); 
	}
#ifdef WIN32
	GLenum erno = glewInit();
	if (erno != GLEW_OK) {
		std::cout << "GLEW initialization fucked up!" << "\n";
		SDL_GL_DeleteContext(context); 
		SDL_DestroyWindow(window); 
		SDL_Quit(); 
	}
#endif 
	loop = true;
	scene = &sc;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); 
	glCullFace(GL_BACK); 

}

Window::~Window()
{
	SDL_GL_DeleteContext(context); 
	SDL_DestroyWindow(window); 

}

Dim Window::getDimensions() {
	
	return dimensions;

}

void Window::Loop() {
	BaseShader *shader = new BaseShader(std::string("vertex.vert"), std::string("fragment.frag") , std::string("geometry.geom"));
	BaseShader *terrain_shader = new BaseShader(std::string("terrain.vert"), std::string("terrain.frag"), std::string("terrain.geom")); 
	std::vector<object_data> st;
	std::vector<object_data> si;
	try {
	st = Echeyde::FILEIO::Importer::load_model(std::string("cube.obj"),true);
    si = Echeyde::FILEIO::Importer::load_model(std::string("sphere.obj"),false);
	}
	catch (const File_not_found &e) {
		std::cout << e.what() << std::endl;
	}
	catch (const MaterialException &e) {
		std::cout << e.what() << std::endl;
	}
	catch (const Importer_Error &e) {
		std::cout << e.what() << std::endl;
	}
	
	Renderer *renderer = Renderer::getInstance();
	LightArray *lights = LightArray::getInstance();

	
	GameObject cube = GameObject(st, shader, true, false);
	GameObject obj2 = GameObject(si, shader, true, false);
	shader_light_data data;
		
		
		cube.scale(glm::vec3(10, 10, 10)); 
		cube.translate(glm::vec3(10, 10, 10));
		data.base.position = glm::vec3(10, 2, 3);
		data.attenuation = glm::vec3(1, 10, 100);
		data.radius = 100.f;
		data.base.colour = glm::vec3(0.1, 0.4, 0.6);
		data.base.power = 50.F;
		lights->addLight(new PointLight(data.base, data.radius, data.attenuation));


		data.base.position = glm::vec3(300, 600, 2);
		data.base.colour = glm::vec3(0.7, 0.5, 0.4);
		data.base.power = 1.F;
		lights->addLight(new DirectionalLight(data.base));


		data.base.position = glm::vec3(181, 30, 200);
		data.attenuation = glm::vec3(1, 100, 10);
		data.radius = 100.f;
		data.base.colour = glm::vec3(0.6, 0.5, 0.3);
		data.base.power = 60.F;
		lights->addLight(new PointLight(data.base, data.radius, data.attenuation));

		data.base.position = glm::vec3(300, 10, 30);
		data.attenuation = glm::vec3(1, 10, 100);
		data.radius = 100.f;
		data.base.colour = glm::vec3(0.6, 0.4, 0.3);
		data.base.power = 100.F;
		lights->addLight(new PointLight(data.base, data.radius, data.attenuation));


		geometry_data terrain_geometry = TerrainGenerator::generateTerrain((HEIGHTMAPS_LOCATION+"heightmap.png").c_str());
		material_data mat_data ;
		object_data terrain_obj;
		mat_data.transparency = false; 
		terrain_obj.data = terrain_geometry;
		mat_data.textures.diffuse = TEXTURES_LOCATION + "/ground" + "/dirt" + "/dirt_01.png";
		mat_data.textures.opacity = TEXTURES_LOCATION + "/blendmaps" + "/blend.png";
		mat_data.textures.normal = TEXTURES_LOCATION + "/ground" + "/gravel_01.png";
		mat_data.textures.optional = TEXTURES_LOCATION + "/ground" + "/snow" + "/snow_1.png";
		mat_data.textures.blend = TEXTURES_LOCATION + "/ground" + "/grass" + "/grass_02.png";
		
		terrain_obj.material = mat_data; 
		std::vector<object_data> ob; 
		ob.push_back(terrain_obj); 
		GameObject terrain = GameObject(ob, terrain_shader, true, true);
	





	
	std::unique_ptr<Camera> user_camera = std::unique_ptr<Camera>(new Camera(5.f, 5.f, glm::vec3(0, 1, 0), 100.f, float(WIDTH / HEIGHT), 0.1f, 5000.f));

	SDL_Event event; 
	float i = 0 , j = 0; 

	while (loop) {
		DirectionalLight *P = dynamic_cast<DirectionalLight*>(lights->getLightArray()[1]);
		while (SDL_PollEvent(&event)) {
			
			if (event.type == SDL_QUIT)
				loop = false;
			if (event.key.keysym.sym == SDLK_ESCAPE)
				loop= false;
			user_camera->Move(event);
			if (event.key.keysym.sym == SDLK_KP_8)
				i+=0.001f;
			if (event.key.keysym.sym == SDLK_KP_2)
				i-=1.5f;
			if (event.key.keysym.sym == SDLK_KP_4)
				j += 1.5f;
			if (event.key.keysym.sym == SDLK_KP_6)
				j -= 1.5f;
			
	//		std::cout << P->getPosition().x << "   " << P->getPosition().y << "   " << P->getPosition().z << std::endl;

		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		
	//	P->setRotation(0.001, glm::vec3(0, 0, 0));
		
		renderer->renderAll(user_camera->getProjectionMatrix(), user_camera->getViewMatrix()); 
		SDL_GL_SwapWindow(window); 
	}
	TextureArray::clean();
	renderer->destroy();
	lights->clean();
	delete shader;
	delete terrain_shader; 


	
}