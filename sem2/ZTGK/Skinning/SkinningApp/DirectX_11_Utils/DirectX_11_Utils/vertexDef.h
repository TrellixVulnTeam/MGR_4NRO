#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

namespace Real_Engine
{
	struct VertexA
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT3 weights;
		BYTE indices[4];

		static const D3D11_INPUT_ELEMENT_DESC Layout[4];
	};

	struct Vertex
	{
		XMFLOAT3 position;

		static const D3D11_INPUT_ELEMENT_DESC Layout[1];
	};

	struct VertexN
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;

		static const D3D11_INPUT_ELEMENT_DESC Layout[2];
	};

	struct VertexN2T
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 tex;

		static const D3D11_INPUT_ELEMENT_DESC Layout[3];
	};

	struct VertexN3T
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT3 tex;

		static const D3D11_INPUT_ELEMENT_DESC Layout[3];
	};

	struct Vertex2T
	{
		XMFLOAT3 position;
		XMFLOAT2 tex;

		static const D3D11_INPUT_ELEMENT_DESC Layout[2];
	};

    struct InstanceData
    {
        XMFLOAT4X4 worldMatrix;

        static const D3D11_INPUT_ELEMENT_DESC Layout[4];
    };

    struct VertexN2TInstanced
    {
        VertexN2T perVertexData;
        InstanceData perInstanceData;

        static const D3D11_INPUT_ELEMENT_DESC Layout[7];
    };
}
