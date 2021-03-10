#include "dxApplication.h"
#include <DirectXMath.h>
#include <windowsx.h>
using namespace mini;
using namespace std;
using namespace DirectX;
DxApplication::DxApplication(HINSTANCE hInstance)
	: WindowApplication(hInstance), m_device(m_window) {
	ID3D11Texture2D* temp = nullptr;
	m_device.swapChain()->GetBuffer(0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&temp));
	const dx_ptr<ID3D11Texture2D> backTexture{ temp };
	m_backBuffer =
		m_device.CreateRenderTargetView(backTexture);

	SIZE wndSize = m_window.getClientSize();
	m_depthBuffer = m_device.CreateDepthStencilView(wndSize);
	auto backBuffer = m_backBuffer.get();
	m_device.context()->OMSetRenderTargets(1, &backBuffer, m_depthBuffer.get());
	Viewport viewport{ wndSize };
	m_device.context()->RSSetViewports(1, &viewport);
	const auto vsBytes = DxDevice::LoadByteCode(L"vs.cso");
	const auto psBytes = DxDevice::LoadByteCode(L"ps.cso");
	m_vertexShader = m_device.CreateVertexShader(vsBytes);
	m_pixelShader = m_device.CreatePixelShader(psBytes);
	const auto vertices = CreateCubeVertices();
	m_vertexBuffer = m_device.CreateVertexBuffer(vertices);
	const auto indices = CreateCubeIndices();
	m_indexBuffer = m_device.CreateIndexBuffer(indices);


	vector<D3D11_INPUT_ELEMENT_DESC> elements{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPositionColor, color),D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	m_layout = m_device.CreateInputLayout(elements, vsBytes);

	XMStoreFloat4x4(&m_modelMtx, XMMatrixIdentity());
	SetViewMtx();
	XMStoreFloat4x4(&m_projMtx, XMMatrixPerspectiveFovLH(XMConvertToRadians(45), static_cast<float>(wndSize.cx) / wndSize.cy, 0.1f, 100.0f));
	m_cbMVP = m_device.CreateConstantBuffer<XMFLOAT4X4>();

	XMStoreFloat4x4(&m_modelMtx2, XMMatrixIdentity() * XMMatrixTranslation(-3.0f, 0.0f, 0.0f));
	//m_cbMVP2 = m_device.CreateConstantBuffer<XMFLOAT4X4>();
}

void DxApplication::SetViewMtx()
{
	XMStoreFloat4x4(&m_viewMtx, XMMatrixRotationX(XMConvertToRadians(v_angle)) * XMMatrixTranslation(0.0f, 0.0f, v_dist));
}

std::vector<XMFLOAT2> DxApplication::CreateTriangleVertices()
{
	std::vector<XMFLOAT2> vec = std::vector<XMFLOAT2>();

	XMFLOAT2 v1 = XMFLOAT2();
	v1.x = -0.8f;
	v1.y = -0.8f;

	XMFLOAT2 v2 = XMFLOAT2();
	v2.x = -0.8f;
	v2.y = 0.8f;

	XMFLOAT2 v3 = XMFLOAT2();
	v3.x = 0.8f;
	v3.y = -0.8f;

	vec.push_back(v1);
	vec.push_back(v2);
	vec.push_back(v3);

	return vec;
}
int DxApplication::MainLoop() {
	MSG msg{};
	do {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else {
			Update();
			Render();
			m_device.swapChain()->Present(0, 0);

		}
	} while (msg.message != WM_QUIT);
	return (int)msg.wParam;

}

