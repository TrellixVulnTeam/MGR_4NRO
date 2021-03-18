#pragma once
#define _USE_MATH_DEFINES
#include<glm/glm.hpp>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

enum class FigureType
{
	Torus, Point, MiddlePoint, Cursor
};

class Figure
{
public:
	Figure(Shader _shader);
	~Figure() = default;
	bool virtual GetGui(int i);
	std::vector<float> GetVertices();
	std::vector<unsigned int> GetIndices();
	glm::mat4 GetModel();
	bool GetSelected() { return selected; };
	void Select() { selected = true; };
	void Unselect() { selected = false; };
	void Scale(float change);
	void Rotate(glm::mat4 rotate);
	void RotateAround(glm::vec3 point, double xAngle, double yAngle);
	void ScaleAround(glm::vec3 point, float change);
	void Move(float x, float y, float z);
	void MoveTo(float x, float y, float z);
	glm::vec3 GetPos();
	virtual void Draw(int transLoc);
	void Initialize();
	void RecalcFigure();

	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::string name;
	int from;
	FigureType figureType;
protected:
	virtual bool Create();
	void RecalcModel();
	glm::mat4 translation;
	glm::mat4 rotation;
	glm::mat4 scale;
	glm::mat4 model;
	float scale_f;
	bool selected = false;
private:
	bool selected_old = false;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
	Shader shader;
};
