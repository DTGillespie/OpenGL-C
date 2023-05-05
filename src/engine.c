#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#define _CRT_SECURE_NO_WARNINGS 1

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <engine.h>

// Prototypes: ENGINE_RUNTIME_GL
static GLFWwindow* gl_Initialize		 (int version_major, int version_minor);
static void        gl_BufferRenderObject (GL_RenderObject *renderObject);
static void		   gl_Render			 (RenderFuncPtr renderFuncPtr, GLFWwindow windowArg);

// Prototypes: SHADER_GL
static void		  gl_Shader_BufferSource_Path			(char *vertexPath, char *fragmentPath);
static GL_Shader* gl_Shader_HeapAllocation_SourceBuffer (void);

// Prototypes: TEXTURE_GL
static GL_Texture* gl_Texture_HeapAllocation_Path(char* path, GL_Shader *shader);

// Prototypes: GL Internal
void		gl_Framebuffer_Size_Callback (GLFWwindow* window, int width, int height);
static void gl_Poll_Input				  (void);

// Prototypes: GL Misc.
static void gl_RenderProc_DrawArrays   (GL_Shader* shader);
static void gl_RenderProc_DrawElements (GL_RenderObject *renderObject);

static ShaderSourceBuffer SHADER_SRC_BUFFER = { .buffered = 0, 0 };

// Dev
GL_Shader dev_Shader = { 0 };
GLFWwindow* window;

GLFWwindow* gl_Initialize(int version_major, int version_minor) {
	
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

	glfwSetFramebufferSizeCallback(window, gl_Framebuffer_Size_Callback);

	return window;
}

static void gl_BufferRenderObject(GL_RenderObject *renderObject) {

	glGenVertexArrays		  (1, &renderObject->renderBuffer.VAO);

	glGenBuffers			  (1, &renderObject->renderBuffer.VBO);
	glGenBuffers			  (1, &renderObject->renderBuffer.EBO);

	glBindVertexArray(renderObject->renderBuffer.VAO);

	glBindBuffer			  (GL_ARRAY_BUFFER, renderObject->renderBuffer.VBO);
	glBufferData			  (GL_ARRAY_BUFFER, sizeof(renderObject->mesh), renderObject->mesh, GL_STATIC_DRAW);

	glBindBuffer			  (GL_ELEMENT_ARRAY_BUFFER, renderObject->renderBuffer.EBO);
	glBufferData			  (GL_ELEMENT_ARRAY_BUFFER, sizeof(renderObject->indices), renderObject->indices, GL_STATIC_DRAW);

	// Position attribute
	unsigned int posAttrib = glGetAttribLocation(renderObject->material.shader->gls_program_id, "aPos");
	glEnableVertexAttribArray (posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	// Texture coord attribute
	unsigned int texCoordAttrib = glGetAttribLocation(renderObject->material.shader->gls_program_id, "aTexCoord");
	glEnableVertexAttribArray (texCoordAttrib);
	glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

static void gl_Render(GL_RenderObject *renderObject, GLFWwindow *windowArg) {

	while (!glfwWindowShouldClose(windowArg)) {

		gl_Poll_Input(windowArg);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		renderObject->renderFuncPtr(&renderObject->material);

		glfwPollEvents();
		glfwSwapBuffers(windowArg);
	}

	glfwTerminate();
}

static void gl_RenderProc_DrawArrays(GL_Shader *shader) {

	// Wireframe rendering
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL 

	glUseProgram(shader->gls_program_id);

	glDrawArrays(GL_TRIANGLES, 0, 3);					 // Rendering VBO/VAO
}

static void gl_RenderProc_DrawElements(GL_RenderObject *renderObject) {

	// Wireframe rendering
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL 

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Rendering EBO
}

void gl_Framebuffer_Size_Callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

static void gl_Poll_Input(void) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
}

static void gl_Shader_BufferSource_Path(char* vertexPath, char* fragmentPath) {

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

static GL_Shader* gl_Shader_HeapAllocation_SourceBuffer(void) {

	if (!SHADER_SRC_BUFFER.buffered) {
		printf("ERROR::shader_heapAllocationFromSource()::SHADER_SOURCE_NOT_BUFFERED\n");
		return NULL;
	}

	GL_Shader* hShader_ptr = (struct Shader*)malloc(sizeof(GL_Shader));

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

static GL_Texture* gl_Texture_HeapAllocation_Path(char* path, GL_Shader *shader) {

	int width_buffer, height_buffer, nrChannels_buffer;
	unsigned char *imageDataBuffer = stbi_load(
		path,
		&width_buffer,
		&height_buffer,
		&nrChannels_buffer,
		0);

	Image imageBuffer = { 
		.data		= imageDataBuffer, 
		.width		= width_buffer, 
		.height		= height_buffer, 
		.nrChannels = nrChannels_buffer
	};

	GL_Texture textureBuffer = {
		.image = imageBuffer,
		.texture_id = -1,
	};

	GL_Texture* hTexture_ptr = (GL_Texture*) malloc(sizeof(textureBuffer));

	hTexture_ptr->image = imageBuffer;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hTexture_ptr->texture_id);

	if (hTexture_ptr->image.data) {

		glTexImage2D(
			GL_TEXTURE_2D, 
			0,
			GL_RGB,
			hTexture_ptr->image.width,
			hTexture_ptr->image.height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			hTexture_ptr->image.data);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		printf("ERROR::gl_Texture_HeapAllocation_Path()::TEXTURE::FAILED_TO_LOAD_TEXTURE");
		return NULL;
	}

	unsigned int outTextureUniformPos = glGetUniformLocation(shader->gls_program_id, "outTexture");
	glUniform1i(outTextureUniformPos, 0);

	// Texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//stbi_image_free(imageDataBuffer);

	glUseProgram(shader->gls_program_id);

	return hTexture_ptr;
}

_ENGINE_RUNTIME_GL const ENGINE_RUNTIME_GL = {
	gl_Initialize, 
	gl_BufferRenderObject,
	gl_Render,
	gl_RenderProc_DrawArrays,
	gl_RenderProc_DrawElements,
};

_SHADER_GL const SHADER_GL = { 
	gl_Shader_HeapAllocation_SourceBuffer,
	gl_Shader_BufferSource_Path,
};

_TEXTURE_GL const TEXTURE_GL = {
	gl_Texture_HeapAllocation_Path,
};