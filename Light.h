#ifndef LIGHT_H
#define LIGHT_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Object.h"
#include "ShaderObj.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Light : public Object{
public:
    Light(glm::vec3 pos);
	virtual void update(ShaderObj* shader);

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class DirectionalLight : public Light{
public:
	DirectionalLight(glm::vec3 pos, glm::vec3 dir);
	void update(ShaderObj* shader);
	glm::vec3 direction;
};

// class PointLight : public Light{
// public:
// 	PointLight(glm::vec3 pos, glm::vec3 dir);
// 	glm::vec3 radius;
// };

// class SpotLight : public Light{
// public:
// 	SpotLight(glm::vec3 pos, glm::vec3 dir);
// 	glm::vec3 direction;
// };

#endif