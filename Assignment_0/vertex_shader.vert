#version 400

layout (location=0) in vec3 vertex_p; // points
layout (location=1) in vec2 vertex_t; // normals (will just use as colours)
layout (location=2) in vec3 vertex_n; // normals (will just use as colours)

uniform mat4 P, V, M;

out vec3 position_eye, normal_eye;

out vec3 n;
out vec2 t;


void main () {
	
	position_eye = vec3 (V * M * vec4 (vertex_p, 1.0));
	normal_eye = vec3 (V * M * vec4 (vertex_n, 0.0));
	//send normals to fragment shader
	gl_Position = P * V * M * vec4 (vertex_p, 1.0);
	
	//gl_Position = P * vec4 (position_eye, 1.0);
	t = vertex_t;
	n = (V * M * vec4 (vertex_n, 0.0)).xyz;
}
