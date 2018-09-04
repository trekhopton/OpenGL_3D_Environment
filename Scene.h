#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "Object.h"
#include "Model.h"
#include "Light.h"
#include "Camera.h"
#include "InputState.h"
#include "Shader.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Scene{
public:

	Scene(std::vector<ShaderObj*> sh);
	~Scene();
	void render();
	void setProjection();
	void updateKeyboard(int, bool);

	//Objects: eg. cameras, lights, models. 
	std::vector<Model*> models;
	std::vector<Camera*> cameras;
	std::vector<Light*> lights;
	int camID;

	float fov;
	int winWidth;
	int winHeight;

	//input data structure
	InputState input;

	double deltaTime;

	//Shaders
	int currentShader;
	std::vector<ShaderObj*> shaders;

	int debugMode;

};

#endif