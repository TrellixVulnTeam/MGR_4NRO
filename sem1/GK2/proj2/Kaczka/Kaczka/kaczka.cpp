#include "kaczka.h"


using namespace mini;
using namespace gk2;
using namespace DirectX;
using namespace std;

#pragma region Constants
const unsigned int Kaczka::BS_MASK = 0xffffffff;

const float Kaczka::SHEET_SIZE = 2.0f;
const float Kaczka::DUCK_SCALING = 0.005f;
const XMFLOAT4 Kaczka::SHEET_COLOR = XMFLOAT4(0.1f, 0.1f, 0.1f, 140.0f / 255.0f);

const float Kaczka::WALL_SIZE = 8.0f;
const XMFLOAT3 Kaczka::WALLS_POS = XMFLOAT3(0.0f, 3.0f, 0.0f);
const XMFLOAT4 LightPos = XMFLOAT4(-1.0f, 3.0f, -3.0f, 1.0f);

#pragma endregion

#pragma region Initalization
Kaczka::Kaczka(HINSTANCE hInstance)
	: Base(hInstance, 1280, 720, L"Kaczka"),
	m_cbWorld(m_device.CreateConstantBuffer<XMFLOAT4X4>()),
	m_cbView(m_device.CreateConstantBuffer<XMFLOAT4X4, 2>()),
	m_cbSurfaceColor(m_device.CreateConstantBuffer<XMFLOAT4>()),
	m_cbPlane(m_device.CreateConstantBuffer<XMFLOAT4, 2>())
{
	//Projection matrix
	auto s = m_window.getClientSize();
	auto ar = static_cast<float>(s.cx) / s.cy;
	XMStoreFloat4x4(&m_projMtx, XMMatrixPerspectiveFovLH(XM_PIDIV4, ar, 0.01f, 100.0f));
	m_cbProj = m_device.CreateConstantBuffer<XMFLOAT4X4>();
	UpdateBuffer(m_cbProj, m_projMtx);
	XMFLOAT4X4 cameraMtx;
	XMStoreFloat4x4(&cameraMtx, m_camera.getViewMatrix());
	UpdateCameraCB(cameraMtx);

	//Sampler States
	SamplerDescription sd;
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.MaxAnisotropy = 16;

	m_samplerWrap = m_device.CreateSamplerState(sd);
	sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.BorderColor[0] = 0.0f;
	sd.BorderColor[1] = 0.0f;
	sd.BorderColor[2] = 0.0f;
	sd.BorderColor[3] = 0.0f;
	sd.MipLODBias = 0.5f;
	m_samplerWrap_back = m_device.CreateSamplerState(sd);

	//Regular shaders
	auto vsCode = m_device.LoadByteCode(L"envVS.cso");
	auto psCode = m_device.LoadByteCode(L"envPS.cso");
	m_envVS = m_device.CreateVertexShader(vsCode);
	m_envPS = m_device.CreatePixelShader(psCode);
	m_envIL = m_device.CreateInputLayout(VertexPositionNormal::Layout, vsCode);


	vsCode = m_device.LoadByteCode(L"waterVS.cso");
	psCode = m_device.LoadByteCode(L"waterPS.cso");
	m_waterVS = m_device.CreateVertexShader(vsCode);
	m_waterPS = m_device.CreatePixelShader(psCode);
	m_waterIL = m_device.CreateInputLayout(VertexPositionNormal::Layout, vsCode);

	vsCode = m_device.LoadByteCode(L"duckVS.cso");
	psCode = m_device.LoadByteCode(L"duckPS.cso");
	m_duckVS = m_device.CreateVertexShader(vsCode);
	m_duckPS = m_device.CreatePixelShader(psCode);
	m_duckIL = m_device.CreateInputLayout(VertexPositionNormalTex::Layout, vsCode);
	//Render states
	CreateRenderStates();

	//Meshes

	m_wall = Mesh::Rectangle(m_device);
	m_sheet = Mesh::Rectangle(m_device, SHEET_SIZE, SHEET_SIZE);
	m_duck = Mesh::LoadMesh(m_device, L"resources/duck/duck.txt");

	//	SetShaders();
	ID3D11Buffer* vsb[] = { m_cbWorld.get(),  m_cbView.get(), m_cbProj.get(), m_cbPlane.get() };
	m_device.context()->VSSetConstantBuffers(0, 4, vsb);
	m_device.context()->GSSetConstantBuffers(0, 1, vsb + 2);
	ID3D11Buffer* psb[] = { m_cbSurfaceColor.get(), m_cbLighting.get() };
	m_device.context()->PSSetConstantBuffers(0, 2, psb);

	CreateSheetMtx();
	CreateWallsMtx();
	SamplerDescription sd2;

	// TODO : 1.05 Create sampler with appropriate border color and addressing (border) and filtering (bilinear) modes
	sd2.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sd2.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sd2.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sd2.BorderColor[0] = 0;
	sd2.BorderColor[1] = 0;
	sd2.BorderColor[2] = 0;
	sd2.BorderColor[3] = 0;
	sd2.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	m_sampler = m_device.CreateSamplerState(sd2);

	//Textures
	// TODO : 1.10 Create shadow texture with appropriate width, height, format, mip levels and bind flags
	Texture2DDescription td;

	td.Width = 1280;
	td.Height = 720;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	td.MipLevels = 1;

	auto shadowTexture = m_device.CreateTexture(td);

	DepthStencilViewDescription dvd;

	// TODO : 1.11 Create depth-stencil-view for the shadow texture with appropriate format
	dvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//ShaderResourceViewDescription srvd;

	//// TODO : 1.12 Create shader resource view for the shadow texture with appropriate format, view dimensions, mip levels and most detailed mip level
	//srvd.Format = DXGI_FORMAT_R32_FLOAT;
	//srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srvd.Texture2D.MipLevels = 1;
	//srvd.Texture2D.MostDetailedMip = 0;

	//m_shadowMap = m_device.CreateShaderResourceView(shadowTexture, srvd);

	normalMap = vector<BYTE>(WATER_N * WATER_N * 4U);

	auto texDesc = Texture2DDescription(WATER_N, WATER_N);
	texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	waterTex = m_device.CreateTexture(texDesc);
	m_waterTexture = m_device.CreateShaderResourceView(waterTex);
	m_envTexture = m_device.CreateShaderResourceView(L"resources/textures/envMap.dds");
	m_duckTexture = m_device.CreateShaderResourceView(L"resources/duck/ducktex.jpg");

	InitializeWater();
	InitializeSimDuck();
}

