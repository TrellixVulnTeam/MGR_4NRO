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
#include "Program.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

enum class FigureType
{
	Cube, Duck, Tool, Obstacle, Cylinder,Puma
};

class Program;



class Figure
{
public:
	Figure();
	~Figure() = default;
	static int idx;
	bool GetGui(int i, std::shared_ptr<Figure> par);
	std::vector<float> GetVertices();
	std::vector<unsigned int> GetIndices();
	glm::mat4 GetModel();
	bool GetSelected() { return selected; };
	void Select() { selected = true; };
	void Unselect() { selected = false; };
	void SetScale(float x, float y, float z);
	void Scale(float change);
	void Rotate(glm::vec3 axis, float angle);
	void Rotate(glm::quat new_q);
	void SetRotation(glm::quat new_q) { rotation_q = new_q; RecalcModel(); }
	void RotateAround(glm::vec3 point, glm::vec3 axis, float angle);
	void ScaleAround(glm::vec3 point, float change);
	void Move(float x, float y, float z);
	void MoveTo(float x, float y, float z);
	void MoveToVec(glm::vec3 v);
	void MoveVec(float a, glm::vec3 v);
	void SetModel(glm::mat4 trans, glm::mat4 scale, glm::quat rotation);
	glm::vec3 GetPos();
	glm::vec3 GetScale();
	glm::quat GetRotation() { return rotation_q; };
	glm::mat4 GetScaleMat() { return scale; };
	glm::mat4 GetTransMat() { return translation; };
	virtual void Draw();
	void virtual Initialize(std::shared_ptr<Program> _program);
	void virtual RecalcFigure();
	bool CanMove() { return canMove; };
	bool HasParent() { return parents.size() > 0; };

	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	int from;
	FigureType figureType;
	std::shared_ptr<Window> window = {};
	std::shared_ptr<Shader> shader = {};
	bool keepOpen = false;
	char name[STRMAX] = "";
	char newName[STRMAX] = "";
	bool isCurve = false;
	void virtual CleanUp();
	std::vector<std::shared_ptr<Figure>> parents;
protected:
	virtual bool Create();
	void RecalcModel();
	glm::mat4 translation;
	glm::quat rotation_q;
	glm::mat4 scale;
	glm::mat4 model;
	bool selected = false;
	bool canMove = true;
	bool virtual GetGuiInternal(std::shared_ptr<Figure> par) = 0;
	std::vector<int> parents_cnt;
	std::shared_ptr<Program> program = {};
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
	std::string gen_random(const int len, int seedMod)
	{
		std::string tmp_s;
		static const char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";

		srand((unsigned)time(NULL) + seedMod);

		tmp_s.reserve(len);

		for (int i = 0; i < len; ++i)
			tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];


		return tmp_s;
	}
	bool showInGui = true;
private:
	bool selected_old = true;
};
