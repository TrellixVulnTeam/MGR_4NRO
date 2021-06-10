#include "teselacja.h"


using namespace mini;
using namespace gk2;
using namespace DirectX;
using namespace std;

#pragma region Constants
const unsigned int Teselacja::BS_MASK = 0xffffffff;

const XMFLOAT4 LightPos = XMFLOAT4(-1.0f, 3.0f, -3.0f, 1.0f);

#pragma endregion

#pragma region Initalization
Teselacja::Teselacja(HINSTANCE hInstance)
	: Base(hInstance, 1280, 720, L"Teselacja"),
	m_cbView(m_device.CreateConstantBuffer<XMFLOAT4X4, 2>()),
	m_cbProj(m_device.CreateConstantBuffer<XMFLOAT4X4>()),
	m_cbWorld(m_device.CreateConstantBuffer<XMFLOAT4X4>()),
	m_cbSurfaceColor(m_device.CreateConstantBuffer<XMFLOAT4>()),
	m_cbParameters(m_device.CreateConstantBuffer<Parameters>()),
	m_cbParameters2(m_device.CreateConstantBuffer<Parameters2>()),
	m_cbLighting(m_device.CreateConstantBuffer<Lighting>()),
	m_vertexStride(sizeof(VertexPositionTex)), m_vertexCount(16)
{
	auto s = m_window.getClientSize();
	auto ar = static_cast<float>(s.cx) / s.cy;
	XMFLOAT4X4 tmpMtx;
	XMStoreFloat4x4(&tmpMtx, XMMatrixPerspectiveFovLH(XM_PIDIV4, ar, 0.01f, 100.0f));
	UpdateBuffer(m_cbProj, tmpMtx);
	UpdateBuffer(m_cbSurfaceColor, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

	CreateRenderStates();

	XMFLOAT4X4 cameraMtx;
	XMStoreFloat4x4(&cameraMtx, m_camera.getViewMatrix());
	UpdateCameraCB(cameraMtx);

	auto vsCode = m_device.LoadByteCode(L"tessellatedBezierVS.cso");
	m_tessVS = m_device.CreateVertexShader(m_device.LoadByteCode(L"tessellatedBezierVS.cso"));
	m_tessHS = m_device.CreateHullShader(m_device.LoadByteCode(L"tessellatedBezierHS.cso"));
	m_tessDS = m_device.CreateDomainShader(m_device.LoadByteCode(L"tessellatedBezierDS.cso"));
	m_tessPS = m_device.CreatePixelShader(m_device.LoadByteCode(L"tessellatedBezierPS.cso"));

	m_layout = m_device.CreateInputLayout(VertexPositionTex::Layout, vsCode);

	auto vsCode2 = m_device.LoadByteCode(L"wireVS.cso");
	m_wireVS = m_device.CreateVertexShader(vsCode2);
	m_wirePS = m_device.CreatePixelShader(m_device.LoadByteCode(L"wirePS.cso"));
	m_wireIL = m_device.CreateInputLayout(VertexPositionNormal::Layout, vsCode2);



	XMFLOAT4X4 mtx;
	XMStoreFloat4x4(&mtx, XMMatrixTranslation(-0.5f, 0.0f, -0.5f)* XMMatrixScaling(3.0f, 3.0f, 3.0f));
	SetWorldMtx(mtx);

	ID3D11Buffer* vsb[] = { m_cbWorld.get(),m_cbView.get(), m_cbProj.get(),m_cbParameters.get() };
	m_device.context()->VSSetConstantBuffers(0, 4, vsb);
	m_device.context()->DSSetConstantBuffers(0, 4, vsb);

	ID3D11Buffer* hsb[] = { m_cbWorld.get(),m_cbView.get(),m_cbParameters.get() };
	m_device.context()->HSSetConstantBuffers(0, 3, hsb);

	ID3D11Buffer* psb[] = { m_cbSurfaceColor.get(), m_cbLighting.get(),m_cbParameters2.get() };
	m_device.context()->PSSetConstantBuffers(0, 3, psb);

	SetVersion();
	SetInitialParameters();
	UpdateParameters();
	Set1Light();

	m_heightTexture = m_device.CreateShaderResourceView(L"resources/textures/height.dds");
	m_normalsTexture = m_device.CreateShaderResourceView(L"resources/textures/normals.dds");
	m_diffuseTexture = m_device.CreateShaderResourceView(L"resources/textures/diffuse.dds");

	SetTexturesDS({ m_heightTexture.get() }, m_samplerWrap);
	SetTexturesPS({ m_diffuseTexture.get(), m_normalsTexture.get() }, m_samplerWrap);
}

void Teselacja::SetVersion()
{
	vector<VertexPositionTex> vtx = Mesh::BezierPatches(patchN, patchM, version);
	m_vertexBuffer = m_device.CreateVertexBuffer(vtx);
	unsigned int offset = 0;
	ID3D11Buffer* b = m_vertexBuffer.get();
	m_device.context()->IASetVertexBuffers(0, 1, &b, &m_vertexStride, &offset);
	m_wire = Mesh::BezierWire(patchN, patchM, m_device, version);

}

void Teselacja::CreateRenderStates()
//Setup render states used in various stages of the scene rendering
{
	DepthStencilDescription dssDesc;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; //Enable writing to depth buffer
	m_dssNoDepthWrite = m_device.CreateDepthStencilState(dssDesc);

	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; //Enable writing to depth buffer
	m_dssDepthWrite = m_device.CreateDepthStencilState(dssDesc);
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	dssDesc.StencilEnable = true;
	dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR_SAT;
	dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dssDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	m_dssStencilWrite = m_device.CreateDepthStencilState(dssDesc);

	dssDesc.StencilEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dssDesc.StencilWriteMask = 0xff;
	dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	dssDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dssDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	dssDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	m_dssStencilWriteSh = m_device.CreateDepthStencilState(dssDesc);

	dssDesc.StencilEnable = true;
	dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dssDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	m_dssStencilTestNoDepthWrite = m_device.CreateDepthStencilState(dssDesc);

	dssDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	m_dssStencilTest = m_device.CreateDepthStencilState(dssDesc);

	dssDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dssDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
	dssDesc.StencilWriteMask = 0x0;
	m_dssStencilTestSh = m_device.CreateDepthStencilState(dssDesc);


	dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_LESS;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dssDesc.StencilWriteMask = 0x0;
	dssDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
	m_dssStencilTestGreaterSh = m_device.CreateDepthStencilState(dssDesc);

	RasterizerDescription rsDesc;
	rsDesc.CullMode = D3D11_CULL_NONE;
	m_rsAll = m_device.CreateRasterizerState(rsDesc);

	rsDesc.FrontCounterClockwise = true;
	m_rsCCW = m_device.CreateRasterizerState(rsDesc);

	rsDesc.FrontCounterClockwise = false;
	rsDesc.CullMode = D3D11_CULL_NONE;
	m_rsCCW_backSh = m_device.CreateRasterizerState(rsDesc);
	rsDesc.CullMode = D3D11_CULL_BACK;
	m_rsCCW_frontSh = m_device.CreateRasterizerState(rsDesc);

	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	m_rsWireframe = m_device.CreateRasterizerState(rsDesc);

	BlendDescription bsDesc;
	bsDesc.RenderTarget[0].BlendEnable = true;
	bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	m_bsAlpha = m_device.CreateBlendState(bsDesc);

	bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	m_bsAlphaInv = m_device.CreateBlendState(bsDesc);

	bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bsDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
	m_bsAdd = m_device.CreateBlendState(bsDesc);
}

void Teselacja::SetInitialParameters()
{
	parameters.edgeTessFactor = 8;
	parameters.insideTessFactor = 8;
	parameters.useLOD = 0;
	parameters.displacementMapping = 0;
	parameters2.colorTexture = 0;
	parameters2.normalMapping = 0;
	for (int i = 0; i < 4; ++i)
	{
		handled[i] = false;
	}
}

#pragma endregion
#pragma region Per-Frame Update
void Teselacja::Update(const Clock& c)
{
	Base::Update(c);
	float dt = c.getFrameTime();
	if (HandleCameraInput(dt))
	{
		XMFLOAT4X4 cameraMtx;
		XMStoreFloat4x4(&cameraMtx, m_camera.getViewMatrix());
		UpdateCameraCB(cameraMtx);
	}
	HandleKeyboard();
}

void Teselacja::HandleKeyboard()
{
	bool moved = false;
	bool keyboard = true;
	KeyboardState kstate;
	if (!m_keyboard.GetState(kstate))
		keyboard = false;
	if (keyboard) {
#pragma region edgeTessFactor
		if (kstate.isKeyDown(DIK_LBRACKET))// [
		{
			if (!handled[0]) {
				if (parameters.edgeTessFactor > 1)
				{
					parameters.edgeTessFactor--;
					UpdateParameters();
				}
				handled[0] = true;
			}
		}
		else if (handled[0])
		{
			handled[0] = false;
		}

		if (kstate.isKeyDown(DIK_RBRACKET))// ]
		{
			if (!handled[1]) {
				parameters.edgeTessFactor++;
				UpdateParameters();
				handled[1] = true;
			}
		}
		else if (handled[1])
		{
			handled[1] = false;
		}
#pragma endregion
#pragma region insideTessFactor
		if (kstate.isKeyDown(DIK_SEMICOLON))// ;
		{
			if (!handled[2]) {
				if (parameters.insideTessFactor > 1)
				{
					parameters.insideTessFactor--;
					UpdateParameters();
				}
				handled[2] = true;
			}
		}
		else if (handled[2])
		{
			handled[2] = false;
		}

		if (kstate.isKeyDown(DIK_APOSTROPHE))// '
		{
			if (!handled[3]) {
				parameters.insideTessFactor++;
				UpdateParameters();
				handled[3] = true;
			}
		}
		else if (handled[3])
		{
			handled[3] = false;
		}
#pragma endregion
#pragma region fillWireframe
		if (kstate.isKeyDown(DIK_COMMA))// ,
		{
			fillWireframe = false;
		}

		if (kstate.isKeyDown(DIK_PERIOD))// .
		{
			fillWireframe = true;
		}
#pragma endregion
#pragma region drawControlPolygon
		if (kstate.isKeyDown(DIK_O))// O
		{
			drawControlPolygon = false;
		}

		if (kstate.isKeyDown(DIK_P))// P
		{
			drawControlPolygon = true;
		}
#pragma endregion
#pragma region useLOD
		if (kstate.isKeyDown(DIK_K))// K
		{
			parameters.useLOD = 0;
			UpdateParameters();
		}

		if (kstate.isKeyDown(DIK_L))// L
		{
			parameters.useLOD = 1;
			UpdateParameters();
		}
#pragma endregion
#pragma region version
		if (kstate.isKeyDown(DIK_Q))// Q
		{
			version = 1;
			SetVersion();
		}
		if (kstate.isKeyDown(DIK_W))// W
		{
			version = 2;
			SetVersion();
		}

#pragma endregion
#pragma region displacementMapping
		if (kstate.isKeyDown(DIK_N))// N
		{
			parameters.displacementMapping = 0;
			UpdateParameters();
		}

		if (kstate.isKeyDown(DIK_M))// M
		{
			parameters.displacementMapping = 1;
			UpdateParameters();
		}
#pragma endregion
#pragma region colorTexture
		if (kstate.isKeyDown(DIK_U))// U
		{
			parameters2.colorTexture = 0;
			UpdateParameters();
		}

		if (kstate.isKeyDown(DIK_I))// I
		{
			parameters2.colorTexture = 1;
			UpdateParameters();
		}
#pragma endregion
#pragma region normalMapping
		if (kstate.isKeyDown(DIK_H))// H
		{
			parameters2.normalMapping = 0;
			UpdateParameters();
		}

		if (kstate.isKeyDown(DIK_J))// J
		{
			parameters2.normalMapping = 1;
			UpdateParameters();
		}
#pragma endregion


	}
}

void Teselacja::UpdateParameters()
{
	UpdateBuffer(m_cbParameters, parameters);
	UpdateBuffer(m_cbParameters2, parameters2);
}

void Teselacja::UpdateCameraCB(DirectX::XMFLOAT4X4 cameraMtx)
{
	XMMATRIX mtx = XMLoadFloat4x4(&cameraMtx);
	XMVECTOR det;
	auto invvmtx = XMMatrixInverse(&det, mtx);
	XMFLOAT4X4 view[2] = { cameraMtx };
	XMStoreFloat4x4(view + 1, invvmtx);
	UpdateBuffer(m_cbView, view);
}

#pragma endregion
#pragma region Frame Rendering Setup
void Teselacja::SetShaders(const dx_ptr<ID3D11InputLayout>& il, const dx_ptr<ID3D11VertexShader>& vs, const dx_ptr<ID3D11PixelShader>& ps, const dx_ptr<ID3D11DomainShader>& ds, const dx_ptr<ID3D11HullShader>& hs)
{
	m_device.context()->IASetInputLayout(il.get());
	m_device.context()->VSSetShader(vs.get(), nullptr, 0);
	m_device.context()->PSSetShader(ps.get(), nullptr, 0);
	m_device.context()->GSSetShader(nullptr, nullptr, 0);
	m_device.context()->DSSetShader(ds.get(), nullptr, 0);
	m_device.context()->HSSetShader(hs.get(), nullptr, 0);
}

void Teselacja::SetShaders(const dx_ptr<ID3D11InputLayout>& il, const dx_ptr<ID3D11VertexShader>& vs, const dx_ptr<ID3D11PixelShader>& ps)
{
	m_device.context()->IASetInputLayout(il.get());
	m_device.context()->VSSetShader(vs.get(), nullptr, 0);
	m_device.context()->PSSetShader(ps.get(), nullptr, 0);
	m_device.context()->DSSetShader(nullptr, nullptr, 0);
	m_device.context()->HSSetShader(nullptr, nullptr, 0);
	m_device.context()->GSSetShader(nullptr, nullptr, 0);
}

void Teselacja::SetTexturesDS(std::initializer_list<ID3D11ShaderResourceView*> resList, const dx_ptr<ID3D11SamplerState>& sampler)
{
	m_device.context()->DSSetShaderResources(0, resList.size(), resList.begin());
	auto s_ptr = sampler.get();
	m_device.context()->DSSetSamplers(0, 1, &s_ptr);
}

void Teselacja::SetTexturesPS(std::initializer_list<ID3D11ShaderResourceView*> resList, const dx_ptr<ID3D11SamplerState>& sampler)
{
	m_device.context()->PSSetShaderResources(0, resList.size(), resList.begin());
	auto s_ptr = sampler.get();
	m_device.context()->PSSetSamplers(0, 1, &s_ptr);
}

#pragma endregion

#pragma region Drawing

void Teselacja::SetWorldMtx(DirectX::XMFLOAT4X4 mtx)
{
	UpdateBuffer(m_cbWorld, mtx);
}

void Teselacja::DrawMesh(const Mesh& m, DirectX::XMFLOAT4X4 worldMtx)
{
	m.Render(m_device.context());
}

void Teselacja::Set1Light()
{
	Lighting l{
		/*.ambientColor = */ XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f),
		/*.surface = */ XMFLOAT4(0.2f, 1.0f, 0.8f, 50.0f),
		/*.lights =*/ {
			{ /*.position =*/ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), /*.color =*/ XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f) }
		}
	};
	ZeroMemory(&l.lights[1], sizeof(Light) * 2);
	UpdateBuffer(m_cbLighting, l);
}

void Teselacja::Render()
{
	Base::Render();
	if (fillWireframe)
		m_device.context()->RSSetState(m_rsAll.get());
	else
		m_device.context()->RSSetState(m_rsWireframe.get());

	unsigned int offset = 0;
	ID3D11Buffer* b = m_vertexBuffer.get();
	m_device.context()->IASetVertexBuffers(0, 1, &b, &m_vertexStride, &offset);
	SetShaders(m_layout, m_tessVS, m_tessPS, m_tessDS, m_tessHS);
	m_device.context()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST);
	m_device.context()->Draw(patchN * patchM * m_vertexCount, 0);

	if (drawControlPolygon)
	{
		SetShaders(m_wireIL, m_wireVS, m_wirePS);
		m_wire.Render(m_device.context());
	}
}
#pragma endregion