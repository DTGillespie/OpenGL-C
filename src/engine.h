#pragma once

#include <stdbool.h>

#define SHADER_SRC_BUFFER_SIZE 1024

#define VERTEX_ARRAY_SIZE 18
#define INDEX_ARRAY_SIZE  6

typedef void (*RenderFuncPtr)(RenderObject* renderObject);

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

	RenderFuncPtr renderFuncPtr;

	// Debugging
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

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
	GLFWwindow* (* const initialize)		    (void);
	void		(* const bufferRenderObject)    (RenderObject *renderObject);
	void		(* const render)		        (RenderFuncPtr renderFuncPtr, GLFWwindow* windowArg);
	void	    (* const renderProc_DrawArrays) (RenderObject* renderObject);
} _ENGINE_RUNTIME_GL;

typedef struct {
	Shader*  (* const heapAllocation_SourceBuffer) (void);
	void	 (* const bufferSource_Path)	       (char* vertexPath, char* fragmentPath);
} _RUNTIME_SHADERS_GL;

extern _ENGINE_RUNTIME_GL  const ENGINE_RUNTIME_GL;
extern _RUNTIME_SHADERS_GL const RUNTIME_SHADERS_GL;