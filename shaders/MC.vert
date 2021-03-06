#version 450
layout(location = 0) in vec3 VertexPosition;

const uint dim = 30;
//const uint dim = 31;

void main () {

	vec3 vertexPosition = vec3(mod(gl_InstanceID, dim) + 1,
							   (int(mod(gl_InstanceID, dim * dim)) / dim) + 1,
							   (gl_InstanceID / (dim * dim))) + 1;

	gl_Position = vec4(vertexPosition , 1.0f);
}