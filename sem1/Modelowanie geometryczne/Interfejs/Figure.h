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

enum class FigureType
{
	Torus, Point, MiddlePoint, Cursor, BezierCurve, PointsLine
};

class Figure
{
public:
	Figure(Shader _shader);
	~Figure() = default;
	bool GetGui(int i,std::vector<Figure*> figures,bool fromMainGui);
	std::vector<float> GetVertices();
	std::vector<unsigned int> GetIndices();
	glm::mat4 GetModel();
	bool GetSelected() { return selected; };
	void Select() { selected = true; };
	void Unselect() { selected = false; };
	void Scale(float change);
	void Rotate(glm::mat4 rotate);
	void RotateAround(glm::vec3 point, double xAngle, double yAngle);
	void RotateAroundWithMtx(glm::vec3 point, glm::mat4 rotate);
	void ScaleAround(glm::vec3 point, float change);
	void Move(float x, float y, float z);
	void MoveTo(float x, float y, float z);
	void MoveVec(float a, glm::vec3 v);
	glm::vec3 GetPos();
	virtual void Draw(int transLoc);
	void Initialize();
	void RecalcFigure();
	bool CanMove() { return canMove; };

	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	int from;
	FigureType figureType;
	bool keepOpen = false;
	char name[STRMAX] = "";
protected:
	virtual bool Create();
	void RecalcModel();
	glm::mat4 translation;
	glm::mat4 rotation;
	glm::mat4 scale;
	glm::mat4 model;
	float scale_f;
	bool selected = false;
	bool canMove = true;
	std::string _name;
	bool virtual GetGuiInternal(std::vector<Figure*> figures, bool fromMainGui) = 0;
	bool showInMainGui = true;
private:
	bool selected_old = false;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
	Shader shader;
};
