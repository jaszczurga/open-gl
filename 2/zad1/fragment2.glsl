// Fragment shader w wersji 3.3
#version 330 core

in vec3 vColor;

out vec4 outColor;


void main()
{

	outColor = vec4(vColor, 1.0);

}