void Kaczka::CreateRenderStates()
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
	rsDesc.FrontCounterClockwise = true;
	m_rsCCW = m_device.CreateRasterizerState(rsDesc);

	rsDesc.FrontCounterClockwise = false;
	rsDesc.CullMode = D3D11_CULL_NONE;
	m_rsCCW_backSh = m_device.CreateRasterizerState(rsDesc);
	rsDesc.CullMode = D3D11_CULL_BACK;
	m_rsCCW_frontSh = m_device.CreateRasterizerState(rsDesc);

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

#pragma endregion
#pragma region Per-Frame Update
void Kaczka::Update(const Clock& c)
{
	Base::Update(c);
	float dt = c.getFrameTime();
	if (HandleCameraInput(dt))
	{
		XMFLOAT4X4 cameraMtx;
		XMStoreFloat4x4(&cameraMtx, m_camera.getViewMatrix());
		UpdateCameraCB(cameraMtx);
	}

	RandomDrops();
	SimDuck(dt);
	UpdateHeights(dt);
}



void Kaczka::UpdateCameraCB(DirectX::XMFLOAT4X4 cameraMtx)
{
	XMMATRIX mtx = XMLoadFloat4x4(&cameraMtx);
	XMVECTOR det;
	auto invvmtx = XMMatrixInverse(&det, mtx);
	XMFLOAT4X4 view[2] = { cameraMtx };
	XMStoreFloat4x4(view + 1, invvmtx);
	UpdateBuffer(m_cbView, view);
}
void Kaczka::UpdatePlaneCB(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 dir)
{
	XMFLOAT4 plane[2] = { pos,dir };
	UpdateBuffer(m_cbPlane, plane);
}
#pragma endregion
#pragma region Frame Rendering Setup
void Kaczka::SetShaders(const dx_ptr<ID3D11InputLayout>& il, const dx_ptr<ID3D11VertexShader>& vs, const dx_ptr<ID3D11PixelShader>& ps)
{
	m_device.context()->IASetInputLayout(il.get());
	m_device.context()->VSSetShader(vs.get(), nullptr, 0);
	m_device.context()->PSSetShader(ps.get(), nullptr, 0);
	m_device.context()->GSSetShader(nullptr, nullptr, 0);
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Kaczka::SetTextures(std::initializer_list<ID3D11ShaderResourceView*> resList, const dx_ptr<ID3D11SamplerState>& sampler)
{
	m_device.context()->PSSetShaderResources(0, resList.size(), resList.begin());
	auto s_ptr = sampler.get();
	m_device.context()->PSSetSamplers(0, 1, &s_ptr);
}

#pragma endregion

#pragma region Drawing

void Kaczka::SetWorldMtx(DirectX::XMFLOAT4X4 mtx)
{
	UpdateBuffer(m_cbWorld, mtx);
}

void Kaczka::DrawMesh(const Mesh& m, DirectX::XMFLOAT4X4 worldMtx)
{
	SetWorldMtx(worldMtx);
	m.Render(m_device.context());
}

void Kaczka::CreateWallsMtx()
{
	XMVECTOR xRot = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR yRot = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR zRot = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);


	float a = WALL_SIZE;
	float scale = WALL_SIZE;

	m_wallsMtx[0] = XMMatrixScaling(scale, scale, scale) * XMMatrixTranslation(0.0f, 0.0f, a / 2);

	for (int i = 1; i < 4; ++i)
	{
		m_wallsMtx[i] = m_wallsMtx[i - 1] * XMMatrixRotationAxis(yRot, DirectX::XM_PIDIV2);
	}
	m_wallsMtx[4] = m_wallsMtx[3] * XMMatrixRotationAxis(zRot, DirectX::XM_PIDIV2);
	m_wallsMtx[5] = m_wallsMtx[4] * XMMatrixRotationAxis(zRot, DirectX::XM_PI);


	for (int i = 0; i < 6; ++i)
	{
		m_wallsMtx[i] = m_wallsMtx[i] * XMMatrixTranslation(WALLS_POS.x, WALLS_POS.y, WALLS_POS.z);
	}
}

