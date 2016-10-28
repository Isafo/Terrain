#version 400
layout( location = 0 ) out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec2 UV;
in vec3 Grad;

uniform vec3 LP;
uniform sampler2D tex;

const float shininess = 802.0;

void main () {

	vec3 normal  = normalize( Normal );							
	vec3 lightDir = normalize(LP - Position);
	vec3 viewDir  = normalize(-Position);
		
	float lightIntensity = 0.6 / length(lightDir);
	lightDir = normalize(lightDir);

	vec3 white = vec3(1.0, 1.0, 1.0);

	float g = max(dot(Grad, vec3(0.0, 1.0, 0.0)), 0.0);
	vec3 color =  vec3(1.0 - g, max(g, 1-g), 1.0-g);

	//vec3 color = Grad;

	//Diffuse part-----------
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * color * lightIntensity;

	//specular part-------------
	vec3 H = normalize(lightDir + viewDir);
	float NdH = max(dot(H, normal), 0.0);
	float spec = pow(NdH, shininess);
	vec3 specular = spec * white;

	// Ambient-------------
	vec3 ambient = 0.3*lightIntensity * color;// * texcolor * lightIntensity;
	
	vec3 resultLight = ambient + diffuse;// + specular;
	FragColor = vec4(resultLight, 1.0);
}

