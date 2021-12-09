#include "DuckApp.h"

#include <vertexDef.h>
#include <DirectXMath.h>

#include "Geometry\Mesh\MeshLoader.h"

using namespace mini;
using namespace utils;
using namespace std;
using namespace DirectX;

namespace duckApp
{
	const XMFLOAT4 DuckApp::LIGHT_POS = { 0.0f, 2.0f, -2.0f, 1.0f };

	DuckApp::DuckApp(HINSTANCE hInstance) : Gk2ExampleBase(hInstance, 1280, 640, L"Duck Application")
	{
		/* Initialize Script Manager Befor Any Use Of Python API */

		/* TODO (4.03) Initialize Scripting Manager */
		this->script_manager = std::make_unique<ScriptManager>();

		/* Create Constant Buffers */
		m_cbWorldMtx = m_device.CreateConstantBuffer<XMFLOAT4X4>();
		m_cbProjMtx = m_device.CreateConstantBuffer<XMFLOAT4X4>();
		m_cbViewMtx = m_device.CreateConstantBuffer<XMFLOAT4X4, 2>();
		m_cbLightPos = m_device.CreateConstantBuffer<XMFLOAT4>();
		m_cbTexMtx = m_device.CreateConstantBuffer<XMFLOAT4X4>();;
		m_cbCameraPos = m_device.CreateConstantBuffer<XMFLOAT4>();

		/* Load textures */
		//dx_ptr<ID3D11ShaderResourceView> m_envTexture = m_device.CreateShaderResourceView(L"Resources/Textures/Colours.dds");
		dx_ptr<ID3D11ShaderResourceView> m_envTexture = m_device.CreateShaderResourceView(L"Resources/Textures/CubeTex/skybox_autum_forest.dds");

		/* Sampler States */
		m_samplerWrap = m_device.CreateSamplerState(SamplerDescription());

		SamplerDescription border_sampler;
		border_sampler.AddressU = border_sampler.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		border_sampler.BorderColor[0] = border_sampler.BorderColor[1] = border_sampler.BorderColor[2] = border_sampler.BorderColor[3] = 1.0f;
		m_samplerBorder = m_device.CreateSamplerState(border_sampler);

		/* Load Duck Robot Mesh */
		MeshLoader mesh_loader;

		duck_instance = std::make_unique<GameObject>(m_device, m_cbWorldMtx, m_cbViewMtx, m_cbProjMtx, m_cbTexMtx, m_samplerWrap);
		water_instance = std::make_unique<Water>(m_device, m_cbWorldMtx, m_cbViewMtx, m_cbProjMtx, m_cbTexMtx, m_samplerBorder);

		/* TODO (4.04) Add duck_instance Game Object To Script Manager And Dispath On Create Method */
		this->script_manager->AddGameObjectScript(duck_instance.get());
		this->script_manager->DispatchOnCreate();

		/* Create Other Meshes */
		vector<VertexPositionNormal> vertices; vector<short> indices;

		tie(vertices, indices) = mesh_loader.CreateBox(9.90f, 10.0f, 9.90f);
		box_mesh = m_device.CreateMesh(indices, vertices);
		XMStoreFloat4x4(&box_mesh_matrix, XMMatrixTranslation(0.0f, 2.0f, 0.0f));

		/* Create Projection Matrix */
		auto client_size = m_window.getClientSize();
		auto aspect_ration = static_cast<float>(client_size.cx) / client_size.cy;

		XMStoreFloat4x4(&m_projMtx, XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect_ration, 0.01f, 100.0f));
		m_cbProjMtx.Update(m_device.context(), m_projMtx);

		/* Create Identity for World Matrix */
		XMFLOAT4X4 worldMtx; XMStoreFloat4x4(&worldMtx, XMMatrixIdentity());
		m_cbWorldMtx.Update(m_device.context(), worldMtx);

		/* Update Lights Position */
		m_cbLightPos.Update(m_device.context(), LIGHT_POS);

		/* Update Texture Transformation Matrix*/
		XMFLOAT4X4 tempMtx;
		XMStoreFloat4x4(&tempMtx, XMMatrixIdentity());
		m_cbTexMtx.Update(m_device.context(), tempMtx);

