#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <engine.h>

// ENGINE_RUNTIME_GL
static GLFWwindow* initialize		  (int version_major, int version_minor);
static void        bufferRenderObject (RenderObject *renderObject);
static void		   render			  (RenderFuncPtr renderFuncPtr, GLFWwindow windowArg);

// RUNTIME_SHADERS_GL
static void	   shader_bufferSource_Path			  (char *vertexPath, char *fragmentPath);
static Shader* shader_heapAllocation_SourceBuffer (void);
static int     shader_deleteShader_Heap			  (void);

// Internal
void		framebuffer_size_callback (GLFWwindow* window, int width, int height);
static void poll_input				  (void);

// Misc.
static void glRenderProc_DrawArrays   (Shader* shader);
static void glRenderProc_DrawElements (Shader* shader);

static ShaderSourceBuffer SHADER_SRC_BUFFER = { .buffered = 0, 0 };


// Dev
Shader dev_Shader = { 0 };
GLFWwindow* window;

GLFWwindow* initialize(int version_major, int version_minor) {
	
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version_minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 600, "OpenGL-C", NULL, NULL);
	if (window == NULL) {
		printf("Failed to initialize GLFW window\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		printf("Failed to initialize GLAD\n");
		glfwTerminate();
		return -1;
	}

	printf("OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return window;
}

static void bufferRenderObject(RenderObject *renderObject) {

	glGenVertexArrays		  (1, &renderObject->VAO);
	glBindVertexArray		  (renderObject->VAO);

	glGenBuffers			  (1, &renderObject->VBO);
	glBindBuffer			  (GL_ARRAY_BUFFER, renderObject->VBO);
	glBufferData			  (GL_ARRAY_BUFFER, sizeof(renderObject->mesh), renderObject->mesh, GL_STATIC_DRAW);


	glGenBuffers			  (1, &renderObject->EBO);
	glBindBuffer			  (GL_ELEMENT_ARRAY_BUFFER, renderObject->EBO);
	glBufferData			  (GL_ELEMENT_ARRAY_BUFFER, sizeof(renderObject->indices), renderObject->indices, GL_STATIC_DRAW);

	glVertexAttribPointer	  (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray (0);
}

static void render(
	RenderObject *renderObject, 
	GLFWwindow	 *windowArg
) {

	while (!glfwWindowShouldClose(windowArg)) {

		poll_input(windowArg);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		renderObject->renderFuncPtr(renderObject->shader);

		glfwPollEvents();
		glfwSwapBuffers(windowArg);
	}

	glfwTerminate();
}

static void glRenderProc_DrawArrays(Shader *shader) {

	// Wireframe rendering
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL 

	glUseProgram(shader->gls_program_id);

	glDrawArrays(GL_TRIANGLES, 0, 3);					 // Rendering VBO/VAO
}

static void glRenderProc_DrawElements(Shader* shader) {

	// Wireframe rendering
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL 

	glUseProgram(shader->gls_program_id);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Rendering EBO
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

static void poll_input(void) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
}

static void shader_bufferSource_Path(char* vertexPath, char* fragmentPath) {

	char buffer[SHADER_SRC_BUFFER_SIZE];
	memset(buffer, 0x00, sizeof(buffer));

	// Vertex source from path
	FILE* fptr_vertex = fopen(vertexPath, "r");

	SHADER_SRC_BUFFER.vertex_src[0] = 0;

	int scan_fptr_vertex = 99;
	while (scan_fptr_vertex != -1) {
		
		memset(buffer, 0x00, sizeof(buffer));

		scan_fptr_vertex = fscanf(fptr_vertex, "%s", buffer);

		int length = 0;
		for (int i = 0; i < sizeof(buffer); i++) {
			
			if (buffer[i] != 0X00) {
				length = i;
			}
			else break;
		}

		if (buffer[length - 1] == 0x5c && buffer[length] == 0x6e) {
			buffer[length - 1] = 0x0a;
			buffer[length] = 0x00;
		}
		else {
			buffer[length + 1] = 0x20;
		}

		strcat(SHADER_SRC_BUFFER.vertex_src, buffer, sizeof(buffer));
	}

	fclose(fptr_vertex);

	// Fragment source from path
	FILE* fptr_fragment = fopen(fragmentPath, "r");

	SHADER_SRC_BUFFER.fragment_src[0] = 0;

	int scan_fptr_fragment = 99;
	while (scan_fptr_fragment != -1) {

		memset(buffer, 0x00, sizeof(buffer));

		scan_fptr_fragment = fscanf(fptr_vertex, "%s", buffer);

		int length = 0;
		for (int i = 0; i < sizeof(buffer); i++) {

			if (buffer[i] != 0X00) {
				length = i;
			}
			else break;
		}

		if (buffer[length - 1] == 0x5c && buffer[length] == 0x6e) {
			buffer[length - 1] = 0x0a;
			buffer[length] = 0x00;
		}
		else {
			buffer[length + 1] = 0x20;
		}

		strcat(SHADER_SRC_BUFFER.fragment_src, buffer, sizeof(buffer));
	}

	fclose(fptr_fragment);

	SHADER_SRC_BUFFER.buffered = 1;
}

Shader* shader_heapAllocation_SourceBuffer(void) {

	if (!SHADER_SRC_BUFFER.buffered) {
		printf("ERROR::shader_heapAllocationFromSource()::SHADER_SOURCE_NOT_BUFFERED\n");
		return NULL;
	}

	Shader* hShader_ptr = (struct Shader*)malloc(sizeof(Shader));

	memcpy(
		hShader_ptr->vertex_src,
		SHADER_SRC_BUFFER.vertex_src,
		sizeof(SHADER_SRC_BUFFER.vertex_src));

	memcpy(hShader_ptr->fragment_src,
		SHADER_SRC_BUFFER.fragment_src,
		sizeof(SHADER_SRC_BUFFER.fragment_src));

	// Create and compile vertex shader
	hShader_ptr->vertex_id = glCreateShader(GL_VERTEX_SHADER);

	char* formatted_vertex_src = strtok(hShader_ptr->vertex_src, "");
	glShaderSource(
		hShader_ptr->vertex_id,
		1,
		&formatted_vertex_src,
		NULL);

	glCompileShader(hShader_ptr->vertex_id);

	int	  vertex_shader_compilation_status = 99;
	char  vertex_shader_compilation_log[512];
		  vertex_shader_compilation_log[0] = 0;

	glGetShaderiv(hShader_ptr->vertex_id, GL_COMPILE_STATUS, &vertex_shader_compilation_status);
	if (!vertex_shader_compilation_status) {
		glGetShaderInfoLog(hShader_ptr->vertex_id, 512, NULL, vertex_shader_compilation_log);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED: \n%s\n", vertex_shader_compilation_log);
		return NULL;
	}

	// Create and compile fragment shader
	hShader_ptr->fragment_id = glCreateShader(GL_FRAGMENT_SHADER);

	char* formatted_fragment_src = strtok(hShader_ptr->fragment_src, "");
	glShaderSource(
		hShader_ptr->fragment_id, 
		1, 
		&formatted_fragment_src, 
		NULL);

	glCompileShader(hShader_ptr->fragment_id);

	int	  fragment_shader_compilation_status = 99;
	char  fragment_shader_compilation_log[512];

	glGetShaderiv(hShader_ptr->fragment_id, GL_COMPILE_STATUS, &fragment_shader_compilation_status);
	if (!fragment_shader_compilation_status) {
		glGetShaderInfoLog(hShader_ptr->fragment_id, 512, NULL, fragment_shader_compilation_log);
		printf("\nERROR::SHADER::FRAGMENT::COMPILATION_FAILED: \n%s\n", fragment_shader_compilation_log);
		return NULL;
	}

	// Link and attach shader program
	int  gls_program_link_status = 99;
	char gls_program_link_log[512];
		 gls_program_link_log[0] = 0;

	hShader_ptr->gls_program_id = glCreateProgram();

	glAttachShader(
		hShader_ptr->gls_program_id, 
		hShader_ptr->vertex_id);

	glAttachShader(
		hShader_ptr->gls_program_id, 
		hShader_ptr->fragment_id);

	glLinkProgram(hShader_ptr->gls_program_id);

	glGetProgramiv(hShader_ptr->gls_program_id, GL_LINK_STATUS, &gls_program_link_status);
	if (!gls_program_link_status) {
		glGetProgramInfoLog(hShader_ptr->gls_program_id, 512, NULL, gls_program_link_log);
		printf("\nERROR::SHADER::PROGRAM::LINKING_FAILED: %s\n", gls_program_link_log);
		return NULL;
	}

	SHADER_SRC_BUFFER.buffered = 0;

	return hShader_ptr;
}

static int shader_deleteShader_Heap(void) {

	return 1;
}

_ENGINE_RUNTIME_GL const ENGINE_RUNTIME_GL = {
	initialize, 
	bufferRenderObject,
	render,
	glRenderProc_DrawArrays,
	glRenderProc_DrawElements,
};

_SHADERS_GL const SHADERS_GL = { 
	shader_heapAllocation_SourceBuffer,
	shader_bufferSource_Path,
};