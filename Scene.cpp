#include "Scene.h"

Scene::Scene(std::vector<ShaderObj*> sh){

	shaders = sh;
	//defaults to use first shader and camera
	currentShader = 0;
	camID = 0;

	fov = (float)M_PI/3.0f;
	winWidth = 500;
	winHeight = 500;
	debugMode = 3;

	glClearColor(0.5F, 0.5F, 0.5F, 0.0F);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(shaders[currentShader]->id);
	
	setProjection();
}

Scene::~Scene(){
	for(int i = 0; i < cameras.size(); i++){
		delete cameras[i];
	}
	for(int i = 0; i < models.size(); i++){
		delete models[i];
	}
	for(int i = 0; i < lights.size(); i++){
		delete lights[i];
	}
}

void Scene::setProjection(){
	glm::mat4 projection;
	projection = glm::perspective(fov, (float) winWidth / winHeight, 1.0f, 4096.0f);

	for(int i = 0; i < shaders.size(); i++){
		int projHandle = glGetUniformLocation(shaders[i]->id, "projection_matrix");
		if(projHandle == -1){
			fprintf(stderr, "Error updating proj matrix in shader %i\n", i);
			exit(1);
		}
		glUseProgram(shaders[i]->id);
		glUniformMatrix4fv(projHandle, 1, false, glm::value_ptr(projection));
	}
}

void Scene::updateKeyboard(int key, bool pressed){
	if(pressed){
		switch(key){
			case GLFW_KEY_D://change debug mode
				if(currentShader == 0){
					if (shaders[currentShader]->mode == 1){
						shaders[currentShader]->mode = 2;
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					} else if (shaders[currentShader]->mode == 2){
						shaders[currentShader]->mode = 3;
					} else {
						shaders[currentShader]->mode = 1;
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					}
				}

				break;
			case GLFW_KEY_S: // change shader to use
				if (currentShader == 0) {
					currentShader = 1;
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				} else {
					currentShader = 0;
					if (shaders[currentShader]->mode == 1){
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					}
				}
				break;
			case GLFW_KEY_C: // change camera
				if (camID == 0) {
					camID = 1;
				} else if (camID == 1){
					camID = 2;
				} else {
					camID = 0;
				}
				break;
			case GLFW_KEY_LEFT:
				input.updateKeyboard(key, true);
				break;
			case GLFW_KEY_RIGHT: 
				input.updateKeyboard(key, true);
				break;
			case GLFW_KEY_UP: 
				input.updateKeyboard(key, true);
				break;
			case GLFW_KEY_DOWN: 
				input.updateKeyboard(key, true);
				break;
		}
	} else {
		switch(key){
			case GLFW_KEY_LEFT: 
				input.updateKeyboard(key, false);
				break;
			case GLFW_KEY_RIGHT: 
				input.updateKeyboard(key, false);
				break;
			case GLFW_KEY_UP: 
				input.updateKeyboard(key, false);
				break;
			case GLFW_KEY_DOWN: 
				input.updateKeyboard(key, false);
				break;
		}
	}
}

void Scene::render(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaders[currentShader]->id);

	//update mouse and keyboard
	float xoff, yoff;
	input.readDeltaAndReset( &xoff, &yoff );
	cameras[camID]->ProcessMouseMovement(xoff, yoff);

	if(input.numKeysPressed != 0){
		if(input.keyMap.find(GLFW_KEY_LEFT)->second){
			cameras[camID]->ProcessKeyboard(LEFT, deltaTime);
		} 
		if(input.keyMap.find(GLFW_KEY_RIGHT)->second){
			cameras[camID]->ProcessKeyboard(RIGHT, deltaTime);
		}
		if(input.keyMap.find(GLFW_KEY_UP)->second){
			cameras[camID]->ProcessKeyboard(FORWARD, deltaTime);
		}
		if(input.keyMap.find(GLFW_KEY_DOWN)->second){
			cameras[camID]->ProcessKeyboard(BACKWARD, deltaTime);
		}
	}
	//update shader mode uniform
	shaders[currentShader]->update();

	// int numPointHandle = glGetUniformLocation(shaders[currentShader]->id, "numPoints");
	// if (numPointHandle == -1){
	// 	fprintf(stderr, "Error: can't find numPoints uniform\n");
	// 	exit(1);
	// }
	// glUniform1i(numPointHandle, mode);

	//update camera uniforms
	cameras[camID]->update(shaders[currentShader]);

	//update light uniforms
	for(int i = 0; i < lights.size(); i++){
		lights[i]->update(shaders[currentShader]);
	}

	//update and render models
	for(int i = 0; i < models.size(); i++){
		models[i]->update(shaders[currentShader]);
	}

	glFlush();
}