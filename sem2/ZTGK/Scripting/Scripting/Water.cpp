#include "Water.h"

#include <vertexDef.h>
#include <dxstructures.h>

#include "Geometry\Vector.h"
#include "Geometry\Mesh\MeshLoader.h"

namespace duckApp
{
	Water::Water(DxDevice m_device,
		const ConstantBuffer<DirectX::XMFLOAT4X4>& cbWorld,
		const ConstantBuffer<DirectX::XMFLOAT4X4, 2>& cbView,
		const ConstantBuffer<DirectX::XMFLOAT4X4>& cbProj,
		const ConstantBuffer<DirectX::XMFLOAT4X4>& cbTex,
		const dx_ptr<ID3D11SamplerState>& sampler) : m_random(random_device{}()), wave_texture(m_device)
	{
		/* Load Mesh */
		MeshLoader mesh_loader; vector<mini::VertexPositionNormal> vertices; vector<short> indices;

		tie(vertices, indices) = mesh_loader.CreateRectangle(2.0f, 2.0f);
		water_mesh = m_device.CreateMesh(indices, vertices);

		XMStoreFloat4x4(&water_mesh_matrix, XMMatrixScaling(5, 1, 5));

		/* Load Shaders */
		auto vsCode = m_device.LoadByteCode(L"WaterVertexShader.cso");
		auto psCode = m_device.LoadByteCode(L"WaterPixelShader.cso");

		//dx_ptr<ID3D11ShaderResourceView> m_envTexture = m_device.CreateShaderResourceView(L"Resources/Textures/Colours.dds");
		dx_ptr<ID3D11ShaderResourceView> m_envTexture = m_device.CreateShaderResourceView(L"Resources/Textures/CubeTex/skybox_autum_forest.dds");

		/* Textures */
		height_map_texture_1 = &this->height_map_textures[0];
		height_map_texture_2 = &this->height_map_textures[1];
		height_map_texture_3 = &this->height_map_textures[2];

		/* Create Effect */
		water_effect = MultiTexturedEffect(m_device.CreateVertexShader(vsCode), m_device.CreatePixelShader(psCode),
			cbWorld, cbView, cbProj, cbTex, cbTex, sampler, m_envTexture, m_envTexture);

		/* SetUp Rasterizers States */
		mini::utils::RasterizerDescription rasterizer_desc;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;

		m_rsCullNone = m_device.CreateRasterizerState(rasterizer_desc);
	}

	void Water::Update(DxDevice m_device)
	{
		this->UpdateHeightMap();
		this->UpdateWaveTextureMap(m_device);
		this->GetRandomDrop();

		water_effect.SetTexture2(this->wave_texture.GetTexture(m_device));
	}

	void Water::Render(DxDevice m_device, ConstantBuffer<XMFLOAT4X4>* m_cbWorldMtx, ConstantBuffer<XMFLOAT4> m_camPos)
	{
		m_device.context()->RSSetState(m_rsCullNone.get());

		water_effect.SetCameraPosition(m_camPos);
		water_effect.Begin(m_device.context());

		m_cbWorldMtx->Update(m_device.context(), water_mesh_matrix);
		water_mesh.Render(m_device.context());

		m_device.context()->RSSetState(nullptr);
	}

	void Water::CreateRipple(int x, int y)
	{
		this->height_map_texture_1->SetValue(x, y, 2.25f);
	}

	void Water::GetRandomDrop()
	{
		static const uniform_real_distribution<float> random(0, 100);

		if (random(m_random) > 65.0f)
		{
			static const uniform_real_distribution<float> random_x(0, 256);
			static const uniform_real_distribution<float> random_y(0, 256);
		
			this->CreateRipple((int)random_x(m_random), (int)random_y(m_random));
		}
	}

	void Water::UpdateWithDuckTrace(XMFLOAT3 duck_position)
	{
		int x = (int)(duck_position.x + 2.5) * 255 / 5;
		int y = (int)(duck_position.z + 2.5) * 255 / 5;

		if (x < 0 || x > 255 || y < 0 || y > 255) return;

		this->CreateRipple(x, y);
	}

	void Water::UpdateHeightMap()
	{
		auto tmp_height_map_texture = this->height_map_texture_3;
		this->height_map_texture_3 = this->height_map_texture_2;
		this->height_map_texture_2 = this->height_map_texture_1;
		this->height_map_texture_1 = tmp_height_map_texture;

		int texture_size = this->height_map_texture_1->GetSize();

		const float wave_speed = 1.0f;
		const float integration_step = 1.0f / texture_size;


		float h_value = 2.0f / (texture_size - 1);
		const float a_coef = wave_speed * wave_speed * integration_step * integration_step / (h_value * h_value);
		const float b_coef = 2.0f - 4.0f * a_coef;

		/* Calculate New Values For HeightMap */
		for (int y = 0; y < texture_size; ++y)
		{
			for (int x = 0; x < texture_size; ++x)
			{
				int lastX = x > 0 ? x - 1 : 0;
				int nextX = x < texture_size - 1 ? x + 1 : texture_size - 1;

				int lastY = y > 0 ? y - 1 : 0;
				int nextY = y < texture_size - 1 ? y + 1 : texture_size - 1;

				float absorption_value = this->absorption_texture.GetValue(x, y);
				float value = absorption_value * (a_coef * (this->height_map_texture_2->GetValue(nextX, y) + this->height_map_texture_2->GetValue(lastX, y) + this->height_map_texture_2->GetValue(x, lastY) + this->height_map_texture_2->GetValue(x, nextY)) + b_coef * this->height_map_texture_2->GetValue(x, y) - this->height_map_texture_3->GetValue(x, y));

				this->height_map_texture_1->SetValue(x, y, value);
			}
		}
	}

	void Water::UpdateWaveTextureMap(DxDevice m_device)
	{
		int texture_size = this->wave_texture.GetSize();

		for (int y = 0; y < texture_size; ++y)
		{
			for (int x = 0; x < texture_size; ++x)
			{
				int prevX = (x + texture_size - 1) % texture_size;
				int nextX = (x + 1) % texture_size;
				int prevY = (y + texture_size - 1) % texture_size;
				int nextY = (y + 1) % texture_size;

				auto p00 = this->height_map_texture_1->GetValue(prevX, prevY);
				auto p10 = this->height_map_texture_1->GetValue(x, prevY);
				auto p20 = this->height_map_texture_1->GetValue(nextX, prevY);

				auto p01 = this->height_map_texture_1->GetValue(prevX, y);
				auto p11 = this->height_map_texture_1->GetValue(x, y);
				auto p21 = this->height_map_texture_1->GetValue(nextX, y);

				auto p02 = this->height_map_texture_1->GetValue(prevX, nextY);
				auto p12 = this->height_map_texture_1->GetValue(x, nextY);
				auto p22 = this->height_map_texture_1->GetValue(nextX, nextY);

				float horizontal = (p01 - p21) * 2.0f + p20 + p22 - p00 - p02;
				float vertical = (p12 - p10) * 2.0f + p22 + p02 - p20 - p00;
				float depth = 1.0f / 2.0f;

				float r = horizontal;
				float g = vertical;
				float b = depth;

				float length = std::sqrt(r * r + g * g + b * b);
				r /= length;
				g /= length;
				b /= length;

				this->wave_texture.SetValue(x, y, Vector(r, g, b));
			}
		}
		this->wave_texture.Update(m_device);
	}
}
