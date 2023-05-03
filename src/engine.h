#pragma once

#include <stdbool.h>

#define SHADER_SRC_BUFFER_SIZE 1024

#define VERTEX_ARRAY_SIZE 512
#define INDEX_ARRAY_SIZE  512

typedef struct {
	unsigned int gls_program_id;
	unsigned int vertex_id;
	unsigned int fragment_id;
	char vertex_src	  [SHADER_SRC_BUFFER_SIZE];
	char fragment_src [SHADER_SRC_BUFFER_SIZE];
} Shader;

typedef void (*RenderFuncPtr)(Shader*);

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
	GLFWwindow* (* const initialize)		      (void);
	void		(* const bufferRenderObject)      (RenderObject *renderObject);
	void		(* const render)		          (RenderFuncPtr renderFuncPtr, GLFWwindow* windowArg);
	void		(* const renderProc_DrawArrays)	  (Shader* shader);
	void		(* const renderProc_DrawElements) (Shader* shader);
} _ENGINE_RUNTIME_GL;

typedef struct {
	Shader*  (* const heapAllocation_SourceBuffer) (void);
	void	 (* const bufferSource_Path)	       (char* vertexPath, char* fragmentPath);
} _SHADERS_GL;

/*
typedef struct {
} _SYSTEM_MEMORY;
*/

extern _ENGINE_RUNTIME_GL const ENGINE_RUNTIME_GL;
extern _SHADERS_GL		  const SHADERS_GL;
//extern _SYSTEM_MEMORY	  const SYSTEM_MEMORY;