#include "ShaderObj.h" 

ShaderObj::ShaderObj(unsigned int ID){
	id = ID;
}

void ShaderObj::update(){
	for(int j = 0; j < uniforms.size(); j++){
		//create handle and check it exists
		int handle = glGetUniformLocation(id, uniforms[j].c_str());
		if (handle == -1){
			fprintf(stderr, "Error: can't find uniform: %s\n", uniforms[j].c_str());
			exit(1);
		}
		//update the unifom depending on what it is
		if(uniforms[j] == "mode"){
			glUniform1i(handle, mode);
		}
	}
}