#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <engine.h>

#define PATH_STRING_SIZE 1024

char cwd[PATH_STRING_SIZE];

GLFWwindow *window;

// Development
void dev_bufferShaderSource(void);
void dev_Setup(void);

char dev_TexturePath[PATH_STRING_SIZE];

int main() {

	/* Initialization */

	window = ENGINE_RUNTIME_GL.initialize(3, 3);

	_getcwd(cwd, PATH_STRING_SIZE);

	dev_Setup();

	ENGINE_RUNTIME_GL.bufferRenderObject(&debugTriangle);

	GL_RenderObject debugTriangle = {

		/*
		.mesh =
			 0.5f,  0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f,*/

		.mesh = 
			// positions          // colors           // texture coords
			0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
			0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,    // top left 

		.indices =
			 0, 1, 3,
			 1, 2, 3,

		.material = {
			.shader	 = SHADER_GL  .heapAllocation_SourceBuffer(),
			.texture = TEXTURE_GL .heapAllocation_Path(dev_TexturePath, debugTriangle.material.shader),
		},

		.renderBuffer = {0},

		.renderFuncPtr = ENGINE_RUNTIME_GL.renderProc_DrawElements,
	};

	ENGINE_RUNTIME_GL.render(&debugTriangle, window);
}

void dev_Setup(void) {
	
	// Shader
	dev_bufferShaderSource();

	// Texture path
	char dev_TexturePath_Suffix[]  = "\\assets\\textures\\container.jpg";
	strncat(dev_TexturePath, cwd, sizeof(cwd));
	strncat(dev_TexturePath, dev_TexturePath_Suffix, sizeof(dev_TexturePath_Suffix));

}

void dev_bufferShaderSource(void) {

	char vertex_shaderPath[PATH_STRING_SIZE];
	char vertex_shaderPath_suffix[23];

	strcpy(vertex_shaderPath_suffix, "\\glsl\\vertex_test.glsl");
	strcpy(vertex_shaderPath, cwd);
	strncat(
			   vertex_shaderPath, 
			   vertex_shaderPath_suffix, 
		sizeof(vertex_shaderPath_suffix));

	char fragment_shaderPath[PATH_STRING_SIZE];
	char fragment_shaderPath_suffix[25];
	
	strcpy(fragment_shaderPath_suffix, "\\glsl\\fragment_test.glsl");
	strcpy(fragment_shaderPath, cwd);
	strncat(
			   fragment_shaderPath,
			   fragment_shaderPath_suffix,
		sizeof(fragment_shaderPath_suffix));

	SHADER_GL.bufferSource_Path(&vertex_shaderPath, &fragment_shaderPath);
}