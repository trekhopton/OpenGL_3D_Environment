#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "Model.h"

Model::Model(glm::vec3 pos, std::string file, unsigned int progID)  : Object(pos){
	inputFile = file;
	std::string basedir = getBaseDir(inputFile);
	std::string err;
	bool ret = tinyobj::LoadObj(shapes, materials, err, inputFile.c_str(), basedir.c_str());

	if (!err.empty()) { // `err` may contain warning message.
	  std::cerr << err << std::endl;
	}

	if (!ret) {
	  exit(1);
	}
	//vaos
	createVAOs(progID);
	//textures
	createTextures();
	
}

void Model::update(ShaderObj* shader){

	glUseProgram(shader->id);

	for(int i = 0; i < shapes.size(); i++){
		for(int j = 0; j < shader->uniforms.size(); j++){
			//create handle and check it exists
			int handle = glGetUniformLocation(shader->id, shader->uniforms[j].c_str());
			if (handle == -1){
				fprintf(stderr, "Error: can't find uniform: %s\n", shader->uniforms[j].c_str());
				exit(1);
			}
			//update the unifom depending on what it is
			if(shader->uniforms[j] == "mtlAmbient"){
				glUniform3fv(handle, 1, materials[shapes[i].mesh.material_ids[0]].ambient);
			}
			else if(shader->uniforms[j] == "mtlDiffuse"){
				glUniform3fv(handle, 1, materials[shapes[i].mesh.material_ids[0]].diffuse);
			}
			else if(shader->uniforms[j] == "mtlSpecular"){
				glUniform3fv(handle, 1, materials[shapes[i].mesh.material_ids[0]].specular);
			}
			else if(shader->uniforms[j] == "texMap"){
				glBindTexture(GL_TEXTURE_2D, texIDs[i]);
				glUniform1i(handle, 0);//texture
			}
			else if(shader->uniforms[j] == "model_matrix"){
				glm::mat4 modelMatrix;
				modelMatrix = glm::translate(modelMatrix, position);
				glUniformMatrix4fv(handle, 1, false, glm::value_ptr(modelMatrix));
			}
			else if(shader->uniforms[j] == "normal_matrix"){
				glm::mat3 normMatrix;
				glm::mat4 modelMatrix;
				modelMatrix = glm::translate(modelMatrix, position);
				normMatrix = glm::mat3(modelMatrix);
				glUniformMatrix3fv(handle, 1, false, glm::value_ptr(normMatrix));
			}
		}
		glBindVertexArray(vaoHandles[i]);
		glDrawElements(GL_TRIANGLES, shapes[i].mesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

std::string Model::getBaseDir(std::string filepath) {
  if (filepath.find_last_of("/\\") != std::string::npos)
	return filepath.substr(0, filepath.find_last_of("/\\")+1);
  return "";
}

void Model::createVAOs(unsigned int progID){

	glUseProgram(progID);

	for(int i = 0; i < shapes.size(); i++){

		unsigned int vaoh;
		vaoHandles.push_back(vaoh);

		glGenVertexArrays(1, &vaoHandles[i]);
		glBindVertexArray(vaoHandles[i]);

		int vertLoc = glGetAttribLocation(progID, "a_vertex");
		int normLoc = glGetAttribLocation(progID, "a_normal");
		int texLoc = glGetAttribLocation(progID, "a_texcoord");
		if (vertLoc == -1 || normLoc == -1 || texLoc == -1) {
			fprintf(stderr, "Error: can't find locations\n");
			exit(1);
		}

		unsigned int buffer[4];
		glGenBuffers(4, buffer);

		// Set vertex position attribute
		glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes[i].mesh.positions.size(), 
		&shapes[i].mesh.positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(vertLoc);
		glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// Normal attributes
		glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes[i].mesh.normals.size(), 
		&shapes[i].mesh.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(normLoc);
		glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// Tex coords
		glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapes[i].mesh.texcoords.size(), 
		&shapes[i].mesh.texcoords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(texLoc);
		glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

		// Vertex indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 
		shapes[i].mesh.indices.size(), &shapes[i].mesh.indices[0], GL_STATIC_DRAW);

		// Un-bind
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Model::createTextures(){
	for(int i = 0; i < materials.size(); i++){

		glActiveTexture(GL_TEXTURE0);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		texIDs.push_back(0);
		glGenTextures(1, &texIDs[i]);
		glBindTexture(GL_TEXTURE_2D, texIDs[i]);

		int x, y, n;

		// Load from file. Force RGB image pixel format
		unsigned char *data = stbi_load((getBaseDir(inputFile) + materials[shapes[i].mesh.material_ids[0]].diffuse_texname).c_str(), &x, &y, &n, 0);

		// Copy to GPU as data for the currently active texture.
		if (n == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		} else if (n == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		} else {
			fprintf(stderr, "Pixels are not RGB in %s\n", materials[shapes[i].mesh.material_ids[0]].diffuse_texname.c_str());
		}
		
		stbi_image_free(data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}