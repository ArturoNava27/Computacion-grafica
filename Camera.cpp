#include "Camera.h"
#include <cmath>

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

	if (keys[GLFW_KEY_W])
	{
		position += front * velocity;
	}

	if (keys[GLFW_KEY_S])
	{
		position -= front * velocity;
	}

	if (keys[GLFW_KEY_A])
	{
		position -= right * velocity;
	}

	if (keys[GLFW_KEY_D])
	{
		position += right * velocity;
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}

void Camera::setPosition(glm::vec3 newPosition)
{
	position = newPosition;
}

void Camera::setLookAt(glm::vec3 target)
{
	glm::vec3 direction = target - position;
	if (glm::length(direction) < 0.001f)
		return;

	direction = glm::normalize(direction);
	pitch = glm::degrees(asin(direction.y));
	yaw = glm::degrees(atan2(direction.z, direction.x));
	update();
}

void Camera::setAerialCamera(glm::vec3 aerialPosition, GLfloat aerialYaw, GLfloat aerialPitch)
{
	position = aerialPosition;
	yaw = aerialYaw;
	pitch = aerialPitch;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	update();
}

void Camera::setThirdPersonCamera(glm::vec3 modelPosition, GLfloat modelRotationY, GLfloat deltaTime, GLfloat distance, GLfloat height, GLfloat smooth)
{
	// En este proyecto la rotacion Y del modelo se maneja en grados.
	// El vector forward coincide con el uso de sin/cos en el plano XZ.
	glm::vec3 forward;
	forward.x = sin(glm::radians(modelRotationY));
	forward.y = 0.0f;
	forward.z = cos(glm::radians(modelRotationY));
	forward = glm::normalize(forward);

	glm::vec3 desiredPosition = modelPosition - (forward * distance) + glm::vec3(0.0f, height, 0.0f);
	float factor = 1.0f - exp(-smooth * deltaTime);
	position = glm::mix(position, desiredPosition, factor);

	glm::vec3 target = modelPosition + glm::vec3(0.0f, height * 0.42f, 0.0f);
	setLookAt(target);
}

glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCameraPosition()
{
	return position;
}


glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}


Camera::~Camera()
{
}
