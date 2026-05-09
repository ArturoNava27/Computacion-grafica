#pragma once

#include <glew.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>

#include <glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);

	// Control directo usado por la rueda de camaras.
	void setPosition(glm::vec3 newPosition);
	void setLookAt(glm::vec3 target);
	void setAerialCamera(glm::vec3 aerialPosition, GLfloat aerialYaw, GLfloat aerialPitch);
	void setThirdPersonCamera(glm::vec3 modelPosition, GLfloat modelRotationY, GLfloat deltaTime, GLfloat distance = 18.0f, GLfloat height = 8.0f, GLfloat smooth = 6.0f);

	glm::vec3 getCameraPosition();
	glm::vec3 getCameraDirection();
	glm::mat4 calculateViewMatrix();

	~Camera();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveSpeed;
	GLfloat turnSpeed;

	void update();
};

