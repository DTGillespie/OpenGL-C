#version 330 core\n
in vec3 color;\n
in vec2 texCoord;\n
out vec4 fragColor;\n
uniform sampler2D oTexture;\n
void main() {\n
	fragColor = texture(oTexture, texCoord);\n
}\n
