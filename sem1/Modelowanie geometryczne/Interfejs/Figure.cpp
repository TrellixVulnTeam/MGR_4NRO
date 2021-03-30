#include "Figure.h"
#include "imgui/imgui.h"
#include "BezierCurve.h"



Figure::Figure(Shader _shader) :shader(_shader)
{
}

bool Figure::Create()
{
	if (selected_old != selected)
	{
		selected_old = selected;
		return true;
	}
	return false;
}

void Figure::RecalcModel()
{
	model = translation * rotation * scale;
	if (figureType == FigureType::Point)
	{
		((Point*)this)->RecalcParent();
	}
}

bool Figure::GetGui(int i, bool fromMainGui)
{
	bool to_ret = false;
	if (fromMainGui && showInMainGui
		||
		!fromMainGui && !showInMainGui)
	{
		char buffer[STRMAX];
		std::string t(name);
		if (i >= 0)
		{
			sprintf_s(buffer, "%s###%s", (t + " - " + std::to_string(i)).c_str(), _name);
		}
		else
		{
			sprintf_s(buffer, "%s###%s", t.c_str(), _name);
		}
		if (ImGui::TreeNode(buffer)) {

			if (ImGui::BeginPopupContextItem())
			{
				ImGui::Text("Edit:");
				ImGui::InputText("##edit", name, IM_ARRAYSIZE(name));
				if (ImGui::Button("Close"))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			if (figureType != FigureType::Cursor)
			{
				ImGui::Checkbox("Selected", &selected);
				if (selected && !selected_old && figureType == FigureType::BezierCurve)
				{
					for (int j = 0; j < program->figures.size(); ++j) if (j!=i && program->figures[j]->figureType == FigureType::BezierCurve) program->figures[j]->Unselect();
				}
				if (fromMainGui) {
					if (ImGui::Button("Remove"))
					{
						if (figureType == FigureType::BezierCurve)
							((BezierCurve*)this)->CleanUp();
						to_ret = true;
					}
				}
			}
			if (GetGuiInternal(fromMainGui))
			{
				to_ret = true;
			}
			ImGui::TreePop();
			ImGui::Separator();
		}
	}
	return to_ret;
}

std::vector<float> Figure::GetVertices()
{
	return vertices;
}

std::vector<unsigned int> Figure::GetIndices()
{
	return indices;
}

glm::mat4 Figure::GetModel()
{
	return model;
}

void Figure::Scale(float change)
{
	if (change > 1 && scale_f > 3.0f) return;
	if (change < 1 && scale_f < 0.3f) return;

	scale_f = scale_f * change;
	scale[0][0] = scale_f;
	scale[1][1] = scale_f;
	scale[2][2] = scale_f;

	RecalcModel();
}

void Figure::Rotate(glm::mat4 rotate)
{
	rotation = rotate * rotation;
	RecalcModel();
}

void Figure::RotateAround(glm::vec3 point, double xAngle, double yAngle)
{
	glm::mat4 rotX = glm::mat4(1.0f);
	glm::mat4 rotY = glm::mat4(1.0f);

	rotX[1][1] = cos(xAngle);
	rotX[2][1] = -sin(xAngle);
	rotX[1][2] = sin(xAngle);
	rotX[2][2] = cos(xAngle);

	rotY[0][0] = cos(yAngle);
	rotY[2][0] = sin(yAngle);
	rotY[0][2] = -sin(yAngle);
	rotY[2][2] = cos(yAngle);

	glm::vec4 pos = rotY * rotX * glm::vec4(GetPos() - point, 1.0f) + glm::vec4(point, 1.0f);
	MoveTo(pos.x, pos.y, pos.z);

	Rotate(rotX);
	Rotate(rotY);
}

void Figure::RotateAroundWithMtx(glm::vec3 point, glm::mat4 rotate)
{
	glm::vec4 pos = rotate * glm::vec4(GetPos() - point, 1.0f) + glm::vec4(point, 1.0f);
	MoveTo(pos.x, pos.y, pos.z);

	Rotate(rotate);
}

void Figure::ScaleAround(glm::vec3 point, float change)
{
	glm::mat4 scale = glm::mat4(change);
	scale[3][3] = 1.0f;
	glm::vec4 pos = scale * glm::vec4(GetPos() - point, 1.0f) + glm::vec4(point, 1.0f);
	MoveTo(pos.x, pos.y, pos.z);

	Scale(change);
}

void Figure::Move(float x, float y, float z)
{
	translation[3][0] += x;
	translation[3][1] += y;
	translation[3][2] += z;
	RecalcModel();
}

void Figure::MoveTo(float x, float y, float z)
{
	translation[3][0] = x;
	translation[3][1] = y;
	translation[3][2] = z;
	RecalcModel();
}

void Figure::MoveVec(float a, glm::vec3 v)
{
	translation[3][0] += a * v.x;
	translation[3][1] += a * v.y;
	translation[3][2] += a * v.z;
	RecalcModel();
}

glm::vec3 Figure::GetPos()
{
	return glm::vec3(translation[3][0],
		translation[3][1],
		translation[3][2]);
}
void Figure::Draw(int transLoc)
{
	glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(GetModel()));
	shader.use();
	glBindVertexArray(VAO);
}

void Figure::Initialize(Program* _program)
{
	program = _program;
	translation = glm::mat4(1.0f);
	scale = glm::mat4(1.0f);
	rotation = glm::mat4(1.0f);
	scale_f = 1.0f;
	RecalcModel();


	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	RecalcFigure();
}

void Figure::RecalcFigure()
{
	if (Create()) {
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(
			GL_ARRAY_BUFFER,
			vertices.size() * sizeof(float),
			&vertices[0],
			GL_STATIC_DRAW
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(unsigned int),
			&indices[0],
			GL_STATIC_DRAW
		);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}
}