void Kaczka::DrawWalls()
{
	for (int i = 0; i < 6; ++i)
	{
		UpdateBuffer(m_cbWorld, m_wallsMtx[i]);
		UpdateBuffer(m_cbSurfaceColor, XMFLOAT4(1.0f, 0.1f, 0.1f, 0.5f));
		m_wall.Render(m_device.context());
	}
}

void Kaczka::DrawSheet(bool colors)
{
	if (colors)
	{
		UpdateBuffer(m_cbSurfaceColor, SHEET_COLOR);
	}
	else
	{
		UpdateBuffer(m_cbSurfaceColor, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
	}
	UpdateBuffer(m_cbWorld, m_sheetMtx);
	m_sheet.Render(m_device.context());

	UpdateBuffer(m_cbWorld, m_revSheetMtx);
	m_sheet.Render(m_device.context());
}

void Kaczka::CreateSheetMtx()
{
	m_sheetMtx = XMMatrixRotationX(DirectX::XM_PIDIV2) * XMMatrixScaling(WALL_SIZE / 2.0f, WALL_SIZE / 2.0f, WALL_SIZE / 2.0f);
	m_revSheetMtx = XMMatrixRotationY(-DirectX::XM_PI) * m_sheetMtx;
}

void Kaczka::RandomDrops()
{
	//	heights[100][100] = 0.25f;
	for (int i = 0; i < WATER_N; ++i)
		for (int j = 0; j < WATER_N; ++j)
		{
			if (rand() % 1000 == 1)
				if (rand() % 1000 == 1)
				{
					//heights[i][j] = 0.25f;
				}
		}
}

XMFLOAT4 Kaczka::DeBoorToBernstein(float a, float b, float c, float d)
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

XMFLOAT2 Kaczka::RandPoint()
{
	XMFLOAT2 res;
	res.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//res.x *= 0.6f;
	//res.x += 0.2f;
	res.y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//res.y *= 0.6f;
	//res.y += 0.2f;
	return res;
}

void Kaczka::InitializeSimDuck()
{
	t = 0.0f;

	deBoor0 = RandPoint();
	deBoor1 = RandPoint();
	deBoor2 = RandPoint();
	deBoor3 = RandPoint();

	bernsteinXs = DeBoorToBernstein(deBoor0.x, deBoor1.x, deBoor2.x, deBoor3.x);
	bernsteinYs = DeBoorToBernstein(deBoor0.y, deBoor1.y, deBoor2.y, deBoor3.y);
}

void Kaczka::SimDuck(float dt)
{
	if (t >= 1.0f)
	{
		deBoor0 = deBoor1;
		deBoor1 = deBoor2;
		deBoor2 = deBoor3;
		deBoor3 = RandPoint();
		while (abs(deBoor3.x - deBoor2.x) + abs(deBoor3.y - deBoor2.y) < 0.8f) deBoor3 = RandPoint();
		bernsteinXs = DeBoorToBernstein(deBoor0.x, deBoor1.x, deBoor2.x, deBoor3.x);
		bernsteinYs = DeBoorToBernstein(deBoor0.y, deBoor1.y, deBoor2.y, deBoor3.y);
		t = 0.0f;
	}

	float coeffs[4];
	coeffs[0] = bernsteinXs.x;
	coeffs[1] = bernsteinXs.y;
	coeffs[2] = bernsteinXs.z;
	coeffs[3] = bernsteinXs.w;

	for (int i = 4; i > 0; --i)
	{
		for (int j = 0; j < i - 1; ++j)
		{
			coeffs[j] = (1 - t) * coeffs[j] + t * coeffs[j + 1];
		}
	}
	lastXPos = xPos;
	lastYPos = yPos;
	xPos = coeffs[0];

	coeffs[0] = bernsteinYs.x;
	coeffs[1] = bernsteinYs.y;
	coeffs[2] = bernsteinYs.z;
	coeffs[3] = bernsteinYs.w;
	for (int i = 4; i > 0; --i)
	{
		for (int j = 0; j < i - 1; ++j)
		{
			coeffs[j] = (1 - t) * coeffs[j] + t * coeffs[j + 1];
		}
	}
	yPos = coeffs[0];

	int i = (int)round(xPos * WATER_N);
	int j = (int)round(yPos * WATER_N);
	heights[i][j] = 0.25f;
	t += dt / 2;
}

void Kaczka::InitializeWater()
{
	for (int i = 0; i < WATER_N; ++i)
	{
		for (int j = 0; j < WATER_N; ++j)
		{
			heights[i][j] = 0.0f;
			heightsOld[i][j] = 0.0f;

			float l = 2.0f * (float)min(i, min(j, min(WATER_N - 1 - i, WATER_N - 1 - j))) / (WATER_N - 1);

			d[i][j] = 0.95f * min(1, l / 0.2f);
		}
	}
}

void Kaczka::UpdateHeights(float dt)
{
	float c = 1.0f;
	float h = 2.0f / (WATER_N - 1);
	dt = 1.0f / WATER_N;
	float A = c * c * dt * dt / (h * h);
	float B = 2 - 4 * A;
	for (int i = 0; i < WATER_N; ++i)
	{
		for (int j = 0; j < WATER_N; ++j)
		{
			if (i == 100 && j == 100)
			{
				int xx = 1;
			}
			float tmp = 0.0f;
			if (i > 0) tmp += heightsOld[i - 1][j];
			if (j > 0) tmp += heightsOld[i][j - 1];
			if (i < WATER_N - 1) tmp += heightsOld[i + 1][j];
			if (j < WATER_N - 1) tmp += heightsOld[i][j + 1];

			heights[i][j] = d[i][j] * (A * tmp + B * heightsOld[i][j] - heights[i][j]);
		}
	}

	auto data = normalMap.data();
	for (int i = 0; i < WATER_N; i++) {
		for (int j = 0; j < WATER_N; j++) {
			float h = heights[i][j] + 1.0f;
			h /= 2.0f;
			*(data++) = static_cast<BYTE>(h * 255.0f);
			*(data++) = static_cast<BYTE>(h * 255.0f);
			*(data++) = static_cast<BYTE>(h * 255.0f);
			*(data++) = 255;
		}
	}

	m_device.context()->UpdateSubresource(waterTex.get(), 0, nullptr, normalMap.data(), WATER_N * 4U, WATER_N * WATER_N * 4U);
	m_device.context()->GenerateMips(m_waterTexture.get());

	std::swap(heights, heightsOld);
}


void Kaczka::Render()
{
	Base::Render();
	SetShaders(m_waterIL, m_waterVS, m_waterPS);
	SetTextures({ m_waterTexture.get() }, m_samplerWrap);
	DrawSheet(true);

	SetShaders(m_envIL, m_envVS, m_envPS);
	SetTextures({ m_envTexture.get() }, m_samplerWrap_back);
	UpdateBuffer(m_cbSurfaceColor, XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f));
	DrawWalls();

	SetShaders(m_duckIL, m_duckVS, m_duckPS);
	SetTextures({ m_duckTexture.get() }, m_samplerWrap);
	XMFLOAT4X4 duckMtx{};

	XMFLOAT2 vec = { xPos - lastXPos,yPos - lastYPos };
	XMFLOAT2 len; XMStoreFloat2(&len, XMVector2Length(XMLoadFloat2(&vec)));
	float angle = atan2(vec.x, -vec.y);
	//XMStoreFloat4x4(&duckMtx, XMMatrixTranslation(WALL_SIZE / DUCK_SCALING * xPos, 0.0f, WALL_SIZE / DUCK_SCALING * xPos) * XMMatrixTranslation(-400.0f, 0.0f, -400.0f) * XMMatrixScaling(DUCK_SCALING, DUCK_SCALING, DUCK_SCALING));
	XMStoreFloat4x4(&duckMtx, XMMatrixScaling(DUCK_SCALING, DUCK_SCALING, DUCK_SCALING) * XMMatrixRotationY(angle) * XMMatrixTranslation(WALL_SIZE * yPos, 0.0f, WALL_SIZE * xPos) * XMMatrixTranslation(-WALL_SIZE / 2, 0.0f, -WALL_SIZE / 2));
	DrawMesh(m_duck, duckMtx);
}
#pragma endregion