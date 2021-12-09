/* TODO (5.01) Include Python Embedding Header */
#include "GameObject.h"

#include <pybind11/embed.h>

namespace py = pybind11;
using namespace duckApp;
/* TODO (5.02) Create C++ API Module To Be Embedded Into Python */
PYBIND11_EMBEDDED_MODULE(API, _module)
{
	py::class_<DirectX::XMFLOAT3>(_module, "Vector3")
		.def(py::init<float,float,float>())
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
			},py::is_operator())
		.def("__add__",
			[](DirectX::XMFLOAT3& vec1, DirectX::XMFLOAT3& vec2)
			{
				DirectX::XMFLOAT3 result;
				XMStoreFloat3(&result, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&vec1), DirectX::XMLoadFloat3(&vec2)));
				return result;
			}, py::is_operator());

	py::class_<duckApp::GameObject>(_module, "GameObject")
		.def("GetPosition", &GameObject::GetPosition, "Gets Position")
		.def("GetScale", &GameObject::GetScale, "Gets Scale")
		.def("GetNextPosition", &GameObject::GetNextPosition, "Gets Next Position")
		.def("SetPosition", &GameObject::SetPosition, "Sets Position")
		.def("SetScale", &GameObject::SetScale, "Sets Scale")
		.def("SetNextPosition", &GameObject::SetNextPosition, "Sets next Position");
}