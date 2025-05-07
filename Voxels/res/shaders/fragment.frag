#version 330 core
out vec4 FragColor;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

in vec2 TexCoords;


uniform Material mats;

void main()
{
	//FragColor = texture2D(gSampler, TexCoords);
	FragColor = vec4(texture(mats.diffuse, TexCoords));
}