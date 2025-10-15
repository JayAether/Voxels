#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 orientation;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	float FOVDegrees = 45.0f;

	Camera(
		glm::vec3 position, 
		glm::vec3 orientation, 
		glm::vec3 up)
	{
		this->position = position;
		this->orientation = orientation;
		this->up = up;
	}

	/*Camera(
		float positionX = 0.0f, float positionY = 0.0f, float positionZ = 0.0f, 
		float orientationX = 0.0f, float orientationY = 0.0f, float orientationZ = 1.0f, 
		float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f)
	{
		this->position.x = positionX;
		this->position.y = positionY;
		this->position.z = positionZ;
		this->orientation.x = orientationX;
		this->orientation.y = orientationY;
		this->orientation.z = orientationZ;
		this->up.x = upX;
		this->up.y = upY;
		this->up.z = upZ;
	}*/


	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(position, position + orientation, up);
	}

	

public:
	bool firstMouse = true;
	//float lastX = 400, lastY = 300;
	float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float pitch = 0.0f;
	float lastX = 640 / 2;
	float lastY = 480 / 2;
};

