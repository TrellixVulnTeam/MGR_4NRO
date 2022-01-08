#include "Puma.h"
#include "imgui\imgui.h"
#include <string>
#include "Camera.h"
#include <glm/gtx/vector_angle.hpp>
Puma::Puma() : Figure()
{
	sprintf_s(name, STRMAX, ("Puma - " + std::to_string(idx++) + " " + gen_random(12, idx)).c_str());
	figureType = FigureType::Puma;

	glm::vec3 yellow = { 1.0f,1.0f,0.0f };
	glm::vec3 red = { 1.0f,0.0f,0.0f };
	glm::vec3 green = { 0.0f,1.0f,0.0f };
	glm::vec3 blue = { 0.0f,0.0f,1.0f };
	cylinders.push_back(std::make_shared<Cylinder>(true, 1.0f, 0.6f, yellow));
	cylinders.push_back(std::make_shared<Cylinder>(false, 0.3f, l1, yellow));
	cylinders.push_back(std::make_shared<Cylinder>(true, 0.5f, 0.6f, red));
	cylinders.push_back(std::make_shared<Cylinder>(false, 0.3f, q2, red));
	cylinders.push_back(std::make_shared<Cylinder>(true, 0.5f, 0.6f, green));
	cylinders.push_back(std::make_shared<Cylinder>(false, 0.3f, -l3, green));
	cylinders.push_back(std::make_shared<Cylinder>(true, 0.5f, 0.6f, blue));
	cylinders.push_back(std::make_shared<Cylinder>(false, 0.3f, l4, blue));

	cylinders[2]->onZ = true;
	cylinders[3]->onX = true;
	cylinders[4]->onZ = true;
	cylinders[7]->onX = true;

	cursorp3 = std::make_shared<Cursor>(1);
	cursorp4 = std::make_shared<Cursor>(2);
	cursor = std::make_shared<Cursor>();
}

void Puma::Initialize(std::shared_ptr<Program> _program)
{
	for (auto& c : cylinders)
		c->Initialize(_program);

	cursorp3->Initialize(_program);
	cursorp4->Initialize(_program);
	cursor->Initialize(_program);
}

void Puma::RecalcFigure()
{
	if (q2 != q2_old) q2_old = q2;
	{ cylinders[3]->h = q2; cylinders[3]->Invalidate(); }
	for (auto& c : cylinders)
		c->RecalcFigure();

	cursorp3->RecalcFigure();
	cursorp4->RecalcFigure();
	cursor->RecalcFigure();
}

float MyAngle(glm::vec3 v1, glm::vec3 v2, glm::vec3 c)
{
	float d = glm::dot(v1, v2);
	auto c2 = glm::normalize(glm::cross(v1, v2));
	float alpha = acos(d);

	float alpha2 = -alpha;

	auto res = glm::distance2(c, c2) > glm::distance2(-c, c2) ? alpha : alpha2;
	return res;
}

