#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
	Camera();
	~Camera() = default;

	glm::mat4 GetViewportMatrix() const { return m_viewport_matrix; };
	glm::mat4 GetInvViewportMatrix() const { return m_inv_viewport_matrix; };
	glm::mat4 GetProjectionMatrix() const { return m_projection_matrix; };
	glm::mat4 GetInvProjectionMatrix() const { return m_inv_projection_matrix; };
	
	void SetPerspective(float aspect);
	void SetPerspective(float w, float ipd, float d, float h, bool left);

	void LookAt(glm::vec3 pos, glm::vec3 front, glm::vec3 up);
	void ResetPosition();

	glm::vec3 pos = { 0,0,3 };
	glm::vec3 front = { 0,0,-1 };
	glm::vec3 up = { 0,-1,0 };
	glm::vec3 right;
	float _near = 1.0f;
	float _far = 5.0f;
	float fov = 1.0f;
private:

	glm::mat4 m_projection_matrix;
	glm::mat4 m_viewport_matrix;
	glm::mat4 m_inv_projection_matrix;
	glm::mat4 m_inv_viewport_matrix;
};

