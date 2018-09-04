#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "InputState.h"
#include "Camera.h"
#include "Shader.hpp"
#include "Object.h"
#include "Scene.h"
#include "Model.h"
#include "ShaderObj.h"

int winX = 800;
int winY = 800;

Scene *scene;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (action == GLFW_PRESS){
		if(key == GLFW_KEY_ESCAPE){
			glfwSetWindowShouldClose(window, GL_TRUE);
		} else {
			scene->updateKeyboard(key, true);
		}
	} else if(action == GLFW_RELEASE){
		if(key != GLFW_KEY_ESCAPE){
			scene->updateKeyboard(key, false);
		}
	}
}

void reshape_callback( GLFWwindow *window, int x, int y ){
	scene->winWidth = x;
	scene->winHeight = y;
	scene->setProjection();
	glViewport( 0, 0, x, y );
}

void mouse_pos_callback(GLFWwindow* window, double x, double y){
	scene->input.updateMouse((float)x, (float)y);
	float FOVmin = ((float)M_PI)/20.0;
	float FOVmax = ((float)M_PI)/2;
	if(scene->input.rMousePressed){
		scene->fov = scene->fov + scene->input.deltaY * 0.005;
		scene->setProjection();
		if(scene->fov < FOVmin){
			scene->fov = FOVmin;
		} else if (scene->fov > FOVmax){
			scene->fov = FOVmax;
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
		scene->input.rMousePressed = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE){
		scene->input.rMousePressed = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		scene->input.lMousePressed = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
		scene->input.lMousePressed = false;
	}
}
//Error callback for GLFW. Simply prints error message to stderr.
static void error_callback(int error, const char* description){
	fputs(description, stderr);
}

int main (int argc, char **argv){
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(1);
	}
	// Specify that we want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Create the window and OpenGL context
	window = glfwCreateWindow(winX, winY, "Model Viewer", NULL, NULL);
	if (!window){
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK){
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}

	glfwSetCursorPos(window, 0, 0);

	//Shaders
	unsigned int debugID = LoadShaders("Shaders/debug.vert", "Shaders/debug.frag");
	unsigned int lightTexID = LoadShaders("Shaders/light-texture.vert", "Shaders/light-texture.frag");
	if (debugID == 0 || lightTexID == 0) {
		fprintf(stderr, "Can't compile shaders!\n");
		exit(1);
	}

	ShaderObj* debugSh = new ShaderObj(debugID);

	debugSh->uniforms.push_back("projection_matrix");
	debugSh->uniforms.push_back("view_matrix");
	debugSh->uniforms.push_back("model_matrix");
	debugSh->uniforms.push_back("normal_matrix");
	
	debugSh->uniforms.push_back("mtlDiffuse");
	debugSh->uniforms.push_back("mode");

	ShaderObj* lightTexSh = new ShaderObj(lightTexID);

	lightTexSh->uniforms.push_back("projection_matrix");
	lightTexSh->uniforms.push_back("view_matrix");
	lightTexSh->uniforms.push_back("model_matrix");
	lightTexSh->uniforms.push_back("normal_matrix");

	lightTexSh->uniforms.push_back("viewPos");
	lightTexSh->uniforms.push_back("lightPos");
	lightTexSh->uniforms.push_back("lightDir");

	lightTexSh->uniforms.push_back("lightAmbient");
	lightTexSh->uniforms.push_back("lightDiffuse");
	lightTexSh->uniforms.push_back("lightSpecular");

	lightTexSh->uniforms.push_back("mtlAmbient");
	lightTexSh->uniforms.push_back("mtlDiffuse");
	lightTexSh->uniforms.push_back("mtlSpecular");
	lightTexSh->uniforms.push_back("texMap");
	// lightTexSh->uniforms.push_back("numPoints");

	std::vector<ShaderObj*> shaders;
	shaders.push_back(debugSh);
	shaders.push_back(lightTexSh);



	// Create scene
	scene = new Scene(shaders);

	//tell the scene to use lightTexSh (2nd shader) initially
	scene->currentShader = 1;

	//Cameras
	scene->cameras.push_back(new Camera(glm::vec3(0.0f, 4.0f, 10.0f)));
	scene->cameras.push_back(new Camera(glm::vec3(3.0f, 5.0f, 5.0f)));
	scene->cameras.push_back(new Camera(glm::vec3(0.0f, 100.0f, 30.0f)));

	//Models
	scene->models.push_back(new Model(glm::vec3(0.0f, -111.0f, 640.0f), "Models/ground/MT038a_obj.obj", scene->shaders[scene->currentShader]->id));

	scene->models.push_back(new Model(glm::vec3(-3.0f, 0.2f, 0.0f), "Models/car/car-n.obj", scene->shaders[scene->currentShader]->id));

	scene->models.push_back(new Model(glm::vec3(-3.0f, 0.0f, -17.0f), "Models/tree/PineTree03.obj", scene->shaders[scene->currentShader]->id));
	scene->models.push_back(new Model(glm::vec3(-6.0f, 0.0f, -4.0f), "Models/tree/PineTree03.obj", scene->shaders[scene->currentShader]->id));
	scene->models.push_back(new Model(glm::vec3(7.0f, 0.0f, -22.0f), "Models/tree/PineTree03.obj", scene->shaders[scene->currentShader]->id));
	scene->models.push_back(new Model(glm::vec3(13.0f, 0.0f, -20.0f), "Models/tree/PineTree03.obj", scene->shaders[scene->currentShader]->id));

	scene->models.push_back(new Model(glm::vec3(5.0f, 0.0f, -12.0f), "Models/pine/pineTransp.obj", scene->shaders[scene->currentShader]->id));
	scene->models.push_back(new Model(glm::vec3(-15.0f, 0.0f, -12.0f), "Models/pine/pineTransp.obj", scene->shaders[scene->currentShader]->id));
	scene->models.push_back(new Model(glm::vec3(8.0f, 0.0f, -11.0f), "Models/pine/pineTransp.obj", scene->shaders[scene->currentShader]->id));

	scene->models.push_back(new Model(glm::vec3(0.0f, -0.4f, 0.0f), "Models/TreeStump/TreeStump03.obj", scene->shaders[scene->currentShader]->id));

	scene->models.push_back(new Model(glm::vec3(5.0f, -0.5f, 0.0f), "Models/house/House2.obj", scene->shaders[scene->currentShader]->id));

	scene->models.push_back(new Model(glm::vec3(-7.0f, 0.0f, 0.0f), "Models/craft-2/Craft2.obj", scene->shaders[scene->currentShader]->id));

	//Lights
	scene->lights.push_back(new Light(glm::vec3(0.0f, 10.0f, -30.0f)));
	scene->lights.push_back(new DirectionalLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-0.2f, -1.0f, -0.3f)));

	// Define callback functions and start main loop
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, reshape_callback);

	// GLFW records the time since the program started (in seconds)
	double start = glfwGetTime();
	double now;

	while (!glfwWindowShouldClose(window)){
		now = glfwGetTime();
		scene->deltaTime = now - start;
		scene->render();
		start = now;
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	delete scene;

	return 0;
}
