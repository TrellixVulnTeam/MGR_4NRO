#include "Puma.h"
#include "imgui\imgui.h"
#include <string>
#include "Camera.h"

Puma::Puma() : Figure()
{
	sprintf_s(name, STRMAX, ("Puma - " + std::to_string(idx++) + " " + gen_random(12, idx)).c_str());
	figureType = FigureType::Puma;

	glm::vec3 yellow = { 1.0f,1.0f,0.0f };
	cylinders.push_back(std::make_shared<Cylinder>(1.0f,0.5f,yellow));
	cylinders.push_back(std::make_shared<Cylinder>(0.5f,3.0f,yellow));
}

void Puma::Initialize(std::shared_ptr<Program> _program)
{
	for (auto& c : cylinders)
		c->Initialize(_program);
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
	for (auto& c : cylinders)
		c->Draw();
}

bool Puma::Create()
{
	return false;
}