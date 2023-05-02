#version 330 core\n
layout (location = 0) in vec3 aPos;\n
layout (location = 1) in vec3 aColor;\n
out vec3 outColor;\n
void main(){\n
	gl_Position = vec4(aPos, 1.0);\n
	outColor = aColor;\n
}\0