#version 330 core
out vec4 FragPos;

in vec4 fColor;

void main()
{
	FragPos = fColor;
}