void Puma::InverseKinematics(glm::vec3 p5, glm::vec3 x5, glm::vec3 y5, glm::vec3 z5)
{
	//p5 = { 2.297f,2.095f,-1.959f };
	//x5 = { 0.7184f,0.13f,-0.6832f };
	//y5 = { 0.4737f,0.6274f,0.6181f };
	//z5 = { 0.5094f,-0.76768f,0.3888f };

	//p5 = { 3.31100011f,1.62199998f,1.08099997f };
	//x5 = { 0.469362676f,0.560579181f,0.682238638f };
	//y5 = { -0.822133899f,0.559336066f,0.106014013f };
	//z5 = { -0.322171390f,-0.610650599f,0.723402619f };
	glm::vec3 x0 = { 1.0f,0.0f,0.0f };
	glm::vec3 y0 = { 0.0f,1.0f,0.0f };
	glm::vec3 z0 = { 0.0f,0.0f,1.0f };

	glm::vec3 p0 = { 0.0f,0.0f, 0.0f };
	auto p1 = p0;
	auto p2 = p0 + l1 * y0;

	auto p4 = p5 - l4 * x5;
	glm::vec3 y4;
	glm::vec3 n;
	if (false)//rownolegle
	{
	}
	else
	{
		n = glm::cross(p4 - p0, p2 - p0);
		auto a = n.x;
		auto b = n.y;
		auto c = n.z;
		auto d = x5.x;
		auto e = x5.y;
		auto f = x5.z;
		//a!=0

		//e-bd/a
		auto k = (-f + c * d / a) / (e - b * d / a);
		auto l = (-b * k - c) / a;

		y4.z = sqrt(1 / (l * l + k * k + 1));
		y4.y = y4.z * k;
		y4.x = y4.z * l;

		auto p3 = p4 + l3 * y4; //+-

		a1 = atan2(-p4.z, p4.x);

		glm::mat4 cur = glm::mat4(1.0f);
		glm::mat4 m01 = glm::rotate(a1, glm::vec3(0, 1, 0));
		cur = cur * m01;

		auto p20 = glm::normalize(p2 - p0);
		auto p32 = glm::normalize(p3 - p2);
		auto z = glm::vec3(cur * glm::vec4(z0, 0.0f));
		auto a2t = MyAngle(p20, p32, z);
		a2 = -a2t + M_PI / 2;

		glm::mat4 m1p2 = glm::rotate(a2, glm::vec3(0, 0, 1));
		cur = cur * m1p2;

		q2 = glm::length(p3 - p2);

		auto p43 = glm::normalize(p4 - p3);
		z = glm::vec3(cur * glm::vec4(z0, 0.0f));
		auto a3t = MyAngle(p32, p43, -z);
		a3 = a3t + M_PI / 2;

		glm::mat4 m2p3 = glm::rotate(a3, glm::vec3(0, 0, 1));
		cur = cur * m2p3;
		auto y = glm::vec3(cur * glm::vec4(y0, 0.0f));
		a4 = MyAngle(glm::normalize(n), glm::normalize(x5), -y) - M_PI / 2;

		glm::mat4 m3p4 = glm::rotate(a4, glm::vec3(0, 1, 0));
		cur = cur * m2p3;
		auto x = glm::vec3(cur * glm::vec4(x0, 0.0f));
		a5 = MyAngle(glm::normalize(p3 - p4), glm::normalize(y5), x);

		cursorp3->outer_mat = glm::translate(p5);
		cursorp3->RecalcModel();
		cursorp4->outer_mat = glm::translate(p4);
		cursorp4->RecalcModel();
	}
}

bool Puma::GetGuiInternal(std::shared_ptr<Figure> par)
{
	bool b = false;
	return b;
}

void Puma::Draw()
{
	glm::mat4 m01 = glm::rotate(a1, glm::vec3(0, 1, 0));
	glm::mat4 m11p = glm::translate(glm::vec3(0, l1, 0));

	auto cur = m01 * m11p;
	cylinders[2]->outer_mat = cur;


	glm::mat4 m1p2 = glm::rotate(a2, glm::vec3(0, 0, 1));
	cur = cur * m1p2;
	cylinders[3]->outer_mat = cur;

	glm::mat4 m22p = glm::translate(glm::vec3(q2, 0, 0));
	cur = cur * m22p;
	cylinders[4]->outer_mat = cur;

	glm::mat4 m2p3 = glm::rotate(a3, glm::vec3(0, 0, 1));
	cur = cur * m2p3;
	cylinders[5]->outer_mat = cur;

	glm::mat4 m33p = glm::translate(glm::vec3(0, -l3, 0));
	cur = cur * m33p;
	cylinders[6]->outer_mat = cur;

	glm::mat4 m3p4 = glm::rotate(a4, glm::vec3(0, 1, 0));
	cur = cur * m3p4;
	cylinders[7]->outer_mat = cur;

	glm::mat4 m44p = glm::translate(glm::vec3(l4, 0, 0));
	cur = cur * m44p;
	glm::mat4 m4p5 = glm::rotate(a5, glm::vec3(1, 0, 0));
	cur = cur * m4p5;
	cursor->outer_mat = cur;


	for (auto& c : cylinders)
	{
		c->RecalcModel();
		c->Draw();
	}

	cursor->RecalcModel();
	//cursorp3->Draw();
	//cursorp4->Draw();
	cursor->Draw();
}

bool Puma::Create()
{
	return false;
}