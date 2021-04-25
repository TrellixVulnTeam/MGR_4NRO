#pragma once
#define _USE_MATH_DEFINES
#define STRMAX 100
#include<glm/glm.hpp>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Math.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

enum class FigureType
{
	Torus, Point, MiddlePoint, Cursor, BezierCurveC0, PointsLine, BezierCurveC2, InterpolationCurveC2
};

class Program;

class Figure
{
public:
	Figure();
	~Figure() = default;
	bool GetGui(int i, Figure* par);
	std::vector<float> GetVertices();
	std::vector<unsigned int> GetIndices();
	glm::mat4 GetModel();
	bool GetSelected() { return selected; };
	void Select() { selected = true; };
	void Unselect() { selected = false; };
	void Scale(float change);
	void Rotate(glm::vec3 axis, float angle);
	void Rotate(glm::quat new_q);
	void RotateAround(glm::vec3 point, glm::vec3 axis, float angle);
	void ScaleAround(glm::vec3 point, float change);
	void Move(float x, float y, float z);
	void MoveTo(float x, float y, float z);
	void MoveVec(float a, glm::vec3 v);
	glm::vec3 GetPos();
	virtual void Draw();
	void virtual Initialize(Program* _program);
	void RecalcFigure();
	bool CanMove() { return canMove; };
	bool HasParent() { return parents.size() > 0; };

	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	int from;
	FigureType figureType;
	bool keepOpen = false;
	char name[STRMAX] = "";
	char gui_name[STRMAX];
	bool isCurve = false;
protected:
	virtual bool Create();
	void RecalcModel();
	glm::mat4 translation;
	glm::quat rotation_q;
	glm::mat4 scale;
	glm::mat4 model;
	float scale_f;
	bool selected = false;
	bool canMove = true;
	std::string _name;
	bool virtual GetGuiInternal(Figure* par) = 0;
	bool showInMainGui = true;
	std::vector<Figure*> parents;
	Program* program = NULL;
	Shader shader;
private:
	bool selected_old = false;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
};
