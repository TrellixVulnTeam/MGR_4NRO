#include "GregoryPatch.h"
#include "imgui\imgui.h"
#include <string>

GregoryPatch::GregoryPatch() : Figure()
{
	sprintf_s(name, STRMAX, ("GregoryPatch - " + std::to_string(idx++) + " " + gen_random(10, idx)).c_str());
	figureType = FigureType::GregoryPatch;
	isPatch = true;
	canMove = false;
	pointsLines = new PointsLines();
}

bool GregoryPatch::GetGuiInternal(Figure* par)
{
	ImGui::SliderInt("Split A", &splitA, 1, 100);
	ImGui::SliderInt("Split B", &splitB, 1, 100);
	ImGui::Checkbox("Draw points", &drawPoints);
	ImGui::Checkbox("Draw line", &drawLine);
	ImGui::Checkbox("Draw", &draw);
	if (ImGui::Button("Delete"))
	{
		draw = false;
		drawPoints = false;
		showInGui = false;
	}
	return false;
}

void GregoryPatch::Initialize(Program* _program)
{
	Figure::Initialize(_program);
	shader = Shader(program->patchGregoryShader);
	pointsLines->Initialize(program);
	GeneratePoints();
}

void GregoryPatch::ClearPoints()
{
	for (int i = 0; i < points.size(); ++i)
	{
		delete points[i];
	}
}

void GregoryPatch::GeneratePoints()
{
	first = true;
	ClearPoints();
	points.clear();
	pointsLines->Clear();
	Point* p;

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.0f, 0.0f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.0f, 0.25f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.0f, 0.5f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.0f, 0.75f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.25f, 0.0f, 0.0f);
	points.push_back(p);

	//5
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.25f, 0.25f, 0.5f);
	points.push_back(p);

	//6
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.25f, 0.25f, 0.0f);
	points.push_back(p);

	//7
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.25f, 0.5f, 0.0f);
	points.push_back(p);

	//8
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.25f, 0.5f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.25f, 0.75f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.5f, 0.0f, 0.0f);
	points.push_back(p);

	//11
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.5f, 0.25f, 0.0f);
	points.push_back(p);

	//12
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.5f, 0.25f, 0.0f);
	points.push_back(p);

	//13
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.5f, 0.5f, 0.0f);
	points.push_back(p);

	//14
	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.5f, 0.5f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.5f, 0.75f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.75f, 0.0f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.75f, 0.25f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.75f, 0.5f, 0.0f);
	points.push_back(p);

	p = new Point();
	p->Initialize(program);
	p->MoveTo(0.75f, 0.75f, 0.0f);
	points.push_back(p);

	pointsLines->AddPoint(points[1]);
	pointsLines->AddPoint(points[6]);

	pointsLines->AddPoint(points[4]);
	pointsLines->AddPoint(points[5]);

	pointsLines->AddPoint(points[2]);
	pointsLines->AddPoint(points[7]);

	pointsLines->AddPoint(points[10]);
	pointsLines->AddPoint(points[11]);

	pointsLines->AddPoint(points[8]);
	pointsLines->AddPoint(points[9]);

	pointsLines->AddPoint(points[12]);
	pointsLines->AddPoint(points[17]);

	pointsLines->AddPoint(points[13]);
	pointsLines->AddPoint(points[18]);

	pointsLines->AddPoint(points[14]);
	pointsLines->AddPoint(points[15]);


	pointsLines->AddPoint(points[0]);
	pointsLines->AddPoint(points[1]);
	pointsLines->AddPoint(points[1]);
	pointsLines->AddPoint(points[2]);
	pointsLines->AddPoint(points[2]);
	pointsLines->AddPoint(points[3]);
	pointsLines->AddPoint(points[3]);
	pointsLines->AddPoint(points[9]);
	pointsLines->AddPoint(points[9]);
	pointsLines->AddPoint(points[15]);
	pointsLines->AddPoint(points[15]);
	pointsLines->AddPoint(points[19]);
	pointsLines->AddPoint(points[19]);
	pointsLines->AddPoint(points[18]);
	pointsLines->AddPoint(points[18]);
	pointsLines->AddPoint(points[17]);
	pointsLines->AddPoint(points[17]);
	pointsLines->AddPoint(points[16]);
	pointsLines->AddPoint(points[16]);
	pointsLines->AddPoint(points[10]);
	pointsLines->AddPoint(points[10]);
	pointsLines->AddPoint(points[4]);
	pointsLines->AddPoint(points[4]);
	pointsLines->AddPoint(points[0]);
}

void GregoryPatch::UpdateMesh(std::vector<glm::vec3> positions)
{
	for (int i = 0; i < 20; ++i)
	{
		auto pos = positions[i];
		points[i]->MoveTo(pos.x, pos.y, pos.z);
	}
	Recalc();
}

void GregoryPatch::Draw()
{
	Figure::Draw();

	if (draw) {
		glPatchParameteri(GL_PATCH_VERTICES, 20);

		int xLoc = glGetUniformLocation(shader.ID, "x");
		glUniform1f(xLoc, splitA);

		int yLoc = glGetUniformLocation(shader.ID, "y");
		glUniform1f(yLoc, splitB);

		glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		if (drawPoints)
			for (int i = 0; i < points.size(); ++i) points[i]->Draw();
		if (drawLine)
			pointsLines->Draw();
	}
}

void GregoryPatch::CleanUp()
{
	ClearPoints();
	delete pointsLines;
}

bool GregoryPatch::Create()
{
	bool fCreate = Figure::Create();
	if (!fCreate && !first) return false;
	if (points.size() == 0) return false;
	first = false;
	pointsLines->RecalcFigure();
	vertices.clear();
	indices.clear();
	for (int i = 0; i < 20; ++i)
	{
		auto pos = points[i]->GetPos();
		vertices.push_back(pos.x);
		vertices.push_back(pos.y);
		vertices.push_back(pos.z);

		vertices.push_back(pos.z);
		vertices.push_back(pos.z);
		vertices.push_back(pos.z);

		indices.push_back(i);
	}
	return true;
}