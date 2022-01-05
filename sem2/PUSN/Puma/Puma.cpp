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
	cylinders.push_back(std::make_shared<Cylinder>(true, 1.0f, 0.5f, yellow));
	cylinders.push_back(std::make_shared<Cylinder>(false, 0.5f, l1, yellow));
	cylinders.push_back(std::make_shared<Cylinder>(true, 0.5f, 0.5f, red));
	cylinders.push_back(std::make_shared<Cylinder>(false, 0.5f, l2, red));
	cylinders.push_back(std::make_shared<Cylinder>(true, 0.5f, 0.5f, green));
}

void Puma::Initialize(std::shared_ptr<Program> _program)
{
	for (auto& c : cylinders)
		c->Initialize(_program);

	cylinders[2]->inner_mat = glm::rotate((float)M_PI / 2, glm::vec3(1, 0, 0));
	cylinders[4]->inner_mat = glm::rotate((float)M_PI / 2, glm::vec3(1, 0, 0));
}

void Puma::RecalcFigure()
{
	for (auto& c : cylinders)
		c->RecalcFigure();
}


bool Puma::GetGuiInternal(std::shared_ptr<Figure> par)
{
	bool b = false;
	return b;
}

void Puma::Draw()
{
	float a1 = M_PI / 4;
	float a2 = 2*M_PI-M_PI / 4;
	glm::mat4 m01 = glm::rotate(a1, glm::vec3(0, 1, 0));
	glm::mat4 m11p = glm::translate(glm::vec3(0, l1, 0));

	auto cur = m01 * m11p;
	cylinders[2]->outer_mat = cur;


	glm::mat4 m1p2 = glm::rotate(a2, glm::vec3(0, 0, 1));
	cur = cur * m1p2;
	cylinders[3]->outer_mat = cur;

	glm::mat4 m22p = glm::translate(glm::vec3(0,l2,0));
	cur = cur * m22p;
	cylinders[4]->outer_mat = cur;

	for (auto& c : cylinders)
	{
		c->RecalcModel();
		c->Draw();
	}
}

bool Puma::Create()
{
	return false;
}