#version 400
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 vertexUV;

out vec3 Position;
out vec3 Normal;
out vec2 UV;
out vec3 Grad;

uniform mat4 MV;
uniform mat4 P;

void main () 
{	
	Position =  vec3( MV * vec4(VertexPosition, 1.0));
	Grad = VertexNormal;
	Normal = normalize(mat3(MV) * VertexNormal);
	UV = vertexUV;

	gl_Position =  (P * MV) * vec4(VertexPosition, 1.0);
}