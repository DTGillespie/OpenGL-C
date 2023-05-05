#version 330 core\n
layout (location = 0) in vec3 aPos;\n
layout (location = 1) in vec2 aTexCoord;\n
out vec2 texCoord;\n
void main() {\n
	gl_Position = vec4(aPos, 1.0);\n
	texCoord = aTexCoord;\n
}\n