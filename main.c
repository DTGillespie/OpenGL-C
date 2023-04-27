#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <shader.h>

#define CWD_BUFFER_SIZE 1024

char *cwd[CWD_BUFFER_SIZE];

int render(void);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Development
void dev_initializeShadersProc(void);

unsigned int VBO;             
unsigned int VAO;
unsigned int EBO;

/*
unsigned int vertexShader;

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 color;\n"
"void main() {\n"
"gl_Position = vec4(aPos , 1.0);\n"
"color = aColor;\n"
"}\0";
*/

/*const char *vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main() {\n"
"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
*/

/*
unsigned int fragmentShader;

const char *fragmentShaderSource =
"#version 330 core\n"
"out vec4 fragColor;\n"
"in vec3 color;\n"
"void main() {\n"
"fragColor = vec4(color, 1.0);\n"
"}\0";
*/

/*
const char *fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 color;\n"
"void main(){\n"
"FragColor = color;"
"}\0";
*/

unsigned int shaderProgram;

const float VERTICES[] = {

	// Positions       Colors
	0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right   
   -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left		
    0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // top				
};

const unsigned int INDICES[] = {
	0, 1, 3,
	1, 2, 3
};

int main() {

	/* Misc Initialization */
	_getcwd(cwd, CWD_BUFFER_SIZE);

	/* OpenGL Initialization */
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL-C", NULL, NULL);
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

	/* Buffer & Object initialization */
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);

	int success;
	char infoLog[512];

	/* Shader creation, compilation, and linking  */

	/*
	// Vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", infoLog);
	}
	*/

	dev_initializeShadersProc();

	/*
	// Fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s", infoLog);
	}

	// Shader program
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
	}

	//glUseProgram(shaderProgram); Can be used within the render loop

	// Delete unused objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	*/

	/* Vertex attributes */
	
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Original

	/* Rainbow triangle example using vertex attributes */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);   // Position attribute
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Color attribute
	glEnableVertexAttribArray(1);

	// Wireframe rendering
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL 

	int frame = 0;

	while (!glfwWindowShouldClose(window)) {

		processInput(window);

		render();

		glfwPollEvents();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

int render(void) {

	// Clear screen
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram);

	/* Code for green triangle example */
	//float timeValue = glfwGetTime();
	//float greenValue = sin(timeValue) / 2.0f + 0.5f;
	//int vertexColorLocation = glGetUniformLocation(shaderProgram, "color");
	//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Rendering EBO

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);					   // Rendering VBO/VAO

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
}

void dev_initializeShadersProc(void) {

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

	shader_CreateFromPath(vertex_shaderPath, fragment_shaderPath);
}