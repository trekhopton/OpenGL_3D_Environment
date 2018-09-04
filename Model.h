#ifndef MODEL_H
#define MODEL_H

#include "string"
#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Libraries/stb_image.h"
#include "Libraries/tiny_obj_loader.h"

#include "Object.h"
#include "ShaderObj.h"

class Model : public Object{
public:
    Model(glm::vec3 pos, std::string file, unsigned int progID);
	void update(ShaderObj* shader);
    void createTextures();
    //input file path name
	std::string inputFile;
    //VAO handles for object
	std::vector<unsigned int> vaoHandles;
    //Textures
    std::vector<GLuint> texIDs;
    //shapes and materials for the object
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
private:
    //used to get the directory of the material
	std::string getBaseDir(std::string filepath);
    //create VAOs for the object
	void createVAOs(unsigned int progID);
};

#endif