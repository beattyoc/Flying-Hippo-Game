#version 400

layout (location=3) in vec3 cube_p;

uniform mat4 P, V;

out vec3 texcoords;

void main () {
	texcoords = cube_p;
	gl_Position = P * V * vec4 (cube_p, 1.0);
}