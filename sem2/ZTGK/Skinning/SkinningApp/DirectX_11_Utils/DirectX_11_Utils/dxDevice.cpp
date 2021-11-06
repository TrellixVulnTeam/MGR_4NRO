#include "dxDevice.h"
#include "exceptions.h"
#include <cassert>
#include "Microsoft/WICTextureLoader.h"
#include "Microsoft/DDSTextureLoader.h"
#include <fstream>
#include "Windows/window.h"

using namespace std;
using namespace Real_Engine;
using namespace DirectX;

DxDevice::DxDevice(const Window& window)
{

	D3D_DRIVER_TYPE driverTypes[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE };
	unsigned driverTypesCount = ARRAYSIZE(driverTypes);
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	unsigned featureLevelsCout = ARRAYSIZE(featureLevels);

	SwapChainDescription desc(window.getHandle(), window.getClientSize());

	unsigned int creationFlags = 0;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL fl;
	HRESULT hr = S_OK;
	for (unsigned int driver = 0; driver < driverTypesCount; ++driver)
	{
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		IDXGISwapChain* swapChain = nullptr;
		hr = D3D11CreateDeviceAndSwapChain(nullptr, driverTypes[driver], nullptr, creationFlags,
			featureLevels, featureLevelsCout, D3D11_SDK_VERSION, &desc, &swapChain, &device, &fl, &context);
		m_device.reset(device);
		m_swapChain = move(DxSwapChain(dx_ptr<IDXGISwapChain>(swapChain)));
		m_immediateContext.reset(context);
		if (SUCCEEDED(hr))
			return;
	}
	THROW_DX(hr);
}


dx_ptr<ID3D11Texture2D> DxDevice::CreateTexture(const Texture2DDescription& desc, const SubresourceData* data) const
{
	assert(m_device);
	ID3D11Texture2D* t = nullptr;
	HR(m_device->CreateTexture2D(&desc, data, &t));
	dx_ptr<ID3D11Texture2D> texture(t);

	return texture;
}

void DxDevice::BeginRender(ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView)
{
	m_immediateContext->RSSetViewports(1, &viewport);
	m_immediateContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

//dx_ptr<ID3D11RenderTargetView> DxDevice::CreateRenderTargetView(const dx_ptr<ID3D11Texture2D>& backTexture) const
//{
//	assert(m_device);
//	ID3D11RenderTargetView* bb = nullptr;
//	HR(m_device->CreateRenderTargetView(backTexture.get(), nullptr, &bb));
//	dx_ptr<ID3D11RenderTargetView> backBuffer(bb);
//	return backBuffer;
//}
ID3D11RenderTargetView* DxDevice::CreateRenderTargetView(const dx_ptr<ID3D11Texture2D>& backTexture) const
{
	assert(m_device);
	ID3D11RenderTargetView* bb = nullptr;
	HR(m_device->CreateRenderTargetView(backTexture.get(), nullptr, &bb));
	return bb;
}

//dx_ptr<ID3D11RenderTargetView> DxDevice::CreateRenderTargetView(const dx_ptr<ID3D11Texture2D>& backTexture, const D3D11_RENDER_TARGET_VIEW_DESC& desc) const
//{
//	assert(m_device);
//	ID3D11RenderTargetView* bb = nullptr;
//	HR(m_device->CreateRenderTargetView(backTexture.get(), &desc, &bb));
//	dx_ptr<ID3D11RenderTargetView> backBuffer(bb);
//	return backBuffer;
//}

ID3D11RenderTargetView* DxDevice::CreateRenderTargetView(const dx_ptr<ID3D11Texture2D>& backTexture, const D3D11_RENDER_TARGET_VIEW_DESC& desc) const
{
	assert(m_device);
	ID3D11RenderTargetView* bb = nullptr;
	HR(m_device->CreateRenderTargetView(backTexture.get(), &desc, &bb));
	return bb;
}

dx_ptr<ID3D11DepthStencilView> DxDevice::CreateDepthStencilView(unsigned int width, unsigned int height) const
{
	auto desc = Texture2DDescription::DepthTextureDescription(width, height);
	auto depthTexture = CreateTexture(desc);
	return CreateDepthStencilView(depthTexture, new DepthViewDescription());
}

dx_ptr<ID3D11DepthStencilView> DxDevice::CreateDepthStencilView(const dx_ptr<ID3D11Texture2D>& depthTexture, const DepthViewDescription* desc) const
{
	assert(m_device);
	ID3D11DepthStencilView* dsv = nullptr;
	HR(m_device->CreateDepthStencilView(depthTexture.get(), desc, &dsv));
	dx_ptr<ID3D11DepthStencilView> depthStencilView(dsv);

	return depthStencilView;
}

void DxDevice::SetViewport(ViewportDescription viewport)
{
	this->viewport = viewport;
}

//
//dx_ptr<ID3D11Buffer> DxDevice::_CreateBuffer(const void * data, unsigned int byteLenght, D3D11_BIND_FLAG flag,
//			D3D11_USAGE usage) const
//{
//	BufferDescription desc(byteLenght, flag, usage);
//	if ((usage & D3D11_USAGE_DYNAMIC) != 0 || (usage & D3D11_USAGE_STAGING) != 0)
//		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	return CreateBuffer(desc, data);
//}

dx_ptr<ID3D11Buffer> DxDevice::CreateBuffer(const BufferDescription& desc, const void* pData) const
{
	assert(m_device);
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = pData;
	ID3D11Buffer* b = nullptr;
	HR(m_device->CreateBuffer(&desc, pData ? &data : nullptr, &b));
	dx_ptr<ID3D11Buffer> buffer(b);
	return buffer;
}

dx_ptr<ID3D11BlendState> DxDevice::CreateBlendState(const BlendDescription& desc) const
{
	assert(m_device);
	ID3D11BlendState* s = nullptr;
	HR(m_device->CreateBlendState(&desc, &s));
	dx_ptr<ID3D11BlendState> state(s);
	return state;
}

dx_ptr<ID3D11DepthStencilState> DxDevice::CreateDepthStencilState(const DepthStencilDescription& desc) const
{
	assert(m_device);
	ID3D11DepthStencilState* s = nullptr;
	HR(m_device->CreateDepthStencilState(&desc, &s));
	dx_ptr<ID3D11DepthStencilState> state(s);
	return state;
}

dx_ptr<ID3D11InputLayout> DxDevice::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* layout,
	unsigned int layoutElements,
	const vector<BYTE>& vsByteCode) const
{
	assert(m_device);
	ID3D11InputLayout* il = nullptr;
	HR(m_device->CreateInputLayout(layout, layoutElements, vsByteCode.data(), vsByteCode.size(), &il));
	dx_ptr<ID3D11InputLayout> inputLayout(il);
	return inputLayout;
}