bool DxApplication::ProcessMessage(mini::WindowMessage& msg)
{
	int ySize = m_window.getClientSize().cy;
	bool setViewMtx = false;
	switch (msg.message)
	{
	case WM_LBUTTONDOWN:
		l_yPos = GET_Y_LPARAM(msg.lParam);
		break;
	case WM_RBUTTONDOWN:
		r_yPos = GET_Y_LPARAM(msg.lParam);
		break;
	case WM_MOUSEMOVE:
		if ((msg.wParam & MK_LBUTTON) != 0)
		{
			int y = GET_Y_LPARAM(msg.lParam);
			float change = (float)(y - l_yPos) / ySize;
			v_angle += -change * 50.0f;
			if (v_angle > 90.0f) v_angle = 90.0f;
			if (v_angle < -90.0f) v_angle = -90.0f;
			setViewMtx = true;
			l_yPos = y;
		}
		if ((msg.wParam & MK_RBUTTON) != 0)
		{
			int y = GET_Y_LPARAM(msg.lParam);
			float change = (float)(y - r_yPos) / ySize;
			v_dist += change * 10.0f;
			if (v_dist > 50.0f) v_dist = 50.0f;
			if (v_dist < 0.0f) v_dist = 0.0f;
			setViewMtx = true;
			r_yPos = y;
		}
		if (setViewMtx)
			SetViewMtx();
		break;
	default:
		return false;
	}

	return false;

}
void DxApplication::Update() {
	LARGE_INTEGER ticks;
	QueryPerformanceCounter(&ticks);
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	float seconds = ((float)ticks.QuadPart) / frequency.QuadPart;
	//seconds = fmod(seconds, 8.0f);

	XMStoreFloat4x4(&m_modelMtx, XMMatrixIdentity() * XMMatrixRotationY(XMConvertToRadians(seconds * 45)));
}

void DxApplication::Draw(XMFLOAT4X4* modelMtx)
{
	D3D11_MAPPED_SUBRESOURCE res;
	m_device.context()->Map(m_cbMVP.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	XMMATRIX mvp = XMLoadFloat4x4(modelMtx) * XMLoadFloat4x4(&m_viewMtx) * XMLoadFloat4x4(&m_projMtx);
	memcpy(res.pData, &mvp, sizeof(XMMATRIX));
	m_device.context()->Unmap(m_cbMVP.get(), 0);
	ID3D11Buffer* cbs[] = { m_cbMVP.get() };
	m_device.context()->VSSetConstantBuffers(0, 1, cbs);
	m_device.context()->DrawIndexed(36, 0, 0);
}

void DxApplication::Render() {
	const float clearColor[] = { 0.5f, 0.5f, 1.0f, 1.0f };
	m_device.context()->ClearRenderTargetView(
		m_backBuffer.get(), clearColor);

	m_device.context()->ClearDepthStencilView(
		m_depthBuffer.get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_device.context()->VSSetShader(
		m_vertexShader.get(), nullptr, 0);
	m_device.context()->PSSetShader(
		m_pixelShader.get(), nullptr, 0);
	m_device.context()->IASetInputLayout(m_layout.get());
	m_device.context()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11Buffer* vbs[] = { m_vertexBuffer.get() };
	UINT strides[] = { sizeof(VertexPositionColor) };
	UINT offsets[] = { 0 };
	m_device.context()->IASetVertexBuffers(0, 1, vbs, strides, offsets);
	m_device.context()->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);

	Draw(&m_modelMtx);
	Draw(&m_modelMtx2);
}

vector < DxApplication::VertexPositionColor>
DxApplication::CreateCubeVertices() {
	return {
		//Front Face
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f } },
		{ { +0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f } },
		{ { +0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ { -0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		//Back Face
		{ { -0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f, 1.0f } },
		{ { +0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f, 1.0f } },
		{ { +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f, 1.0f } },
		{ { -0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f, 1.0f } },

		//Top Face
		{ { -0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ { -0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ { +0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ { +0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
		//Bottom Face
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f, 1.0f } },
		{ { +0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f, 0.0f } },
		{ { +0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f } },

		//Left Face
		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },
		{ { -0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, 1.0f } },
		{ { -0.5f, +0.5f, +0.5f }, { 0.0f, 0.0f, 0.0f } },
		{ { -0.5f, +0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f } },
		//Right Face
		{ { +0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },
		{ { +0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, 1.0f } },
		{ { +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f, 1.0f } },
		{ { +0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f } }
	};

}
vector<unsigned short> DxApplication::CreateCubeIndices() {
	return {
	 0,2,1, 0,3,2,

	 5,7,4, 5,6,7,

	 11,9,10, 11,8,9,

	 15,13,12, 15,14,13,

	 16,18,19, 16,17,18,

	 20,22,21, 20,23,22
	};
}