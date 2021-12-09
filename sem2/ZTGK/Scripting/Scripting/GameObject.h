#pragma once

#include <list>
#include <vector>
#include <random>

#include <DirectXMath.h>
#include <mesh.h>
#include <dxDevice.h>
#include <constantBuffer.h>
#include <dxptr.h>
#include <effect.h>

#include "Geometry\UV.h"
#include "Geometry\Vector.h"
#include "Geometry\Mesh\MeshRobot.h"

#include "Gk2BaseClasses\TexturedEffect.h"

#include "Script.h"


using namespace std;
using namespace mini;
using namespace DirectX;

/* TODO (3.01) Include Python And PyBind11 Headers */


namespace duckApp
{
	struct DuckLayout
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;

		static const D3D11_INPUT_ELEMENT_DESC Layout[3];

		DuckLayout() : position(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), uv(0.0f, 0.0f) { }
	};

	class GameObject /* Represents Duck Instance */
	{
	public:
		GameObject() = default;

		GameObject(DxDevice m_device,
			const ConstantBuffer<DirectX::XMFLOAT4X4>& cbWorld,
			const ConstantBuffer<DirectX::XMFLOAT4X4, 2>& cbView,
			const ConstantBuffer<DirectX::XMFLOAT4X4>& cbProj,
			const ConstantBuffer<DirectX::XMFLOAT4X4>& cbTex,
			const dx_ptr<ID3D11SamplerState>& sampler);

		void Update(DxDevice m_device, float dtime);
		void Render(DxDevice m_device, ConstantBuffer<XMFLOAT4X4>* m_cbWorldMtx,
			ConstantBuffer<XMFLOAT4> camera_position, ConstantBuffer<XMFLOAT4> light_position);

		/* TODO (3.03) Define Methods For Script Management */
		/*
			* Returns Script Associated With This Game Object (Duck)
		*/
		Script* GetPythonScript() { return this->script.get(); };

		/* TODO (3.04) Define Script Methods */
		/*
			* On Create Method
			* On Update Method
		*/


		/* TODO (3.05) Define C++ API Methods */
		/*
			* Returns Next Duck Position
			* Returns Current Duck Scale
			* Returns Current Duck Position

			* Sets Next Duck Position
			* Sets Current Duck Scale
			* Sets Current Duck Position
		*/
		XMFLOAT3 GetPosition() { return this->current_position; }
		XMFLOAT3 GetScale() { return this->current_scale; }
		XMFLOAT3 GetNextPosition() { return this->next_position; }

		void SetPosition(XMFLOAT3 v) { this->current_position = v; }
		void SetScale(XMFLOAT3 v) { this->current_scale = v; }
		void SetNextPosition(XMFLOAT3 v) { this->next_position = v; }

	private:
		/* TODO (3.02) Add Script Pointer Instance */
		std::unique_ptr<Script> script;

		XMFLOAT3 current_scale = { 0.1f, 0.1f, 0.1f };
		XMFLOAT3 next_position = { 1, 0, 0 };
		XMFLOAT3 current_position = { 0, 0, 0 };
		XMFLOAT4X4 current_rotation;

		Mesh duck_mesh;
		XMFLOAT4X4 duck_mesh_matrix;

		vector<short> indices;
		vector<DuckLayout> duck_layout;
		dx_ptr<ID3D11Buffer> m_vertices;
		dx_ptr<ID3D11Buffer> m_indices;

		static const unsigned int OFFSET;
		static const unsigned int STRIDE;

		TexturedEffect duck_textured_effect;
		dx_ptr<ID3D11InputLayout> m_inputLayout;

		void LoadDuckLayout(DxDevice m_device, MeshRobot duck_mesh_info);
	};
}