dx_ptr<ID3D11RasterizerState> DxDevice::CreateRasterizerState(const RasterizerDescription& desc) const
{
	assert(m_device);
	ID3D11RasterizerState* s = nullptr;
	HR(m_device->CreateRasterizerState(&desc, &s));
	dx_ptr<ID3D11RasterizerState> state(s);
	return state;
}

dx_ptr<ID3D11SamplerState> DxDevice::CreateSamplerState(const SamplerDescription& desc) const
{
	assert(m_device);
	ID3D11SamplerState* s = nullptr;
	HR(m_device->CreateSamplerState(&desc, &s));
	dx_ptr<ID3D11SamplerState> sampler(s);
	return sampler;
}

dx_ptr<ID3D11ShaderResourceView> DxDevice::CreateShaderResourceView(const wstring& texPath) const
{
	assert(m_device);
	ID3D11ShaderResourceView* rv;
	HRESULT hr = 0;
	const wstring ext{ L".dds" };
	if (texPath.size() > ext.size() && texPath.compare(texPath.size() - ext.size(), ext.size(), ext) == 0)
		hr = CreateDDSTextureFromFile(m_device.get(), m_immediateContext.get(), texPath.c_str(), nullptr, &rv);
	else
		hr = CreateWICTextureFromFile(m_device.get(), m_immediateContext.get(), texPath.c_str(), nullptr, &rv);
	dx_ptr<ID3D11ShaderResourceView> resourceView(rv);

	HR(hr);

	return resourceView;
}

dx_ptr<ID3D11ShaderResourceView> DxDevice::CreateShaderResourceView(const dx_ptr<ID3D11Texture2D>& texture,
	const ShaderResourceViewDescription* desc) const
{
	assert(m_device);
	ID3D11ShaderResourceView *srv;
	HR(m_device->CreateShaderResourceView(texture.get(), desc, &srv));
	dx_ptr<ID3D11ShaderResourceView> resourceView(srv);
	return resourceView;
}

ID3DX11Effect* DxDevice::LoadEffect(const std::wstring& fileName)
{
	ifstream sIn(fileName, ios::in | ios::binary);
	if (!sIn)
		THROW(L"Unable to open " + fileName);
	sIn.seekg(0, ios::end);
	auto size = sIn.tellg();
	sIn.seekg(0, ios::beg);
	vector<char> compiledShader(size);
	if (!sIn.read(&compiledShader[0], size))
		THROW(L"Error reading" + fileName);
	sIn.close();
	ID3DX11Effect* mFX;
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, m_device.get(), &mFX));
	return mFX;
}

dx_ptr<ID3D11InputLayout> DxDevice::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutElements, const uint8_t* signature, size_t signatureSize) const
{
	ID3D11InputLayout* il = nullptr;
	HR(m_device->CreateInputLayout(layout, layoutElements, signature, signatureSize, &il));
	dx_ptr<ID3D11InputLayout> inputLayout(il);
	return inputLayout;
}
