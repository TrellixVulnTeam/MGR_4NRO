#pragma once

#include <DirectXMath.h>
#include <d3d11.h>

using namespace DirectX;

namespace SkinningApp
{
	struct DirectionalLight
	{
		DirectionalLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;
		XMFLOAT4 Direction;
	};

	struct Material
	{
		Material() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular; // w = SpecPower
		XMFLOAT4 Reflect;
	};

	struct Transform
	{
		Transform(): position(0,0,0), rotation(0,0,0), scaling(1,1,1) {  }
		Transform(XMFLOAT3 _position, XMFLOAT3 _rotation, XMFLOAT3 _scaling) :
			position(_position), rotation(_rotation), scaling(_scaling) {}

		XMFLOAT3 GetPosition() { return position; }
		XMFLOAT3 GetRotation() { return rotation; }
		XMFLOAT3 GetScaling() { return scaling; }

		bool SetPosition(XMFLOAT3 value) { return SetValue(position, value); }
		bool SetRotation(XMFLOAT3 value) { return SetValue(rotation, value); }
		bool SetScaling(XMFLOAT3 value) { return SetValue(scaling, value); }
	private:
		XMFLOAT3 position;
		XMFLOAT3 rotation;
		XMFLOAT3 scaling;

		bool SetValue(XMFLOAT3& variable,XMFLOAT3 value)
		{
			if (XMVector3NotEqual(XMLoadFloat3(&variable), XMLoadFloat3(&value)))
			{
				variable = value;
				return true;
			}

			return false;
		}
	};
}