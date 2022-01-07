#include "Puma.h"
#include "imgui\imgui.h"
#include <string>
#include "Camera.h"

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
	cylinders.push_back(std::make_shared<Cylinder>(false , 0.3f, -l3, green));
	cylinders.push_back(std::make_shared<Cylinder>(true, 0.5f, 0.6f, blue));
	cylinders.push_back(std::make_shared<Cylinder>(false , 0.3f, l4, blue));

	cylinders[2]->onZ = true;
	cylinders[3]->onX = true;
	cylinders[4]->onZ = true;
	cylinders[7]->onX = true;

	cursor=std::make_shared<Cursor>();
}

void Puma::Initialize(std::shared_ptr<Program> _program)
{
	for (auto& c : cylinders)
		c->Initialize(_program);

	cursor->Initialize(_program);
}

void Puma::RecalcFigure()
{
	if (q2 != q2_old) q2_old = q2;
	{ cylinders[3]->h = q2; cylinders[3]->Invalidate(); }
	for (auto& c : cylinders)
		c->RecalcFigure();

	cursor->RecalcFigure();
}

void Puma::InverseKinematics(glm::vec3 p5, glm::vec3 x, glm::vec3 y, glm::vec3 z)
{
	glm::vec3 p0 = { 0.0f,0.0f,0.0f };
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

	glm::mat4 m22p = glm::translate(glm::vec3(q2,0,0));
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

	for (auto& c : cylinders)
	{
		c->RecalcModel();
		c->Draw();
	}

	cursor->Draw();
}

bool Puma::Create()
{
	return false;
}