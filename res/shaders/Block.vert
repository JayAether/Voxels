#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

vec3 calculateBlockOffset();

out vec3 fPos;
out vec3 fNorm;
out vec2 fTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform ivec3 chunkOffset;
uniform int chunkSize;

void main()
{
	float chunkDisplacement = 2; //this is the distance between each chunk

	vec3 chunkPos = chunkOffset * chunkSize * chunkDisplacement; // chunkOffset * chunkSize gives the true chunk position
	// vec3 blockPos = aOffset + (chunkPos * chunkDisplacement); // true block pos; 
	// the `* 2` is cause the chunks intersect each other by default
	

	vec4 globalPos = vec4(aPos + chunkPos, 1.0); // added with aPos gives the position for triangle 
	// the `* 1` on the blockPos is the spacing between the blocks
	
	
	gl_Position = projection * view * globalPos;

	fPos = aPos;
	fNorm = aNorm;
	fTexCoord = aTexCoord;
}

// calculates the block offset based on its rendering position (or instance index) from the chunk's origin
vec3 calculateBlockOffset()
{
	int x = 0, y = 0, z = 0;

	// x goes up until chunkSize, then resets and y goes up next, then resets and then z (thats the plan)
	for (int i = gl_InstanceID; i > 0; i--)
	{
		x++;
		if (x % chunkSize == 0)
		{
			x = 0;
			y++;
			if (y % chunkSize == 0)
			{
				y = 0;
				z++;
			}
		}
	}
	
	return vec3(x, y, z);
}