#pragma once

#include <stdbool.h>

#define SHADER_SRC_BUFFER_SIZE 1024

#define VERTEX_ARRAY_SIZE 18
#define INDEX_ARRAY_SIZE  6

typedef struct {
	unsigned int gls_program_id;
	unsigned int vertex_id;
	unsigned int fragment_id;
	char vertex_src	  [SHADER_SRC_BUFFER_SIZE];
	char fragment_src [SHADER_SRC_BUFFER_SIZE];

} Shader;

typedef struct {
	const float		   mesh	   [VERTEX_ARRAY_SIZE];
	const unsigned int indices [INDEX_ARRAY_SIZE];
	Shader *shader;
} RenderObject;

typedef struct {
	int buffered;
	char vertex_src   [SHADER_SRC_BUFFER_SIZE];
	char fragment_src [SHADER_SRC_BUFFER_SIZE];

} ShaderSourceBuffer;

typedef struct {
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
} RenderBuffer;

typedef struct {
	GLFWwindow* (* const initialize_OpenGL)(void);
	void		(* const buffer_vertexData)(float vertexData[VERTEX_ARRAY_SIZE]);
	void		(* const render)			(Shader dev_Shader, GLFWwindow *windowArg);
} _ENGINE_RUNTIME;

typedef struct {
	Shader* (* const heapAllocation_SourceBuffer)(void);
	void	(* const bufferSource_Path)	         (char* vertexPath, char* fragmentPath);
} _RUNTIME_SHADERS;

extern _ENGINE_RUNTIME	 const ENGINE_RUNTIME;
extern _RUNTIME_SHADERS	 const RUNTIME_SHADERS;