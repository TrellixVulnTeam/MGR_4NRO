#include "Figure.h"
#include "imgui/imgui.h"
#include "SomeCurve.h"
#include "SomePatch.h"


int Figure::idx = 0;

Figure::Figure() :shader()
{
}

void Figure::CleanUp()
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
	glm::mat4 rotation = glm::toMat4(rotation_q);
	model = translation * rotation * scale;
	if (figureType == FigureType::Point)
	{
		((Point*)this)->RecalcParent();
	}
}

bool Figure::GetGui(int i, Figure* par)
{
	bool to_ret = false;
	std::string t(name);
	std::string s;
	if (i >= 0)
		s = t + " - " + std::to_string(i);
	else
		s = t;
	if (ImGui::TreeNode(s.c_str())) {

		if (ImGui::BeginPopupContextItem())
		{
			ImGui::Text("Edit:");
			ImGui::InputText("##edit", newName, IM_ARRAYSIZE(newName));
			if (ImGui::Button("SAVE"))
			{
				ImGui::CloseCurrentPopup();
				strcpy_s(name, STRMAX, newName);
			}
			ImGui::EndPopup();
		}
		if (figureType != FigureType::Cursor)
		{
			ImGui::Checkbox("Selected", &selected);
			if (selected && !selected_old && figureType == FigureType::BezierCurveC0)
			{
				for (int j = 0; j < program->figures.size(); ++j) if (j != i && program->figures[j]->figureType == FigureType::BezierCurveC0) program->figures[j]->Unselect();
			}
			if (par == nullptr) {
				if (ImGui::Button("Remove"))
				{
					to_ret = true;
				}
			}
		}
		if (GetGuiInternal(par))
		{
			to_ret = true;
		}
		ImGui::TreePop();
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

void Figure::SetScale(float x, float y, float z)
{
	scale[0][0] = x;
	scale[1][1] = y;
	scale[2][2] = z;

	RecalcModel();
}
void Figure::Scale(float change)
{
	if (change > 1 && scale[0][0] > 3.0f) return;
	if (change > 1 && scale[1][1] > 3.0f) return;
	if (change > 1 && scale[1][1] > 3.0f) return;
	if (change < 1 && scale[0][0] < 0.3f) return;
	if (change < 1 && scale[1][1] < 0.3f) return;
	if (change < 1 && scale[2][2] < 0.3f) return;

	scale[0][0] *= change;
	scale[1][1] *= change;
	scale[2][2] *= change;

	RecalcModel();
}

void Figure::Rotate(glm::vec3 axis, float angle)
{
	auto new_q = glm::angleAxis(angle, axis);
	Rotate(new_q);
}

void Figure::Rotate(glm::quat new_q)
{
	rotation_q = new_q * rotation_q;
	RecalcModel();
}

void Figure::RotateAround(glm::vec3 point, glm::vec3 axis, float angle)
{
	auto new_q = glm::angleAxis(angle, axis);

	glm::vec4 pos = new_q * glm::vec4(GetPos() - point, 1.0f) + glm::vec4(point, 1.0f);
	MoveTo(pos.x, pos.y, pos.z);

	Rotate(new_q);
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
glm::vec3 Figure::GetScale()
{
	return glm::vec3(scale[0][0],
		scale[1][1],
		scale[2][2]);
}
void Figure::Draw()
{
	shader.use();
	int transLoc = glGetUniformLocation(shader.ID, "transform");

	glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(GetModel()));
	glBindVertexArray(VAO);
}

void Figure::Initialize(Program* _program)
{
	strcpy_s(newName, STRMAX, name);
	program = _program;
	shader = Shader(program->shader);
	translation = glm::mat4(1.0f);
	scale = glm::mat4(1.0f);

	rotation_q.w = 1.0f;
	scale[0][0] = 1.0f;
	scale[1][1] = 1.0f;
	scale[2][2] = 1.0f;
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
