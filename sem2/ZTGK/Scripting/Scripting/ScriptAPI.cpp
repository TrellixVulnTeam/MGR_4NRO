/* TODO (5.01) Include Python Embedding Header */
#include "GameObject.h"

#include <pybind11/embed.h>

namespace py = pybind11;
using namespace duckApp;

class Bezier
{
public:
	Bezier() = default;
	void AddControlPoint(XMFLOAT2 newPoint)
	{
		deBoor0 = deBoor1;
		deBoor1 = deBoor2;
		deBoor2 = deBoor3;
		deBoor3 = newPoint;
	}

	float GetXPos(float t)
	{
		return ResFromBernstein(DeBoorToBernstein(deBoor0.x, deBoor1.x, deBoor2.x, deBoor3.x), t);
	}

	float GetYPos(float t)
	{
		return ResFromBernstein(DeBoorToBernstein(deBoor0.y, deBoor1.y, deBoor2.y, deBoor3.y), t);
	}
private:
	XMFLOAT2 deBoor0, deBoor1, deBoor2, deBoor3;
	XMFLOAT4 DeBoorToBernstein(float a, float b, float c, float d)
	{
		float p1 = a + 2.0f / 3.0f * (b - a);
		float p2 = b + 1.0f / 3.0f * (c - b);
		float p3 = b + 2.0f / 3.0f * (c - b);
		float p4 = c + 1.0f / 3.0f * (d - c);

		XMFLOAT4 bernsteins;
		bernsteins.x = (p1 + p2) / 2.0f;
		bernsteins.y = p2;
		bernsteins.z = p3;
		bernsteins.w = (p3 + p4) / 2.0f;
		return bernsteins;
	}
	float ResFromBernstein(XMFLOAT4 b, float t)
	{
		float coeffs[4];
		coeffs[0] = b.x;
		coeffs[1] = b.y;
		coeffs[2] = b.z;
		coeffs[3] = b.w;
		for (int i = 4; i > 0; --i)
		{
			for (int j = 0; j < i - 1; ++j)
			{
				coeffs[j] = (1 - t) * coeffs[j] + t * coeffs[j + 1];
			}
		}
		return coeffs[0];
	}
};

/* TODO (5.02) Create C++ API Module To Be Embedded Into Python */
PYBIND11_EMBEDDED_MODULE(API, _module)
{
#pragma region Vectors 


	py::class_<DirectX::XMFLOAT3>(_module, "Vector3")
		.def(py::init<float, float, float>())
		.def("__repr__", [](const DirectX::XMFLOAT3& vec) {
		using namespace std::string_literals;
		return "["s + std::to_string(vec.x) + ","s + std::to_string(vec.y) + ","s + std::to_string(vec.z) + "]"s;
			})
		.def_readwrite("x", &DirectX::XMFLOAT3::x)
		.def_readwrite("y", &DirectX::XMFLOAT3::y)
		.def_readwrite("z", &DirectX::XMFLOAT3::z)
		.def("__mul__",
			[](DirectX::XMFLOAT3& vec1, DirectX::XMFLOAT3& vec2)
			{
				DirectX::XMFLOAT3 result;
				XMStoreFloat3(&result, DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&vec1), DirectX::XMLoadFloat3(&vec2)));
				return result;
			}, py::is_operator())
		.def("__add__",
			[](DirectX::XMFLOAT3& vec1, DirectX::XMFLOAT3& vec2)
			{
				DirectX::XMFLOAT3 result;
				XMStoreFloat3(&result, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&vec1), DirectX::XMLoadFloat3(&vec2)));
				return result;
			}, py::is_operator());

	py::class_<DirectX::XMFLOAT2>(_module, "Vector2")
		.def(py::init<float, float>())
		.def("__repr__", [](const DirectX::XMFLOAT2& vec) {
		using namespace std::string_literals;
		return "["s + std::to_string(vec.x) + ","s + std::to_string(vec.y) + "]"s;
			})
		.def_readwrite("x", &DirectX::XMFLOAT2::x)
		.def_readwrite("y", &DirectX::XMFLOAT2::y)
		.def("__mul__",
			[](DirectX::XMFLOAT2& vec1, DirectX::XMFLOAT2& vec2)
			{
				DirectX::XMFLOAT2 result;
				XMStoreFloat2(&result, DirectX::XMVectorMultiply(DirectX::XMLoadFloat2(&vec1), DirectX::XMLoadFloat2(&vec2)));
				return result;
			}, py::is_operator())
		.def("__add__",
			[](DirectX::XMFLOAT2& vec1, DirectX::XMFLOAT2& vec2)
			{
				DirectX::XMFLOAT2 result;
				XMStoreFloat2(&result, DirectX::XMVectorAdd(DirectX::XMLoadFloat2(&vec1), DirectX::XMLoadFloat2(&vec2)));
				return result;
			}, py::is_operator());


	py::class_<DirectX::XMFLOAT4>(_module, "Vector4")
		.def(py::init<float, float, float, float>())
		.def("__repr__", [](const DirectX::XMFLOAT4& vec) {
		using namespace std::string_literals;
		return "["s + std::to_string(vec.x) + ","s + std::to_string(vec.y) + ","s + std::to_string(vec.z) + ","s + std::to_string(vec.w) + "]"s;
			})
		.def_readwrite("x", &DirectX::XMFLOAT4::x)
		.def_readwrite("y", &DirectX::XMFLOAT4::y)
		.def_readwrite("z", &DirectX::XMFLOAT4::z)
		.def_readwrite("w", &DirectX::XMFLOAT4::w)
		.def("__mul__",
			[](DirectX::XMFLOAT4& vec1, DirectX::XMFLOAT4& vec2)
			{
				DirectX::XMFLOAT4 result;
				XMStoreFloat4(&result, DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&vec1), DirectX::XMLoadFloat4(&vec2)));
				return result;
			}, py::is_operator())
		.def("__add__",
			[](DirectX::XMFLOAT4& vec1, DirectX::XMFLOAT4& vec2)
			{
				DirectX::XMFLOAT4 result;
				XMStoreFloat4(&result, DirectX::XMVectorAdd(DirectX::XMLoadFloat4(&vec1), DirectX::XMLoadFloat4(&vec2)));
				return result;
			}, py::is_operator());
#pragma endregion

	py::class_<Bezier>(_module, "Bezier")
		.def(py::init<>())
		.def("AddControlPoint", &Bezier::AddControlPoint, "Adds control point to Bezier")
		.def("GetXPos", &Bezier::GetXPos, "Gets X Pos")
		.def("GetYPos", &Bezier::GetYPos, "Gets Y Pos");

	py::class_<duckApp::GameObject>(_module, "GameObject")
		.def("GetPosition", &GameObject::GetPosition, "Gets Position")
		.def("GetScale", &GameObject::GetScale, "Gets Scale")
		.def("GetNextPosition", &GameObject::GetNextPosition, "Gets Next Position")
		.def("SetPosition", &GameObject::SetPosition, "Sets Position")
		.def("SetScale", &GameObject::SetScale, "Sets Scale")
		.def("SetNextPosition", &GameObject::SetNextPosition, "Sets next Position");
}