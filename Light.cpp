#include "Light.h"

Light::Light(glm::vec3 pos) : Object(pos){

}

void Light::update(ShaderObj* shader){
	glUseProgram(shader->id);

	for(int i = 0; i < shader->uniforms.size(); i++){
		//create handle and check it exists
		int handle = glGetUniformLocation(shader->id, shader->uniforms[i].c_str());
		if (handle == -1){
			fprintf(stderr, "Error: can't find uniform: %s\n", shader->uniforms[i].c_str());
			exit(1);
		}
		//update the unifom depending on what it is
		if(shader->uniforms[i] == "lightAmbient"){
			ambient = glm::vec3(0.1f, 0.1f, 0.1f);
			glUniform3fv(handle, 1, glm::value_ptr(ambient));
		} else if(shader->uniforms[i] == "lightDiffuse"){
			diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(handle, 1, glm::value_ptr(diffuse));
		} else if(shader->uniforms[i] == "lightSpecular"){
			specular = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(handle, 1, glm::value_ptr(specular));
		} else if(shader->uniforms[i] == "lightPos"){
			glUniform3fv(handle, 1, glm::value_ptr(position));
		} 
	}
}

DirectionalLight::DirectionalLight(glm::vec3 pos, glm::vec3 dir) : Light(pos){
	direction = dir;
}

void DirectionalLight::update(ShaderObj* shader){
	glUseProgram(shader->id);

	for(int i = 0; i < shader->uniforms.size(); i++){
		//create handle and check it exists
		int handle = glGetUniformLocation(shader->id, shader->uniforms[i].c_str());
		if (handle == -1){
			fprintf(stderr, "Error: can't find uniform: %s\n", shader->uniforms[i].c_str());
			exit(1);
		}
		//update the unifom depending on what it is
		if(shader->uniforms[i] == "lightAmbient"){
			ambient = glm::vec3(0.1f, 0.1f, 0.1f);
			glUniform3fv(handle, 1, glm::value_ptr(ambient));
		} else if(shader->uniforms[i] == "lightDiffuse"){
			diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(handle, 1, glm::value_ptr(diffuse));
		} else if(shader->uniforms[i] == "lightSpecular"){
			specular = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(handle, 1, glm::value_ptr(specular));
		} else if(shader->uniforms[i] == "lightPos"){
			glUniform3fv(handle, 1, glm::value_ptr(position));
		} else if(shader->uniforms[i] == "lightDir"){
			glUniform3fv(handle, 1, glm::value_ptr(direction));
		}
	}
}