#define _CRT_SECURE_NO_WARNINGS 1

#include <shader.h>

#include <glad/gl.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define SHADER_SRC_BUFFER_SIZE 2048

/* struct Shader */void shader_CreateFromPath(char *vertexPath, char *fragmentPath) {
	
	struct Shader shaderBuffer = {0};

	char buffer[SHADER_SRC_BUFFER_SIZE];

	FILE *fptr_vertex, *fptr_fragment;

	// Vertex
	fptr_vertex = fopen(vertexPath, "r");
	char vertex_shaderSrc[SHADER_SRC_BUFFER_SIZE];
		 vertex_shaderSrc[0] = 0;

	while (fscanf(fptr_vertex, "%s", buffer) > 0) {
		strncat(vertex_shaderSrc, buffer, sizeof(buffer));
	}
	strcpy(shaderBuffer.vertexSrc, vertex_shaderSrc);

	// Fragment
	fptr_fragment = fopen(fragmentPath, "r");
	char fragment_shaderSrc[SHADER_SRC_BUFFER_SIZE];
	     fragment_shaderSrc[0] = 0;

	while (fscanf(fptr_fragment, "%s", buffer) > 0) {
		strncat(fragment_shaderSrc, buffer, sizeof(buffer));
	}

	strcpy(shaderBuffer.fragmentSrc, fragment_shaderSrc);

}