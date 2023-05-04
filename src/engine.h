#pragma once

#include <stdbool.h>

#define SHADER_SRC_BUFFER_SIZE 1024

#define VERTEX_ARRAY_SIZE 512
#define INDEX_ARRAY_SIZE  512

typedef struct {
	int width, height, nrChannels;
	unsigned char* data;
} Image;

typedef struct {
	unsigned int texture_id;
	Image *image;
} GL_Texture;

typedef struct {

	unsigned int gls_program_id;
	unsigned int vertex_id;
	unsigned int fragment_id;

	char vertex_src	  [SHADER_SRC_BUFFER_SIZE];
	char fragment_src [SHADER_SRC_BUFFER_SIZE];

} GL_Shader;

typedef struct {
	GL_Shader  *shader;
	GL_Texture *texture;
} GL_Material;

typedef void (*RenderFuncPtr)(GL_Shader*);

typedef struct {

	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

} GL_RenderBuffer;

typedef struct {

	GL_Material		material;
	RenderFuncPtr   renderFuncPtr;
	GL_RenderBuffer renderBuffer;

	const float		   mesh	   [VERTEX_ARRAY_SIZE];
	const unsigned int indices [INDEX_ARRAY_SIZE];

} GL_RenderObject;

typedef struct {

	int buffered;

	char vertex_src   [SHADER_SRC_BUFFER_SIZE];
	char fragment_src [SHADER_SRC_BUFFER_SIZE];

} ShaderSourceBuffer;

typedef struct {
	GLFWwindow* (* const initialize)		      (void);
	void		(* const bufferRenderObject)      (GL_RenderObject *renderObject);
	void		(* const render)		          (RenderFuncPtr renderFuncPtr, GLFWwindow* windowArg);
	void		(* const renderProc_DrawArrays)	  (GL_Shader* shader);
	void		(* const renderProc_DrawElements) (GL_Shader* shader);
} _ENGINE_RUNTIME_GL;

typedef struct {
	GL_Shader* (* const heapAllocation_SourceBuffer) (void);
	void	   (* const bufferSource_Path)	         (char* vertexPath, char* fragmentPath);
} _SHADER_GL;

typedef struct {
	GL_Texture* (* const heapAllocation_Path) (char* path);
} _TEXTURE_GL;

extern _ENGINE_RUNTIME_GL const ENGINE_RUNTIME_GL;
extern _SHADER_GL		  const SHADER_GL;
extern _TEXTURE_GL        const TEXTURE_GL;