		/* Update Camera with New Projection Matrix */
		auto viewMtx = m_camera.getViewMatrix();
		UpdateCamera(viewMtx);

		/* Map Env Textured Effect */
		auto vsCode = m_device.LoadByteCode(L"EnvTexturedVertexShader.cso");
		auto psCode = m_device.LoadByteCode(L"EnvTexturedPixelShader.cso");

		textured_wall_effect = TexturedEffect(m_device.CreateVertexShader(vsCode), m_device.CreatePixelShader(psCode),
			m_cbWorldMtx, m_cbViewMtx, m_cbProjMtx, m_cbTexMtx, m_samplerWrap, m_envTexture);

		/* Create And Set Input Layout for Shaders */
		m_inputlayout = m_device.CreateInputLayout(VertexPositionNormal::Layout, vsCode);

		m_device.context()->IASetInputLayout(m_inputlayout.get());
		m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void DuckApp::Update(const Clock& clock)
	{
		float dt = (float)clock.getFrameTime();

		/* Handles Camera Input (Gk2ExaplmeBase) */
		HandleCameraInput(dt);

		/* Updates Camera Position with Current Values */
		UpdateCamera(m_camera.getViewMatrix());

		/* Updates Projection Matrix */
		m_cbProjMtx.Update(m_device.context(), m_projMtx);

		/* TODO (4.05) Script Manager - Dispath On Update Method */
		this->script_manager->DispatchOnUpdate();

		/* Updates Scene Objects */
		this->UpdateWater();
		this->UpdateDuck(dt);
	}

	void DuckApp::UpdateCamera(XMMATRIX viewMtx)
	{
		XMMATRIX invViewMtx = XMMatrixInverse(nullptr, viewMtx);

		XMFLOAT4X4 view[2];
		XMStoreFloat4x4(&view[0], viewMtx);
		XMStoreFloat4x4(&view[1], invViewMtx);

		m_cbViewMtx.Update(m_device.context(), view);
		m_cbCameraPos.Update(m_device.context(), m_camera.getCameraPosition());
	}

	void DuckApp::UpdateDuck(float dtime)
	{
		duck_instance->Update(m_device, dtime);
	}

	void DuckApp::UpdateWater()
	{
		this->water_instance->Update(m_device);
		this->water_instance->UpdateWithDuckTrace(this->duck_instance->GetPosition());
	}

	void DuckApp::Render()
	{
		/* SetUp Blue Background Colour (DxApplication) */
		DxApplication::Render();

		/* Default Render Target */
		getDefaultRenderTarget().Begin(m_device.context());

		/* Draw Scene Objects */
		this->DrawDuck();
		this->DrawWater();
		this->DrawWalls();
	}

	void DuckApp::DrawDuck()
	{
		duck_instance->Render(m_device, &m_cbWorldMtx, m_cbCameraPos, m_cbLightPos);

		/* CleanUp After Duck Drawing */
		m_device.context()->GSSetShader(nullptr, nullptr, 0);
		m_device.context()->IASetInputLayout(m_inputlayout.get());
		m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void DuckApp::DrawWater()
	{
		this->water_instance->Render(m_device, &m_cbWorldMtx, m_cbCameraPos);
	}

	void DuckApp::DrawWalls()
	{
		textured_wall_effect.Begin(m_device.context());

		DrawMesh(box_mesh, box_mesh_matrix);
	}

	/* Draws Mesh And Updates World Matrix */
	void DuckApp::DrawMesh(const Mesh& m, DirectX::XMFLOAT4X4 worldMtx)
	{
		SetWorldMtx(worldMtx);
		m.Render(m_device.context());
	}

	/* Updates World Matrix */
	void DuckApp::SetWorldMtx(DirectX::XMFLOAT4X4 mtx)
	{
		m_cbWorldMtx.Update(m_device.context(), mtx);
	}

	/* Updates Surface Colour */
	void DuckApp::SetSurfaceColor(DirectX::XMFLOAT4 color)
	{
		m_cbSurfaceColor.Update(m_device.context(), color);
	}
}
