#ifndef SHADEROBJ_H
#define SHADEROBJ_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class ShaderObj {
public:
	ShaderObj(unsigned int ID);
	void update();

	unsigned int id;
	std::vector<std::string> uniforms;

	int mode;

};

#endif