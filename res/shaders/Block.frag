#version 450 core

out vec4 FragPos;

layout (binding = 0) uniform sampler2D stone;

in vec3 fPos;
in vec3 fNorm;
in vec2 fTexCoord;

void main()
{
	vec3 col = vec3(0,0,1);
	if (fPos.y-10 > 0)
		col = vec3(1,1,0);
	if (fPos.y-20 > 0)
		col = vec3(0,1,0);

	float dif = 0;
	if (fNorm.x > 0)
		dif = 0.2;
	if (fNorm.y > 0)
		dif = 0.1;
	if (fNorm.z > 0)
		dif = 0.3;



	//FragPos = texture(stone, fTexCoord) + vec4(col, 0) - dif;// * dif;// - vec4(0.5);// + fColor;
	//FragPos = texture(stone, fTexCoord);// + vec4(normalize(fTexCoord),0, 0);// * dif;// - vec4(0.5);// + fColor;
	FragPos = vec4((fNorm + 1) / 2, 0);// * dif;// - vec4(0.5);// + fColor;
}