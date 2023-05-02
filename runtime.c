#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <engine.h>

#define CWD_BUFFER_SIZE 1024

char cwd[CWD_BUFFER_SIZE];

GLFWwindow *window;

// Development
void dev_bufferDebugShaderSource(void);

int main() {

	/* Initialization */

	window = ENGINE_RUNTIME.initialize_OpenGL(3, 3);

	_getcwd(cwd, CWD_BUFFER_SIZE);
	 
	dev_bufferDebugShaderSource();

	Shader *testShader = RUNTIME_SHADERS.heapAllocation_SourceBuffer();
	/*
	RenderObject debugTriangle = {

		.mesh =
			// Position      | Color
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		   -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,

		.indices =
			0, 1, 3,
			1, 2, 3,

		.shader = RUNTIME_SHADERS.heapAllocation_SourceBuffer(),
				
	}; 
	*/

	//ENGINE_RUNTIME.buffer_vertexData(debugTriangle.mesh, debugTriangle.indices);

	printf("Debug");

	//ENGINE_RUNTIME.render(*debugTriangle.shader, window);

	printf("Debug");
}

void dev_bufferDebugShaderSource(void) {

	char vertex_shaderPath[CWD_BUFFER_SIZE];
	char vertex_shaderPath_suffix[23];

	strcpy(vertex_shaderPath_suffix, "\\glsl\\vertex_test.glsl");
	strcpy(vertex_shaderPath, cwd);
	strncat(
			   vertex_shaderPath, 
			   vertex_shaderPath_suffix, 
		sizeof(vertex_shaderPath_suffix));

	char fragment_shaderPath[CWD_BUFFER_SIZE];
	char fragment_shaderPath_suffix[25];
	
	strcpy(fragment_shaderPath_suffix, "\\glsl\\fragment_test.glsl");
	strcpy(fragment_shaderPath, cwd);
	strncat(
			   fragment_shaderPath,
			   fragment_shaderPath_suffix,
		sizeof(fragment_shaderPath_suffix));

	RUNTIME_SHADERS.bufferSource_Path(&vertex_shaderPath, &fragment_shaderPath);
}