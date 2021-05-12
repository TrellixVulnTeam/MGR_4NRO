#pragma once

#include "dxptr.h"
#include <vector>
#include <DirectXMath.h>
#include <D3D11.h>
#include "vertexTypes.h"
#include "dxDevice.h"

namespace mini
{
	struct Triangle {
		DirectX::XMFLOAT3 p1;
		DirectX::XMFLOAT3 p2;
		DirectX::XMFLOAT3 p3;
	};

	struct Edge
	{
		DirectX::XMFLOAT3 posFrom;
		DirectX::XMFLOAT3 posTo;
		int tr1;
		int tr2;
	};

	struct Shadows
	{
		Shadows() = default;
		void operator=(Shadows&& src) noexcept {
			for (auto& ver : src.edges) {
				edges.push_back(ver);
			}
			for (auto& ver : src.triangles) {
				triangles.push_back(ver);
			}
		}

		void operator=(const Shadows& src) noexcept  {
			for (auto& ver : src.edges) {
				edges.push_back(ver);
			}
			for (auto& ver : src.triangles) {
				triangles.push_back(ver);
			}
		}

		Shadows(Shadows&& src) noexcept {
			for (auto& ver : src.edges) {
				edges.push_back(ver);
			}
			for (auto& ver : src.triangles) {
				triangles.push_back(ver);
			}
		}

		Shadows(const Shadows& src) noexcept {
			for (auto& ver : src.edges) {
				edges.push_back(ver);
			}
			for (auto& ver : src.triangles) {
				triangles.push_back(ver);
			}
		}

		std::vector<Edge> edges;
		std::vector<Triangle> triangles;
	};

	class Mesh
	{
	public:
		Mesh();
		Mesh(dx_ptr_vector<ID3D11Buffer>&& vbuffers,
			std::vector<unsigned int>&& vstrides,
			dx_ptr<ID3D11Buffer>&& indices,
			unsigned int indexCount,
			D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			: Mesh(std::move(vbuffers), std::move(vstrides), std::vector<unsigned>(vbuffers.size(), 0U),
				std::move(indices), indexCount, primitiveType)
		{ }
		Mesh(dx_ptr_vector<ID3D11Buffer>&& vbuffers,
			std::vector<unsigned int>&& vstrides,
			std::vector<unsigned int>&& voffsets,
			dx_ptr<ID3D11Buffer>&& indices,
			unsigned int indexCount,
			D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Mesh(Mesh&& right) noexcept;
		Mesh(const Mesh& right) = delete;
		void Release();
		~Mesh();

		Mesh& operator=(const Mesh& right) = delete;
		Mesh& operator=(Mesh&& right) noexcept;
		void Render(const dx_ptr<ID3D11DeviceContext>& context) const;

		static Mesh SimpleTriMesh(const DxDevice& device, const std::vector<VertexPositionNormal> verts, const std::vector<unsigned short> idxs, const Shadows shadows)
		{
			if (idxs.empty())
				return {};
			Mesh result;

			result.shadows = shadows;
			result.m_indexBuffer = device.CreateIndexBuffer(idxs);
			result.m_vertexBuffers.push_back(device.CreateVertexBuffer(verts));
			result.m_strides.push_back(sizeof(VertexPositionNormal));
			result.m_offsets.push_back(0);
			result.m_indexCount = idxs.size();
			result.m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			return result;
		}

		template<typename VertexType>
		static Mesh SimpleTriMesh(const DxDevice& device, const std::vector<VertexType> verts, const std::vector<unsigned short> idxs)
		{
			if (idxs.empty())
				return {};
			Mesh result;
			result.m_indexBuffer = device.CreateIndexBuffer(idxs);
			result.m_vertexBuffers.push_back(device.CreateVertexBuffer(verts));
			result.m_strides.push_back(sizeof(VertexType));
			result.m_offsets.push_back(0);
			result.m_indexCount = idxs.size();
			result.m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			return result;
		}

		//Box Mesh Creation

		static std::vector<VertexPositionColor> ColoredBoxVerts(float width, float height, float depth);
		static std::vector<VertexPositionColor> ColoredBoxVerts(float side = 1.0f) { return ColoredBoxVerts(side, side, side); }
		static std::vector<VertexPositionNormal> ShadedBoxVerts(float width, float height, float depth);
		static std::vector<VertexPositionNormal> ShadedBoxVerts(float side = 1.0f) { return ShadedBoxVerts(side, side, side); }
		static std::vector<unsigned short> BoxIdxs();
		static Mesh ColoredBox(const DxDevice& device, float width, float height, float depth) { return SimpleTriMesh(device, ColoredBoxVerts(width, height, depth), BoxIdxs()); }
		static Mesh ColoredBox(const DxDevice& device, float side = 1.0f) { return ColoredBox(device, side, side, side); }
		static Mesh ShadedBox(const DxDevice& device, float width, float height, float depth) { return SimpleTriMesh(device, ShadedBoxVerts(width, height, depth), BoxIdxs()); }
		static Mesh ShadedBox(const DxDevice& device, float side = 1.0f) { return ShadedBox(device, side, side, side); }

		//Rectangle Mesh Creation

		static std::vector<VertexPositionNormal> RectangleVerts(float width, float height);
		static std::vector<unsigned short> RectangleIdxs();
		static Mesh Rectangle(const DxDevice& device, float width = 1.0f, float height = 1.0f);

		//Shadow Box
		static Mesh ShadowBox(const DxDevice& device, Mesh& source, DirectX::XMFLOAT4 lightPosition, DirectX::XMFLOAT4X4 world);

		//Cylinder Mesh Creation

		static std::vector<VertexPositionNormal> CylinderVerts(float radius, float length, int radiusSplit, int lengthSplit);
		static Mesh Cylinder(const DxDevice& device, float radius = 1.0f, float length = 1.0f, int radiusSplit = 100, int lengthSplit = 100);

		static Mesh LoadMesh(const DxDevice& device, const std::wstring& meshPath);
	private:
		dx_ptr<ID3D11Buffer> m_indexBuffer;
		dx_ptr_vector<ID3D11Buffer> m_vertexBuffers;
		std::vector<unsigned int> m_strides;
		std::vector<unsigned int> m_offsets;
		std::vector<DirectX::XMFLOAT3> vertex_;
		std::vector<DirectX::XMFLOAT2> indices_;
		Shadows shadows;
		unsigned int m_indexCount;
		D3D_PRIMITIVE_TOPOLOGY m_primitiveType;
	};
}
