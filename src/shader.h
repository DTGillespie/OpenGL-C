#pragma once

#include <glad/gl.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>

struct Shader {
	unsigned int id;
	char *vertexSrc;
	char *fragmentSrc;
};

void shader_CreateFromPath(char *vertexPath, char *fragmentPath);

void shader_Use(struct Shader shader);

void shader_SetBool (char *name, bool  value);
void shader_SetInt  (char *name, int   value);
void shader_SetFloat(char *name, float value);


