#define _CRT_SECURE_NO_WARNINGS 1

#include <shader.h>

#include <glad/gl.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define SHADER_SRC_BUFFER_SIZE 1024

/* struct Shader */void shader_CreateFromPath(char *vertexPath, char *fragmentPath) {

	char buffer[SHADER_SRC_BUFFER_SIZE];
		 buffer[0] = 0;

	// Vertex
	FILE *fptr_vertex = fopen(vertexPath, "r");

	char vertex_shaderSrc[SHADER_SRC_BUFFER_SIZE];
		 vertex_shaderSrc[0] = 0;

	while (fscanf(fptr_vertex, "%s", buffer) > 0) {
		strncat(vertex_shaderSrc, buffer, sizeof(buffer));
	}

	// Fragment
	FILE *fptr_fragment = fopen(fragmentPath, "r");

	char fragment_shaderSrc[SHADER_SRC_BUFFER_SIZE];
		 fragment_shaderSrc[0] = 0;

	while (fscanf(fptr_fragment, "%s", buffer) > 0) {
		strncat(fragment_shaderSrc, buffer, sizeof(buffer));
	}

	struct Shader sShader_buffer = { 0 };
	//sShader_buffer.id = 9;
	strcpy(sShader_buffer.vertexSrc, vertex_shaderSrc, sizeof(vertex_shaderSrc));
	strcpy(sShader_buffer.fragmentSrc, fragment_shaderSrc, sizeof(vertex_shaderSrc));

	int test = sizeof(sShader_buffer);

	struct Shader *hShader_ptr = (struct Shader*) malloc(sizeof(sShader_buffer, 1));

	memcpy(hShader_ptr, &sShader_buffer, sizeof(sShader_buffer));

}