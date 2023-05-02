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

static GLFWwindow* initialize_OpenGL				  (int version_major, int version_minor);
static void        buffer_vertexData				  (float vertexData[VERTEX_ARRAY_SIZE], unsigned int indexData[INDEX_ARRAY_SIZE]);
static void		   render							  (Shader dev_Shader, GLFWwindow windowArg);
void		       framebuffer_size_callback		  (GLFWwindow* window, int width, int height);
static void        poll_input						  (void);
static void	       shader_bufferSource_Path			  (char* vertexPath, char* fragmentPath);
static Shader*     shader_heapAllocation_SourceBuffer (void);
static int         shader_deleteShader_Heap			  (void);

static ShaderSourceBuffer SHADER_SRC_BUFFER = { .buffered = 0, 0 };
static RenderBuffer		  RENDER_BUFFER = { 0 };

// Development variables
Shader dev_Shader = { 0 };
GLFWwindow* window;

GLFWwindow* initialize_OpenGL(int version_major, int version_minor) {
	
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

	printf("Initialized OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Maximum vertex attributes (vertex shader)
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	printf("Supported vertex attributes: %d\n", nrAttributes);

	// Wireframe rendering
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL 

	return window;
}

static void buffer_vertexData(float vertexData[VERTEX_ARRAY_SIZE], unsigned int indexData[INDEX_ARRAY_SIZE]) {

	glGenVertexArrays	(1, &RENDER_BUFFER.VAO);

	glGenBuffers		(1, &RENDER_BUFFER.VBO);
	glGenBuffers		(1, &RENDER_BUFFER.EBO);

	glBindVertexArray	(RENDER_BUFFER.VAO);

	glBindBuffer		(GL_ARRAY_BUFFER, RENDER_BUFFER.VBO);
	glBufferData		(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RENDER_BUFFER.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

	/* Vertex attributes	NOT SURE WHERE TO PUT THIS YET */

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Original

	/* Rainbow triangle example using vertex attributes */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);   // Position attribute
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Color attribute
	glEnableVertexAttribArray(1);

}

static void render(Shader dev_Shader, GLFWwindow *windowArg) {

	while (!glfwWindowShouldClose(windowArg)) {

		poll_input(windowArg);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(dev_Shader.gls_program_id);

		/* Code for green triangle example */
		//float timeValue = glfwGetTime();
		//float greenValue = sin(timeValue) / 2.0f + 0.5f;
		//int vertexColorLocation = glGetUniformLocation(shaderProgram, "color");
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		glBindVertexArray(RENDER_BUFFER.VAO); // Should this go in the buffer function?

		glDrawArrays(GL_TRIANGLES, 0, 3);					   // Rendering VBO/VAO
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Rendering EBO


		glfwPollEvents();

		glfwSwapBuffers(windowArg);
	}

	glfwTerminate();

	//return 0;
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
	buffer[0] = 0;

	// Load vertex Source
	FILE* fptr_vertex = fopen(vertexPath, "r");

	SHADER_SRC_BUFFER.vertex_src[0] = 0;

	while (fscanf(fptr_vertex, "%s", buffer) > 0) {

		strncat(buffer, " ", sizeof(" "));
		strncat(SHADER_SRC_BUFFER.vertex_src, buffer, sizeof(buffer));
	}

	fclose(fptr_vertex);

	// Load fragment Source
	FILE* fptr_fragment = fopen(fragmentPath, "r");

	SHADER_SRC_BUFFER.fragment_src[0] = 0;

	while (fscanf(fptr_fragment, "%s", buffer) > 0) {
		strncat(SHADER_SRC_BUFFER.fragment_src, buffer, sizeof(buffer));
	}

	fclose(fptr_fragment);

	SHADER_SRC_BUFFER.buffered = 1;
}

static Shader* shader_heapAllocation_SourceBuffer(void) {

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

	// Debugging shader
	const char* debug_vertex_shader =
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPos; \n"
		"layout(location = 1) in vec3 aColor; \n"
		"out vec3 outColor; \n"
		"void main() {"
		"\n"
		"gl_Position = vec4(aPos, 1.0); \n"
		"outColor = aColor; \n"
		"}\0";

	char* formatted_vertex_src = strtok(hShader_ptr->vertex_src, "");
	glShaderSource(
		hShader_ptr->vertex_id,
		1,
		&debug_vertex_shader,
		//&formatted_vertex_src,
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

	// Debugging shader
	const char* debug_fragment_shader =
		"#version 330 core\n"
		"in vec3 color; \n"
		"out vec4 fragColor; \n"
		"void main() {"
		"\n"
		"fragColor = vec4(color, 1.0); \n"
		"}\0";

	char* formatted_fragment_src = strtok(hShader_ptr->fragment_src, "");
	glShaderSource(
		hShader_ptr->fragment_id, 
		1, 
		&debug_fragment_shader,
		//&formatted_fragment_src, 
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

	glUseProgram(dev_Shader.gls_program_id);

	SHADER_SRC_BUFFER.buffered = 0;

	/* Delete unused objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	*/

	return hShader_ptr;
}

static int shader_deleteShader_Heap(void) {

	return 1;
}

_ENGINE_RUNTIME  const ENGINE_RUNTIME = {
	initialize_OpenGL, 
	buffer_vertexData,
	render 
};

_RUNTIME_SHADERS const RUNTIME_SHADERS = { 
	shader_heapAllocation_SourceBuffer,
	shader_bufferSource_Path,
};