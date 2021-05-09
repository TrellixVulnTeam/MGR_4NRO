#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera()
{
}

void Camera::ResetPosition()
{
	pos = { 0,0,1 };
	front = { 0,0,-1 };
	up = { 0,-1,0 };
}

void Camera::SetPerspective(float w, float ipd, float d, float h, bool left)
{
	glm::mat4 persp = glm::mat4(1.0f);

	float r = _near * (w + ipd) / (2.0f * d);
	float l = -_near * (w - ipd) / (2.0f * d);
	if (!left)
	{
		r = _near * (w - ipd) / (2.0f * d);
		l = -_near * (w + ipd) / (2.0f * d);
	}
	float t = _near * h / (2.0f * d);
	float b = -_near * h / (2.0f * d);

	persp[0][0] = 2.0f * _near / (r - l);
	persp[1][1] = 2.0f * _near / (t - b);
	persp[2][0] = (r + l) / (r - l);
	persp[2][1] = (t + b) / (t - b);

	persp[2][2] = (_far + _near) / (_far - _near);
	persp[3][2] = ((_far * _near * 2.0f) / (_far - _near));
	persp[3][3] = 0.0f;
	persp[2][3] = -1.0f;
	m_projection_matrix = persp;
	m_inv_projection_matrix = glm::inverse(m_projection_matrix);
}
void Camera::LookAt(glm::vec3 pos, glm::vec3 front, glm::vec3 up)
{
	this->pos = pos;
	this->front = front;
	this->up = up;
	right = glm::normalize(glm::cross(front, up));

	glm::vec3 const f(glm::normalize(front));
	glm::vec3 const s(glm::normalize(cross(f, up)));
	glm::vec3 const u(glm::cross(s, f));

	glm::mat4 Result(1.0f);
	Result[0][0] = s.x;
	Result[1][0] = s.y;
	Result[2][0] = s.z;
	Result[0][1] = u.x;
	Result[1][1] = u.y;
	Result[2][1] = u.z;
	Result[0][2] = -f.x;
	Result[1][2] = -f.y;
	Result[2][2] = -f.z;
	Result[3][0] = -glm::dot(s, pos);
	Result[3][1] = -glm::dot(u, pos);
	Result[3][2] = glm::dot(f, pos);
	m_viewport_matrix = Result;
	m_inv_viewport_matrix = glm::inverse(m_viewport_matrix);
	//m_world_matrix = glm::lookAt(pos, pos + front, up);
}
